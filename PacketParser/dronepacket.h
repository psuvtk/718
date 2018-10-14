#ifndef DRONEPACKET_H
#define DRONEPACKET_H

#include <vector>
#include <cmath>
#include <cinttypes>

using std::vector;

struct DetObj_t
{
  double x;
  double y;
};

class DronePacket {
    const uint32_t HEAD_STRUCT_SIZE_BYTES = 36;
    const uint32_t TL_STRUCT_SIZE_BYTES = 8;
    const uint32_t DESCR_STRUCT_SIZE_BYTES = 4;

    const uint32_t OBJ_STRUCT_SIZE_BYTES = 12;

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
    };

    struct __tl_t
    {
       uint32_t type;
       uint32_t length;
    };

    struct __dataObjDescr_t
    {
        uint16_t numObj;
        uint16_t xyzQFormat;
    };

    struct __detObj_t
    {
      uint16_t rangIdx;
      int16_t dopplerIdx;
      uint16_t peakVal;
      int16_t x;
      int16_t y;
      int16_t z;
    };

public:
    DronePacket() = delete;
    explicit DronePacket(const char *pDronePacket, int length);
    bool isValid() const { return _valid; }

    void parse(const char *pDronePacket);
    uint32_t getVersion() const { return _pHeader->version; }
    uint32_t getTotalPacketLen() const { return _pHeader->totalPacketLen; }
    uint32_t getPlatform() const { return _pHeader->platform; }
    uint32_t getFrameNumber() const { return _pHeader->frameNumber; }
    uint32_t getTimeCpuCycles() const { return _pHeader->timeCpuCycles; }
    uint32_t getNumDetectedObj() const { return _pHeader->numDetectedObj; }
    uint32_t getNumTLVs() const { return _pHeader->numTLVs; }

    const vector<DetObj_t>& getDetObjs() const;

private:
    uint32_t getTlvType(const char *tl) const { return reinterpret_cast<const __tl_t*>(tl)->type; }
    uint32_t getTlvLength(const char *tl) const { return reinterpret_cast<const __tl_t*>(tl)->length; }
    uint16_t getDescrNumObj(const char *descr) const { return reinterpret_cast<const __dataObjDescr_t*>(descr)->numObj; }
    uint16_t getDescrQFormat(const char *descr) const { return reinterpret_cast<const __dataObjDescr_t*>(descr)->xyzQFormat; }

    void extractDetObj(const char *tl);


private:
    const struct __header_t *_pHeader = nullptr;
    vector<DetObj_t> _detObjs;

    bool _valid = true;
};

#endif // DRONEPACKET_H
