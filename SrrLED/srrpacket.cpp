#include "srrpacket.h"
#include <QDebug>


SrrPacket::SrrPacket(const char *pSrrPacket) {
    _pSrrPacketHeader = pSrrPacket;
    _pSrrPacketBody = pSrrPacket + HeaderLength;

    const char *tl = _pSrrPacketBody;
    const char *v = tl + TlLength;
    for (int i=0; i < getNumTLVs(); i++) {
        quint32 type = getTlvType(tl);
        quint32 len = getTlvLength(tl);

        switch (type) {
        case TlvType::MMWDEMO_UART_MSG_DETECTED_POINTS:
            break;
        case TlvType::MMWDEMO_UART_MSG_CLUSTERS:
            break;
        case TlvType::MMWDEMO_UART_MSG_TRACKED_OBJ:
            break;
        case TlvType::MMWDEMO_UART_MSG_PARKING_ASSIST:
            break;
        case TlvType::MMWDEMO_UART_MSG_STATS:
            break;
        default:
            break;
        }

    }
}

void SrrPacket::queryHeader() {
    qDebug() << "\nPacket Header-------------";
    qDebug() << "Sync: " << _pSrrPacketHeader[0] << " "
             << _pSrrPacketHeader[1] << " "
             << _pSrrPacketHeader[2] << " "
             << _pSrrPacketHeader[3];
    qDebug() << "Version: " << getVersion();
    qDebug() << "TotalPacketLen: " << getTotalPacketLen();
    qDebug() << "Platform: " << getPlatform();
    qDebug() << "FrameNumber: " << getFrameNumber();
    qDebug() << "TimeCpuCycles: " << getTimeCpuCycles();
    qDebug() << "NumDetectedObj: " << getNumDetectedObj();
    qDebug() << "NumTLVs: " << getNumTLVs();
    qDebug() << "SubframeNumber: " << getSubframeNumber();
}
