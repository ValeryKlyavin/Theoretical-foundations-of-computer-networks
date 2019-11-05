#include "MainWindow.h"

#include <QApplication>
#include <QProcess>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow* w;
    QProcess* proc = nullptr;
    if(argc == 1)
    {
        w = new MainWindow("Server");
        if(w->getComPortsInformation())
        {
            proc = new QProcess(w);
            QStringList args("Client");
            proc->start(argv[0], args);
        }
        else
        {
            QMessageBox::warning(nullptr, "Error", "No COM ports available");
            return -1;
        }
    }
    else
    {
        w = new MainWindow("Client");
        w->getComPortsInformation();
    }

    w->show();
    app.exec();
    if(proc)
    {
        proc->kill();
        proc->waitForFinished();
        proc->deleteLater();
    }
    w->close();
    delete w;
    return 0;
}
