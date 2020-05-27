#include "sons.h"

QDir dir;

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
    connect(this,SIGNAL(stringfound(QString)),this,SLOT(write_srvside(QString)),Qt::DirectConnection);
    qDebug()<< socketDescriptor <<"started";
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this, SLOT(read_lists()));
    timer->start(15000);
    exec();
}

void Sons::readyRead()
{

}
void Sons::disconnected()
{
    qDebug()<<socketDescriptor<<"deleted";
    socket->deleteLater();
    exit(0);
}

void Sons::write_srvside(QString line)
{
 // qDebug()<<line;
  if (socket != nullptr)
   {
    socket->write(line.toStdString().c_str());
    socket->flush(); }
}



void  Sons::read_lists()
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
            {   qDebug()<<"Cant read file"<<dir.path()+qPrintable(QString("%2").arg(list.at(i).fileName()));
                return;
            }
            qDebug()<<"reading file"<<dir.path()+qPrintable(QString("%2").arg(list.at(i).fileName()));
            QTextStream in(&file);
            int i=0;
            while (!in.atEnd())
            {
                i++;

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
                             && QDateTime::fromString(Start_date.toString(),"ddMMyy").addYears(100)<=QDateTime::currentDateTimeUtc()
                              && QDateTime::currentDateTimeUtc()<=QDateTime::fromString(End_date.toString(),"ddMMyy").addYears(100)
                               && QDateTime::fromString(Start_time.toString(),"hhmm").time()<=QDateTime::currentDateTimeUtc().time()
                                && QDateTime::currentDateTimeUtc().time()<=QDateTime::fromString(End_time.toString(),"hhmm").time()
                                 )
                                for (int i = 0; i < station_list.size(); ++i)
                                {
                                    QStringRef StationShortName(&station_list.at(i),0,3);
                                    QStringRef StationLongName(&station_list.at(i),4,station_list.at(i).length()-4);
                                    if (StationName.toString()==StationShortName.toString())
                                      {
                                        emit stringfound("DX de SHIVA:     "+Freq.toString()+".0 "+StationLongName.toString().replace(" ","_")+" AM \r\n");
                                      }
                                }
                     }
                    else if (DayoftheWeek.contains(QString::number(QDate::currentDate().dayOfWeek()))
                         && QDateTime::fromString(Start_date.toString(),"ddMMyy").addYears(100)<=QDateTime::currentDateTimeUtc()
                          && QDateTime::currentDateTimeUtc()<=QDateTime::fromString(End_date.toString(),"ddMMyy").addYears(100)
                           && QDateTime::fromString(Start_time.toString(),"hhmm").time()<=QDateTime::currentDateTimeUtc().time()
                            && QDateTime::currentDateTimeUtc().time()<=QDateTime::fromString(End_time.toString(),"hhmm").time()
                             )
                        for (int i = 0; i < station_list.size(); ++i)
                        {
                            QStringRef StationShortName(&station_list.at(i),0,3);
                            QStringRef StationLongName(&station_list.at(i),4,station_list.at(i).length()-4);
                            if (StationName.toString()==StationShortName.toString())
                                    {
                                      emit stringfound("DX de SHIVA:     "+Freq.toString()+".0 "+StationLongName.toString().replace(" ","_")+" AM \r\n");
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
            // do something
            qDebug()<<i<<"lines read";
            file.close();
        }
    }
   if (j==0)
       qDebug()<<"No List Found";
}

QStringList Sons::station_name_list()
{
    QFile file(dir.path()+"broadcas.txt");
    QStringList station_list;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
       {
        qDebug()<<"Cant read broadcas.txt file";
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

