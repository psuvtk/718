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

    _settings->setSpeedThreshold(ui->dsbSpeedThreshold->value());
    _settings->setFilterThreshold(ui->dsbFilterThreshold->value());
    _settings->setFrameRate(ui->cbFrameRate->currentIndex() + 1);
    _settings->setFullscreenOnStartup(ui->cbFullScreen->isChecked());
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

    ui->cbFrameRate->setCurrentIndex(_settings->getFrameRate()-1);
    ui->dsbSpeedThreshold->setValue(_settings->getSpeedThreshold());
    ui->dsbFilterThreshold->setValue(_settings->getFilterThreshold());
    ui->cbFullScreen->setChecked(_settings->getFullscreenOnStartup());
}
