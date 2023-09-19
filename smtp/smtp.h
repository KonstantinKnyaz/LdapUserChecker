#ifndef _SMTP_H_
#define _SMTP_H_

#include "SimpleMail/SimpleMail.h"

class Smtp : public QObject
{
    Q_OBJECT

public:
    enum SmtpType
    {
        Async,
        Classic
    };
    Q_ENUM(SmtpType)

    Smtp(const QString& host, const QString& username, const QString& pass, const Smtp::SmtpType &type = Classic);
    ~Smtp();
    void SetHost(const QString &host, const QString &username, const QString &pass);
    void Send(const QString& sender, const QString& to,
              const QString& subject, const QString& body);

signals:
    void finished(bool error, const QString &response_text);

private:
    Server *server = nullptr;
    Sender smtp;

    SmtpType current_type;

    void sendAsync(const MimeMessage &message);
    void sendClassic(const MimeMessage &message);
};

#endif // _SMTP_H_
