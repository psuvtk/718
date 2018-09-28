#include "srrpacket.h"
#include <QDebug>


SrrPacket::SrrPacket(const char *pSrrPacket) {
    _pSrrPacketHeader = pSrrPacket;

    const char *tl = pSrrPacket + HeaderLength;
    const char *v = tl + TlLength;

    for (int i=0; i < getNumTLVs(); i++) {
        quint32 type = getTlvType(tl);
        quint32 len = getTlvLength(tl);
        quint32 num;

        switch (type) {
        case TlvType::MMWDEMO_UART_MSG_DETECTED_POINTS:
            num = (len - TlLength) / sizeof(struct __DetObj_t);
            for (int j = 0; j < num; j++) {
                extractDetObj();
                v += sizeof(struct __DetObj_t);
            }
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


        tl += len;
        v = tl + TlLength;
    }
}

void SrrPacket::query() {
    qDebug() << "\nPacket Header-------------";
    qDebug() << "Sync: ";
    qDebug("%x %x %x %x", _pSrrPacketHeader[0], _pSrrPacketHeader[1],
            _pSrrPacketHeader[2], _pSrrPacketHeader[3]);

    qDebug() << "Version: " << getVersion();
    qDebug() << "TotalPacketLen: " << getTotalPacketLen();
    qDebug() << "Platform: ";
    qDebug("%x", getPlatform());
    qDebug() << "FrameNumber: " << getFrameNumber();
    qDebug() << "TimeCpuCycles: " << getTimeCpuCycles();
    qDebug() << "NumDetectedObj: " << getNumDetectedObj();
    qDebug() << "NumTLVs: " << getNumTLVs();
    qDebug() << "SubframeNumber: " << getSubframeNumber();

    // TODO: display detobj tracker cluster ...
}

void SrrPacket::extractDetObj() {

}
