#ifndef WINDOWTHREAD_H
#define WINDOWTHREAD_H

#include <QThread>


class WindowThread : public QThread
{
    Q_OBJECT
    class MainWindow* window;
public:
    WindowThread(class MainWindow* wnd);

private slots:
    void windowClosed(int index);
    void inforControllerAboutToken(int id);

signals:
    void tokenAdded(int id);

};

#endif // WINDOWTHREAD_H
