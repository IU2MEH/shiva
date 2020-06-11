#include "baselistener.h"

QString callsign;
QString dxcluster;
unsigned short dxclusterport;
int timeout;
unsigned short listenport;
QTcpSocket *socket_c;
QString line;
QTimer *nodata_timer;


BaseListener::BaseListener(QObject *parent) :
    QTcpServer(parent)
{
    int i=load_config();
    if (i<7)
      {
        qInfo()<<"Check the config file";
        exit (-1);
      }
    socket_c = new QTcpSocket (this);
    connect(socket_c,SIGNAL(connected()),this,SLOT(connected()));
    connect(socket_c,SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(socket_c,SIGNAL(readyRead()),this,SLOT(readyRead()));
    nodata_timer = new QTimer(this);
    nodata_timer->setSingleShot(true);
    connect(nodata_timer, SIGNAL(timeout()),this, SLOT(nodata_disconn()));
}

void BaseListener::connected()
{
    QString data;
    qInfo()<<"Connected to:"<<dxcluster<<":"<<dxclusterport;
    nodata_timer->start(600000);
    socket_c->write((callsign+"\r\n").toStdString().c_str());
    qDebug()<<"logged in as:"<<callsign;
}

void BaseListener::disconnected()
{
    qInfo()<<"Disconnected from dxcluster";
    qInfo()<<"Reconnecting in 3 minutes...";
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this, SLOT(reconnect()));
    timer->setSingleShot(true);
    timer->start(180000);
}

void BaseListener::StartServer()
{
    if(!this->listen(QHostAddress::Any,listenport))
    {
        qInfo()<<"Cant start server on port"<<listenport;
    }
    socket_c->connectToHost(dxcluster,dxclusterport);
    if (!socket_c->waitForConnected(timeout))
    {
        qDebug()<<"ERROR: "<<socket_c->errorString();
        qInfo()<<"Reconnecting in 3 minutes...";
        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()),this, SLOT(reconnect()));
        timer->setSingleShot(true);
        timer->start(180000);
    }
    return;
}

void BaseListener::reconnect()
{
	socket_c->connectToHost(dxcluster,dxclusterport);
	return;
}

void BaseListener::nodata_disconn()
{
    socket_c->close();
}

void BaseListener::readyRead()
{
    line=socket_c->readAll();
    qDebug()<<"received:"<<line.trimmed();
    nodata_timer->start(600000);
    emit line_received(line);
}

void BaseListener::incomingConnection(qintptr socketDescriptor)
{
    Sons *thread = new Sons(socketDescriptor,this);
    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
    connect(this,SIGNAL(line_received(QString)),thread,SLOT(write_srvside(QString)));

    thread->start();
}

int BaseListener::load_config()
{
    QFile file("//etc//shiva//shiva.conf");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {   qInfo()<<"Cant read config file"<<file.fileName();
        return -1;
    }
    qDebug()<<"Reading config file"<<file.fileName();
    QTextStream in(&file);
    int i=0;
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (!line.startsWith("#"))
        {
            if (line.startsWith("timeout="))
               { timeout=line.remove(0,QString("timeout=").length()).toInt(); i++; }
            if (line.startsWith("callsign="))
               { callsign=line.remove(0,QString("callsign=").length()); i++; }
            if (line.startsWith("dxcluster="))
               { dxcluster=line.remove(0,QString("dxcluster=").length()); i++; }
            if (line.startsWith("dxclusterport="))
               { dxclusterport=line.remove(0,QString("dxclusterport=").length()).toUShort(); i++; }
            if (line.startsWith("listenport="))
               { listenport=line.remove(0,QString("listenport=").length()).toUShort(); i++; }
            if (line.startsWith("workingdir="))
               { dir.setPath(line.remove(0,QString("workingdir=").length())); i++; }
            if (line.startsWith("interval="))
               { interval=line.remove(0,QString("interval=").length()).toInt(); i++; }
        }
    }
    return i;
}
