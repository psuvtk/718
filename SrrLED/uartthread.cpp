#include "uartthread.h"

CommThread::CommThread(QSerialPort *device)
{
    if (device == nullptr) return;

    // 要在设置handle之后进行connect·
    _device = device;
    connect(_device, &QSerialPort::readyRead, this, &CommThread::handleReadReady);
    connect(_device, &QSerialPort::errorOccurred, this, &CommThread::handleError);
}

void CommThread::run() {
    exec();
}

void CommThread::handleDispDone() {
    _isDispDone = true;
    qDebug() << "Elapsed Time: " << _tic.msecsTo(QTime::currentTime()) << "ms";
}

void CommThread::handleFrameRateChanged(CommThread::FramePerSecond fm)
{
    _frameRate = fm;
}

CommThread::DeviceState CommThread::deviceState() const
{
    return _deviceState;
}

void CommThread::handleDeviceStateChanged(const CommThread::DeviceState &deviceState)
{
    _deviceState = deviceState;
}

void CommThread::handleDeviceOpen()
{
    if (_device->open(QIODevice::ReadOnly)) {
        emit deviceOpenSuccess();
        msleep(3);
        _deviceState = OPEN;
    } else {
        emit deviceOpenFailed();
    }
}

/**
 * @brief 处理串口到来事件
 * @date 20181011
 */
void CommThread::handleReadReady()
{
    static QByteArray SYNC = QByteArray::fromHex("0201040306050807");
    qint32 skipLength = 0;

    if (_deviceState == PAUSE) {
        bufRecv.clear();
        return;
    }

    // 等待绘图完成
    if (!_isDispDone) {
        qDebug() << "\nNot Disp Done Yet\n";
        return;
    } else {
        bufFrame.clear();
    }

    /**
     * 将读取的数据全部放入缓冲区，然后分为几种情况处理
     * 1. 缓冲区前八个字节为同步码
     *   (1) 缓冲区内除开始八个字节之外不存在另一个同步码，则继续等待下一次读取
     *   (2) 缓冲区内除开始八个字节之外还存在另一个同步码，说明存在完整一帧，
     *       若帧有效追加到bufFrame并发送信号进行处理该帧，否则跳过该帧
     *
     * 2. 缓冲区前八个字节不是同步码
     *   (1) 缓冲区内存在同步码，则清楚同步码之前的字节
     *   (2) 缓冲区内不村子啊同步码，则清除整个缓冲区
     */
    bufRecv.append(_device->readAll());
    if (bufRecv.startsWith(SYNC)) {
        skipLength = bufRecv.indexOf(SYNC, 8);
        if (skipLength != -1) {
            bufFrame.append(bufRecv.left(skipLength));
            bufRecv.remove(0, skipLength);

            SrrPacket packet(bufFrame.data(), bufFrame.length());
            if (packet.isValid()) {
                if (packet.getFrameNumber()%(2*unsigned(_frameRate)) < 2) {
                    qDebug() << "Parsed Frame #" << packet.getFrameNumber();
                    emit frameChanged(&packet);
                    _tic = QTime::currentTime();
                    _isDispDone = false;
                } else {
                    qDebug() << "Skiped Frame #" << packet.getFrameNumber();
                    skipLength = bufRecv.indexOf(SYNC, 8);
                    bufRecv.remove(0, skipLength);
                    bufFrame.clear();
                }
            } else {
                qDebug() << "\nPacket Broken!\n";
                bufFrame.clear();
            }
        } else {
            // pass
        }
    } else {
        skipLength = bufRecv.indexOf(SYNC);
        if (skipLength == -1) bufRecv.clear();
        bufRecv.remove(0, skipLength);
        qDebug() << "\n未检测到同步码元\n";
    }

}

/**
 * @brief 处理串口错误
 * @param error
 * @date 20181011
 */
void CommThread::handleError(QSerialPort::SerialPortError error)
{
    qDebug() << error;
    // 仅处理串口断开错误
    if (error != QSerialPort::ResourceError) return;
    _device->close();
    msleep(1000);

    // 重试一次
    if (_device->open(QIODevice::ReadOnly)) {
        bufFrame.clear();
        bufRecv.clear();
        return;
    }

    _deviceState = CLOSE;
    emit connectionLost();
}
