#include "srrpacket.h"
#include <QDebug>


SrrPacket::SrrPacket(const char *pSrrPacket) {
    _pSrrPacket = pSrrPacket;

    const char *tl = pSrrPacket + HEAD_STRUCT_SIZE_BYTES;
    query();
    for (uint32_t i = 0; i < getNumTLVs(); i++) {

        uint32_t type = getTlvType(tl);
        uint32_t len = getTlvLength(tl);
        tl += TL_STRUCT_SIZE_BYTES;
        uint16_t numObjs = getDescrNumObj(tl);
        uint16_t xyzQFormat = getDescrQFormat(tl);
        tl += DESCR_STRUCT_SIZE_BYTES;

        switch (type) {
        case __TLV_Type::MMWDEMO_UART_MSG_DETECTED_POINTS:
            qDebug() << "tlvType => MMWDEMO_UART_MSG_DETECTED_POINTS";
            for (uint16_t j = 0; j < numObjs; j++) {
                extractDetObj(tl, xyzQFormat);
                tl += OBJ_STRUCT_SIZE_BYTES;
            }
            break;
        case __TLV_Type::MMWDEMO_UART_MSG_CLUSTERS:
            qDebug() << "tlvType => MMWDEMO_UART_MSG_CLUSTERS";
            for (uint16_t j = 0; j < numObjs; j++) {
                tl += CLUSTER_STRUCT_SIZE_BYTES;
            }
            break;
        case __TLV_Type::MMWDEMO_UART_MSG_TRACKED_OBJ:
            qDebug() << "tlvType => MMWDEMO_UART_MSG_TRACKED_OBJ";
            for (uint16_t j = 0; j < numObjs; j++) {
                tl += TRACKER_STRUCT_SIZE_BYTES;
            }
            break;
        case __TLV_Type::MMWDEMO_UART_MSG_PARKING_ASSIST:
            qDebug() << "tlvType => MMWDEMO_UART_MSG_PARKING_ASSIST";
            for (uint16_t j = 0; j < numObjs; j++) {
                tl += PARKING_ASSIST_BIN_SIZE_BYTES;
            }
            break;
        case __TLV_Type::MMWDEMO_UART_MSG_STATS:
            qDebug() << "tlvType => MMWDEMO_UART_MSG_STATS";
            for (uint16_t j = 0; j < numObjs; j++) {
                tl += STATS_SIZE_BYTES;
            }
            break;
        default:
            qDebug() << "tlvType => Unkown";
            break;
        }
    }
    qDebug() << "----------------------------\n\n";
}

void SrrPacket::query() {
    qDebug() << "\n\nPacket Header-------------";
    qDebug("Sync: 0x%04x 0x%04x 0x%04x 0x%04x",
           *(uint16_t*)_pSrrPacket,
           *(uint16_t*)(_pSrrPacket+2),
           *(uint16_t*)(_pSrrPacket+4),
           *(uint16_t*)(_pSrrPacket+6));

    qDebug("Version: 0x%08X", getVersion()) ;
    qDebug() << "TotalPacketLen: " << getTotalPacketLen();
    qDebug("Platform: 0x%08X", getPlatform());
    qDebug() << "FrameNumber: " << getFrameNumber();
    qDebug() << "TimeCpuCycles: " << getTimeCpuCycles();
    qDebug() << "NumDetectedObj: " << getNumDetectedObj();
    qDebug() << "NumTLVs: " << getNumTLVs();
    qDebug() << "SubframeNumber: " << getSubframeNumber();
    qDebug() << "----------------------------";

    // TODO: display detobj tracker cluster ...

}

void SrrPacket::extractDetObj(const char *ptr, uint16_t oneQFromat) {
    double invQFormat = 1.0 / (1<<oneQFromat);
    qDebug() << "invQFromat: "<< invQFormat;
    DetObj_t detObj;
    const __detObj_t *rawDetObj = (struct __detObj_t *)ptr;

    detObj.x = rawDetObj->x * invQFormat;
    detObj.y = rawDetObj->y * invQFormat;
    detObj.speed = rawDetObj->speed * invQFormat;
    detObj.range = std::sqrt(detObj.x * detObj.x + detObj.y * detObj.y);
    qDebug() << "x: " << detObj.x;
    qDebug() << "y: " << detObj.y;
    qDebug() << "speed: " << detObj.speed;
    qDebug() << "range: " << detObj.range;
}

void SrrPacket::extractCluster() {
    qDebug() << "Not implement extraCluster";
}

void SrrPacket::extractTracker() {
    qDebug() << "Not implement extraTracker";
}
