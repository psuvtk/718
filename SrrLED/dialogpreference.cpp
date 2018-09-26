#include "dialogpreference.h"
#include "ui_dialogpreference.h"

#include <QSerialPort>
#include <QSerialPortInfo>

DialogPreference::DialogPreference(Settings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPreference),
    _settings(settings)
{
    ui->setupUi(this);
    fillComboBoxes();
}

DialogPreference::~DialogPreference()
{
    delete ui;
}


void DialogPreference::on_pbCancel_clicked()
{
    this->reject();
}

void DialogPreference::on_pbSave_clicked()
{
    QString portName;
    QSerialPort::BaudRate baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    QSerialPort::FlowControl flowControl;

    portName = ui->lineEditPortName->text();

    switch (ui->cbBaudRate->currentIndex()) {
    case 0:
        baudRate = QSerialPort::Baud9600;
        break;
    case 1:
        baudRate = QSerialPort::Baud19200;
        break;
    case 2:
        baudRate = QSerialPort::Baud38400;
        break;
    case 3:
        baudRate = QSerialPort::Baud57600;
        break;
    case 4:
        baudRate = QSerialPort::Baud115200;
        break;
    default:
        break;
    }

    switch (ui->cbDataBits->currentIndex()) {
    case 0:
        dataBits = QSerialPort::Data5;
        break;
    case 1:
        dataBits = QSerialPort::Data6;
        break;
    case 2:
        dataBits = QSerialPort::Data7;
        break;
    case 3:
        dataBits = QSerialPort::Data8;
        break;
    default:
        break;
    }

    switch (ui->cbParity->currentIndex()) {
    case 0:
        parity = QSerialPort::NoParity;
        break;
    case 1:
        parity = QSerialPort::EvenParity;
        break;
    case 2:
        parity = QSerialPort::OddParity;
        break;
    case 3:
        parity = QSerialPort::MarkParity;
        break;
    case 4:
        parity = QSerialPort::SpaceParity;
        break;
    default:
        break;
    }

    switch (ui->cbStopBits->currentIndex()) {
    case 0:
        stopBits = QSerialPort::OneStop;
        break;
    case 1:
        stopBits = QSerialPort::OneAndHalfStop;
        break;
    case 2:
        stopBits = QSerialPort::TwoStop;
        break;
    default:
        break;
    }

    switch (ui->cbFlowControl->currentIndex()) {
    case 0:
        flowControl = QSerialPort::NoFlowControl;
        break;
    case 1:
        flowControl = QSerialPort::HardwareControl;
        break;
    case 2:
        flowControl = QSerialPort::SoftwareControl;
        break;
    default:
        break;
    }

    _settings->setPortName(portName);
    _settings->setBaudRate(baudRate);
    _settings->setDataBits(dataBits);
    _settings->setParity(parity);
    _settings->setStopBits(stopBits);
    _settings->setFlowControl(flowControl);

    this->accept();
}

void DialogPreference::fillComboBoxes() {
    QString portName = _settings->getPortName();
    QSerialPort::BaudRate baudRate = _settings->getBaudRate();
    QSerialPort::DataBits dataBits = _settings->getDataBits();
    QSerialPort::Parity parity = _settings->getParity();
    QSerialPort::StopBits stopBits = _settings->getStopBits();
    QSerialPort::FlowControl flowControl = _settings->getFlowControl();

    ui->lineEditPortName->setText(portName);
    switch (baudRate) {
    case QSerialPort::Baud9600:
        ui->cbBaudRate->setCurrentIndex(0);
        break;
    case QSerialPort::Baud19200:
        ui->cbBaudRate->setCurrentIndex(1);
        break;
    case QSerialPort::Baud38400:
        ui->cbBaudRate->setCurrentIndex(2);
        break;
    case QSerialPort::Baud57600:
        ui->cbBaudRate->setCurrentIndex(3);
        break;
    case QSerialPort::Baud115200:
        ui->cbBaudRate->setCurrentIndex(4);
        break;
    default:
        break;
    }

    switch (dataBits) {
    case QSerialPort::Data5:
        ui->cbDataBits->setCurrentIndex(0);
        break;
    case QSerialPort::Data6:
        ui->cbDataBits->setCurrentIndex(1);
        break;
    case QSerialPort::Data7:
        ui->cbDataBits->setCurrentIndex(2);
        break;
    case QSerialPort::Data8:
        ui->cbDataBits->setCurrentIndex(3);
        break;
    default:
        break;
    }

    switch (parity) {
    case QSerialPort::NoParity:
        ui->cbParity->setCurrentIndex(0);
        break;
    case QSerialPort::EvenParity:
        ui->cbParity->setCurrentIndex(1);
        break;
    case QSerialPort::OddParity:
        ui->cbParity->setCurrentIndex(2);
        break;
    case QSerialPort::MarkParity:
        ui->cbParity->setCurrentIndex(3);
        break;
    case QSerialPort::SpaceParity:
        ui->cbParity->setCurrentIndex(4);
        break;
    default:
        break;
    }

    switch (stopBits) {
    case QSerialPort::OneStop:
        ui->cbStopBits->setCurrentIndex(0);
        break;
    case QSerialPort::OneAndHalfStop:
        ui->cbStopBits->setCurrentIndex(1);
        break;
    case QSerialPort::TwoStop:
        ui->cbStopBits->setCurrentIndex(2);
        break;
    default:
        break;
    }

    switch (flowControl) {
    case QSerialPort::NoFlowControl:
        ui->cbFlowControl->setCurrentIndex(0);
        break;
    case QSerialPort::HardwareControl:
        ui->cbFlowControl->setCurrentIndex(1);
        break;
    case QSerialPort::SoftwareControl:
        ui->cbFlowControl->setCurrentIndex(2);
        break;
    default:
        break;
    }
}















