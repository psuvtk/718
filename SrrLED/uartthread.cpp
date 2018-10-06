#include "uartthread.h"

CommThread::CommThread(){}

void CommThread::setHandle(QSerialPort *handle) {
    _device = handle;
}

void CommThread::run() {
    QByteArray SYNC = QByteArray::fromHex("0201040306050807");
    QByteArray bufRecv;
    QByteArray bufFrame;
    qint64 skipLength = 0;
    qint32 nErr = 0;

    quint32 cur =0, last=0;
    quint64 totalCycle =0;
    quint64 cnt = 0;

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
            if (last == 0 ) {
                last = packet.getTimeCpuCycles();
                continue;
            }

            int fs = 5;

            if (packet.isValid()) {
                if (packet.getFrameNumber()%(2*fs) < 2) {
                    emit frameChanged(&packet);
                    waitForDispDone();

                    cur = packet.getTimeCpuCycles();
                    cnt++;
                    totalCycle += (cur - last);
                    last = cur;
                    double time = double(totalCycle)/cnt/516.0*0.86/1000;
                    qDebug() << "-------------------";
                    qDebug() << "Frame Number: " << packet.getFrameNumber();
                    qDebug() << "Average CPU Cycle: " << totalCycle/cnt;
                    qDebug() << "Average Frame Time(ms): " << time;
                    qDebug() << "Average Frame Rate: " << 1/time * 1000;
                } else {
                    skipLength = bufRecv.indexOf(SYNC, 8);
                    bufRecv.remove(0, skipLength);
                }
            }

        }
    }
}

void CommThread::waitForDispDone() {
    _isDispDone = false;
    while (!_isDispDone)
        ;
}

void CommThread::onDispDone() {
    _isDispDone = true;
}
