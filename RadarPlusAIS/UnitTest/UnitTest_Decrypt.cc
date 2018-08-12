#include "TransParaFactoryUtil.h"
#include <stdio.h>

int getBaudRate(std::string encrypt_attach) {
    std::vector<byte> attach = MyUtils::TransParaFactoryUtil::Base64Decode(encrypt_attach);
    return *(int*)(attach.data() + 4);
}

int main() {

    int br = getBaudRate("ZQAAAACWAAA=");

    printf("%d", br);

    return 0;
}