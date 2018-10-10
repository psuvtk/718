#include "uartthread.h"

CommThread::CommThread(QSerialPort *device)
{
    if (device == nullptr) return;

    // 要在设置handle之后进行connect·
    _device = device;
    connect(_device, &QSerialPort::readyRead, this, &CommThread::onDataReady);
//    connect(_device, &QSerialPort::errorOccurred, this, [](){
//        qDebug() << "Serail Port Connection Disconnect";
//    });

    connect(_device, &QSerialPort::errorOccurred, this, &CommThread::onSerialPortError);
}

void CommThread::run() {
    exec();
}

void CommThread::onFrameRateChanged(CommThread::FramePerMinute fm)
{
    _frameRate = fm;
}

void CommThread::onDispDone() {
    _isDispDone = true;
}

CommThread::DeviceState CommThread::deviceState() const
{
    return _deviceState;
}

void CommThread::onDeviceStateChanged(const CommThread::DeviceState &deviceState)
{
    _deviceState = deviceState;
}

void CommThread::onDeviceOpen()
{
    if (_device->open(QIODevice::ReadOnly)) {
        _deviceState = OPEN;
        emit deviceOpenSuccess();
    } else {
        emit deviceOpenFailed();
    }
}

void CommThread::onDataReady()
{
    static QByteArray SYNC = QByteArray::fromHex("0201040306050807");
    static QByteArray bufRecv;
    QByteArray bufFrame;
    qint32 skipLength = 0;
    static int errorCounter = 0;

    if (_deviceState == PAUSE || _deviceState == CLOSE) {
        bufRecv.clear();
        return;
    }

//    if (_deviceState == OPEN && !_device->isOpen()){
//        qDebug() << "Device Lost Connection!";
//        reconnect();
//        return;
//    }


    if (!_isDispDone) {
        qDebug() << "Not Disp Done Yet";
        return;
    } else {
        bufFrame.clear();
    }

    // 读取
    bufRecv.append(_device->readAll());
    if (bufRecv.startsWith(SYNC)) {
        skipLength = bufRecv.indexOf(SYNC, 8);
        if (skipLength != -1) {
            bufFrame.append(bufRecv.left(skipLength));
            bufRecv.remove(0, skipLength);

            SrrPacket packet(bufFrame.data(), bufFrame.length());
            if (packet.isValid()) {
                qDebug() << "Parsed Frame #" << packet.getFrameNumber();
                if (packet.getFrameNumber()%(2*static_cast<unsigned>(_frameRate)) < 2) {
                    emit frameChanged(&packet);
                    _isDispDone = false;
                } else {
                    skipLength = bufRecv.indexOf(SYNC, 8);
                    bufRecv.remove(0, skipLength);
                    bufFrame.clear();
                }
            } else {
                qDebug() << "Packet Broken!";
                bufFrame.clear();
            }
        } else {
            // pass
        }
    } else {
        skipLength = bufRecv.indexOf(SYNC);
        if (skipLength == -1) bufRecv.clear();
        bufRecv.remove(0, skipLength);
        qDebug() << "未检测到同步码元" << "\n";
    }

}

void CommThread::onSerialPortError(QSerialPort::SerialPortError error)
{
    // 重试三次
    for (int i = 0; i < 3; i++) {
//        if (_device->isOpen()) return;
//        if (_device->open(QIODevice::ReadOnly)) return;
        msleep(300);
    }
    emit connectionLost();
}
