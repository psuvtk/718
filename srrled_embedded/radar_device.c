#include "./include/radar_device.h"

bool sensor_start() {
    int fd = open(PORT_UART, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror("Faild to open uart port");
        exit(-1);
    }

    config_serial(fd, B115200);

    // urgly but work
    char cmd1[] = "advFrameCfg\n";
    char cmd2[] = "sensorStart\n";

    char recvbuf[1024];
    write(fd, cmd1, strlen(cmd1));
    usleep(100000);
    write(fd, cmd2, strlen(cmd2));
}


bool sensor_stop() {
    // not implement
}


bool device_open(int *fd) {

    *fd = open(PORT_DATA, O_RDONLY | O_NOCTTY | O_NDELAY);
    if (*fd == -1) return false;

    if (!config_serial(*fd, B921600) ) {
        perror("Faild to config auxiliary port.");
        exit(-1);
    }
}


bool device_close(int fd) {

}

bool config_serial(int fd, int baudrate) {
    struct termios options;

    if (tcgetattr(fd, &options) == -1) {
        perror("tcgetattr failed.");
        return false;
    }

    // set up raw mode / no echo / binary
    options.c_cflag |= (tcflag_t)  (CLOCAL | CREAD);
    options.c_lflag &= (tcflag_t) ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG | IEXTEN); //|ECHOPRT

    options.c_oflag &= (tcflag_t) ~(OPOST);
    options.c_iflag &= (tcflag_t) ~(INLCR | IGNCR | ICRNL | IGNBRK);
#ifdef IUCLC
    options.c_iflag &= (tcflag_t) ~IUCLC;
#endif
#ifdef PARMRK
    options.c_iflag &= (tcflag_t) ~PARMRK;
#endif

    // 设置波特率
    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);

    // 8位数据位
    options.c_cflag &= (tcflag_t) ~CSIZE;
    options.c_cflag |= CS8;

    // 1位停止位
    options.c_cflag &= (tcflag_t) ~(CSTOPB);

    // 无校验
    options.c_iflag &= (tcflag_t) ~(INPCK | ISTRIP);
    options.c_cflag &= (tcflag_t) ~(PARENB | PARODD);

#ifdef IXANY
    options.c_iflag &= (tcflag_t) ~(IXON | IXOFF | IXANY);
#else
    options.c_iflag &= (tcflag_t) ~(IXON | IXOFF);
#endif

#ifdef CRTSCTS
    options.c_cflag &= (unsigned long) ~(CRTSCTS);
#elif defined CNEW_RTSCTS
    options.c_cflag &= (unsigned long) ~(CNEW_RTSCTS);
#else
#error "OS Support seems wrong."
#endif

    options.c_cc[VTIME] = 0;
    options.c_cc[VMIN] = 0;

    tcsetattr(fd, TCSANOW, &options);
}

