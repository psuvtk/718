#include "uartthread.h"

bool isDispDone = true;
QMutex mutexDispDone;

CommThread::CommThread() {
}

void CommThread::setHandle(QSerialPort *handle) {
    _device = handle;
    // 要在设置handle之后进行connect·
    connect(_device, &QSerialPort::readyRead, this, &CommThread::onDataReady);
}

void CommThread::run() {
////    QByteArray SYNC = QByteArray::fromHex("0201040306050807");
////    QByteArray bufRecv;
////    QByteArray bufFrame;
////    qint32 skipLength = 0;
////    qint32 nErr = 0;
////       auto         start = QTime::currentTime();
////    while (true) {
////        if (_deviceState == CLOSE || _deviceState == PAUSE) {
////            bufRecv.clear();
////            if (_device->isOpen())_device->clear();
////            qDebug() << QTime::currentTime()<< ": waiting";
////            msleep(100);
////            continue;
////        }

////        qDebug() << "before";
////        QThread::msleep(5);
////        bufRecv.append(_device->readAll());
////        qDebug() << "after";

////        if (bufRecv.startsWith(SYNC)) {
////            skipLength = bufRecv.indexOf(SYNC, 8);
////            if (skipLength != -1) {
////                bufFrame.append(bufRecv.left(skipLength));
////                bufRecv.remove(0, skipLength);

////                SrrPacket packet(bufFrame.data());
////                if (packet.isValid()) {
////                    qDebug() << QTime::currentTime() << packet.getTimeCpuCycles();

////                    if (packet.getFrameNumber()%(2*_overN) < 2) {
////                        qDebug() << "in";
////                        emit frameChanged(&packet);
////                        qDebug() << "after  emit";
////                        waitForDispDone();
////                        bufFrame.clear();

////                    } else {
////                        skipLength = bufRecv.indexOf(SYNC, 8);
////                        bufRecv.remove(0, skipLength);
////                        bufFrame.clear();
////                    }
////                }
////            } else {
////                continue;
////            }
////        } else {
////            skipLength = bufRecv.indexOf(SYNC);
////            if (skipLength == -1) bufRecv.clear();
////            nErr++;
////            if (nErr > 2) {
////                nErr = 0;
////                bufRecv.clear();
////                bufFrame.clear();
////            }
////            qDebug() << "未检测到同步码元" << "\n";
////            continue;
////        }


////    }
///

//    connect(_device, &QSerialPort::readyRead, this, &CommThread::onDataReady);

}

void CommThread::waitForDispDone() {
    _isDispDone = false;
    while (!_isDispDone)
        ;
}

void CommThread::onDispDone() {
    _isDispDone = true;
}

CommThread::DeviceState CommThread::deviceState() const
{
    return _deviceState;
}

void CommThread::setDeviceState(const DeviceState &deviceState)
{
    _deviceState = deviceState;
}

void CommThread::setOverN(uint overN)
{
    _overN = overN;
}

void CommThread::onDataReady()
{
    static QByteArray SYNC = QByteArray::fromHex("0201040306050807");
    static QByteArray bufRecv;
    QByteArray bufFrame;
    qint32 skipLength = 0;
    qDebug() << QTime::currentTime();
    bufRecv.append(_device->readAll());
    if (bufRecv.startsWith(SYNC)) {
        skipLength = bufRecv.indexOf(SYNC, 8);
        if (skipLength != -1) {
            bufFrame.append(bufRecv.left(skipLength));
            bufRecv.remove(0, skipLength);

            SrrPacket packet(bufFrame.data());
            if (packet.isValid()) {
                qDebug() << QTime::currentTime() << packet.getTimeCpuCycles();
                if (packet.getFrameNumber()%(2*_overN) < 2) {
                    emit frameChanged(&packet);
                    bufFrame.clear();
                } else {
                    skipLength = bufRecv.indexOf(SYNC, 8);
                    bufRecv.remove(0, skipLength);
                    bufFrame.clear();
                }
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

void CommThread::setIsDispDone(bool isDispDone)
{
    _isDispDone = isDispDone;
}
