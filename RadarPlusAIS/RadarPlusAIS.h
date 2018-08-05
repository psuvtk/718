#ifndef __RADARPLUSAIS_H__
#define __RADARPLUSAIS_H__

#include <stdio.h>
#include <stdlib.h> // atoi()
#include <string.h> // strtok()
#include <ctype.h> // isspace()
#include <unistd.h> // sleep()
#include <assert.h>


#include <fcntl.h> //O_RDWR
#include <termios.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h> // uint16_t
#include <arpa/inet.h>

#include <pthread.h>

#include "TransParaFactoryUtil.h"

#define DEBUG 1
// #define DEBUG_NO_DEVICE 1

#define IP_ADDR_LEN 16
#define BUFLEN_PC_AIS    1024
#define BUFLEN_PC_RADAR  1024
#define BUFLEN_RADAR     65536
#define BUFLEN_AIS      64

/**power control**/
typedef struct {
    int pin;
    int data;
} am335x_gpio_arg;

#define DEVICE_AIS "/dev/ttyUSB0"
#define DEV_GPIO   "/dev/am335x_gpio"

#define GPIO_IOC_MAGIC   'G'
#define IOCTL_GPIO_SETOUTPUT              _IOW(GPIO_IOC_MAGIC, 0, int)
#define IOCTL_GPIO_SETINPUT               _IOW(GPIO_IOC_MAGIC, 1, int)
#define IOCTL_GPIO_SETVALUE               _IOW(GPIO_IOC_MAGIC, 2, int)
#define IOCTL_GPIO_GETVALUE           	  _IOR(GPIO_IOC_MAGIC, 3, int)

#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))

#define POWER_IO_GROUP   1
#define POWER_IO_NUM     15
#define POWER_ENABLE     1
#define POWER_DISABLE    0

#define RADAR_IO_GROUP   1
#define RADAR_IO_NUM     14
#define RADAR_ENABLE     1
#define RADAR_DISABLE    0
/**end power control**/


// 工作状态
#define STATUS_OFF 0
#define STATUS_ON 1

// 开启雷达、AIS 状态查询时间间隔
#define POLLING_INTERVAL 8

#define forever for(;;)


typedef struct
{
    int socket_arm_radar;
    int socket_arm_pc_for_radar;
    int socket_arm_pc_for_ais;

    in_addr_t ip_radar;
    in_addr_t ip_pc;
    in_addr_t ip_arm_radar;
    in_addr_t ip_arm_pc;

    in_port_t port_radar;
    in_port_t port_arm_radar;
    in_port_t port_arm_pc_for_ais;
    in_port_t port_arm_pc_for_radar;

    struct sockaddr_in addr_radar;
    struct sockaddr_in addr_arm_radar;
    struct sockaddr_in addr_pc_for_radar;
    struct sockaddr_in addr_arm_pc_for_radar;
    struct sockaddr_in addr_pc_for_ais;
    struct sockaddr_in addr_arm_pc_for_ais;

    int ais_serial_fd;
    int am335x_gpio_fd;
    am335x_gpio_arg power_io_arg;
    am335x_gpio_arg radar_io_arg;

    /**
     * 指示工作状态
     * 0： 关闭
     * 1： 工作
     */
    int status_radar;
    int status_ais;
} gMCB_t;

void initMcb();

void* forwardRadar2Pc(void* params);
void* forwardAis2Pc(void* params);
void* ctrlPc2Radar(void* params);
void* ctrlPc2Ais(void* params);
void* sendAsyncAis2Pc(void* params);

int parseConfig(FILE *fd);
// utils
char* trimValue(char *s);
int getBaudRate(std::string encrypt_attach);
int configSerialPort(int br);
#endif //__RADARPLUSAIS_H__