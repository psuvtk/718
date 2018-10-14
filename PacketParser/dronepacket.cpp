#include "dronepacket.h"

DronePacket::DronePacket(const char *pDronePacket, int length) {
    _pHeader = reinterpret_cast<const struct __header_t*>(pDronePacket);
    const char *tl = pDronePacket + HEAD_STRUCT_SIZE_BYTES;

    if (static_cast<uint32_t>(length) != getTotalPacketLen()) {
        _valid = false;
        return;
    }

    for (uint32_t i = 0; i < getNumTLVs(); i++) {
        uint32_t type = getTlvType(tl);
        uint32_t len = getTlvLength(tl);
        tl += TL_STRUCT_SIZE_BYTES;

        switch (type) {
        case __TLV_Type::MMWDEMO_UART_MSG_DETECTED_POINTS:
            extractDetObj(tl);
            tl += len;
            break;
        // 其他类型
        default:
            _valid = false;
            return;
        }
    }
}

void DronePacket::extractDetObj(const char *tl) {
    uint16_t numObjs = getDescrNumObj(tl);
    uint16_t xyzQFormat = getDescrQFormat(tl);
    double invQFormat = 1.0 / (1 << xyzQFormat);
    tl += DESCR_STRUCT_SIZE_BYTES;

    const __detObj_t *rawDetObj = reinterpret_cast<const struct __detObj_t *>(tl);

    for (uint16_t j = 0; j < numObjs; j++, rawDetObj++) {
        DetObj_t detObj;
        detObj.x = rawDetObj->x * invQFormat;
        detObj.y = rawDetObj->y * invQFormat;
        _detObjs.push_back(detObj);
    }
}

const vector<DetObj_t>& DronePacket::getDetObjs() const
{
    return _detObjs;
}
