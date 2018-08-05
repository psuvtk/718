#include <time.h>
#include <algorithm>
#include <memory>
#include "TransParaFactoryUtil.h"
#include "md5.h"

MyUtils::CDES MyUtils::TransParaFactoryUtil::_DES;
static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}
std::string MyUtils::TransParaFactoryUtil::DESEncrypt(const std::string& str, const std::string& key)
{
	const char* ch_value = str.c_str();
	const char* ch_key = key.c_str();
	unsigned int paddinglen = getPaddingLength(str);
	
	char *out = (char *)malloc(sizeof(char)* paddinglen);
	_DES.RunPad(2, ch_value, str.length(), out, paddinglen);

	char *re = (char *)malloc(sizeof(char)* paddinglen);
	unsigned char keylen = 8;
	_DES.RunDes(0, 0, out, re, paddinglen, ch_key, keylen);

	std::string res;
	res = MyUtils::TransParaFactoryUtil::Base64Encode(reinterpret_cast<const unsigned char*>(re), paddinglen); //base64��ʾ

	free(out);
	out = NULL;
	free(re);
	re = NULL;
	return res;
}

unsigned int MyUtils::TransParaFactoryUtil::getPaddingLength(std::string str)
{
	unsigned int re = 0;
	if (str.length() % 8 != 0)
	{
		re = (str.length() / 8 + 1) * 8;
	}
	else{
		re = str.length() + 8;
	}
	return re;
}

std::string MyUtils::TransParaFactoryUtil::DESDecrypt(const std::string& encStr, const std::string& key)
{
	std::vector<byte> tem = MyUtils::TransParaFactoryUtil::Base64Decode(encStr);

	char *in = (char *)malloc(sizeof(char)* tem.size());
	memcpy(&in[0], tem.data(), tem.size());
	char *out = (char *)malloc(sizeof(char)* tem.size());

	unsigned char keylen = 8;

	_DES.RunDes(1, 0, in, out, tem.size(), key.c_str(), keylen);

	auto str = std::string(out);

	free(in);
	in = NULL;
	free(out);
	out = NULL;

    return str;
}

std::string MyUtils::TransParaFactoryUtil::MD5Encrypt(const std::string& str)
{
    std::string encStr;
	encStr = MD5(str).toStr();
    return encStr;
}

std::string MyUtils::TransParaFactoryUtil::Base64Encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';

	}

	return ret;

}
std::vector<byte> MyUtils::TransParaFactoryUtil::Base64Decode(const std::string& encStr)
{
	std::vector<byte> vec;

	int in_len = encStr.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];

	while (in_len-- && (encStr[in_] != '=') && is_base64(encStr[in_])) {
		char_array_4[i++] = encStr[in_]; in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
				vec.push_back(char_array_3[i]);
            i = 0;
        }
    }
    
    if (i) {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++)
			vec.push_back(char_array_3[j]);
    }

    return vec;
}

std::string MyUtils::TransParaFactoryUtil::GetGMT8ShortDateString()
{
/*
    time_t obj;
    time(&obj);
    obj += 8 * 3600;
    char buffer[64];
    std::auto_ptr<tm> ptr(new tm);
    gmtime_s(ptr.get(), &obj);
    ::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ptr.get());
*/
    return "2010-01-01 00:00:00";
}

std::vector<std::string> MyUtils::TransParaFactoryUtil::SplitString(const std::string& str, const std::string& splitter)
{
    std::vector<std::string> vec;
    std::string::size_type pos1, pos2;
    pos2 = str.find(splitter);
    pos1 = 0;
    while (std::string::npos != pos2)
    {
        vec.push_back(str.substr(pos1, pos2 - pos1));

        pos1 = pos2 + splitter.size();
        pos2 = str.find(splitter, pos1);
    }
    if (pos1 != str.length())
        vec.push_back(str.substr(pos1));
    return vec;
}

void MyUtils::TransParaFactoryUtil::ReplaceString(std::string& str, const std::string srcStr, const std::string dstStr)
{
    std::string::size_type pos = 0;
    while ((pos = str.find(srcStr, pos)) != std::string::npos)
    {
        str.replace(pos, srcStr.size(), dstStr);
        pos += dstStr.size();
    }
}

std::vector<byte> MyUtils::TransParaFactoryUtil::Wrap(const ResType& restype)
{
    std::vector<byte> buffer;

    std::map<std::string, std::string> obj;
    obj.insert(std::make_pair("time", MyUtils::TransParaFactoryUtil::GetGMT8ShortDateString()));
    obj.insert(std::make_pair("attach", ""));

    switch (restype)
    {
    case ResType::RadarPowerOnRes:
        obj.insert(std::make_pair("type", "radarpoweronres"));
        break;
    case ResType::RadarPowerOffRes:
        obj.insert(std::make_pair("type", "radarpoweroffres"));
        break;
    default:
        return buffer;
    }
    obj.insert(std::make_pair("auth", MyUtils::TransParaFactoryUtil::MD5Encrypt("39dd194d" + obj["type"] + obj["time"])));

    std::string str;
    std::for_each(obj.begin(), obj.end(), [&str](std::pair<std::string, std::string> item)
    {
        ReplaceString(item.second, "/", "@S");
        ReplaceString(item.second, "@", "@A");
        str += item.first + "@=" + item.second + "/";
    });

    //�ȼ���
    auto encStr = DESEncrypt(str);

    buffer.resize(8);

    //��encStr����ת��ΪС��������push��buffer
    auto size = encStr.size();
    ::memcpy(&buffer.data()[0], &size, 4);
    size = 718;
    ::memcpy(&buffer.data()[4], &size, 4);
    for (int i = 0; i != encStr.size(); ++i)
        buffer.push_back(encStr[i]);
    buffer.push_back(0x00);

    return buffer;
}

bool MyUtils::TransParaFactoryUtil::TryParse(const std::vector<byte>& buffer, std::map<std::string, std::string>& obj)
{
    obj.clear();

    if (buffer.size() <= 9)
        return false;
    if (buffer[4] != 0xce || buffer[5] != 0x02 || buffer.back() != 0x00)
        return false;

    int size = 0;
    ::memcpy(&size, &buffer.data()[0], 4);
    if (size <= 0)
        return false;

    auto str = DESDecrypt(std::string((char*)&buffer.data()[8]));
    
    auto vec = SplitString(str, "/");
	vec.pop_back();
    std::for_each(vec.begin(), vec.end(), [&obj](std::string item)
    {
        auto subVec = SplitString(item, "@=");
		if (subVec.size() != 1)
		{
			ReplaceString(subVec[1], "@A", "@");
			ReplaceString(subVec[1], "@S", "/");
		}
        obj.insert(std::make_pair(subVec[0], subVec.size() == 1 ? "" : subVec[1]));
    });

	if (obj.size() == 0)
		return false;

	auto iter = obj.find("type");
	if (iter == obj.end())
		return false;
	iter = obj.find("time");
	if (iter == obj.end())
		return false;
	iter = obj.find("auth");
	if (iter == obj.end())
		return false;
	iter = obj.find("attach");
	if (iter == obj.end())
		return false;

    return MyUtils::TransParaFactoryUtil::MD5Encrypt("39dd194d" + obj["type"] + obj["time"]) == obj["auth"];
}
