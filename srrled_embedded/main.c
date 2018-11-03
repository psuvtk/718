#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./include/radar_device.h"
#include "./include/bytearray.h"

#define MAX_BUF_LEN 2048
#define SYNC_LEN 8

int main (int argc, char *argv[]) {
    if (sensor_start()) {
        printf("[+] sensor start success.");
    } else {
        perror("\nsensor start failed.");
        exit(-1);
    }

    int fd;
    if (device_open(&fd)) {
        printf("[+] device open success.");
    } else {
        perror("device open failed.");
        exit(-1);
    }

    bytearray_t framebuf;
    char recvbuf[MAX_BUF_LEN];
    const char sync[] = {0x02, 0x01, 0x04, 0x03, 0x06, 0x05, 0x08, 0x07};

    while (1) {  
        int num_recv = read(fd, recvbuf, MAX_BUF_LEN);
        bytearray_append(&framebuf, recvbuf, num_recv);

        if (bytearray_startswith(&framebuf, sync, SYNC_LEN)) {
            int next = bytearray_find(&framebuf, sync, 8, 8);
            if (next != -1) {
                // deal_with_a_frame

                bytearray_lremove(&framebuf, next);
            } else {
                continue;
            }
        } else {
            int len = bytearray_find(&framebuf, sync, 8, 0);
            if (len == -1) 
                bytearray_clear(&framebuf);
            else
                bytearray_lremove(&framebuf, len);
        }
    }

    return 0;
}