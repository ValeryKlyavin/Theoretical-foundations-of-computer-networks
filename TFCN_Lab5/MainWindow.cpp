#include "MainWindow.h"
#include <QCloseEvent>
#include <QThread>
#include <QDebug>
#include "ui_MainWindow.h"

MainWindow::MainWindow(int index, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    id(index),
    sourceAddress(0),
    destinationAddress(0),
    earlyTokenRelease(false),
    currentPosition(0),
    hasToken(false)
{
    ui->setupUi(this);
    QString title = "Station ";
    title += QString::number(index + 1);
    setWindowTitle(title);
    BaseTitle = title;
    connect(&timer, SIGNAL(timeout()), this, SLOT(transmitPacket()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::show()
{
    QMainWindow::show();
}

void MainWindow::disableAddTokenButton()
{
    ui->pushButton->setEnabled(false);
}

void MainWindow::receive(Packet p)
{
    QString content;
    content += p.getType();
    content += " ";
    if(p.getType() == 'T')
    {
        setWindowTitle(BaseTitle + " Token here");
        hasToken = true;
        timer.start(1000);
        ui->frameLabel->setText(content);
        return;
    }
    else
    {
        content += QString::number(p.getDestinationAddress()) + " " + QString::number(p.getSourceAddress()) + " ";
        content += QString(p.getAccessControl()) + " ";
        content += QString(p.getData());
        ui->frameLabel->setText(content);
        if(p.getDestinationAddress() == sourceAddress)
        {
            if(p.getAccessControl() == ' ')
            {
                p.setAccessControl('C');
                auto text = ui->outputTextEdit->toPlainText();
                auto data = p.getData();
                text += data;
                ui->outputTextEdit->setText(text);
            }
        }
        if (p.getSourceAddress() == sourceAddress)
        {
            if(p.getAccessControl() == ' ')
            {
                qDebug() << "Failed to transmit packet ";
            }
            return;
        }
        if(!hasToken)
        {
            if(buffer.empty())
                timer.start(1000);
            buffer.push_back(p);
        }
        //emit transmit(p);
    }
}

void MainWindow::on_sourceSpinBox_valueChanged(int arg1)
{
    sourceAddress = static_cast<char>(arg1);
}

void MainWindow::on_destinationSpinBox_valueChanged(int arg1)
{
    destinationAddress = static_cast<char>(arg1);
}


void MainWindow::on_pushButton_clicked()
{
    emit addTokenButtonClicked(id);
    setWindowTitle(BaseTitle + " Token here");
    hasToken = true;
    timer.start(1000);
}

void MainWindow::on_earlyTokenReleaseCheckBox_stateChanged(int arg1)
{
    earlyTokenRelease = static_cast<bool>(arg1);
    ui->earlyTokenReleaseCheckBox->setText(earlyTokenRelease ? "Enabled" : "Disabled");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(event->type() == QCloseEvent::Close)
    {
        emit closed(id);
    }
}

void MainWindow::transmitPacket()
{
    if(hasToken)
    {
        if(currentPosition < ui->inputTextEdit->toPlainText().length())
        {
            transmitFrame();
        }
        else
        {
            transmitToken();
        }
    }
    else
    {
        if(!buffer.empty())
        {
            emit transmit(buffer.front());
            buffer.pop_front();
        }
    }
}

void MainWindow::transmitToken()
{
    //timer.stop();
    Packet token;
    setWindowTitle(BaseTitle);
    token.setType('T');
    token.setSourceAddress(sourceAddress);
    token.setDestinationaddress(destinationAddress);
    emit transmit(token);
    hasToken = false;
}

void MainWindow::transmitFrame()
{
    Packet frame;
    frame.setType('F');
    frame.setSourceAddress(sourceAddress);
    frame.setDestinationaddress(destinationAddress);
    frame.setData(ui->inputTextEdit->toPlainText().at(currentPosition++));
    frame.setAccessControl(' ');
    emit transmit(frame);
    if(earlyTokenRelease)
        transmitToken();
}
