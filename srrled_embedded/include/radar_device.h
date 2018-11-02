#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>    // 错误号定义
#include <termios.h>  // 终端定义
#include <fcntl.h>
#incldue <unistd.h>
#incldue <time.h>

#define PORT_UART "/dev/ttyACM0"
#define PORT_DATA "/dev/ttyACM1"

bool sensor_start();
bool sensor_stop();

bool device_open(int *fd);
bool device_close(int fd);
bool config_serial();


