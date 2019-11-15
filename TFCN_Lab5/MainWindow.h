#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Packet.h"
#include <QTimer>
#include <deque>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Ui::MainWindow *ui;
    int id;
    char sourceAddress;
    char destinationAddress;
    bool earlyTokenRelease;
    unsigned int currentPosition;
    std::deque<Packet> buffer;
    bool hasToken;
    QTimer timer;
    QString BaseTitle;
public:

    inline int getId() { return  id; }

    explicit MainWindow(int index, QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void show();
    void disableAddTokenButton();
    void receive(Packet p);

signals:
    void closed(int id);
    void addTokenButtonClicked(int id);
    void transmit(Packet p);

private slots:
    void on_sourceSpinBox_valueChanged(int arg1);
    void on_destinationSpinBox_valueChanged(int arg1);
    void on_pushButton_clicked();
    void on_earlyTokenReleaseCheckBox_stateChanged(int arg1);
    void closeEvent(QCloseEvent* event);
    void transmitToken();
    void transmitFrame();
    void transmitPacket();
};

#endif // MAINWINDOW_H
