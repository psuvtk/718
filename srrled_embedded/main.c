#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./include/radar_device.h"
#include "./include/radar_packet.h"
#include "./include/bytearray.h"

#define MAX_BUF_LEN 1024
#define SYNC_LEN 8


void parse_packet(bytearray_t *buf, int next);

int main (int argc, char *argv[]) {
    if (sensor_start()) {
        printf("[+] sensor start success.\n");
    } else {
        perror("\nsensor start failed.");
        exit(-1);
    }

    int fd;
    if (device_open(&fd)) {
        printf("[+] device open success.\n");
    } else {
        perror("device open failed.");
        exit(-1);
    }

    bytearray_t framebuf;
    bytearray_init(&framebuf);

    char recvbuf[MAX_BUF_LEN];
    const char sync[] = {0x02, 0x01, 0x04, 0x03, 0x06, 0x05, 0x08, 0x07};

    while (1) {
        printf("\n\n---------------------------------------------");
        int num_recv = read(fd, recvbuf, MAX_BUF_LEN);
        bytearray_append(&framebuf, recvbuf, num_recv);

        if (bytearray_startswith(&framebuf, sync, SYNC_LEN)) {
            printf("start with sync\n");
            // for (int i=0; i < framebuf.size; i++) {
            //         printf("%02x ", (framebuf.data)[i]);
            //     }
            int next = bytearray_find(&framebuf, sync, 8, 8);
            if (next != -1) {
                // deal_with_a_frame
                parse_packet(&framebuf, next);
                bytearray_lremove(&framebuf, next);
            } else {
                printf("not find next \n");
                continue;
            }
        } else {
            printf("Not start with sync\n");
            int len = bytearray_find(&framebuf, sync, 8, 0);
            if (len == -1)
                bytearray_clear(&framebuf);
            else
                bytearray_lremove(&framebuf, len);
        }
    }

    bytearray_destroy(&framebuf);
    return 0;
}


void parse_packet(bytearray_t *buf, int next) {
    packet_t packet;

    packet_parse(&packet, buf->data, next);
    printf("Frame NUmber: %d\n", packet.header.frameNumber);
    // for (int i=0; i < framebuf.size; i++) {
    //         printf("%02x ", (framebuf.data)[i]);
    // }
}