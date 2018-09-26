#include "dialogpreference.h"
#include "ui_dialogpreference.h"

#include <QSerialPort>
#include <QSerialPortInfo>

DialogPreference::DialogPreference(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPreference)
{
    ui->setupUi(this);
    fillDeviceComboBox();
}

DialogPreference::~DialogPreference()
{
    delete ui;
}

void DialogPreference::fillDeviceComboBox() {
    QList<QSerialPortInfo> devices = QSerialPortInfo::availablePorts();
    for (auto device: devices) {
        ui->cbDevices->addItem(device.portName());
    }
}

void DialogPreference::on_pbCancel_clicked()
{
    this->reject();
}

void DialogPreference::on_pbSave_clicked()
{
    QString portName = ui->cbDevices->currentText();

    QSerialPort::BaudRate baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    QSerialPort::FlowControl flowControl;


    switch (ui->cbBaudRate->currentText().toInt()) {
    case "9600":
        baudRate = QSerialPort::Baud9600;
        break;
    case "19200":
        baudRate = QSerialPort::Baud19200;
        break;
    case "38400":
        baudRate = QSerialPort::Baud38400;
        break;
    case "57600":
        baudRate = QSerialPort::Baud57600;
        break;
    case "115200":
        baudRate = QSerialPort::Baud115200;
        break;
    default:
        break;
    }

    switch (ui->cbDataBits->currentText()) {
    case "5":
        dataBits = QSerialPort::Data5;
        break;
    case "6":
        dataBits = QSerialPort::Data6;
        break;
    case "7":
        dataBits = QSerialPort::Data7;
        break;
    case "8":
        dataBits = QSerialPort::Data8;
        break;
    default:
        break;
    }

    switch (ui->cbParity->currentText()) {
    case "None":
        parity = QSerialPort::NoParity;
        break;
    case "Even":
        parity = QSerialPort::EvenParity;
        break;
    case "Odd":
        parity = QSerialPort::OddParity;
        break;
    case "Mark":
        parity = QSerialPort::MarkParity;
        break;
    case "Space":
        parity = QSerialPort::SpaceParity;
        break;
    default:
        break;
    }

    switch (ui->cbStopBits->currentText()) {
    case "1":
        stopBits = QSerialPort::OneStop;
        break;
    case "1.5":
        stopBits = QSerialPort::OneAndHalfStop;
        break;
    case "2":
        stopBits = QSerialPort::TwoStop
        break;
    default:
        break;
    }

    switch (ui->cbFlowControl->currentText()) {
    case "None":
        flowControl = QSerialPort::NoFlowControl;
        break;
    case "RTS/CTS":
        flowControl = QSerialPort::HardwareControl;
        break;
    case "XON/XOFF":
        flowControl = QSerialPort::SoftwareControl;
        break;
    default:
        break;
    }

    this->accept();
}

















