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

    if ( (gMcbInst.ais_serial_fd = open("/dev/tty10", O_RDWR | O_NOCTTY | O_NDELAY)) < 0 ) {
        perror("serial port open faild");
        exit(-1);
    }

    configSerialPort(9600);



    printf("start listening:\n");

    bzero(buf_ais, BUFLEN_AIS);
    forever {
        if ( (read_len = read(gMcbInst.ais_serial_fd, buf_ais, BUFLEN_AIS)) > 0 ) {
            printf("\n[DEBUG] [SEND] ");
            for (int i = 0; i < read_len; i++) {
                printf("%c", buf_ais[i]);
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

    // cfmakeraw(&new_serial_setting);
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