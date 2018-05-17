#ifndef RADARPACKET_H
#define RADARPACKET_H

#include <QtCore>
#include <cstddef>

struct RadarPacket {
public:
    static const size_t sizeFrameHeader = 52;
    static const size_t sizeTlvHeader = 8;
    static const size_t sizePointStruct= 16;
    static const size_t sizeTargetStruct = 68;
    static const size_t sizeIndexStruct = 1;

    enum TLVTYPE {POINT_CLOUD_2D=0x06, TARGET_LIST_2D=0x07, TARGET_INDEX=0x08};



    static const char *getFrameBody(const char *p) { return p+52; }

    // 一些其他的数据
    static const QByteArray getSync(){ return QByteArray::fromHex("0201040306050807");}

    //p 指向 完整帧结构
    static quint32 getTimeStamp(const char *p) { return *(quint32*)(p+16);}
    static quint32 getPacketLength(const char *p) { return *(quint32*)(p+20); }
    static quint16 getNumTLVs(const char *p) { return *(quint16*)(p+48); }


    // p 指向 TLV Header结构
    static TLVTYPE getTlvHeaderType(const char *p) {
        switch (*(quint32*)p) {
        case 0x06:
            return TLVTYPE::POINT_CLOUD_2D;
            break;
        case 0x07:
            return TLVTYPE::TARGET_LIST_2D;
            break;
        case 0x08:
            return TLVTYPE::TARGET_INDEX;
            break;
        default:
            break;
        }
    }
    static quint32 getTlvHeaderLength(const char *p) { return *(quint32*)(p+4); }

    // p 指向 PointCloud结构
    static float getPointRange(const char *p) { return *(float*)p; }
    static float getPointAzimuth(const char *p) { return *(float*)(p+4); }
    static float getPointDoppler(const char *p) { return *(float*)(p+8); }
    static float getPointSnr(const char *p) { return *(float*)(p+12); }

    // p 指向TargetList结构
    static quint32 getTargetTid(const char *p) { return *(quint32*)p; }
    static float getTargetPosX(const char *p) { return *(float*)(p+4); }
    static float getTargetPosY(const char *p) { return *(float*)(p+8); }
    static float getTargetVelX(const char *p) { return *(float*)(p+12); }
    static float getTargetVelY(const char *p) { return *(float*)(p+16); }
    static float getTargetAccX(const char *p) { return *(float*)(p+20); }
    static float getTargetAccY(const char *p) { return *(float*)(p+24); }
    static float *getTarget(const char *p) { return (float*)(p+28); }
    static float getTargetGain(const char *p) { return *(float*)(p+64); }
};

#endif // RADARPACKET_H
