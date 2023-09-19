#ifndef LDAPSEARCHER_H
#define LDAPSEARCHER_H

#include <QObject>
#include <QString>

#include <ldap.h>
#include <lber.h>

class LdapSearcher : public QObject
{
    Q_OBJECT
public:
    explicit LdapSearcher(QObject *parent = nullptr);
    ~LdapSearcher();

    int LDInit(QString url);

    int LDBind(QString dn, QString password);

    int LDSearch(QString searchStr, int &count);

    int LDClose();

signals:
    void searchFinish(QString searchResult);

private:
    LDAP *ld = Q_NULLPTR;
};

#endif // LDAPSEARCHERLITE_H
