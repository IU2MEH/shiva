#include <QCoreApplication>
#include "baselistener.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    BaseListener Server;
    Server.StartServer();
    return a.exec();
}
