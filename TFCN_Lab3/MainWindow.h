#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPortInfo>
#include "COMPort.h"
#include "InputTextEdit.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct COMPortSettings
{
    int speedIndex;
    int byteSizeIndex;
    int parityIndex;
    int stopBitsIndex;
};

class MainWindow : public QMainWindow
{
    friend class COMPort;
    Q_OBJECT

    Ui::MainWindow *ui;
    QList<QSerialPortInfo>* availablePorts;
    COMPort* com;
    COMPortParams* portParams;
    COMPortSettings settings;

    bool isConnected;
    long long previousTextLength;

    char source;
    char destination;

public:

    int getComPortsInformation();

    MainWindow(QString type ,QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_selectComPortButton_clicked();

    void on_inputEdit_textChanged();

    void on_byteSizeList_activated(int index);

    void on_stopBitsList_activated(int index);

    void on_parityList_activated(int index);

    void showCOMPortError();

    void on_speedList_activated(int index);

    void on_sourceAddressValue_valueChanged(int arg1);

    void on_destinationAddressValue_valueChanged(int arg1);

    void on_ErrorCheckBox_stateChanged(int arg1);

signals:
    void readyToCreatePacket();
    void speedChanged();
    void parityChanged();
    void stopBitsChanged();
    void byteSizeChanged();
};
#endif // MAINWINDOW_H
