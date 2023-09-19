#include "LdapSearcher.h"
#include <iostream>
#include <QDebug>
#include <QTextCodec>

LdapSearcher::LdapSearcher(QObject *parent) : QObject(parent)
{
    qInfo() << "Класс для работы с LDAP загружен";
}

LdapSearcher::~LdapSearcher()
{
    if(ld != Q_NULLPTR) {
        LDClose();
        ldap_memfree(ld);
        ld = Q_NULLPTR;
    }
}

int LdapSearcher::LDInit(QString url)
{
    if(ld != Q_NULLPTR) {
        ldap_unbind_ext_s(ld,nullptr,nullptr);
        ldap_memfree(ld);
        ld = Q_NULLPTR;
    }

    int result = LDAP_SUCCESS;
    result = ldap_create(&ld);

    if(result != LDAP_SUCCESS) {
        qCritical() << "ldap_create() error:" << ldap_err2string(result);
        return result;
    }

    std::string strUrl = url.toStdString();
    const char *aurl = strUrl.c_str();

    if(!url.isEmpty()) {
        result = ldap_set_option(ld, LDAP_OPT_URI, aurl);
        if(result != LDAP_SUCCESS) {
            qCritical() << "ldap_set_optino() url error:" << ldap_err2string(result);
            return result;
        }
    }

    const int version = LDAP_VERSION3;

    result = ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &version);

    if(result != LDAP_SUCCESS) {
        qCritical() << "ldap_set_optino() ver error:" << ldap_err2string(result);
        return result;
    }

    const int reff = 0;

    result = ldap_set_option(ld, LDAP_OPT_REFERRALS, &reff);

    if(result != LDAP_SUCCESS) {
        qCritical() << "ldap_set_optino() ref error:" << ldap_err2string(result);
    }

    return result;
}

int LdapSearcher::LDBind(QString dn, QString password)
{
    const char *adn;
    adn = (char*)dn.toStdString().c_str();

    berval *creds = new berval();

    creds->bv_val = (char*)password.toStdString().c_str();
    creds->bv_len = password.length();

    int result = ldap_sasl_bind_s(ld, adn, LDAP_SASL_SIMPLE, creds, nullptr, nullptr, nullptr);

    if(result != LDAP_SUCCESS) {
        qCritical() << "ldap_sasl_bind_s() error:" << ldap_err2string(result);
        ldap_memfree(creds);
    }

    return result;
}

int LdapSearcher::LDSearch(QString searchStr, int &count)
{
    int result = LDAP_SUCCESS;
    QString codePage = "utf-8";
    QTextCodec *codec = QTextCodec::codecForName(codePage.toUtf8());

    QString finishResult;

    LDAPMessage *searchResultMsg;

    QString find = searchStr;
    std::string filtStr = find.toStdString();
    char *findC = (char*)filtStr.c_str();

    result = ldap_search_ext_s(ld, nullptr, LDAP_SCOPE_SUBTREE, findC, nullptr, 0, nullptr, nullptr, nullptr, LDAP_NO_LIMIT, &searchResultMsg);

    if(result != LDAP_SUCCESS) {
        qCritical() << "ldap_search_ext_s() error:" << ldap_err2string(result);
        ldap_memfree(searchResultMsg);
        return result;
    }

    int value = 0;
    value = ldap_count_entries(ld, searchResultMsg);

    count = value;

    //    qInfo() << "Search results count =" << value;

    LDAPMessage *msg;

    msg = ldap_first_message(ld, searchResultMsg);

    while(msg != NULL) {
        int msgType = ldap_msgtype(msg);

        if(msgType == LDAP_RES_SEARCH_ENTRY) {
            BerElement *ber;

            struct berval **values;
            struct berval value;

            char *a;

            a = ldap_first_attribute(ld, msg, &ber);

            while(a != NULL) {

                values = ldap_get_values_len(ld, msg, a);

                if(values == NULL) {
                    return 1; // временно
                }

                for(int i = 0; values[i] != NULL; i++) {
                    value = *values[i];
                    QByteArray buf = value.bv_val;
                    QString text = codec->toUnicode(buf);
                    //                    qInfo() << a << ":" << text;
                    finishResult.append(a);
                    finishResult.append(": ");
                    finishResult.append(text);
                    finishResult.append("\n");
                }

                a = ldap_next_attribute(ld, msg, ber);
            }
            finishResult.append("\n");
            ldap_memfree(a);
            ldap_memfree(values);
        }
        msg = ldap_next_message(ld, msg);
    }

    ldap_memfree(msg);
    ldap_memfree(searchResultMsg);

    //emit searchFinish(finishResult);

    return LDAP_SUCCESS;
}

int LdapSearcher::LDClose()
{
    if(ld == Q_NULLPTR)
        return 0;
    int result = ldap_unbind_ext_s(ld, nullptr, nullptr);

    if(result != LDAP_SUCCESS) {
        qCritical() << "ldap_unbind_ext_s() error:" << ldap_err2string(result);
    }
    ld = Q_NULLPTR;
    return result;
}
