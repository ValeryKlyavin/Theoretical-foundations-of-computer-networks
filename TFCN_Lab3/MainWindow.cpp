#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include "PacketManager.h"
#ifdef unix
#include <unistd.h>
#endif


int MainWindow::getComPortsInformation()
{
    int size;
    availablePorts = new QList<QSerialPortInfo>(QSerialPortInfo::availablePorts());
    size = availablePorts->size();
    if(size > 0)
    {
        for(auto i : *availablePorts)
            ui->availableComPortsList->addItem(i.portName());

        return size;
    }
    delete availablePorts;
    availablePorts = nullptr;
    return 0;
}

MainWindow::MainWindow(QString type, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      com(nullptr),
      isConnected(false),
      previousTextLength(0),
      source(0),
      destination(1)
{
    ui->setupUi(this);
    setWindowTitle(type);
    ui->inputEdit->setUndoRedoEnabled(false);
    for(auto i : QSerialPortInfo::standardBaudRates())
        ui->speedList->addItem(QString::number(i) + " bps", i);


    //adding byte size;
    //ui->byteSizeList->addItem("Unknown", QSerialPort::UnknownDataBits);
    ui->byteSizeList->addItem("5", QSerialPort::Data5);
    ui->byteSizeList->addItem("6", QSerialPort::Data6);
    ui->byteSizeList->addItem("7", QSerialPort::Data7);
    ui->byteSizeList->addItem("8", QSerialPort::Data8);


    //adding parity
    //ui->parityList->addItem("Unknown parity", QSerialPort::UnknownParity);
    ui->parityList->addItem("Even parity", QSerialPort::EvenParity);
    ui->parityList->addItem("Mark parity", QSerialPort::MarkParity);
    ui->parityList->addItem("No parity", QSerialPort::NoParity);
    ui->parityList->addItem("Space parity", QSerialPort::SpaceParity);
    ui->parityList->addItem("Odd parity", QSerialPort::OddParity);

    //adding stop bits
    //ui->stopBitsList->addItem("Unknown stop bits", QSerialPort::StopBits::UnknownStopBits);
    ui->stopBitsList->addItem("1 stop bit", QSerialPort::StopBits::OneStop);
    ui->stopBitsList->addItem("1.5 stop bits", QSerialPort::StopBits::OneAndHalfStop);
    ui->stopBitsList->addItem("2 stop bits", QSerialPort::StopBits::TwoStop);

}

MainWindow::~MainWindow()
{
    delete ui;
    if(com)
    {
        if(com->isWorking())
        {
            com->setFinishFlag();
            com->quit();
        }
        com->deleteLater();
    }
}

void MainWindow::on_selectComPortButton_clicked()
{
    if(!isConnected)
    {
        for(auto i : QSerialPortInfo::availablePorts())
        {
            if(i.portName() == ui->availableComPortsList->currentText())
            {
                com = new COMPort(&i);
                break;
            }
        }
        if(com)
        {
            com->setWindow(this);
            int initResult = com->initPort();
            switch (initResult)
            {
            case 0:
            {
                portParams = com->getPortParams();
                ui->speedList->setCurrentIndex(settings.speedIndex = ui->speedList->findData(portParams->speed));
                ui->parityList->setCurrentIndex(settings.parityIndex = ui->parityList->findData(portParams->parity));
                ui->byteSizeList->setCurrentIndex(settings.byteSizeIndex = ui->byteSizeList->findData(portParams->byteSize));
                ui->stopBitsList->setCurrentIndex(settings.stopBitsIndex = ui->stopBitsList->findData(portParams->stopBitsAmount));
                ui->inputEdit->setEnabled(true);
                ui->byteSizeList->setEnabled(true);
                ui->parityList->setEnabled(true);
                ui->stopBitsList->setEnabled(true);
                ui->speedList->setEnabled(true);
                ui->sourceAddressValue->setEnabled(true);
                ui->destinationAddressValue->setEnabled(true);
                ui->ErrorCheckBox->setEnabled(true);
                com->resetFinishFlag();
                com->start();
                isConnected = true;
                ui->selectComPortButton->setText("Disconnect");
            }break;
            case 1:
            {
                QMessageBox::warning(nullptr, "Error", "This serial port is already opened.");
            }break;
            case 2:
            {
                QMessageBox::warning(nullptr, "Error", "Cannot find this serial port.");
            }break;
            case 3:
            {
                QMessageBox::warning(nullptr, "Error", "Cannot open this serial port.");
            }

            }
        }
        else
        {
            QMessageBox::warning(nullptr, "Error", "Cannot find matching COM port.");
        }
    }else
    {
        isConnected = false;
        com->setFinishFlag();

        com->quit();
        while(!com->isFinished())
        {
#ifdef _WIN32 | _WIN64
            _sleep(1);
#elif unix
            sleep(1);
#endif
        }
        com->deleteLater();
        com = nullptr;
        ui->inputEdit->setEnabled(false);
        ui->byteSizeList->setEnabled(false);
        ui->parityList->setEnabled(false);
        ui->stopBitsList->setEnabled(false);
        ui->speedList->setEnabled(false);
        ui->sourceAddressValue->setEnabled(false);
        ui->destinationAddressValue->setEnabled(false);
        ui->ErrorCheckBox->setEnabled(false);
        ui->selectComPortButton->setText("Connect to this COM port");
        ui->inputEdit->clear();
        ui->outputEdit->clear();
    }
}

void MainWindow::on_inputEdit_textChanged()
{
    long long length = ui->inputEdit->toPlainText().length();
    previousTextLength = length;
    if (length % 7 == 0)
        emit readyToCreatePacket();
}

void MainWindow::on_byteSizeList_activated(int index)
{
    auto data = ui->byteSizeList->currentText();
    if(data == "5" && portParams->stopBitsAmount == QSerialPort::StopBits::TwoStop)
    {
        QMessageBox::warning(nullptr, "Error", "The use of 5 data bits with 2 stop bits is an invalid combination.");
        ui->byteSizeList->setCurrentIndex(settings.byteSizeIndex);
        return;
    }

    if((data == "6" || data == "7" || data == "8") && portParams->stopBitsAmount == QSerialPort::StopBits::OneAndHalfStop)
    {
        QMessageBox::warning(nullptr, "Error", "The use of 6, 7, or 8 data bits with 1.5 stop bits is an invalid combination.");
        ui->byteSizeList->setCurrentIndex(settings.byteSizeIndex);
        return;
    }

    if(data == "5") portParams->byteSize = QSerialPort::Data5;
    if(data == "6") portParams->byteSize = QSerialPort::Data6;
    if(data == "7") portParams->byteSize = QSerialPort::Data7;
    if(data == "8") portParams->byteSize = QSerialPort::Data8;
    settings.byteSizeIndex = index;
    com->setPortParams(portParams);
    return;

}

void MainWindow::on_stopBitsList_activated(int index)
{
    auto data = ui->stopBitsList->currentText();
    if(data == "1.5 stop bits" &&
            (portParams->byteSize == QSerialPort::Data6 || portParams->byteSize == QSerialPort::Data7 || portParams->byteSize == QSerialPort::Data8)
            )
    {
        QMessageBox::warning(nullptr, "Error", "The use of 6, 7, or 8 data bits with 1.5 stop bits is an invalid combination.");
        ui->stopBitsList->setCurrentIndex(settings.stopBitsIndex);
        return;
    }
    if((data == "2 stop bits") && portParams->byteSize == QSerialPort::Data5)
    {
        QMessageBox::warning(nullptr, "Error", "The use of 5 data bits with 2 stop bits is an invalid combination.");
        ui->stopBitsList->setCurrentIndex(settings.stopBitsIndex);
        return;
    }
    else
    {
        if(data == "1 stop bit") portParams->stopBitsAmount = QSerialPort::OneStop;
        if(data == "1.5 stop bits") portParams->stopBitsAmount = QSerialPort::OneAndHalfStop;
        if(data == "2 stop bits") portParams->stopBitsAmount = QSerialPort::TwoStop;
        settings.stopBitsIndex = index;
        com->setPortParams(portParams);
        return;
    }
}

void MainWindow::on_parityList_activated(int index)
{
    settings.parityIndex = index;
    auto data = ui->parityList->currentText();
    QSerialPort::Parity parity;

    if(data == "Unknown parity") parity = QSerialPort::UnknownParity;
    if(data == "Even parity")    parity = QSerialPort::EvenParity;
    if(data == "Mark parity")    parity = QSerialPort::MarkParity;
    if(data == "No parity")      parity = QSerialPort::NoParity;
    if(data == "Space parity")   parity = QSerialPort::SpaceParity;
    if(data == "Odd parity")     parity = QSerialPort::OddParity;

    portParams->parity = parity;
    com->setPortParams(portParams);

}

void MainWindow::showCOMPortError()
{
    auto info = com->getErrorInfo();
    QMessageBox::warning(nullptr, "Error", QString("Error code: ") + info.errorCode + "\nInfo: " + info.errorInfo);
}

void MainWindow::on_speedList_activated(int index)
{
    settings.speedIndex = index;
    auto data = ui->speedList->currentText();
    data.chop(4);
    portParams->speed = (QSerialPort::BaudRate)(data.toInt());
    com->setPortParams(portParams);
}

// LAB2
//=============================================================================================================================
void MainWindow::on_ErrorCheckBox_stateChanged(int arg)
{
    if(!arg)
        ui->ErrorCheckBox->setText("false");
    else
        ui->ErrorCheckBox->setText("true");
    PacketManager::setErrorSimulation((bool)arg);
}

void MainWindow::on_sourceAddressValue_valueChanged(int arg1)
{
    if(arg1 == ui->destinationAddressValue->value())
    {
        if(arg1 > source)
        {
            if(arg1 + 1 <= 255)
            {
                source = arg1 + 1;
            }
            else if (arg1 - 1 >= 0)
            {
                source = arg1 - 1;
            }
            ui->sourceAddressValue->setValue(source);
            return;
        }
        else
        {
            if(arg1 - 1 >= 0)
            {
                source = arg1 - 1;
            }
            else if (arg1 + 1 <= 255)
            {
                source = arg1 + 1;
            }
            ui->sourceAddressValue->setValue(source);
            return;
        }
    }
    source = arg1;
}

void MainWindow::on_destinationAddressValue_valueChanged(int arg1)
{
    if(arg1 == ui->sourceAddressValue->value())
    {
        if(arg1 > destination)
        {
            if(arg1 + 1 <= 255)
            {
                destination = arg1 + 1;
            }
            else if (arg1 - 1 >= 0)
            {
                destination = arg1 - 1;
            }
            ui->destinationAddressValue->setValue(destination);
            return;
        }
        else
        {
            if(arg1 - 1 >= 0)
            {
                destination = arg1 - 1;
            }
            else if (arg1 + 1 <= 255)
            {
                destination = arg1 + 1;
            }
            ui->destinationAddressValue->setValue(destination);
            return;
        }
    }
    destination = arg1;
}

