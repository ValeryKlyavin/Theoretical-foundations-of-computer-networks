#ifndef THREADCONTROLLER_H
#define THREADCONTROLLER_H
#include <vector>
#include "MainWindow.h"
#include "WindowThread.h"

class ThreadController : public QObject
{
    Q_OBJECT
    std::vector<MainWindow*> windows;
    std::vector<WindowThread*> threads;
    int id;
public:

    bool allThreadsFinished();
    void prepareToDelete();

    ThreadController(int number);
    ~ThreadController();
public slots:
    void disableAddTokenButton(int id);

signals:
    void tokenAdded();
};

#endif // THREADCONTROLLER_H
