/**
 * RadarPlusAIS.c
 * @author：Kristoffer
 * @email：psuvtk@gmail.com
 * @date: 20180801
 */

#include "RadarPlusAIS.h"

// int socket_arm_radar, 
//     socket_arm_pc_for_radar, 
//     socket_arm_pc_for_ais;
// struct sockaddr_in addr_radar, addr_arm_radar;
// struct sockaddr_in addr_pc_for_radar, addr_arm_pc_for_radar;
// struct sockaddr_in addr_pc_for_ais, addr_arm_pc_for_ais;



gMCB_t gMcbInst;
char buf_radar[BUFLEN_RADAR];
char buf_ais[BUFLEN_AIS];
char buf_pc2ais[BUFLEN_AIS];
char buf_pc2radar[BUFLEN_PC_RADAR];


int main(int argc, char *argv[])
{   

    initMcb();

    // 初始化监听线程
    if ( pthread_create(
        &gMcbInst.thread_forward_radar_pc,
        NULL,
        forwardRadar2Pc,
        NULL) != 0) {
        perror("thread create failed");
        exit(-1);
    }
    if ( pthread_create(
        &gMcbInst.thread_forward_ais_pc,
        NULL,
        forwardAis2Pc,
        NULL) != 0) {
        perror("thread create failed");
        exit(-1);
    }

    if ( pthread_create(
        &gMcbInst.thread_receive_pc_radar,
        NULL,
        receivePc2Radar,
        NULL) != 0) {
        perror("thread create failed");
        exit(-1);
    }

    if ( pthread_create(
        &gMcbInst.thread_receive_pc_ais,
        NULL,
        receivePc2Ais,
        NULL) != 0) {
        perror("thread create failed");
        exit(-1);
    }


    // 因为四个线程级别相同 就不放在主线程里面啦
    while(1)
        ;
    return 0;
}

