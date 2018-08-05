#pragma once

#include <string>
#include <vector>
#include <map>

#include "DES.h"

typedef unsigned char byte;

namespace MyUtils
{
    class TransParaFactoryUtil
    {
    public:
        typedef enum _ResType
        {
            UnknownRes = -1,
            RadarPowerOnRes,
            RadarPowerOffRes,
            AisPowerOnRes,
            AisPowerOffRes,
        }ResType;

    public:
        static std::string DESEncrypt(const std::string& str, const std::string& key = "39dd194d");
        static std::string DESDecrypt(const std::string& encStr, const std::string& key = "39dd194d");
        static std::string MD5Encrypt(const std::string& str);
        static std::vector<byte> Base64Decode(const std::string& encStr);
		static std::string Base64Encode(unsigned char const*, unsigned int len);
        static std::string GetGMT8ShortDateString();
        static std::vector<std::string> SplitString(const std::string& str, const std::string& splitter);
        static void ReplaceString(std::string& str, const std::string srcStr, const std::string dstStr);
        static std::vector<byte> Wrap(const ResType& restype);
        static bool TryParse(const std::vector<byte>& buffer, std::map<std::string, std::string>& obj);
		static unsigned int getPaddingLength(std::string str);
    private:
        static CDES _DES;

    private:
        TransParaFactoryUtil()
        {

        }
    };
}