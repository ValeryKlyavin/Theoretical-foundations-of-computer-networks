#include "MainWindow.h"

#include <QApplication>
#include <QProcess>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow* w;

    w = new MainWindow("Lab3");
    if(!w->getComPortsInformation())
    {
        QMessageBox::warning(nullptr, "Error", "No COM ports available");
        return -1;
    }
    w->show();
    app.exec();
    w->close();
    delete w;
    return 0;
}
