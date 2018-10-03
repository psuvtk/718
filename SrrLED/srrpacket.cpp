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
    DetObj_t detObj;
    const __detObj_t *rawDetObj = (const struct __detObj_t *)ptr;
    double invQFormat = 1.0 / (1 << oneQFromat);

    detObj.x = rawDetObj->x * invQFormat;
    detObj.y = rawDetObj->y * invQFormat;
//    detObj.peakVal = rawDetObj->peakVal;
    detObj.doppler = rawDetObj->speed * invQFormat;
    detObj.range = std::sqrt(detObj.x * detObj.x + detObj.y * detObj.y);

    _detObjs.push_back(detObj);
}

void SrrPacket::extractCluster(const char *ptr, uint16_t oneQFromat) {
    Cluster_t clusterObj;
    const __cluster_t *rawClusterObj = (const struct __cluster_t *)ptr;
    double invQFormat = 1.0 / (1<<oneQFromat);

    double x = rawClusterObj->x * invQFormat;
    double y = rawClusterObj->y * invQFormat;
    double x_size = rawClusterObj->x_size * invQFormat;
    double y_size = rawClusterObj->y_size * invQFormat;

    double x_loc;
    double y_loc;

    clusterObj.x_loc = x_loc;
    clusterObj.y_loc = y_loc;
    _clusters.push_back(clusterObj);
}

void SrrPacket::extractTracker(const char *ptr, uint16_t oneQFromat) {
    Tracker_t trackerObj;
    const __tracker_t *rawTrackerObj = (const __tracker_t *)ptr;
    double invQFormat = 1.0 / (1<<oneQFromat);

    trackerObj.x = rawTrackerObj->x * invQFormat;
    trackerObj.y = rawTrackerObj->y * invQFormat;
    trackerObj.vx = rawTrackerObj->xd * invQFormat;
    trackerObj.vy = rawTrackerObj->yd * invQFormat;

    double x_size = rawTrackerObj->xSize * invQFormat;
    double y_size = rawTrackerObj->ySize * invQFormat;
    double x_loc;
    double y_loc;

    trackerObj.cluster_x_loc = x_loc;
    trackerObj.cluster_y_loc = y_loc;
    trackerObj.range = std::sqrt(trackerObj.x * trackerObj.x + trackerObj.y * trackerObj.y);
    trackerObj.doppler = (trackerObj.x * trackerObj.vx + trackerObj.y*trackerObj.vy) / trackerObj.range;
}
