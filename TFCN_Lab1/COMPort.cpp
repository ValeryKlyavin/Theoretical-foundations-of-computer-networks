#include "MainWindow.h"
#include "ui_MainWindow.h"
COMPortParams *COMPort::getPortParams()
{
    if(port)
    {
        COMPortParams* params = new COMPortParams();
        params->speed = (QSerialPort::BaudRate)port->baudRate();
        params->parity = (QSerialPort::Parity)port->parity();
        params->byteSize = (QSerialPort::DataBits)port->dataBits();
        params->stopBitsAmount = (QSerialPort::StopBits)port->stopBits();
        return params;
    }
    else
    {
        return nullptr;
    }
}

int COMPort::initPort()
{
    if(port)
    {
        if(!port->isOpen())
        {
            if(port->open(QIODevice::ReadWrite))
                return  0;
            else
                return 3;
        }
        else
        {
            return 1;
        }
    }
    return 2;
}

void COMPort::run()
{
    _isWorking = true;
    connect(port, &QSerialPort::readyRead, this, &COMPort::read);
    connect(window, &MainWindow::inputTextChanged, this, &COMPort::write);
    //connect(port, &QSerialPort::errorOccurred, this, &COMPort::collectErrorInfo);
    connect(port, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error), this, &COMPort::collectErrorInfo);
    while (!finishFlag)
    {
        usleep(1);
    }
    _isWorking = false;
    disconnect(port, &QSerialPort::readyRead, this, &COMPort::read);
    disconnect(window, &MainWindow::inputTextChanged, this, &COMPort::write);
    //disconnect(port, &QSerialPort::errorOccurred, this, &COMPort::collectErrorInfo);
    disconnect(port, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error), this, &COMPort::collectErrorInfo);
}

void COMPort::setPortParams(COMPortParams *params)
{
    if(port)
    {
        port->setParity(params->parity);
        port->setBaudRate(params->speed);
        port->setStopBits(params->stopBitsAmount);
        port->setDataBits(params->byteSize);
    }
}

COMPort::COMPort(QSerialPortInfo* portInfo)
    : port(nullptr),
      _isWorking(false),
      finishFlag(false)
{
    port = new QSerialPort(*portInfo);

}

COMPort::~COMPort()
{
    if(port)
    {
        if(port->isOpen())
        {
            port->clear();
            port->close();
        }
        port->deleteLater();

    }
}

void COMPort::read()
{
    auto arr = port->readAll();
    QString str(arr);
    window->ui->outputEdit->setText(window->ui->outputEdit->toPlainText() + str.toUtf8());
}

void COMPort::write()
{
    auto text = window->ui->inputEdit->toPlainText();
    auto length = text.length();
    QByteArray arr;
    QString str(text[length - 1]);
    arr.append(str.toUtf8());
    port->write(arr);
}

void COMPort::collectErrorInfo()
{
    errInfo.errorCode = (int)(port->error());
    errInfo.errorInfo = port->errorString();
    emit COMPortError();
    port->clearError();
}
