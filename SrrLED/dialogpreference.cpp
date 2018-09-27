#include "dialogpreference.h"
#include "ui_dialogpreference.h"



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
    _settings->setPortNameUartPort(ui->lePortName_UartPort->text());
    _settings->setPortNameDataPort(ui->lePortName_DataPort->text());

    if (ui->cbConfig->currentIndex() == 0) {
        _settings->setConfigFilePath("");
    } else {
        _settings->setConfigFilePath(ui->leConfigPath->text());
    }

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


    if (_settings->getConfigFilePath() == "") {
        ui->cbConfig->setCurrentIndex(0);
        ui->leConfigPath->clear();
        ui->leConfigPath->setEnabled(false);
    } else {
        ui->cbConfig->setCurrentIndex(1);
        ui->leConfigPath->setText(_settings->getConfigFilePath());
        ui->leConfigPath->setClearButtonEnabled(true);
    }
}



void DialogPreference::on_cbConfig_currentIndexChanged(int index)
{
    if (index == 0) ui->leConfigPath->setEnabled(false);
    else ui->leConfigPath->setEnabled(true);
}
