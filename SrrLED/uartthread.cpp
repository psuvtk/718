#include "uartthread.h"
#include "srrpacket.h"
#include <QDebug>
#include <QByteArray>

UartThread::UartThread(){}

void UartThread::setHandle(QSerialPort *handle) {
    _device = handle;
}

void UartThread::run() {
    QByteArray bufRecv;
    QByteArray bufFrame;
    qint64 skipLength;
    qint32 nErr = 0;
    const QByteArray SYNC = SrrPacket::getSync();

labelRecover:
    bufRecv.clear();
    bufFrame.clear();
    while (_device->waitForReadyRead()) {
        bufRecv.append(_device->readAll());

        if (bufRecv.startsWith(SYNC)) {
            skipLength = bufRecv.indexOf(SYNC, 8);
            if (skipLength != -1) {
                bufFrame.append(bufRecv.left(skipLength));
                // 缓冲区中包含完整一帧
            } else {
                continue;
            }
        } else {
            skipLength = bufRecv.indexOf(SYNC);
            if (skipLength != -1) {
                qDebug() << "未检测到同步码元";
                nErr++;
                if (nErr > 2) {
                    bufRecv.clear();
                    bufFrame.clear();
                    nErr = 0;
                    goto labelRecover;
                }
                continue;
            } else {
                bufRecv.clear();
                nErr++;
                if (nErr > 2) {
                    bufRecv.clear();
                    bufFrame.clear();
                    nErr = 0;
                    goto labelRecover;
                }
                qDebug() << "未检测到同步码元" << "\n";
                continue;
            }
        }

        // 处理完整一帧
        SrrPacket packet(bufFrame.data());
        qDebug() << "处理完整的一帧";
        packet.queryHeader();
//        qDebug() << bufFrame;
        bufRecv.remove(0, skipLength);
        bufFrame.clear();
    }
}
