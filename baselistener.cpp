#include "baselistener.h"

QString callsign;
QString dxcluster;
unsigned short dxclusterport;
int timeout;
unsigned short listenport;

QTcpSocket *socket_c;
QString line;

BaseListener::BaseListener(QObject *parent) :
    QTcpServer(parent)
{
    int i=load_config();
    if (i<6)
      {
        qDebug()<<"Check the config file";
        exit (-1);
      }
    socket_c = new QTcpSocket (this);
    connect(socket_c,SIGNAL(connected()),this,SLOT(connected()));
    connect(socket_c,SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(socket_c,SIGNAL(readyRead()),this,SLOT(readyRead()));
}

void BaseListener::connected()
{
    qDebug()<<"Connected to:"<<dxcluster<<":"<<dxclusterport;
    socket_c->write((callsign+"\r\n").toStdString().c_str());
}
void BaseListener::disconnected()
{
    qDebug()<<"Disconnected from dxcluster";
}

void BaseListener::readyRead()
{
    line=socket_c->readAll();
    emit line_received(line);
}

void BaseListener::StartServer()
{
    if(!this->listen(QHostAddress::Any,listenport))
    {
        qDebug()<<"Cant start server on port"<<listenport;
    }
    socket_c->connectToHost(dxcluster,dxclusterport);
    if (!socket_c->waitForConnected(timeout))
    {
        qDebug()<<"ERROR: "<<socket_c->errorString();
    }
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
    QFile file("/etc/shiva/shiva.conf");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {   qDebug()<<"Cant read config file"<<file.fileName();
        return -1;
    }
    qDebug()<<"Reading config file"<<file.fileName();
    QTextStream in(&file);
    int i=0;
    while (!in.atEnd())
    {
        QString line = in.readLine();
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
    }
    return i;
}





