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

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this, SLOT(read_lists()));
    timer->start(interval);
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
    thread = new Sons(socketDescriptor,this);
    thread->setParent(nullptr);
    thread->moveToThread(thread);
    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
    connect(this,SIGNAL(line_received(QString)),thread,SLOT(write_srvside(QString)));
    connect(this,SIGNAL(stringfound(QString)),thread,SLOT(write_srvside(QString)));
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





void  BaseListener::read_lists()
{
   QStringList station_list=station_name_list();
   dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks );
   QFileInfoList list = dir.entryInfoList();
   int j=0;
   for (int i = 0; i < list.size(); ++i)
    {
        if (list.at(i).fileName()=="LIST.list")
        {
            j++;
            QFile file(dir.path()+qPrintable(QString("%2").arg(list.at(i).fileName())));
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            {   qInfo()<<"Cant read file"<<dir.path()+qPrintable(QString("%2").arg(list.at(i).fileName()));
                return;
            }
            qDebug()<<"Reading file"<<dir.path()+qPrintable(QString("%2").arg(list.at(i).fileName()));
            QTextStream in(&file);
            int i=0;
            while (!in.atEnd())
            {
                QString line = in.readLine();
                QStringRef Freq(&line,0,5);
                QStringRef Start_date(&line,80,6);
                QStringRef End_date(&line,87,6);
                QStringRef Start_time(&line,6,4);
                QStringRef End_time(&line,11,4);
                QStringRef DayoftheWeek(&line,72,7);
                QStringRef Language(&line,102,10);
                QStringRef Power(&line,51,4);
                QStringRef StationName(&line,117,3);
                if (!Freq.toString().contains(";"))
                {
                    if (End_time.toString()=="2400")
                    {
                        QString endt= "2359";
                        End_time.clear();
                        End_time=QStringRef(&endt,0,4);
                        if (DayoftheWeek.contains(QString::number(QDate::currentDate().dayOfWeek()))
                             && QDate::fromString(Start_date.toString(),"ddMMyy").addYears(100)<=QDateTime::currentDateTimeUtc().date()
                              && QDateTime::currentDateTimeUtc().date()<=QDate::fromString(End_date.toString(),"ddMMyy").addYears(100)
                               && QDateTime::fromString(Start_time.toString(),"hhmm").time()<=QDateTime::currentDateTimeUtc().time()
                                && QDateTime::currentDateTimeUtc().time()<=QDateTime::fromString(End_time.toString(),"hhmm").time()
                                 )
                                for (int i = 0; i < station_list.size(); ++i)
                                {
                                    QStringRef StationShortName(&station_list.at(i),0,3);
                                    QStringRef StationLongName(&station_list.at(i),4,station_list.at(i).length()-4);
                                    if (StationName.toString()==StationShortName.toString())
                                      {
                                        emit stringfound("DX de SHIVA:     "+Freq.toString()+".0 "+StationLongName.toString().replace(" ","_")+" AM "+QDateTime::currentDateTimeUtc().time().toString("hhmm")+"Z\r\n");
                                      }
                                }
                     }
                    else if (DayoftheWeek.contains(QString::number(QDate::currentDate().dayOfWeek()))
                         && QDate::fromString(Start_date.toString(),"ddMMyy").addYears(100)<=QDateTime::currentDateTimeUtc().date()
                          && QDateTime::currentDateTimeUtc().date()<=QDate::fromString(End_date.toString(),"ddMMyy").addYears(100)
                           && QDateTime::fromString(Start_time.toString(),"hhmm").time()<=QDateTime::currentDateTimeUtc().time()
                            && QDateTime::currentDateTimeUtc().time()<=QDateTime::fromString(End_time.toString(),"hhmm").time()
                             )
                        for (int i = 0; i < station_list.size(); ++i)
                        {
                            QStringRef StationShortName(&station_list.at(i),0,3);
                            QStringRef StationLongName(&station_list.at(i),4,station_list.at(i).length()-4);
                            if (StationName.toString()==StationShortName.toString())
                                    {
                                      emit stringfound("DX de SHIVA:     "+Freq.toString()+".0 "+StationLongName.toString().replace(" ","_")+" AM "+QDateTime::currentDateTimeUtc().time().toString("hhmm")+"Z\r\n");
                                    }
                        }
                  /*  else
                        for (int i = 0; i < station_list.size(); ++i)
                        {
                            QStringRef StationShortName(&station_list.at(i),0,3);
                            QStringRef StationLongName(&station_list.at(i),4,station_list.at(i).length()-4);
                            if (StationName.toString()==StationShortName.toString())
                                qDebug()<<"Skipped:"<<Freq<<Start_date<<Start_time<<End_date<<End_time<<DayoftheWeek<<StationLongName<<Language.trimmed()<<Power.trimmed();
                        }*/
                }
                i++;
            }
            qDebug()<<i<<"lines read";
            file.close();
        }
        else if (list.at(i).suffix()=="list" && list.at(i).fileName()!="LIST.list")
        {
            j++;
            QFile file(dir.path()+qPrintable(QString("%2").arg(list.at(i).fileName())));
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            {   qDebug()<<"Cant read file"<<dir.path()+qPrintable(QString("%2").arg(list.at(i).fileName()));
                return;
            }
            qDebug()<<"Reading file"<<dir.path()+qPrintable(QString("%2").arg(list.at(i).fileName()));
            QTextStream in(&file);
            int i=0;
            while (!in.atEnd())
            {
                QString line = in.readLine();
                QStringList details = line.split(',');
        if ( details.count() == 8 )
                {
                    QString Start_date = details.at(0);
                    QString End_date = details.at(1);
                    QString DayoftheWeek = details.at(2);
                    QString Start_time = details.at(3);
                    QString End_time = details.at(4);
                    float Freq = details.at(5).toInt();
                    Freq=Freq/1000;
                    QString Mod = details.at(6);
                    QString StationName = details.at(7);
                    if ( End_date.endsWith("****") ) //yearly events
                    {
                        End_date.replace(4,4,QDateTime::currentDateTimeUtc().toString("yyyy"));
                    }
                    if (DayoftheWeek.contains(QString::number(QDate::currentDate().dayOfWeek()))
                                         && QDate::fromString(Start_date,"ddMMyyyy")<=QDateTime::currentDateTimeUtc().date()
                                          && QDateTime::currentDateTimeUtc().date()<=QDate::fromString(End_date,"ddMMyyyy")
                                           && QDateTime::fromString(Start_time,"hhmm").time()<=QDateTime::currentDateTimeUtc().time()
                                            && QDateTime::currentDateTimeUtc().time()<=QDateTime::fromString(End_time,"hhmm").time()
                                             )
                    {
                        emit stringfound("DX de SHIVA:     "+QString::number(Freq,'f',1)+" "+StationName.replace(" ","_")+" "+Mod+" "+QDateTime::currentDateTimeUtc().time().toString("hhmm")+"Z\r\n");
                    }
                }
                i++;
            }
            qDebug()<<i<<"lines read";
            file.close();
        }
    }
   if (j==0)
       qInfo()<<"No List files Found";
}


QStringList BaseListener::station_name_list()
{
    QFile file(dir.path()+"broadcas.txt");
    QStringList station_list;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
       {
        qInfo()<<"Cant read broadcas.txt file";
        return {"error","999"};
       }
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        station_list.append(line);
    }
    file.close();
    return station_list;
}
