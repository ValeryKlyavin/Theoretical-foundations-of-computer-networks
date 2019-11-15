#include "WindowThread.h"
#include "MainWindow.h"

WindowThread::WindowThread(MainWindow* wnd)
    :window(wnd)
{
    connect(this, SIGNAL(started()), window, SLOT(show()));
    connect(window, SIGNAL(closed(int)), this, SLOT(windowClosed(int)));
    connect(window, SIGNAL(addTokenButtonClicked(int)), this, SLOT(inforControllerAboutToken(int)));

}

void WindowThread::windowClosed(int index)
{
    this->exit(0);
}

void WindowThread::inforControllerAboutToken(int id)
{
    emit tokenAdded(id);
}
