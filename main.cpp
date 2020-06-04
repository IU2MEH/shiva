#include <QCoreApplication>
#include "baselistener.h"
#include <QDebug>
#include <QtGlobal>

void noDebugOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QString current_time=QDateTime::currentDateTime().toString("ddMMyyyy-hh:mm:ss");
    switch (type) {
    case QtDebugMsg:
//        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
//        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        fprintf(stderr,"%s Info: %s \n",current_time.toStdString().c_str(),localMsg.constData());
        break;
    case QtWarningMsg:
//        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "%s Critical: %s (%s:%u, %s)\n",current_time.toStdString().c_str(), localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "%s Fatal: %s (%s:%u, %s)\n",current_time.toStdString().c_str(), localMsg.constData(), context.file, context.line, context.function);
        abort();
    }

}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    if ( ! app.arguments().contains(QLatin1String("--with-debug"))) {
        qInstallMessageHandler(noDebugOutput);
    }
    BaseListener Server;
    Server.StartServer();
    return app.exec();
}


