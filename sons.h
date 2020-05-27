#ifndef SONS_H
#define SONS_H
#include <QThread>
#include <QtNetwork/QTcpSocket>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QDate>
#include <QTimer>

extern QDir dir;

class Sons : public QThread
{
    Q_OBJECT
public:
    explicit Sons(qintptr ID, QObject *parent = nullptr);
    void run();
    QStringList station_name_list();
    bool stop;
signals:
 //   void error(QTcpSocket::SocketError socketerror);
    void stringfound(QString);
public slots:
    void readyRead();
    void disconnected();
    void write_srvside(QString);
    void read_lists();
private:
    QTcpSocket * socket;
    qintptr socketDescriptor;
};

#endif // SONS_H
