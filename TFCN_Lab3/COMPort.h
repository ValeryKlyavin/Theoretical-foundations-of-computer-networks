#ifndef COMPORT_H
#define COMPORT_H
#include <QThread>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

struct COMPortParams
{
    QSerialPort::BaudRate speed;
    QSerialPort::Parity parity;
    QSerialPort::DataBits byteSize;
    QSerialPort::StopBits stopBitsAmount;
};

struct COMPortErrorInfo
{
    int errorCode;
    QString errorInfo;
};

class COMPort : public QThread
{
    Q_OBJECT
    QSerialPort* port;
    class MainWindow* window;
    bool _isWorking;
    bool finishFlag;
    COMPortErrorInfo errInfo;

public:
    inline bool isWorking() { return _isWorking; }
    inline MainWindow* getWindow() { return  window; }
    inline void setWindow(MainWindow* wnd) { window = wnd; }
    inline void setFinishFlag() { finishFlag = true; }
    inline void resetFinishFlag() { finishFlag = false; }
    inline COMPortErrorInfo& getErrorInfo() { return  errInfo; }

    COMPortParams* getPortParams();
    int initPort();
    void run() override;
    void setPortParams(COMPortParams* params);
    COMPort(QSerialPortInfo* portInfo);
    ~COMPort() override;
public slots:
    void read();
    void write();
    void collectErrorInfo();
signals:
    void COMPortError();
};

#endif // COMPORT_H
