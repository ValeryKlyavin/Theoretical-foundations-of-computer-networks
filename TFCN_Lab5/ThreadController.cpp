#include "ThreadController.h"

ThreadController::ThreadController(int numbers)
    :id(0)
{
    MainWindow* wnd;
    WindowThread* thr;
    for (int i = 0; i < numbers; i++)
    {
        wnd = new MainWindow(id++);
        thr = new WindowThread(wnd);
        threads.push_back(thr);
        windows.push_back(wnd);
        thr->start();
        connect(thr, SIGNAL(tokenAdded(int)), this, SLOT(disableAddTokenButton(int)));
        connect(this, SIGNAL(tokenAdded()), wnd, SLOT(disableAddTokenButton()));
    }
    for (auto i = 0, j = 1; i < windows.size(); i++)
    {
        connect(windows[i], SIGNAL(transmit(Packet)), windows[j], SLOT(receive(Packet)));
        j++;
        if (j == windows.size())
            j = 0;
    }
}

ThreadController::~ThreadController()
{

}

void ThreadController::disableAddTokenButton(int id)
{
    emit tokenAdded();
}

bool ThreadController::allThreadsFinished()
{
    bool finished = true;
    for (auto& i : threads)
        finished &= i->isFinished();
    return finished;
}

void ThreadController::prepareToDelete()
{
    for(auto& i : threads)
    {
        i->exit(0);
        i->deleteLater();
    }
    threads.clear();
    for(auto& i : windows)
    {
        i->hide();
        i->deleteLater();
    }
    windows.clear();
}
