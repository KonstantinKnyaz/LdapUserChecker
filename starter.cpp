#include "starter.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QSqlQuery>
#include <QSqlError>
#include <QSettings>


//DB
#define DATA_BASE     "DataBase"
#define DB_NAME       "DBName"
#define DB_REDMINE    "DBRedmine"
#define DB_HOSTNAME   "HostName"
#define DB_PORT       "Port"
#define DB_USER       "UserName"
#define DB_PASSWORD   "Password"
//LDAP
#define LDAP          "Ldap"
#define LD_SERVER     "Server"
#define LD_PORT       "Port"
#define LD_USER       "UserLogin"
#define LD_PASS       "UserPass"

//"schelikalinmu@mars"
#define EMAIL         "kniazevkp@mars"

Starter::Starter(QObject *parent) : QObject(parent)
{

    if(takeDBInfoConf()) {
        qCritical() << "Не удалсь прочитать файл конфига БД";
        return;
    }

    QSettings settings("./Settings.ini", QSettings::IniFormat);
    settings.beginGroup("Mails");
    mails = settings.value("mail_recipients").toString().split(",");
    settings.endGroup();
    settings.beginGroup("Exeptions");
    saiExeptions = settings.value("exeption_sai").toString();
    redmineExeptions = settings.value("exeption_redmine").toString();
    settings.endGroup();

//    smtp = new Smtp("mail.mars", SCHELIKALIN_EMAIL, QString(QChar(117)) + QChar(107) + QChar(51) + QChar(51) + QChar(114));
    smtp = new Smtp("mail.mars", EMAIL, "mNNBs");

    ldap = new LdapSearcher(this);

    if(setLdap() != LDAP_SUCCESS) {
        qCritical() << "Не удалось подключится к серверу LDAP";
        mailSendToManyPeople("Не удалось подключится к серверу LDAP", lastError.getErrorString());
        return;
    }

    if(!setDB()) {
        if(!saiSearch() && !redmineSearch()) {
            sendInfoToMail();
        }
    }
}

Starter::~Starter()
{
    ldap->LDClose();

    if(psqlDB.isOpen())
        psqlDB.close();

    if(psqlRedmineDB.isOpen())
        psqlRedmineDB.close();

    if(!smtp.isNull())
        delete smtp;
}

Error Starter::getLastError()
{
    return lastError;
}

int Starter::takeDBInfoConf()
{
    QFile file("./Conf.json");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Не удалось открыть файл с найтройками БД";
        return FILE_OPEN_ERROR;
    }
    QByteArray byteArray = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument ConfDoc = QJsonDocument::fromJson(byteArray, &error);
    qInfo() << "Парсинг Json завершился:" << error.errorString();
    if(!ConfDoc.isObject()) {
        qCritical() << "Битые данные в файле с настройками БД. Не соответсвуют формату Json";
        return JSON_DOC_ERROR;
    }

    QJsonObject ConfObj = ConfDoc.object();
    QJsonValue psqlConfVal = ConfObj.value(DATA_BASE);

    dbConf.insert(DB_NAME, psqlConfVal.toObject().value(DB_NAME).toString());
    dbConf.insert(DB_HOSTNAME, psqlConfVal.toObject().value(DB_HOSTNAME).toString());
    dbConf.insert(DB_PORT, psqlConfVal.toObject().value(DB_PORT).toString());
    dbConf.insert(DB_USER, psqlConfVal.toObject().value(DB_USER).toString());
    dbConf.insert(DB_PASSWORD, psqlConfVal.toObject().value(DB_PASSWORD).toString());
    dbConf.insert(DB_REDMINE, psqlConfVal.toObject().value(DB_REDMINE).toString());

    QJsonValue ldapVal = ConfObj.value(LDAP);

    ldapConf.insert(LD_SERVER, ldapVal.toObject().value(LD_SERVER).toString());
    ldapConf.insert(LD_PORT, ldapVal.toObject().value(LD_PORT).toString());
    ldapConf.insert(LD_USER, ldapVal.toObject().value(LD_USER).toString());
    ldapConf.insert(LD_PASS, ldapVal.toObject().value(LD_PASS).toString());

    return NO_ERROR;
}

