#include "commandlineinterface.h"

CommandLineInterface::CommandLineInterface(QString portName,
                                           QObject *parent)
{
    _cli = new QSerialPort;
    _cli->setPortName(portName);
    _cli->setBaudRate(QSerialPort::Baud115200);
    _cli->setDataBits(QSerialPort::Data8);
    _cli->setParity(QSerialPort::NoParity);
    _cli->setStopBits(QSerialPort::OneStop);
    _cli->setFlowControl(QSerialPort::NoFlowControl);

    connect(_cli, &QSerialPort::readyRead,
            this, &CommandLineInterface::handleReadyRead);
    if (!_cli->open(QIODevice::ReadWrite))
        qDebug() << _cli->errorString();

}

CommandLineInterface::~CommandLineInterface()
{
    if (_cli->isOpen()) _cli->close();
    delete _cli;
}

bool CommandLineInterface::sendConfigFile(const QString &path)
{
    return true;
}

bool CommandLineInterface::sendCmd(const QStringList &cmds)
{
    if (!_cli->isOpen() && !_cli->open(QIODevice::ReadWrite)){
        qDebug() << _cli->errorString();
        return false;
    }


    return true;
}

bool CommandLineInterface::sendCmd(const QString &cmd)
{
    if (!_cli->isOpen() && !_cli->open(QIODevice::ReadWrite | QIODevice::Text))
        return false;

    return true;
}

void CommandLineInterface::setDelay(int delay)
{
    _delay = delay;
}

void CommandLineInterface::handleReadyRead()
{
    _bufRecv.append(_cli->readAll());
    qDebug() << _bufRecv;
}

void CommandLineInterface::handleSendCmd(const QString &cmd)
{
    _bufRecv.clear();
    _cli->write(cmd.trimmed().toLatin1());
    _cli->write("\n", 1);
    _cli->waitForBytesWritten(10);
//    _cli->waitForReadyRead(10);
    qDebug()<< "lalal" << _bufRecv;
}