void initMcb() {
    FILE *config_fd = fopen("./config.txt", "r");
    parseConfig(config_fd);

    // 初始化本地端口
    bzero(&gMcbInst.addr_arm_radar, sizeof(struct sockaddr_in));
    gMcbInst.addr_arm_radar.sin_family = AF_INET;
    gMcbInst.addr_arm_radar.sin_port = gMcbInst.port_arm_radar;
    gMcbInst.addr_arm_radar.sin_addr.s_addr = gMcbInst.ip_arm_radar;

    bzero(&gMcbInst.addr_arm_pc_for_ais, sizeof(struct sockaddr_in));
    gMcbInst.addr_arm_pc_for_ais.sin_family = AF_INET;
    gMcbInst.addr_arm_pc_for_ais.sin_port = gMcbInst.port_arm_pc_for_ais;
    gMcbInst.addr_arm_pc_for_ais.sin_addr.s_addr = gMcbInst.ip_arm_pc;

    bzero(&gMcbInst.addr_arm_pc_for_radar, sizeof(struct sockaddr_in));
    gMcbInst.addr_arm_pc_for_radar.sin_family = AF_INET;
    gMcbInst.addr_arm_pc_for_radar.sin_port = gMcbInst.port_arm_pc_for_radar;
    gMcbInst.addr_arm_pc_for_radar.sin_addr.s_addr = gMcbInst.ip_arm_pc;

    // 初始化 远程端口
    bzero(&gMcbInst.addr_radar, sizeof(struct sockaddr_in));
    gMcbInst.addr_radar.sin_family = AF_INET;
    gMcbInst.addr_radar.sin_port = gMcbInst.port_radar;
    gMcbInst.addr_radar.sin_addr.s_addr = gMcbInst.ip_radar;

    bzero(&gMcbInst.addr_pc_for_ais, sizeof(struct sockaddr_in));
    gMcbInst.addr_pc_for_ais.sin_family = AF_INET;
    gMcbInst.addr_pc_for_ais.sin_port = gMcbInst.port_arm_pc_for_ais;
    gMcbInst.addr_pc_for_ais.sin_addr.s_addr = gMcbInst.ip_pc;
    
    bzero(&gMcbInst.addr_pc_for_radar, sizeof(struct sockaddr_in));
    gMcbInst.addr_pc_for_radar.sin_family = AF_INET;
    gMcbInst.addr_pc_for_radar.sin_port = gMcbInst.port_arm_pc_for_radar;
    gMcbInst.addr_pc_for_radar.sin_addr.s_addr = gMcbInst.ip_pc;

    // 创建socket
    if ( (gMcbInst.socket_arm_radar = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket create faild!\n");
        exit(-1);
    }

    if ( (gMcbInst.socket_arm_pc_for_ais = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket create faild!\n");
        exit(-1);
    }

    if ( (gMcbInst.socket_arm_pc_for_radar = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket create faild!\n");
        exit(-1);
    }

    // 将socket与本地端口进行绑定
    // if (bind(gMcbInst.socket_arm_radar,
    //         (struct sockaddr*)&gMcbInst.addr_arm_radar,
    //         sizeof(struct sockaddr)) 
    //     != 0) {
    //     perror("bind error");
    //     exit(-1);
    // }

    // if (bind(gMcbInst.socket_arm_pc_for_ais,
    //          (struct sockaddr*)&gMcbInst.addr_arm_pc_for_ais,
    //          sizeof(struct sockaddr)) 
    //     != 0) {
    //     perror("bind error");
    //     exit(-1);
    // }

    // if (bind(gMcbInst.socket_arm_pc_for_radar,
    //          (struct sockaddr*)&gMcbInst.addr_arm_pc_for_radar,
    //          sizeof(struct sockaddr)) 
    //     != 0) {
    //     perror("bind error");
    //     exit(-1);
    // }


    // 初始化工作状态
    gMcbInst.status_radar = 0;
    gMcbInst.status_ais = 0;


    printf("[+] Init success.\n");
#ifdef DEBUG
    // 内省
#endif
}

int parseConfig(FILE *fd)
{
#define STRLEN 100
    char *s;       
    char name[STRLEN], value[STRLEN];
    char buff[2 * STRLEN + 1];
    
    fseek(fd, 0, SEEK_SET);
    while ( (s = fgets(buff, sizeof buff, fd)) != NULL ) {
        /* 跳过空行以及注释 */
        if (buff[0] == '\n' || buff[0] == '#') {
            continue;
        }

        /* Parse name */
        s = strtok(buff, "=");
        if (s == NULL) {
            continue;
        }
        else {
            strncpy(name, s, STRLEN);
        }

        /* Parse Value*/
        s = strtok(NULL, "=");
        if (s == NULL) {
            continue;
        }
        else {
            strncpy(value, s, STRLEN);
        }
        trimValue(value);

        // 解析ip配置
        if (strcmp(name, "ipRadar") == 0) {
            gMcbInst.ip_radar = inet_addr(value);
#ifdef DEBUG
            printf("Paresed Parameter [ipRadar]: %s\n", value);
            printf("ip expected:%u\n", inet_addr("192.168.2.100"));
            printf("ip:%u\n\n", gMcbInst.ip_radar);
#endif
        }

        if (strcmp(name, "ipArmRadar") == 0) {
            gMcbInst.ip_arm_radar = (long)inet_addr(value);
#ifdef DEBUG
            printf("Paresed Parameter [ipArmRadar]: %s\n", value);
            printf("ip expected:%u\n", inet_addr("192.168.2.23"));
            printf("ip:%u\n\n", gMcbInst.ip_arm_radar);
#endif
        }

        if (strcmp(name, "ipPc") == 0) {
            gMcbInst.ip_pc = inet_addr(value);
#ifdef DEBUG
            printf("Paresed Parameter [ipPc]: %s\n", value);
            printf("ip expected:%u\n", inet_addr("10.236.137.158"));
            printf("ip:%u\n\n", gMcbInst.ip_pc);
#endif
        }

        if (strcmp(name, "ipArmPc") == 0) {
            gMcbInst.ip_arm_pc = inet_addr(value);
#ifdef DEBUG
            printf("Paresed Parameter [ipArmPc]: %s\n", value);
            printf("ip expected:%u\n",  inet_addr("10.246.255.104"));
            printf("ip:%u\n\n", gMcbInst.ip_arm_pc);
#endif
        }

        // 解析端口配置
        if (strcmp(name, "portRadar") == 0) {
            gMcbInst.port_radar = atoi(value);
#ifdef DEBUG
            printf("Paresed Parameter [portRadar]: %s\n", value);
            printf("atoi(value) port: %d\n\n", gMcbInst.port_radar);
#endif
        }
        if (strcmp(name, "portArmRadar") == 0) {
            gMcbInst.port_arm_radar = atoi(value);
#ifdef DEBUG
            printf("Paresed Parameter [portArmRadar]: %s\n", value);
            printf("atoi(value) port: %d\n\n", gMcbInst.port_arm_radar);
#endif
        }

        if (strcmp(name, "portArmPcForAis") == 0) {
            gMcbInst.port_arm_pc_for_ais = atoi(value);
#ifdef DEBUG
            printf("Paresed Parameter [portArmPcForAis]: %s\n", value);
            printf("atoi(value) port: %d\n\n", gMcbInst.port_arm_pc_for_ais);
#endif
        }

        if (strcmp(name, "portArmPcForRadar") == 0) {
            gMcbInst.port_arm_pc_for_radar = atoi(value);
#ifdef DEBUG
            printf("Paresed Parameter [portArmPcForRadar]: %s\n", value);
            printf("atoi(value) port: %d\n\n", gMcbInst.port_arm_pc_for_radar);
#endif
        }
    }
}

char* trimValue(char *s)
{
    /* Initialize start, end pointers */
    char *s1 = s, *s2 = &s[strlen(s) - 1];

    /* Trim and delimit right side */
    while ((isspace(*s2)) && (s2 >= s1))
        s2--;
    *(s2 + 1) = '\0';

    /* Trim left side */
    while ((isspace(*s1)) && (s1 < s2))
        s1++;

    /* Copy finished string */
    strcpy(s, s1);
    return s;
}

void* forwardRadar2Pc(void *pParams) {
    ssize_t size_rec, size_send;
    socklen_t len;
    len = sizeof(gMcbInst.addr_pc_for_radar);

    printf("[+] Start listeing&forwarding radar to pc!\n");
    while (1) {
        size_rec = recvfrom(
            gMcbInst.socket_arm_radar,
            buf_radar,
            BUFLEN_RADAR,
            0,
            (struct sockaddr*)&gMcbInst.addr_radar,
            (socklen_t *__restrict)&len
        );

        size_send = sendto(
            gMcbInst.socket_arm_pc_for_radar,
            buf_radar,
            size_rec,
            0,
            (struct sockaddr *)&gMcbInst.addr_pc_for_radar,
            sizeof(gMcbInst.addr_pc_for_radar)
        );
#ifdef DEBUG
        printf("Receive %ld byte from radar, Transfer %ld byte to Pc\n", size_rec, size_send);
#endif 
    }
}

void* forwardAis2Pc(void *pParams) {
    // not inplement
    printf("from forwardAis2pc thread\n");
    while(1)
        ;
}

void* receivePc2Radar(void *pParams) {
    // not inplement
    printf("from receivePc2Radar thread\n");
    while(1)
        ;
}

void* receivePc2Ais(void *pParams) {
    // not implement
    printf("from receivePc2Ais thread\n");
    while(1)
        ;
}