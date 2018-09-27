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


    this->accept();
}

void DialogPreference::fillComboBoxes() {
    QString portNameUartPort = _settings->getPortNameUartPort();
    qint32 baudRateUartPort = _settings->getBaudRateUartPort();
    QString portNameDataPort = _settings->getPortNameDataPort();
    qint32 baudRateDataPort = _settings->getBaudRateDataPort();

    ui->lePortName_UartPort->setText(portNameUartPort);
    ui->lePortName_DataPort->setText(portNameDataPort);

    switch (baudRateUartPort) {
    case 115200:
        ui->cbBaudRate_UartPort->setCurrentIndex(0);
        break;
    case 921600:
        ui->cbBaudRate_UartPort->setCurrentIndex(1);
        break;
    default:
        break;
    }

    switch (baudRateDataPort) {
    case 115200:
        ui->cbBaudRate_DataPort->setCurrentIndex(0);
        break;
    case 921600:
        ui->cbBaudRate_DataPort->setCurrentIndex(1);
        break;
    default:
        break;
    }


    // TODO: baudRate display
}















