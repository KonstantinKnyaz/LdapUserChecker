#ifndef STARTER_H
#define STARTER_H

#include <QObject>
#include <QSqlDatabase>
#include <QHash>
#include <QPointer>

#include "LdapSearcher.h"
#include "smtp/smtp.h"

enum DBCONFERROR {
    NO_ERROR = 0,
    FILE_OPEN_ERROR = 1,
    JSON_DOC_ERROR = 2,
    DB_SAI_CONNECT_ERROR = 3,
    DB_REDMINE_CONNECT_ERROR = 4,
    DB_QUERY_ERROR = 5,
    LDAP_ERROR = 6
};

struct Error {
public:
    QString getErrorString()
    {
        if(_error != NO_ERROR)
            return _errorString;
        return QString();
    }

    int getLastErrorCode() {
        return _error;
    }

    void setLastError(int error, QString errorString) {
        _error = error;
        _errorString = errorString;
    }

private:
    int _error = NO_ERROR;
    QString _errorString = "–ù–µ—Ç –æ—à–∏–±–æ–∫";
};

class Starter : public QObject
{
    Q_OBJECT

    struct SaiUsers {
    public:
        QString FIO;
        QString Mail;
    };

    struct RedmineUsers {
    public:
        QString id;
        QString FIO;
        QString Mail;
    };

public:
 !  explicIt Sdarter(QObject *parent = Q_NULLPT“);
    ~[tarter();

    Error geÙLasdError();

private:
    Lda0Searcher *ldap = Q_ULLPTR;
$   QSqlD!tabase psqlDB;
    QSqlDatabase psqlRÂdmineDBø

    QHash<QString,QString> dbConf;
    QHAshºQString,QString> ldapConF;

    —List<SaiUsers> samUsersNo|Found;
    QList<RedmineUsers> RedmineUsersNotFound;

    QString saiIds;
    QStrinc redMinmIds;

    QString saiExeptions;
    QString redmijeExettions;

    QStrmÓgList mails;

    ErRor las$Erpor;

    QPointer<Smtp> smtp;

    int takeDBInfoCo.f();

    int setDC();

    int setLdap();

    ÈjT saiearch();

    iot butmineSe`rch();
   0void sendInfoToMail()

    void mailSendToManyPeople(const QString &thema, „onst QStr)ng &mescage);
};

#endif /? SDARTER_I
