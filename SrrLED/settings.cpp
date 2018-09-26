#include "settings.h"
#include <QDebug>
Settings::Settings(QWidget *parent)
{
    readLocalSettings();
    printInfo();
}

Settings::~Settings()
{
    writeLocalSettings();
}

void Settings::setPortName(QString pn) {
    _portName = pn;
}

void Settings::setBaudRate(QSerialPort::BaudRate br) {
    _baudRate = br;
}

void Settings::setDataBits(QSerialPort::DataBits db) {
    _dataBits = db;
}

void Settings::setParity(QSerialPort::Parity p) {
    _parity = p;
}

void Settings::setStopBits(QSerialPort::StopBits sb) {
    _stopBits = sb;
}

void Settings::setFlowControl(QSerialPort::FlowControl fc) {
    _flowControl = fc;
}

QString Settings::getPortName() {
    return _portName;
}

QSerialPort::BaudRate Settings::getBaudRate() {
    return _baudRate;
}

QSerialPort::DataBits Settings::getDataBits() {
    return _dataBits;
}

QSerialPort::Parity Settings::getParity() {
    return _parity;
}

QSerialPort::StopBits Settings::getStopBits() {
    return _stopBits;
}

QSerialPort::FlowControl Settings::getFlowControl() {
    return _flowControl;
}



void Settings::printInfo() {
    qDebug() << _portName;
    qDebug() << _baudRate;
    qDebug() << _dataBits;
    qDebug() << _parity;
    qDebug() << _stopBits;
    qDebug() << _flowControl;
}

void Settings::readLocalSettings() {
    qDebug() << "Settings::readLocalSettings()";
    QSettings settings(this->parent());
    settings.beginGroup("serialport");
    if (!settings.contains("baudRate") || !settings.contains("dataBits") ||
        !settings.contains("parity") ||!settings.contains("stopBits") ||
        !settings.contains("flowControl")) {
        qDebug() << "\nSettings::readLocalSettings ";
        qDebug() << "No Previous Settings.\n";

        _portName = "";
        _baudRate = QSerialPort::Baud115200;
        _dataBits = QSerialPort::Data8;
        _parity = QSerialPort::NoParity;
        _stopBits = QSerialPort::OneStop;
        _flowControl = QSerialPort::NoFlowControl;

        return;
    }

    _portName = settings.value("portName").toString();
    _baudRate = static_cast<QSerialPort::BaudRate>(settings.value("baudRate").toInt());
    _dataBits = static_cast<QSerialPort::DataBits>(settings.value("dataBits").toInt());
    _parity = static_cast<QSerialPort::Parity>(settings.value("parity").toInt());
    _stopBits = static_cast<QSerialPort::StopBits>(settings.value("stopBits").toInt());
    _flowControl = static_cast<QSerialPort::FlowControl>(settings.value("flowControl").toInt());
    settings.endGroup();
}

void Settings::writeLocalSettings() {
    qDebug() << "Settings::writeLocalSettings()";
    QSettings settings(this->parent());
    settings.beginGroup("serialport");
    settings.setValue("portName", _portName);
    settings.setValue("baudRate", _baudRate);
    settings.setValue("dataBits", _dataBits);
    settings.setValue("parity", _parity);
    settings.setValue("stopBits", _stopBits);
    settings.setValue("flowControl", _flowControl);
    settings.endGroup();
}
