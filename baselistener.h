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
signals:
    void line_received(QString);

public slots:
    void connected();
    void disconnected();
    void readyRead();
    void reconnect();
    void nodata_disconn();
protected:
    void incomingConnection(qintptr socketDescriptor) override;
private:
    int load_config();

};

#endif // BASELISTENER_H
