#include "srrpacket.h"
#include <QDebug>

SrrPacket::SrrPacket()
{

}

SrrPacket::SrrPacket(const char *pSrrPacket) {
    int errCount = 0;
    _pHeader = reinterpret_cast<const struct __header_t*>(pSrrPacket);

    const char *tl = pSrrPacket + HEAD_STRUCT_SIZE_BYTES;
//    query();

    for (uint32_t i = 0; i < getNumTLVs(); i++) {
        uint32_t type = getTlvType(tl);
        uint32_t len = getTlvLength(tl);
        tl += TL_STRUCT_SIZE_BYTES;

        switch (type) {
        case __TLV_Type::MMWDEMO_UART_MSG_DETECTED_POINTS:
//            qDebug() << "tlvType => MMWDEMO_UART_MSG_DETECTED_POINTS";
            extractDetObj(tl);
            tl += len;
            break;
        case __TLV_Type::MMWDEMO_UART_MSG_CLUSTERS:
//            qDebug() << "tlvType => MMWDEMO_UART_MSG_CLUSTERS";
            extractCluster(tl);
            tl += len;
            break;
        case __TLV_Type::MMWDEMO_UART_MSG_TRACKED_OBJ:
//            qDebug() << "tlvType => MMWDEMO_UART_MSG_TRACKED_OBJ";
            extractTracker(tl);
            tl += len;
            break;
        case __TLV_Type::MMWDEMO_UART_MSG_PARKING_ASSIST:
//            qDebug() << "tlvType => MMWDEMO_UART_MSG_PARKING_ASSIST";
            extractParkingAssisBin(tl);
            tl += len;
            break;
        case __TLV_Type::MMWDEMO_UART_MSG_STATS:
//            qDebug() << "tlvType => MMWDEMO_UART_MSG_STATS";
            extractStatsInfo(tl);
            tl += STATS_SIZE_BYTES;
            break;
        default:
//            qDebug() << "tlvType => Unkown";
            _valid = false;
            if (errCount++ > 2 ) return;
            break;
        }
    }
}

void SrrPacket::query() {
    qDebug() << "\n\nPacket Header-------------";
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

void SrrPacket::extractDetObj(const char *tl) {
    uint16_t numObjs = getDescrNumObj(tl);
    uint16_t xyzQFormat = getDescrQFormat(tl);
    double invQFormat = 1.0 / (1 << xyzQFormat);
    tl += DESCR_STRUCT_SIZE_BYTES;
    const __detObj_t *rawDetObj = (const struct __detObj_t *)tl;

    for (uint16_t j = 0; j < numObjs; j++, rawDetObj++) {
        DetObj_t detObj;
        detObj.x = rawDetObj->x * invQFormat;
        detObj.y = rawDetObj->y * invQFormat;
        detObj.peakVal = rawDetObj->peakVal;
        detObj.doppler = rawDetObj->speed * invQFormat;
        detObj.range = std::sqrt(detObj.x * detObj.x + detObj.y * detObj.y);

        _detObjs.push_back(detObj);
    }
}

void SrrPacket::extractCluster(const char *tl) {
    uint16_t numObjs = getDescrNumObj(tl);
    uint16_t xyzQFormat = getDescrQFormat(tl);
    double invQFormat = 1.0 / (1 << xyzQFormat);
    tl += DESCR_STRUCT_SIZE_BYTES;

    const __cluster_t *rawClusterObj = (const struct __cluster_t *)tl;

    for (uint16_t i = 0; i < numObjs; i++, rawClusterObj++) {
        Cluster_t clusterObj;

        clusterObj.xCenter = rawClusterObj->x * invQFormat;
        clusterObj.yCenter = rawClusterObj->y * invQFormat;

        clusterObj.xSize = rawClusterObj->x_size * invQFormat;
        clusterObj.ySize = rawClusterObj->y_size * invQFormat;

        _clusters.push_back(clusterObj);
    }
}

void SrrPacket::extractTracker(const char *tl) {
    uint16_t numObjs = getDescrNumObj(tl);
    uint16_t xyzQFormat = getDescrQFormat(tl);
    double invQFormat = 1.0 / (1 << xyzQFormat);
    tl += DESCR_STRUCT_SIZE_BYTES;

    const __tracker_t *rawTrackerObj = (const __tracker_t *)tl;

    for (uint16_t i = 0; i < numObjs; i++, rawTrackerObj++) {
        Tracker_t trackerObj;

        trackerObj.x = rawTrackerObj->x * invQFormat;
        trackerObj.y = rawTrackerObj->y * invQFormat;
        trackerObj.vx = rawTrackerObj->xd * invQFormat;
        trackerObj.vy = rawTrackerObj->yd * invQFormat;

        trackerObj.xSize = rawTrackerObj->xSize * invQFormat;
        trackerObj.ySize = rawTrackerObj->ySize * invQFormat;

        trackerObj.range = std::sqrt(trackerObj.x * trackerObj.x + trackerObj.y * trackerObj.y);
        trackerObj.doppler = (trackerObj.x * trackerObj.vx + trackerObj.y*trackerObj.vy) / trackerObj.range;

        _trackers.push_back(trackerObj);
    }

}

void SrrPacket::extractParkingAssisBin(const char *tl)
{
    uint16_t numObjs = getDescrNumObj(tl);
    uint16_t xyzQFormat = getDescrQFormat(tl);
    double invQFormat = 1.0 / (1 << xyzQFormat);
    tl += DESCR_STRUCT_SIZE_BYTES;


    QVector<double> range;
    for (int i=0; i < numObjs; i++, tl+=PARKING_ASSIST_BIN_SIZE_BYTES) {
        range << *((uint16_t *)tl) * invQFormat;
    }

    // fftshift
    for (int i = 0; i < numObjs/2; i++) {
        double tmp = range[i];
        range[i] = range[i+numObjs/2];
        range[i+numObjs/2] = tmp;
    }
    range << range[0];

    QVector<double> xl, yl;
    for (int i=0; i<numObjs+2; i++) {
        xl << -1+i*2.0/(numObjs+1);
        yl << std::sqrt(1-xl[i] * xl[i]);
    }

    for (int i=0; i<numObjs+1; i++) {
        ParkingAssistBin_t p;
        p.x1 = range[i] * xl[i];
        p.x2 = range[i] * xl[i+1];
        p.y1 = range[i] * yl[i];
        p.y2 = range[i] * yl[i+1];
        _parkingAssistBins.push_back(p);
    }
}

void SrrPacket::extractStatsInfo(const char *tl)
{
    const __statsInfo_t *rawStatsInfo = reinterpret_cast<const __statsInfo_t *>(tl);

    _statsInfo.interFrameProcessingTime = rawStatsInfo->interFrameProcessingTime;
    _statsInfo.transmitOutputTime = rawStatsInfo->transmitOutputTime;
    _statsInfo.interFrameProcessingMargin = rawStatsInfo->interFrameProcessingMargin;
    _statsInfo.interChirpProcessingMargin = rawStatsInfo->interChirpProcessingMargin;
    _statsInfo.activeFrameCpuLoad = rawStatsInfo->activeFrameCpuLoad;
    _statsInfo.interFrameCpuLoad = rawStatsInfo->interFrameCpuLoad;
}
