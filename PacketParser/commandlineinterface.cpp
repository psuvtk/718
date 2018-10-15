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
}

CommandLineInterface::~CommandLineInterface()
{
    if (_cli->isOpen()) _cli->close();
    delete _cli;
}

bool CommandLineInterface::open()
{
    if (_cli->isOpen()) _cli->close();
    return _cli->open(QIODevice::ReadWrite);
}

void CommandLineInterface::close()
{
    if (_cli->isOpen()) _cli->close();
}

bool CommandLineInterface::sendConfigFile(const QString &path)
{
    QFile cfgFile(path);
    if (!cfgFile.open(QIODevice::ReadOnly))
        return false;

    while (!cfgFile.atEnd()) {
        QString cmd(cfgFile.readLine());
        if (!sendCmd(cmd)) return false;
    }

    return true;
}

bool CommandLineInterface::sendCmd(const QStringList &cmds)
{
    for (auto cmd: cmds) {
        if (!sendCmd(cmd)) return false;
    }
    return true;
}

bool CommandLineInterface::sendCmd(const QString &cmd)
{
    if (!_cli->isOpen() && !_cli->open(QIODevice::ReadWrite))
        return false;

    _bufRecv.clear();
    _cli->write(cmd.trimmed().toLatin1());
    _cli->write("\n", 1);
    _cli->waitForBytesWritten(10);
    _cli->waitForReadyRead(10);

    qDebug() << "after wait:" << _bufRecv;

    // TODO
    return true;
}


void CommandLineInterface::handleReadyRead()
{
    _bufRecv.append(_cli->readAll());
//    qDebug() << _bufRecv;
}
