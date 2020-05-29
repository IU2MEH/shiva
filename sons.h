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
#include <QMutex>

extern QDir dir;
extern int interval;

class Sons : public QThread
{
    Q_OBJECT
public:
    explicit Sons(qintptr ID, QObject *parent = nullptr);
    void run();
    bool stop;
signals:
 //   void error(QTcpSocket::SocketError socketerror);
    void stringfound(QString);
public slots:
    void readyRead();
    void disconnected();
    void read_lists();
    void write_srvside(QString);
private:
    int check_user();
    QStringList station_name_list();
    void banner();
    QTcpSocket * socket;
    qintptr socketDescriptor;
};

#endif // SONS_H