int Starter::setDB()
{
    psqlDB = QSqlDatabase::addDatabase("QPSQL", "SAI");
    psqlDB.setDatabaseName(dbConf.value(DB_NAME));
    psqlDB.setHostName(dbConf.value(DB_HOSTNAME));
    psqlDB.setPort(dbConf.value(DB_PORT).toInt());
    psqlDB.setUserName(dbConf.value(DB_USER));
    psqlDB.setPassword(dbConf.value(DB_PASSWORD));

    if(!psqlDB.open()) {
        qCritical() << "Не удалось подключится к БД SAI";
        qCritical() << psqlDB.lastError().text();
        lastError.setLastError(DB_SAI_CONNECT_ERROR, psqlDB.lastError().text());
        mailSendToManyPeople("Не удалось подключится к БД SAI", lastError.getErrorString());
        return DB_SAI_CONNECT_ERROR;
    }

    psqlRedmineDB = QSqlDatabase::addDatabase("QPSQL", "REDMINE");
    psqlRedmineDB.setDatabaseName(dbConf.value(DB_REDMINE));
    psqlRedmineDB.setHostName(dbConf.value(DB_HOSTNAME));
    psqlRedmineDB.setPort(dbConf.value(DB_PORT).toInt());
    psqlRedmineDB.setUserName(dbConf.value(DB_USER));
    psqlRedmineDB.setPassword(dbConf.value(DB_PASSWORD));

    if(!psqlRedmineDB.open()) {
        qCritical() << "Не удалось подключится к БД Redmine";
        qCritical() << psqlRedmineDB.lastError().text();
        lastError.setLastError(DB_REDMINE_CONNECT_ERROR, psqlRedmineDB.lastError().text());
        mailSendToManyPeople("Не удалось подключится к БД Redmine", lastError.getErrorString());
        return DB_REDMINE_CONNECT_ERROR;
    }

    return NO_ERROR;
}

int Starter::setLdap()
{
    QString ldapUrl = QString("ldap://%1:%2")
            .arg(ldapConf.value(LD_SERVER))
            .arg(ldapConf.value(LD_PORT));

    int res = ldap->LDInit(ldapUrl);

    if(res != LDAP_SUCCESS) {
        qCritical() << "Не удалось подключится к серверу  LDAP";
        lastError.setLastError(res, "Не удалось подключится к серверу LDAP");
        return res;
    }

    res = ldap->LDBind(ldapConf.value(LD_USER), ldapConf.value(LD_PASS));

    if(res != LDAP_SUCCESS) {
        qCritical() << "Не удалось выполнить bind к серверу  LDAP";
        lastError.setLastError(res, "Не удалось выполнить bind к серверу  LDAP");
        return res;
    }

    return res;
}

int Starter::saiSearch()
{
    QSqlQuery query(psqlDB);

    QString sql = QString(R"sql(
                            SELECT id, "ФИО", "e-mail", "Отдел", "Пароль", "Телефон", "Доступ права",
                            "Комната", "Заблокирован", type
                            FROM public.users
                            WHERE NOT "Заблокирован" AND "e-mail" != '' AND type = 'User'
                            )sql");

    if(!saiExeptions.isEmpty())
        sql.append(QString(" AND id NOT IN(%1)").arg(saiExeptions));

    if(!query.exec(sql)) {
        qWarning() << "Не удалось выполнить запрос:" << sql;
        lastError.setLastError(DB_QUERY_ERROR, QString("Не удалось выполнить запрос: ").append(sql));
        mailSendToManyPeople("Ошибка при выполнении запроса к sai", lastError.getErrorString());
        return DB_QUERY_ERROR;
    }

    int userNum = 0;
    QString idUsers = "WHERE id IN(";

    while(query.next()) {
        QString UserSaiName = query.value(1).toString();
        QString UserSaiMail = QString("(mail=%1)").arg(query.value(2).toString());
        int result = 0;

        if((ldap->LDSearch(UserSaiMail, result) != LDAP_SUCCESS)) {
            qCritical() << "Не удалось выполнить поиск в LDAP";
            lastError.setLastError(LDAP_ERROR, QString("Не удалось выполнить поиск в LDAP"));
            mailSendToManyPeople("Ошибка при поиске пользователей Sai", lastError.getErrorString());
            return LDAP_ERROR;
        }

        if(!result) {
            userNum++;
            idUsers.append(query.value(0).toString());
            idUsers.append(",");
            SaiUsers sUsers;
            sUsers.FIO = UserSaiName;
            sUsers.Mail = query.value(2).toString();
            saiUsersNotFound.append(sUsers);
        }
    }
    idUsers.chop(1);
    idUsers.append(")");

    saiIds = idUsers;

    return NO_ERROR;
}

