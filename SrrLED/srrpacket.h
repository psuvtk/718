#ifndef SRRPACKET_H
#define SRRPACKET_H

#include <vector>
#include <cmath>
#include <cinttypes>

using std::vector;

struct DetObj_t
{
    double x;
    double y;
    double doppler;
    double peakVal;
    double range;
};

struct Cluster_t
{
    double xCenter;
    double yCenter;
    double xSize;
    double ySize;

};

struct Tracker_t
{
    double x;
    double y;
    double vx;
    double vy;
    double xSize;
    double ySize;
    double range;
    double doppler;
};

struct ParkingAssistBin_t
{
    double x1;
    double x2;
    double y1;
    double y2;
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
        MMWDEMO_UART_MSG_DETECTED_POINTS    = 1,     // 目标
        MMWDEMO_UART_MSG_CLUSTERS           = 2,     // 聚类
        MMWDEMO_UART_MSG_TRACKED_OBJ        = 3,     // 跟踪
        MMWDEMO_UART_MSG_PARKING_ASSIST     = 4,     // 停车辅助
        MMWDEMO_UART_MSG_STATS              = 6,     // 状态信息
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
       uint32_t type;          // 类型
       uint32_t length;        // 长度
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
        uint16_t numObj;
        uint16_t xyzQFormat;
    };

public:
    SrrPacket() = delete;
    explicit SrrPacket(const char *pSrrPacket, int length);
    bool isValid() const { return _valid; }

    void parse(const char *pSrrPacket);
    uint32_t getVersion() const { return _pHeader->version; }
    uint32_t getTotalPacketLen() const { return _pHeader->totalPacketLen; }
    uint32_t getPlatform() const { return _pHeader->platform; }
    uint32_t getFrameNumber() const { return _pHeader->frameNumber; }
    uint32_t getTimeCpuCycles() const { return _pHeader->timeCpuCycles; }
    uint32_t getNumDetectedObj() const { return _pHeader->numDetectedObj; }
    uint32_t getNumTLVs() const { return _pHeader->numTLVs; }
    uint32_t getSubframeNumber() const { return _pHeader->subFrameNumber; }

    vector<DetObj_t>& getDetObjs() { return _detObjs; }
    vector<Tracker_t>& getTackers() { return _trackers; }
    vector<Cluster_t>& getClusters() { return _clusters; }
    vector<ParkingAssistBin_t>& getParkingAssistBins() { return _parkingAssistBins; }
    StatsInfo_t getStatsInfo() { return _statsInfo; }

    void setDetObjs(const vector<DetObj_t> &detObjs);


private:
    uint32_t getTlvType(const char *tl) const { return reinterpret_cast<const __tl_t*>(tl)->type; }
    uint32_t getTlvLength(const char *tl) const { return reinterpret_cast<const __tl_t*>(tl)->length; }
    uint16_t getDescrNumObj(const char *descr) const { return reinterpret_cast<const __dataObjDescr_t*>(descr)->numObj; }
    uint16_t getDescrQFormat(const char *descr) const { return reinterpret_cast<const __dataObjDescr_t*>(descr)->xyzQFormat; }

    void extractDetObj(const char *tl);
    void extractCluster(const char *tl);
    void extractTracker(const char *tl);
    void extractStatsInfo(const char *tl);
    void extractParkingAssisBin(const char *tl);

private:
    const struct __header_t *_pHeader = nullptr;

    vector<DetObj_t> _detObjs;
    vector<Cluster_t> _clusters;
    vector<Tracker_t> _trackers;
    vector<ParkingAssistBin_t> _parkingAssistBins;
    StatsInfo_t _statsInfo;

    bool _valid = true;
};

#endif // SRRPACKET_H
