#ifndef SRRPACKET_H
#define SRRPACKET_H

#include <QtCore>
#include <cmath>

#define SYNC (QByteArray::fromHex("0201040306050807"))

struct DetObj_t
{
    double  speed;        /*!< @brief Doppler index */
    double peakVal;     /*!< @brief Peak value */
    double  x;             /*!< @brief x - coordinate in meters. */
    double  y;             /*!< @brief y - coordinate in meters.  */
    double range;
};

struct Tracker_t
{
    qint16 x;                  /**< the tracking output -> x co-ordinate */
    qint16 y;                  /**< the tracking output -> y co-ordinate */
    qint16 xd;                 /**< velocity in the x direction */
    qint16 yd;                 /**< velocity in the y direction */
    qint16 xSize;              /**< cluster size (x direction). */
    qint16 ySize;              /**< cluster size (y direction). */
};

struct Cluster_t
{
    qint16 xCenter;               /**< the clustering center on x direction */
    qint16 yCenter;               /**< the clustering center on y direction */
    qint16 xSize;                 /**< the clustering size on x direction */
    qint16 ySize;                 /**< the clustering size on y direction */
};

struct ParkingAssistBin_t
{

};

struct StatsInfo_t
{

};

class SrrPacket {
    const quint32 HEAD_STRUCT_SIZE_BYTES = 40;
    const quint32 TL_STRUCT_SIZE_BYTES = 8;
    const quint32 DESCR_STRUCT_SIZE_BYTES = 4;

    const quint32 OBJ_STRUCT_SIZE_BYTES = 8;
    const quint32 CLUSTER_STRUCT_SIZE_BYTES = 8;
    const quint32 TRACKER_STRUCT_SIZE_BYTES = 12;
    const quint32 PARKING_ASSIST_BIN_SIZE_BYTES = 2;
    const quint32 STATS_SIZE_BYTES = 16;

    enum __TLV_Type
    {
        MMWDEMO_UART_MSG_DETECTED_POINTS    = 1,
        MMWDEMO_UART_MSG_CLUSTERS           = 2,
        MMWDEMO_UART_MSG_TRACKED_OBJ        = 3,
        MMWDEMO_UART_MSG_PARKING_ASSIST     = 4,
        MMWDEMO_UART_MSG_STATS = 6,
    };

    struct __header_t
    {
        quint16 magicWord[4];
        quint32 version;
        quint32 totalPacketLen;
        quint32 platform;
        quint32 frameNumber;
        quint32 timeCpuCycles;
        quint32 numDetectedObj;
        quint32 numTLVs;
        quint32 subFrameNumber;
    };

    struct __tl_t
    {
       quint32 type;
       quint32 length;
    };

    struct __detObj_t
    {
        qint16  speed;        /*!< @brief Doppler index */
        quint16 peakVal;     /*!< @brief Peak value */
        qint16  x;             /*!< @brief x - coordinate in meters. */
        qint16  y;             /*!< @brief y - coordinate in meters.  */
    };

    struct __tracker_t
    {
        qint16 x;                  /**< the tracking output -> x co-ordinate */
        qint16 y;                  /**< the tracking output -> y co-ordinate */
        qint16 xd;                 /**< velocity in the x direction */
        qint16 yd;                 /**< velocity in the y direction */
        qint16 xSize;              /**< cluster size (x direction). */
        qint16 ySize;              /**< cluster size (y direction). */
    };

    struct __cluster_t
    {
        qint16 xCenter;               /**< the clustering center on x direction */
        qint16 yCenter;               /**< the clustering center on y direction */
        qint16 xSize;                 /**< the clustering size on x direction */
        qint16 ySize;                 /**< the clustering size on y direction */
    };

    struct __parkingAssistBin_t
    {

    };

    struct __statsInfo_t
    {

    };

    struct __dataObjDescr_t
    {
        /* Number of detected object*/
        quint16 numObj;
        /* Q format of detected objects x/y/z coordinates */
        quint16 xyzQFormat;
    };

public:
    SrrPacket() = delete;
    explicit SrrPacket(const char *pSrrPacket);

    void query();

    static QByteArray getSync() { return QByteArray::fromHex("0201040306050807");}

    quint32 getVersion() { return ((struct __header_t *)_pSrrPacket)->version; }
    quint32 getTotalPacketLen() { return ((struct __header_t *)_pSrrPacket)->totalPacketLen; }
    quint32 getPlatform() { return ((struct __header_t *)_pSrrPacket)->platform; }
    quint32 getFrameNumber() { return ((struct __header_t *)_pSrrPacket)->frameNumber; }
    quint32 getTimeCpuCycles() { return ((struct __header_t *)_pSrrPacket)->timeCpuCycles; }
    quint32 getNumDetectedObj() { return ((struct __header_t *)_pSrrPacket)->numDetectedObj; }
    quint32 getNumTLVs() { return ((struct __header_t *)_pSrrPacket)->numTLVs; }
    quint32 getSubframeNumber() { return ((struct __header_t *)_pSrrPacket)->subFrameNumber; }

    QList<DetObj_t> getDetObjs() { return _detObjs; }
    QList<Tracker_t> getTackers() { return _trackers; }
    QList<Cluster_t> getClusters() { return _clusters; }
    QList<ParkingAssistBin_t> getParkingAssistBins() { return _parkingAssistBins; }
    QList<StatsInfo_t> getStatsInfo() { return _statsInfo; }

private:
    quint32 getTlvType(const char *p) { return ((struct __tl_t *)p)->type; }
    quint32 getTlvLength(const char *p) { return ((struct __tl_t *)p)->length; }
    quint32 getDescrNumObj(const char *p) { return ((struct __dataObjDescr_t *)p)->numObj; }
    quint32 getDescrQFormat(const char *p) { return ((struct __dataObjDescr_t *)p)->xyzQFormat; }

    void extractDetObj(const char *ptr, quint16 oneQFromat);
    void extractCluster();
    void extractTracker();
    void extractStatsInfo();
    void extractParkingAssisBin();

private:
    const char *_pSrrPacket;

    QList<DetObj_t> _detObjs;
    QList<Cluster_t> _clusters;
    QList<Tracker_t> _trackers;
    QList<ParkingAssistBin_t> _parkingAssistBins;
    QList<StatsInfo_t> _statsInfo;

    quint32 _num;
};

#endif // SRRPACKET_H


