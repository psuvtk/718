#ifndef __RADARPLUSAIS_H__
#define __RADARPLUSAIS_H__

#include <stdio.h>
#include <stdlib.h> // atoi()
#include <string.h> // strtok()
#include <ctype.h> // isspace()


#include <sys/socket.h>
#include <netinet/in.h> // uint16_t
#include <arpa/inet.h>

#include <pthread.h>

#define DEBUG 1


#define IP_ADDR_LEN 16
#define BUFLEN_PC_AIS    1024
#define BUFLEN_PC_RADAR  1024
#define BUFLEN_RADAR     65536
#define BUFLEN_AIS       65536



// 工作状态
#define STATUS_OFF 0
#define STATUS_ON 1

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

    pthread_t thread_forward_radar_pc;
    pthread_t thread_forward_ais_pc;
    pthread_t thread_receive_pc_radar;
    pthread_t thread_receive_pc_ais;

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
void* receivePc2Radar(void* params);
void* receivePc2Ais(void* params);


int parseConfig(FILE *fd);
// utils
char* trimValue(char *s);


#endif //__RADARPLUSAIS_H__