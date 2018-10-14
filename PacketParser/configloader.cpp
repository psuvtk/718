#include "configloader.h"

ConfigLoader::ConfigLoader(QString portName, QObject *parent) : QObject(parent)
{
    _cli = new QSerialPort;
    _cli->setPortName(portName);
    _cli->setBaudRate(QSerialPort::Baud115200);
    _cli->setDataBits(QSerialPort::Data8);
    _cli->setParity(QSerialPort::NoParity);
    _cli->setStopBits(QSerialPort::OneStop);
    _cli->setFlowControl(QSerialPort::NoFlowControl);

    connect(_cli, &QSerialPort::readyRead, this, &ConfigLoader::handleReadyRead);
}

void ConfigLoader::handleReadyRead()
{
    static QByteArray bufRecv;
    bufRecv.append(_cli->readAll());
}