int Starter::redmineSearch()
{
    QSqlQuery redmineQuery(psqlRedmineDB);

    QString sql = QString(R"sql(
                            SELECT users.id, login, lastname || ' ' || firstname, admin, status,
                            last_login_on, language, auth_source_id, users.created_on, users.updated_on,
                            type, address
                            FROM public.users
                            LEFT JOIN email_addresses ON users.id = user_id
                            WHERE type = 'User' AND address IS NOT NULL
                            AND status = 1 AND address != 'net@mars'
                            AND users.id NOT IN (797)
                            )sql");

    if(!redmineExeptions.isEmpty())
        sql.append(QString(" AND users.id NOT IN(%1)").arg(redmineExeptions));

    sql.append(" ORDER BY users.id");

    if(!redmineQuery.exec(sql)) {
        qWarning() << "Не удалось выполнить запрос:" << sql;
        lastError.setLastError(DB_QUERY_ERROR, QString("Не удалось выполнить запрос: ").append(sql));
        mailSendToManyPeople("Ошибка при выполнении запроса к redmine", lastError.getErrorString());
        return DB_QUERY_ERROR;
    }

    int userNum = 0;
    QString idUsers = "WHERE id IN(";

    while(redmineQuery.next()) {
        QString UserRedmineName = redmineQuery.value(2).toString();
        QString UserRedmineMail = QString("(mail=%1)").arg(redmineQuery.value(11).toString());
        int result = 0;

        if((ldap->LDSearch(UserRedmineMail, result) != LDAP_SUCCESS)) {
            qCritical() << "Не удалось выполнить поиск в LDAP";
            lastError.setLastError(LDAP_ERROR, QString("Не удалось выполнить поиск в LDAP"));
            mailSendToManyPeople("Ошибка при поиске пользователей Redmine", lastError.getErrorString());
            return LDAP_ERROR;
        }

        if(!result) {
            userNum++;
            idUsers.append(redmineQuery.value(0).toString());
            idUsers.append(",");
            RedmineUsers rUsers;
            rUsers.id = redmineQuery.value(0).toString();
            rUsers.FIO = UserRedmineName;
            rUsers.Mail = redmineQuery.value(11).toString();
            RedmineUsersNotFound.append(rUsers);
        }
    }

    idUsers.chop(1);
    idUsers.append(")");

    redmineIds = idUsers;

    return NO_ERROR;
}

void Starter::sendInfoToMail()
{
    QString Theme, date, message;

    date = QDateTime::currentDateTime().toString();

    Theme = QString("[%1](%2) Отсутствующие пользователи в LDAP").arg(qAppName()).arg(date);

    QString saiUsrs = QString(R"d(
                                <table BORDER="1" width=100% cellspacing=1 cellpadding=1 bordercolor="black">
                                <tr>
                                <th>№</th>
                                <th>ФИО</th>
                                <th>Почта</th>
                                </tr>
                                )d");

    for(int i = 0; i < saiUsersNotFound.count(); i++) {
        QString strFio = QString("<tr><td>%1</td><td>%2</td><td>%3</td></tr>")
                .arg(i+1)
                .arg(saiUsersNotFound.at(i).FIO)
                .arg(saiUsersNotFound.at(i).Mail);
                saiUsrs.append(strFio);
    }

    saiUsrs.append("</table>");

    QString redmineUsrs = QString(R"d(
                                <table BORDER="1" width=100% cellspacing=1 cellpadding=1 bordercolor="black">
                                <tr>
                                <th>№</th>
                                <th>Ид</th>
                                <th>ФИО</th>
                                <th>Почта</th>
                                <th>Ссылка</th>
                                </tr>
                                    )d");


    for(int i = 0; i < RedmineUsercNo|Found.count,); i++) {

      "0QQtring link = QStsing(R"html(<a hrgf="http://dbnewmars/redmine/useps/%1">http://d�new.mars/redmine/users/%1</a>)atml").arg(RedmineUsersN�tFounD.at*i).id);J "      QString strFio = QString("<tr><�d>%1</td><td>%2</td><td>%3</vd�,td>%4</td><td>%5</td></tr>")
       !      ! .arg(i+1)
      �         .arg(RedIineUsdrsNotFound.at(i).id)
                .arg(RedmineQsersNopFound.at(i).FH)
"             � .arg(RedmineUsersLotFound.at(i).Mayl)
�    "          .arg(link);
�               sedmineUsrs.append(strFio);
    }

(   redmineUsrs.qppend("<.table>"): 

  ! meswage = QSuring(R"d(
                        <body lini=#1133FF>
              $ �       <font vace="ARIAL" colov=#36AA36<f>#Отсут��твуюQ�ие пользователи</b><p<p>
                        <HR size=1></font>
                        <br><b>ПользОвТтإл��`о񂑁утсвующиP� в LDAP, но есть В �Є SAI:</b</br>
         0`  $       0  <br>%1</br>
               (        <b�>%2</br>
            `           <br><b>Пользователи отсутс�2ующие в LTAP, но есть0в!БД RedmiNe:</�></br>
                �       <br>%</br>
$                       <"r/%4</br>
                        )d")
            .are(3aiUsrs),arg�saiIds).arg(redmineUsrs).arg(redmineIds);

�   mailSendToMaNy�eople(Theme, messqge);
}

voId`Starter::mailSendToManyQeople(const QStrhng &thume, const QString &mess�g%)
{
    if(mails.isEmpty())
( "     return;

 � �fnr(int i = 0; � < mails.count(); i++) z
       $smtp->SeNd(qAppName(), miils.a4(i), tx�me, -essage+3
 !  }
}
