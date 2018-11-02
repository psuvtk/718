#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./include/radar_device.h"

#define MAX_BUF_LEN 2048

int main () {
    char recvbuf[MAX_BUF_LEN];
    char framebuf[MAX_BUF_LEN];

    sensor_start();

    int fd;
    device_open(&fd);

    while(1) {
        int n = read(fd, recvbuf, 1024);
        
        for (int i = 0; i < n; i++) {
            printf("%02x ", recvbuf[i]);
        }
    }

    return 0;
}
