#ifndef SRRPACKET_H
#define SRRPACKET_H

#include <QtCore>

enum TlvType
{
    MMWDEMO_UART_MSG_DETECTED_POINTS    = 1,
    MMWDEMO_UART_MSG_CLUSTERS           = 2,
    MMWDEMO_UART_MSG_TRACKED_OBJ        = 3,
    MMWDEMO_UART_MSG_PARKING_ASSIST     = 4,
    MMWDEMO_UART_MSG_STATS = 6,
};

struct DetObj_t
{
    qint16  speed;        /*!< @brief Doppler index */
    quint16 peakVal;     /*!< @brief Peak value */
    qint16  x;             /*!< @brief x - coordinate in meters. */
    qint16  y;             /*!< @brief y - coordinate in meters.  */
};


struct __DetObj_t
{
    qint16  speed;        /*!< @brief Doppler index */
    quint16 peakVal;     /*!< @brief Peak value */
    qint16  x;             /*!< @brief x - coordinate in meters. */
    qint16  y;             /*!< @brief y - coordinate in meters.  */
};

struct __Tracker_t
{
    qint16 x;                  /**< the tracking output -> x co-ordinate */
    qint16 y;                  /**< the tracking output -> y co-ordinate */
    qint16 xd;                 /**< velocity in the x direction */
    qint16 yd;                 /**< velocity in the y direction */
    qint16 xSize;              /**< cluster size (x direction). */
    qint16 ySize;              /**< cluster size (y direction). */
};

struct __Cluster_t
{
    qint16 xCenter;               /**< the clustering center on x direction */
    qint16 yCenter;               /**< the clustering center on y direction */
    qint16 xSize;                 /**< the clustering size on x direction */
    qint16 ySize;                 /**< the clustering size on y direction */
};

struct __ParkingAssistBin_t
{

};

struct __StatsInfo_t
{

};

struct __DataObjDescr_t
{
    quint16 numObj;
    quint16 oneQFormat;
};

struct SrrPacketHeader_t
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

struct TL_t
{
   quint32 type;
   quint32 length;
};

class SrrPacket {
    const quint32 HeaderLength = 40;
    const quint32 TlLength = 8;
public:
    SrrPacket() = delete;
    explicit SrrPacket(const char *pSrrPacket);

    void query();

    static QByteArray getSync() { return QByteArray::fromHex("0201040306050807");}

    quint32 getVersion() { return *(quint32*)(_pSrrPacketHeader+8); }
    quint32 getTotalPacketLen() { return *(quint32*)(_pSrrPacketHeader+12); }
    quint32 getPlatform() { return *(quint32*)(_pSrrPacketHeader+16); }
    quint32 getFrameNumber() { return *(quint32*)(_pSrrPacketHeader+20); }
    quint32 getTimeCpuCycles() { return *(quint32*)(_pSrrPacketHeader+24); }
    quint32 getNumDetectedObj() { return *(quint32*)(_pSrrPacketHeader+28); }
    quint32 getNumTLVs() { return *(quint32*)(_pSrrPacketHeader+32); }
    quint32 getSubframeNumber() { return *(quint32*)(_pSrrPacketHeader+36); }



//    QList<DetObj_t> getDetObjs();
//    QList<Tracker_t> getTackers();
//    QList<Clusters_t> getClusters();
//    getParkingAssistBins();
//    getStatsInfo();
private:
    quint32 getTlvType(const char *p) { return *(qint32*)(p); }
    quint32 getTlvLength(const char *p) { return *(qint32*)(p+4); }
    quint32 getDescrNumObj(const char *p) { return *(quint32*)(p); }
    quint32 getDescrOneQFormat(const char *p) { return *(quint32*)(p+4); }

    void extractDetObj();
    void extractCluster();
    void extractTracker();
    void extractStatsInfo();
    void extractParkingAssisBin();

private:
    const char *_pSrrPacketHeader;
//    const char *_pSrrPacketBody;
//    QList<__DetObj_t> _detObjs;
//    QList<__Cluster_t> _clusters;
//    QList<__Tracker_t> _trackers;
//    QList<__ParkingAssistBin_t> _parkingAssistBins;
//    QList<__StatsInfo_t> _statsInfo;
};

#endif // SRRPACKET_H


