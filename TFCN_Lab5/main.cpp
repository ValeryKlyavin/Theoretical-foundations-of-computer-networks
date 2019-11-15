#include <QApplication>
#include "ThreadController.h"

#define WINDOWS_AMOUNT 3

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ThreadController threads(WINDOWS_AMOUNT);
    app.exec();
    while (!threads.allThreadsFinished())
        QThread::usleep(100);

    threads.prepareToDelete();
    return 0;
}

