#include "TransParaFactoryUtil.h"
#include "RadarPlusAIS.h"


#include <stdio.h>


gMCB_t gMcbInst;
char buf_ais[BUFLEN_AIS];
char buf_send[BUFLEN_AIS];
char buf_pc_ais[BUFLEN_AIS];

int configSocket();

int main() {

    ssize_t read_len = 0, send_len = 0;
    int cur_index = 0;

    if ( (gMcbInst.ais_serial_fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY)) < 0 ) {
        perror("serial port open faild");
        exit(-1);
    }

    configSerialPort(38400);
    configSocket();

    if ( pthread_create(
            &gMcbInst.thread_receive_pc_ais,
            NULL,
            ctrlPc2Ais,
            NULL)
         != 0) {
        perror("thread create failed");
        exit(-1);
    }


    printf("start listening:\n");

    bzero(buf_ais, BUFLEN_AIS);
    forever {
        if (gMcbInst.status_ais == STATUS_OFF) {
            sleep(5);
            continue;
        }

        // for (int i =0; i< 15; i++) {
        //     buf_send[i]=i+65;
        // }
        // buf_send[15] = '\n';
        // sendto(
        //     gMcbInst.socket_arm_pc_for_ais,
        //     buf_send,
        //     16,
        //     0,
        //     (struct sockaddr *)&gMcbInst.addr_pc_for_ais,
        //     sizeof(gMcbInst.addr_pc_for_ais)
        // );

        if ( (read_len = read(gMcbInst.ais_serial_fd, buf_ais, BUFLEN_AIS)) > 0 ) {

            if ( (BUFLEN_AIS - cur_index) >= read_len ) {
                memcpy(buf_send+cur_index, buf_ais, read_len);
                cur_index+=read_len;
            } else {
                send_len = sendto(
                    gMcbInst.socket_arm_pc_for_ais,
                    buf_send,
                    cur_index,
                    0,
                    (struct sockaddr *)&gMcbInst.addr_pc_for_ais,
                    sizeof(gMcbInst.addr_pc_for_ais)
                );
                printf("\n[DEBUG] [SEND] ");
                for (int i = 0; i < send_len; i++) {
                    printf("%c", buf_send[i]);
                }
                assert(send_len == cur_index);
                cur_index = 0;
            }
            bzero(buf_ais, BUFLEN_AIS);
        }
    }

    return 0;
}
int configSerialPort(int baudrate) {

    struct termios new_serial_setting, cur_serial_setting;

    new_serial_setting.c_cflag |= CREAD;

    // 设置波特率
    switch (baudrate) {
    case 2400:
        cfsetispeed(&new_serial_setting, B2400);
        cfsetospeed(&new_serial_setting, B2400);
        break;
    case 4800:
        cfsetispeed(&new_serial_setting, B4800);
        cfsetospeed(&new_serial_setting, B4800);
        break;
    case 9600:
        cfsetispeed(&new_serial_setting, B9600);
        cfsetospeed(&new_serial_setting, B9600);
        break;
    case 19200:
        cfsetispeed(&new_serial_setting, B19200);
        cfsetospeed(&new_serial_setting, B19200);
        break;
    case 38400:
        cfsetispeed(&new_serial_setting, B38400);
        cfsetospeed(&new_serial_setting, B38400);
        break;
    case 115200:
        cfsetispeed(&new_serial_setting, B115200);
        cfsetospeed(&new_serial_setting, B115200);
        break;
    default:
        cfsetispeed(&new_serial_setting, B38400);
        cfsetospeed(&new_serial_setting, B38400);
        perror("[THREAD  ctrlPc2Radar] :: not support designated baudrate");
        exit(-1);
        break;
    }

    cfmakeraw(&new_serial_setting);
    //设置等待时间和最小接收字符
    new_serial_setting.c_cc[VTIME] = 0;
    new_serial_setting.c_cc[VMIN] = 1;

    tcflush(gMcbInst.ais_serial_fd, TCIFLUSH);
    if ( tcsetattr(gMcbInst.ais_serial_fd, TCSANOW, &new_serial_setting) != 0 ) {
        perror("[THREAD  ctrlPc2Radar] :: termios set faild");
        exit(-1);
    }
    return 0;
}

int configSocket() {
    bzero(&gMcbInst.addr_arm_pc_for_ais, sizeof(struct sockaddr_in));
    gMcbInst.addr_arm_pc_for_ais.sin_family = AF_INET;
    gMcbInst.addr_arm_pc_for_ais.sin_port = htons(9090);
    gMcbInst.addr_arm_pc_for_ais.sin_addr.s_addr = inet_addr("10.236.58.234");

    // bzero(&gMcbInst.addr_pc_for_ais, sizeof(struct sockaddr_in));
    // gMcbInst.addr_pc_for_ais.sin_family = AF_INET;
    // gMcbInst.addr_pc_for_ais.sin_port = htons(9090);
    // gMcbInst.addr_pc_for_ais.sin_addr.s_addr = inet_addr("10.246.45.131");

    if ( (gMcbInst.socket_arm_pc_for_ais = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket create faild!\n");
        exit(-1);
    }

    if (bind(gMcbInst.socket_arm_pc_for_ais,
             (struct sockaddr*)&gMcbInst.addr_arm_pc_for_ais,
             sizeof(struct sockaddr))
        != 0) {
        perror("bind error");
        exit(-1);
    }

    gMcbInst.status_ais = STATUS_OFF;
}


void* ctrlPc2Ais(void *pParams) {
    // not implement
    socklen_t len;
    len = sizeof(gMcbInst.addr_arm_pc_for_radar);

    printf("[+] [THREAD ctrlPc2Radar] :: start listening control message from PC to AIS!\n");

    forever {
        ssize_t recLen;
        recLen = recvfrom(
            gMcbInst.socket_arm_pc_for_ais,
            buf_pc_ais,
            BUFLEN_AIS,
            0,
            (struct sockaddr*)&gMcbInst.addr_pc_for_ais,
            &len
        );

        if (recLen > 0) {
            printf("[RECEIVE] ");
            for (int i =0; i< recLen; i++) {
                printf("%c", buf_pc_ais[i]);
            }
            printf("\n");
            fflush(stdout);

            if (gMcbInst.status_ais == STATUS_ON) {
                gMcbInst.status_ais = STATUS_OFF;
            } else {
                gMcbInst.status_ais = STATUS_ON;
            }


        }
    }
}