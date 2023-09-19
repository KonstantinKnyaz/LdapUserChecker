#include "smtp.h"

/// конструктор
Smtp::Smtp(const QString &host, const QString &username, const QString &pass, const SmtpType &type)
    : current_type(type)
{
    qDebug() << "### Smtp start";

    switch (current_type) {
    case Classic:
        smtp.setHost(host);
        smtp.setConnectionType(Sender::TcpConnection);
        smtp.setUser(username);
        smtp.setPassword(pass);
        break;

    case Async:
        server = new Server();
        server->setHost(host);
        server->setConnectionType(Server::TcpConnection);
        server->setUsername(username);
        server->setPassword(pass);
        break;
    }
}


/// Устанавливает хост
void Smtp::SetHost(const QString& host, const QString& username, const QString& pass)
{
    switch (current_type) {
    case Classic:
        smtp.setHost(host);
        smtp.setUser(username);
        smtp.setPassword(pass);
        break;

    case Async:
        server->setHost(host);
        server->setUsername(username);
        server->setPassword(pass);
        break;
    }
}


/// Отправка сообщения
void Smtp::Send(const QString& sender, const QString& to,
                const QString& subject, const QString& body)
{
    qDebug() << "### Send from:  " + sender;
    qDebug() << "### To:  " + to;
    qDebug() << "### Subject: " + subject;
    qDebug() << "### Body: " + body;

    MimeMessage message;

    message.setSender(EmailAddress(sender));
    message.addTo(EmailAddress(to));
    message.setSubject(subject);

    MimeHtml *html = new MimeHtml;
    html->setHtml(body);

    message.addPart(html);

    switch (current_type) {
    case Classic:
        sendClassic(message);
        break;

    case Async:
        sendAsync(message);
        break;
    }
}


/// Деструктор
Smtp::~Smtp()
{
    qDebug() << "### Smtp stop and delete";

    if (server)
        server->deleteLater();
}


/// Асинхронно отправляет сообщения
void Smtp::sendAsync(const MimeMessage &message)
{
    ServerReply *reply = server->sendMail(message);
    QObject::connect(reply, &ServerReply::finished, [reply, this] {
       qDebug() << "### ServerReply finished!";

       if (reply->error())
           qDebug() << "### Failed to Send mail!" << reply->responseText();

       emit finished(reply->error(), reply->responseText());
       reply->deleteLater();
    });
}


/// Последовательно отправляет сообщения
void Smtp::sendClassic(const MimeMessage &message)
{
    Sender smtp_sender(smtp.host(), smtp.port(), smtp.connectionType());
    smtp_sender.setUser(smtp.user());
    smtp_sender.setPassword(smtp.password());

    if (!smtp_sender.sendMail(message)) {
        qDebug() << "### Failed to send mail!" << smtp_sender.lastError();
        emit finished(true, smtp_sender.lastError());
    } else {
        qDebug() << "### Sender finished!" << smtp_sender.responseText();
        emit finished(false, smtp_sender.responseText());
    }

    smtp_sender.quit();
}
