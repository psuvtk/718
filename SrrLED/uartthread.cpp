#include "uartthread.h"

UartThread::UartThread()
{

}

bool UartThread::open(Settings *settings) {
    _device.setPortName(settings->getPortName());
    _device.setBaudRate(settings->getBaudRate());
    _device.setDataBits(settings->getDataBits());
    _device.setParity(settings->getParity());
    _device.setStopBits(settings->getStopBits());
    _device.setFlowControl(settings->getFlowControl());

    return _device.open(QIODevice::ReadWrite);
}


void UartThread::run() {
    while (true) {
        if (_device.bytesAvailable() >= 20) {

        }
    }
}
