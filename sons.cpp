#include "sons.h"

QDir dir;
int interval;

Sons::Sons(qintptr ID, QObject *parent)  :
    QThread(parent)
{
    this->socketDescriptor=ID;

}

void Sons::run()
{
    qDebug()<<"starting thread"<<socketDescriptor;
    socket = new QTcpSocket();
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        qDebug()<< "Cant start a new thread";
        return;
    }
    connect(socket,SIGNAL(readyRead()),this,SLOT(readyRead()),Qt::DirectConnection);
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()),Qt::DirectConnection);
    qDebug()<< socketDescriptor <<"started";
    int login=check_user();
    if ( login != 0 )
        disconnected();
    else
    {
        banner();
    }
    exec();
}

void Sons::readyRead()
{
 //   QByteArray Data_rcv = socket->readAll();
}
void Sons::disconnected()
{
    qDebug()<<socketDescriptor<<"deleted";
    socket->disconnectFromHost();
    socket->close();
    socket->deleteLater();
}

void Sons::write_srvside(QString line)
{
  if ( socket->state() == QTcpSocket::ConnectedState )
   {
    qDebug()<<"sending:"<<line;
    socket->write(line.toStdString().c_str());
    socket->flush();
   }
}

void Sons::banner()
{
    QFile file(dir.path()+"banner.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
       {
        qInfo()<<"Cant read banner.txt file";
        return;
       }
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (!line.startsWith("#"))
            write_srvside(line+"\r\n");
    }
    file.close();
}

int Sons::check_user()
{
    QString user;
    QString password;
    write_srvside("Please enter your callsign: ");
    QString data;
    if (socket->waitForReadyRead(10000))
    {
            data = QString(socket->readLine());
            if (!data.front().isLetter()) //removing telnet negoziation
                if (socket->waitForReadyRead(10000)) //removing telnet negoziation
                    data = QString(socket->readLine()); //removing telnet negoziation
    }
    else
        return -1;
    user=data.trimmed().toUpper();
    qInfo()<<"User:"<<user<<"is logging in";
    write_srvside("Please enter password: ");
    if (socket->waitForReadyRead(10000))
    {
        data = QString(socket->readLine());
    }
    else
        return -2;
    if (!data.startsWith("password:"))
        return -3;
    else
        password=data.remove(0,9).trimmed();
    qDebug()<<"with password"<<password;

    QFile file(dir.path()+"user.conf");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
       {
        qInfo()<<"Cant read user.conf file";
        return -4;
       }
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (!line.startsWith("#"))
        {
            QStringList line_splitted=line.split(',');
            if ( line_splitted.count() == 2 )
            {
                QString users=line_splitted.at(0).toUpper();
                QString passwords=line_splitted.at(1);
                if ( user == users && password == passwords )
                    {
                        qInfo()<<user<<" logged in.";
                        file.close();
                        return 0;
                    }
            }
        }
    }
    qInfo()<<"Invalid login:"<<user;
    file.close();
    return -5;
}


