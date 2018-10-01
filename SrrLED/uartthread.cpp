#include "uartthread.h"

#include <random>

UartThread::UartThread(){}

void UartThread::setHandle(QSerialPort *handle) {
    _device = handle;
}

void UartThread::run() {
    QByteArray SYNC = QByteArray::fromHex("0201040306050807");
    QByteArray bufRecv;
    QByteArray bufFrame;
    qint64 skipLength = 0;
    qint32 nErr = 0;

    while (true) {
        if (!_device->isOpen()) {
            msleep(100);
            continue;
        }
labelRecover:
        bufRecv.clear();
        bufFrame.clear();
        while (_device->waitForReadyRead()) {
            bufFrame.clear();
            bufRecv.append(_device->readAll());

            if (bufRecv.startsWith(SYNC)) {
                skipLength = bufRecv.indexOf(SYNC, 8);
                if (skipLength != -1) {
                    bufFrame.append(bufRecv.left(skipLength));
                    bufRecv.remove(0, skipLength);
                    // 缓冲区中包含完整一帧
                } else {
                    continue;
                }
            } else {
                skipLength = bufRecv.indexOf(SYNC);
                if (skipLength == -1) bufRecv.clear();
                nErr++;
                if (nErr > 2) {
                    nErr = 0;
                    goto labelRecover;
                }
                qDebug() << "未检测到同步码元" << "\n";
                continue;
            }

            // 处理完整一帧
            SrrPacket packet(bufFrame.data());
            emit frameChanged(&packet);
            waitForDispDone();
            // 选取速度
            double speed = selectSpeed();
            emit speedChanged(speed);

        }
    }
}

double UartThread::selectSpeed() {
    qDebug() << "TODO: impl select speed.";
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(1, 6);
    return (double)dist6(rng);
}

void UartThread::waitForDispDone() {
    _isDispDone = false;
    while (!_isDispDone)
        ;
}

void UartThread::onDispDone() {
    _isDispDone = true;
}
