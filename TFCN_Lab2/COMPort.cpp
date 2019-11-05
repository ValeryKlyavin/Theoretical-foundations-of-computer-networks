#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "PacketManager.h"
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
    connect(window, &MainWindow::readyToCreatePacket, this, &COMPort::write);
    //connect(window, &MainWindow::charRemoved, this, );
    connect(port, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error), this, &COMPort::collectErrorInfo);
    while (!finishFlag)
    {
        usleep(1);
    }
    _isWorking = false;
    disconnect(port, &QSerialPort::readyRead, this, &COMPort::read);
    disconnect(window, &MainWindow::readyToCreatePacket, this, &COMPort::write);
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
    window->ui->debugEdit->append("Received:");
    window->ui->debugEdit->append(PacketManager::toString(arr));
    window->ui->debugEdit->append("Hex:");
    window->ui->debugEdit->append(PacketManager::toHexString(arr));
    window->ui->debugEdit->append("After debitstuffing:");
    auto unstuffed = PacketManager::processPacketWithDebitstuffing(arr);
    window->ui->debugEdit->append(PacketManager::toString(unstuffed));
    window->ui->debugEdit->append("Hex:");
    window->ui->debugEdit->append(PacketManager::toHexString(unstuffed));
    unstuffed.remove(0, 1);

    if ((char)unstuffed[0] == (char)(window->ui->sourceAddressValue->value()))
    {
        unstuffed.remove(0, 2);
        QChar data[7];
        for(int i = 0; i < 7; i++)
            data[i] = QChar(unstuffed[i]);
        unstuffed.remove(0,7);
        if ((char)(unstuffed[0]) == 0)
        {
            for(int i = 0; i < 7; i++)
            {
                window->ui->outputEdit->setText(window->ui->outputEdit->toPlainText() + data[i]);
            }
        }
        else
        {
            window->ui->debugEdit->append("This packet contains an error.");
        }
    }
    else
    {
        window->ui->debugEdit->append("This packet is addressed to another station.");
    }
}

void COMPort::write()
{
    auto text = window->ui->inputEdit->toPlainText();
    auto length = text.length();
    QChar packetContent[7];
    for (auto i = length - 7, index = 0; i < length; i++, index++)
    {
        packetContent[index] = text[i];
    }
    auto packet = PacketManager::createPacket(window->ui->sourceAddressValue->value(),
                                window->ui->destinationAddressValue->value(),
                                packetContent);
    window->ui->debugEdit->append("Sending data:");
    window->ui->debugEdit->append(PacketManager::toString(packet));
    window->ui->debugEdit->append("Hex:");
    window->ui->debugEdit->append(PacketManager::toHexString(packet));
    window->ui->debugEdit->append("After bitstuffing:");
    auto bitstuffedPacket = PacketManager::processPacketWithBitstuffing(packet);
    window->ui->debugEdit->append(PacketManager::toString(bitstuffedPacket));
    window->ui->debugEdit->append("Hex:");
    window->ui->debugEdit->append(PacketManager::toHexString(bitstuffedPacket));
    port->write(bitstuffedPacket);
}

void COMPort::collectErrorInfo()
{
    errInfo.errorCode = (int)(port->error());
    errInfo.errorInfo = port->errorString();
    emit COMPortError();
    port->clearError();
}
