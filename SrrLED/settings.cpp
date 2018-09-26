#include "settings.h"

Settings::Settings(QWidget *parent)
{

}

Settings::~Settings()
{

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
