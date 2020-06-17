#ifndef BASELISTENER_H
#define BASELISTENER_H
#include <QtNetwork/QTcpServer>
#include <QDebug>
#include <QFile>
#include <QThread>
#include "sons.h"

extern QString line;
extern QTcpSocket *socket_c;
extern int timeout;
extern QString callsign;
extern QString dxcluster;
extern unsigned short dxclusterport;
extern unsigned short listenport;

class BaseListener : public QTcpServer
{
    Q_OBJECT
public:
    explicit BaseListener(QObject *parent = nullptr);
    void StartServer();
    Sons *thread;
signals:
    void line_received(QString);
    void stringfound(QString);
public slots:
    void connected();
    void disconnected();
    void readyRead();
    void reconnect();
    void nodata_disconn();
    void read_lists();
protected:
    void incomingConnection(qintptr socketDescriptor) override;
private:
    int load_config();
    QStringList station_name_list();

};

#endif // BASELISTENER_H
