#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

#include "./include/radar_device.h"
#include "./include/radar_packet.h"
#include "./include/bytearray.h"

#define MAX_BUF_LEN 1024
#define SYNC_LEN 8

#define DEBUG 1

typedef struct {
    double speed;
    bool isexceed;
} speedinfo_t;


speedinfo_t speedinfo = {0.0f, false};


void timer_handler(int signum);
static void led_init();
static void led_write(double speed, bool isexceed);
static void deal_with_packet(packet_t *packet, speedinfo_t *speedinfo);
static int _compIncrence(const void *x, const void *y);

int main (int argc, char *argv[]) {
    if (sensor_start()) {
        printf("[+] sensor start success.\n");
    } else {
        perror("\nsensor start failed.");
        goto clear_and_exit;
    }

    int fd;
    if (device_open(&fd)) {
        printf("[+] device open success.\n");
    } else {
        perror("device open failed.");
        goto clear_and_exit;
    }
    
    // 定时器函数
    struct sigaction sa;
    struct itimerval timer;

    memset(&sa, 0, sizeof (sa));
    sa.sa_handler = &timer_handler;
    sigaction(SIGVTALRM, &sa, NULL);

    /**
     * TODO: 计时不准确，原因未知
     * 实际延时大约15倍于设置值
     */
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 100000;
    // 定时间隔
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 400000;
    setitimer(ITIMER_VIRTUAL, &timer, NULL);
    printf("[+] timer start.");

    char recvbuf[MAX_BUF_LEN];
    const char sync[] = {0x02, 0x01, 0x04, 0x03, 0x06, 0x05, 0x08, 0x07};

    packet_t packet;
    packet_init(&packet);
    bytearray_t framebuf;
    bytearray_init(&framebuf);

    /**
     * 将串口数据追加到缓冲(bytearray)，然后：
     * 1. 如果缓冲区以sync code开始，并存在另一个sync code，则存在完整一帧数据，进行解析；
     * 2. 如果缓冲区以sync code开始，且不存在另一个sync code，继续读取；
     * 3. 如果缓冲区不以sync code开始，且不存在另一个sync code，则清空缓冲区；
     * 4. 如果缓冲区不以sync code开始，并存在sync code，则将sync code之前的数据删除。
     */

    while (1) {
        int num_recv = read(fd, recvbuf, MAX_BUF_LEN);
        if (num_recv == 0) continue;

        bytearray_append(&framebuf, recvbuf, num_recv);
        if (bytearray_startswith(&framebuf, sync, SYNC_LEN)) {
            int next = bytearray_find(&framebuf, sync, 8, 8);
            if (next != -1) {
                // 得到完整一帧，进行处理
                packet_parse(&packet, framebuf.data, next);
                
                speedinfo.speed = 0.0f;
                speedinfo.isexceed = false;

                if (packet.isvalid) {
                    deal_with_packet(&packet, &speedinfo);
                    // led_write(speedinfo.speed, speedinfo.isexceed);
                } else {
                    printf("Broken packet\n");
                }
                // 移除处理完成的帧数据
                packet_clear(&packet);
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

clear_and_exit:
    perror("clear and exit...");
    device_close(fd);
    bytearray_destroy(&framebuf);
    return 0;
}

static void led_write(double speed, bool isexceed) {
    // not implement
    printf("Speed: %lf, ", speed);
    if (isexceed) {
        printf("Isexceed: true\n");
    } else {
        printf("Isexceed: false\n");
    }
}

static void deal_with_packet(packet_t *packet, speedinfo_t *speedinfo) {
    if (!packet->isvalid ||packet->trackers == NULL) return;

    /**
     * 如果使用qsort会导致 segment fault, 具体原因未知
     * 反正数据量很小, 手动排序一下
     */
    {
        Tracker_t tmp;
        for (int i = 0; i < packet->nTracker-1; i++) {
            for (int j = i+1; j < packet->nTracker; j++) {
                if ((packet->trackers)[i].range  > (packet->trackers)[i].range) {
                    memcpy(&tmp, &(packet->trackers)[i], sizeof(Tracker_t));
                    memcpy(&(packet->trackers)[i], &(packet->trackers)[j], sizeof(Tracker_t));
                    memcpy(&(packet->trackers)[j], &tmp, sizeof(Tracker_t));
                }
            }
        }
    }

    static int acc_counter = 0;
    static double acc_doppler = 0;
    const double detect_threshhold = 1.0;
    const double exceed_threshhold = 3.0;
    double doppler = 0;
    
    // printf("\n\nFrame Number: %d\n", packet->header.frameNumber);

    for (int i=0; i < packet->nTracker; i++) {
#ifdef DEBUG
    // printf("#%d: Range: %lf; Doppler:%lf\n", i, (packet->trackers[i]).range, (packet->trackers[i]).doppler *3.6);
#endif
        doppler = (packet->trackers[i]).doppler * 3.6;
        if (abs(doppler) > detect_threshhold && doppler < 0) { 
            acc_counter++;
            acc_doppler += abs(doppler);
            break;
        }
    }

    // 对检测到目标的10针进行平滑
    if (acc_counter > 5) {
        // printf("\n\nFrame Number: %d\n", packet->header.frameNumber);
        // printf("Doppler: %lf\n", acc_doppler/10.0f);

        speedinfo->speed = acc_doppler /= 10.0f;

        if (acc_doppler > exceed_threshhold * 1.1f)
            speedinfo->isexceed = true;
        else
            speedinfo->isexceed = false;

        led_write(speedinfo->speed, speedinfo->isexceed);
        acc_counter = 0;
        acc_doppler = 0;
    }
}

void timer_handler (int signum) {
    led_write(speedinfo.speed, speedinfo.isexceed);
}


