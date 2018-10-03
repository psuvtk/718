#ifndef SRRPACKET_H
#define SRRPACKET_H

#include <vector>
#include <cmath>
#include <cinttypes>

using std::vector;

struct DetObj_t
{
    double  doppler;        /*!< @brief Doppler index */
    double peakVal;     /*!< @brief Peak value */
    double  x;             /*!< @brief x - coordinate in meters. */
    double  y;             /*!< @brief y - coordinate in meters.  */
    double range;
};

struct Cluster_t
{
    double x_loc;               /**< the clustering center on x direction */
    double y_loc;               /**< the clustering center on y direction */
};

struct Tracker_t
{
    double x;                  /**< the tracking output -> x co-ordinate */
    double y;                  /**< the tracking output -> y co-ordinate */
    double vx;                 /**< velocity in the x direction */
    double vy;                 /**< velocity in the y direction */
    double cluster_x_loc;
    double cluster_y_loc;
    double range;
    double doppler;
};



struct ParkingAssistBin_t
{

};

struct StatsInfo_t
{
    uint32_t interFrameProcessingTime;
    uint32_t transmitOutputTime;
    uint32_t interFrameProcessingMargin;
    uint32_t interChirpProcessingMargin;
    uint32_t activeFrameCpuLoad;
    uint32_t interFrameCpuLoad;
};

class SrrPacket {
    const uint32_t HEAD_STRUCT_SIZE_BYTES = 40;
    const uint32_t TL_STRUCT_SIZE_BYTES = 8;
    const uint32_t DESCR_STRUCT_SIZE_BYTES = 4;

    const uint32_t OBJ_STRUCT_SIZE_BYTES = 8;
    const uint32_t CLUSTER_STRUCT_SIZE_BYTES = 8;
    const uint32_t TRACKER_STRUCT_SIZE_BYTES = 12;
    const uint32_t PARKING_ASSIST_BIN_SIZE_BYTES = 2;
    const uint32_t STATS_SIZE_BYTES = 16;

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
        uint16_t magicWord[4];
        uint32_t version;
        uint32_t totalPacketLen;
        uint32_t platform;
        uint32_t frameNumber;
        uint32_t timeCpuCycles;
        uint32_t numDetectedObj;
        uint32_t numTLVs;
        uint32_t subFrameNumber;
    };

    struct __tl_t
    {
       uint32_t type;
       uint32_t length;
    };

    struct __detObj_t
    {
        int16_t  speed;        /*!< @brief Doppler index */
        uint16_t peakVal;     /*!< @brief Peak value */
        int16_t  x;             /*!< @brief x - coordinate in meters. */
        int16_t  y;             /*!< @brief y - coordinate in meters.  */
    };

    struct __cluster_t
    {
        int16_t x;               /**< the clustering center on x direction */
        int16_t y;               /**< the clustering center on y direction */
        int16_t x_size;                 /**< the clustering size on x direction */
        int16_t y_size;                 /**< the clustering size on y direction */
    };

    struct __tracker_t
    {
        int16_t x;                  /**< the tracking output -> x co-ordinate */
        int16_t y;                  /**< the tracking output -> y co-ordinate */
        int16_t xd;                 /**< velocity in the x direction */
        int16_t yd;                 /**< velocity in the y direction */
        int16_t xSize;              /**< cluster size (x direction). */
        int16_t ySize;              /**< cluster size (y direction). */
    };



    struct __parkingAssistBin_t
    {
        uint16_t range;
    };

    struct __statsInfo_t
    {
        uint32_t interFrameProcessingTime;
        uint32_t transmitOutputTime;
        uint32_t interFrameProcessingMargin;
        uint32_t interChirpProcessingMargin;
        uint32_t activeFrameCpuLoad;
        uint32_t interFrameCpuLoad;
    };

    struct __dataObjDescr_t
    {
        /* Number of detected object*/
        uint16_t numObj;
        /* Q format of detected objects x/y/z coordinates */
        uint16_t xyzQFormat;
    };

public:
    SrrPacket() = delete;
    explicit SrrPacket(const char *pSrrPacket);

    void query();

    uint32_t getVersion() { return ((struct __header_t *)_pSrrPacket)->version; }
    uint32_t getTotalPacketLen() { return ((struct __header_t *)_pSrrPacket)->totalPacketLen; }
    uint32_t getPlatform() { return ((struct __header_t *)_pSrrPacket)->platform; }
    uint32_t getFrameNumber() { return ((struct __header_t *)_pSrrPacket)->frameNumber; }
    uint32_t getTimeCpuCycles() { return ((struct __header_t *)_pSrrPacket)->timeCpuCycles; }
    uint32_t getNumDetectedObj() { return ((struct __header_t *)_pSrrPacket)->numDetectedObj; }
    uint32_t getNumTLVs() { return ((struct __header_t *)_pSrrPacket)->numTLVs; }
    uint32_t getSubframeNumber() { return ((struct __header_t *)_pSrrPacket)->subFrameNumber; }

    vector<DetObj_t>& getDetObjs() { return _detObjs; }
    vector<Tracker_t>& getTackers() { return _trackers; }
    vector<Cluster_t>& getClusters() { return _clusters; }
    vector<ParkingAssistBin_t>& getParkingAssistBins() { return _parkingAssistBins; }
    StatsInfo_t getStatsInfo() { return _statsInfo; }

private:
    uint32_t getTlvType(const char *p) { return ((const struct __tl_t *)p)->type; }
    uint32_t getTlvLength(const char *p) { return ((const struct __tl_t *)p)->length; }
    uint32_t getDescrNumObj(const char *p) { return ((const struct __dataObjDescr_t *)p)->numObj; }
    uint32_t getDescrQFormat(const char *p) { return ((const struct __dataObjDescr_t *)p)->xyzQFormat; }

    void extractDetObj(const char *ptr, uint16_t oneQFromat);
    void extractCluster(const char *ptr, uint16_t oneQFromat);
    void extractTracker(const char *ptr, uint16_t oneQFromat);
    void extractStatsInfo(const char *ptr, uint16_t oneQFromat);
    void extractParkingAssisBin(const char *ptr);

private:
    const char *_pSrrPacket;

    vector<DetObj_t> _detObjs;
    vector<Cluster_t> _clusters;
    vector<Tracker_t> _trackers;
    vector<ParkingAssistBin_t> _parkingAssistBins;
    StatsInfo_t _statsInfo;
};

#endif // SRRPACKET_H


