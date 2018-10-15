#include "commthread.h"

CommThread::CommThread(const QString &portName)
{
    _loggingHandle = new QSerialPort;
}

void CommThread::handleDeviceOpen()
{

}

void CommThread::handleReadReady()
{
    _bufRecv.append(_loggingHandle->readAll());
}

void CommThread::handleError(QSerialPort::SerialPortError error)
{

}
