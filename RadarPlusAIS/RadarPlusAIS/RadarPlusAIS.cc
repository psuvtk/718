/**
 * RadarPlusAIS.c
 * @author：Kristoffer
 * @email：psuvtk@gmail.com
 * @date: 20180801
 */

#include "RadarPlusAIS.h"


gMCB_t gMcbInst;
char buf_radar[BUFLEN_RADAR];
char buf_ais[BUFLEN_AIS];
char buf_pc2ais[BUFLEN_AIS];
char buf_pc2radar[BUFLEN_PC_RADAR];

    char buf_send_ping[BUFLEN_AIS];
    char buf_send_pong[BUFLEN_AIS];

int main(int argc, char *argv[])
{
    pthread_t thread_forward_radar_pc;
    pthread_t thread_forward_ais_pc;
    pthread_t thread_receive_pc_radar;
    pthread_t thread_receive_pc_ais;
    pthread_t thread_ais_async_send;

    initMcb();
    // spawn_threads();
    // 初始化监听线程
    if ( pthread_create(
            &thread_forward_radar_pc,
            NULL,
            forwardRadar2Pc,
            NULL)
         != 0) {
        perror("thread create failed");
        exit(-1);
    }
    if ( pthread_create(
            &thread_forward_ais_pc,
            NULL,
            forwardAis2Pc,
            NULL)
         != 0) {
        perror("thread create failed");
        exit(-1);
    }

    if ( pthread_create(
            &thread_receive_pc_radar,
            NULL,
            ctrlPc2Radar,
            NULL)
         != 0) {
        perror("thread create failed");
        exit(-1);
    }

    if ( pthread_create(
            &thread_receive_pc_ais,
            NULL,
            ctrlPc2Ais,
            NULL)
         != 0) {
        perror("thread create failed");
        exit(-1);
    }

    // 因为四个线程级别相同 就不放在主线程里面啦
    // TODO: 计时器，一定时间内未获得信号则下电
    while(1)
        ;
    return 0;
}

void initMcb() {
    parseConfig("./config.txt");

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

    // 没有必要设置，
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


    if (bind(gMcbInst.socket_arm_pc_for_ais,
             (struct sockaddr*)&gMcbInst.addr_arm_pc_for_ais,
             sizeof(struct sockaddr))
        != 0) {
        perror("bind error");
        exit(-1);
    }

#ifndef DEBUG
    // 将socket与本地端口进行绑定
    if (bind(gMcbInst.socket_arm_radar,
            (struct sockaddr*)&gMcbInst.addr_arm_radar,
            sizeof(struct sockaddr))
        != 0) {
        perror("bind error");
        exit(-1);
    }



    if (bind(gMcbInst.socket_arm_pc_for_radar,
             (struct sockaddr*)&gMcbInst.addr_arm_pc_for_radar,
             sizeof(struct sockaddr))
        != 0) {
        perror("bind error");
        exit(-1);
    }

    // 打开串口
    gMcbInst.ais_serial_fd = open(DEVICE_AIS, O_RDONLY | O_NOCTTY | O_NDELAY);
    if ( gMcbInst.ais_serial_fd < 0 ) {
        perror("ais serial port open failed");
        exit(-1);
    }

    gMcbInst.am335x_gpio_fd = open(DEV_GPIO, O_RDWR);
    if ( gMcbInst.am335x_gpio_fd < 0 ) {
        perror("am335x gpio open failed");
        exit(-1);
    }

    初始化引脚
    gMcbInst.power_io_arg.pin = GPIO_TO_PIN(POWER_IO_GROUP, POWER_IO_NUM);
    gMcbInst.power_io_arg.data = POWER_DISABLE;

    gMcbInst.radar_io_arg.pin = GPIO_TO_PIN(RADAR_IO_GROUP, POWER_IO_NUM);
    gMcbInst.radar_io_arg.data = POWER_DISABLE;

    ioctl(gMcbInst.am335x_gpio_fd, IOCTL_GPIO_SETOUTPUT, &gMcbInst.power_io_arg);
    ioctl(gMcbInst.am335x_gpio_fd, IOCTL_GPIO_SETOUTPUT, &gMcbInst.radar_io_arg);
    ioctl(gMcbInst.am335x_gpio_fd, IOCTL_GPIO_SETVALUE, &gMcbInst.power_io_arg);
    ioctl(gMcbInst.am335x_gpio_fd, IOCTL_GPIO_SETVALUE, &gMcbInst.radar_io_arg);
#endif
    // 打开串口
    gMcbInst.ais_serial_fd = open(DEVICE_AIS, O_RDONLY | O_NOCTTY | O_NDELAY);
    if ( gMcbInst.ais_serial_fd < 0 ) {
        perror("AIS serial port open failed");
        exit(-1);
    }
    // 初始化工作状态
    gMcbInst.status_radar = 0;
    gMcbInst.status_ais = 0;


    printf("[+] Init success.\n");
}

int parseConfig(char *config_filename)
{
#define STRLEN 100
    char *s;
    FILE *fd;
    char name[STRLEN], value[STRLEN];
    char buff[2 * STRLEN + 1];

    fd = fopen("./config.txt", "r");
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
            printf("[DEBUG] Paresed Parameter [ipRadar]: %s\n", value);
            printf("[DEBUG] after inet_addr(big-endian): 0x%08X\n\n", gMcbInst.ip_radar);
#endif
        }

        if (strcmp(name, "ipArmRadar") == 0) {
            gMcbInst.ip_arm_radar = inet_addr(value);
#ifdef DEBUG
            printf("[DEBUG] Paresed Parameter [ipArmRadar]: %s\n", value);
            printf("[DEBUG] after inet_addr(big-endian): 0x%08X\n\n", gMcbInst.ip_arm_radar);
#endif
        }

        if (strcmp(name, "ipPc") == 0) {
            gMcbInst.ip_pc = inet_addr(value);
#ifdef DEBUG
            printf("[DEBUG] Paresed Parameter [ipPc]: %s\n", value);
            printf("[DEBUG] after inet_addr(big-endian): 0x%08X\n\n", gMcbInst.ip_pc);
#endif
        }

        if (strcmp(name, "ipArmPc") == 0) {
            gMcbInst.ip_arm_pc = inet_addr(value);
#ifdef DEBUG
            printf("[DEBUG] Paresed Parameter [ipArmPc]: %s\n", value);
            printf("[DEBUG] after inet_addr(big-endian): 0x%08X\n\n", gMcbInst.ip_arm_pc);
#endif
        }

        // 解析端口配置
        if (strcmp(name, "portRadar") == 0) {
            gMcbInst.port_radar = htons(atoi(value));
#ifdef DEBUG
            printf("[DEBUG] Paresed Parameter [portRadar](little-endian): %s\n", value);
            printf("[DEBUG] before htons(little-endian): 0x%04X\n", atoi(value));
            printf("[DEBUG]     after htons(big-endian): 0x%04X\n\n", gMcbInst.port_radar);
#endif
        }
        if (strcmp(name, "portArmRadar") == 0) {
            gMcbInst.port_arm_radar = htons(atoi(value));
#ifdef DEBUG
            printf("[DEBUG] Paresed Parameter [portArmRadar](little-endian): %s\n", value);
            printf("[DEBUG] before htons(little-endian): 0x%04X\n", atoi(value));
            printf("[DEBUG]     after htons(big-endian): 0x%04X\n\n", gMcbInst.port_arm_radar);
#endif
        }

        if (strcmp(name, "portArmPcForAis") == 0) {
            gMcbInst.port_arm_pc_for_ais = htons(atoi(value));
#ifdef DEBUG
            printf("[DEBUG] Paresed Parameter [portArmPcForAis](little-endian): %s\n", value);
            printf("[DEBUG] before htons(little-endian): 0x%04X\n", atoi(value));
            printf("[DEBUG]     after htons(big-endian): 0x%04X\n\n", gMcbInst.port_arm_pc_for_ais);
#endif
        }

        if (strcmp(name, "portArmPcForRadar") == 0) {
            gMcbInst.port_arm_pc_for_radar = htons(atoi(value));
#ifdef DEBUG
            printf("[DEBUG] Paresed Parameter [portArmPcForRadar](little-endian): %s\n", value);
            printf("[DEBUG] before htons(little-endian): 0x%04X\n", atoi(value));
            printf("[DEBUG]     after htons(big-endian): 0x%04X\n\n", gMcbInst.port_arm_pc_for_radar);
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


void* ctrlPc2Radar(void *pParams) {
    // not inplement
    socklen_t len;
    len = sizeof(gMcbInst.addr_arm_pc_for_radar);

    printf("[+] [THREAD ctrlPc2Radar] :: start listening control message from PC to RADAR!\n");

    while (1) {
        ssize_t recLen;
        recLen = recvfrom(
            gMcbInst.socket_arm_pc_for_radar,
            buf_pc2radar,
            BUFLEN_PC_RADAR,
            0,
            (struct sockaddr*)&gMcbInst.addr_pc_for_radar,
            &len
        );

        //先将buffer转换为vector<byte>，长度为length
        std::vector<byte> vec(recLen);
        ::memcpy(vec.data(), buf_pc2radar, vec.size());

        //首先调用TryParse函数，对vec进行拆包，obj为拆包得到的数据map
        std::map<std::string, std::string> obj;
        std::vector<byte> buf_wrap;
        if (MyUtils::TransParaFactoryUtil::TryParse(vec, obj))
        {
            // memcpy((struct sockaddr*)&target_addr_PC, (struct sockaddr*)&addrARM_PC, len_addr_PC);
            //ARM板接收到的数据有三种类型，雷达上电信号，雷达下电信号，雷达控制信号
            //雷达上电信号
            if (obj["type"] == "radarpoweronreq")
            {
                //接收到雷达上电信号并成功上电后向显控发送上电成功信号
                buf_wrap = MyUtils::TransParaFactoryUtil::Wrap(MyUtils::TransParaFactoryUtil::ResType::RadarPowerOnRes);
                printf("[+] [THREAD ctrlPc2Radar] :: radar power on \n");
                gMcbInst.power_io_arg.data = POWER_ENABLE;
                gMcbInst.radar_io_arg.data = POWER_ENABLE;
                ioctl(gMcbInst.am335x_gpio_fd, IOCTL_GPIO_SETVALUE, &gMcbInst.power_io_arg);
                sleep(1);
                ioctl(gMcbInst.am335x_gpio_fd, IOCTL_GPIO_SETVALUE, &gMcbInst.radar_io_arg);

                gMcbInst.status_radar = STATUS_ON;

                //udpClient.SendToPc(bufPC);
                sendto(
                    gMcbInst.socket_arm_pc_for_radar,
                    buf_wrap.data(),
                    buf_wrap.size(),
                    0,
                    (struct sockaddr *)&gMcbInst.addr_pc_for_radar,
                    sizeof(gMcbInst.addr_pc_for_radar)
                );
            }
            //雷达下电信号
            else if (obj["type"] == "radarpoweroffreq")
            {
                //接收到雷达下电信号并成功上电后向显控发送下电成功信号
                buf_wrap = MyUtils::TransParaFactoryUtil::Wrap(MyUtils::TransParaFactoryUtil::ResType::RadarPowerOffRes);
                //udpClient.SendToPc(bufPC);
                printf("[+] [THREAD ctrlPc2Radar] :: radar power off\n");
                gMcbInst.power_io_arg.data = POWER_DISABLE;
                gMcbInst.radar_io_arg.data = POWER_DISABLE;
                ioctl(gMcbInst.am335x_gpio_fd, IOCTL_GPIO_SETVALUE, &gMcbInst.radar_io_arg);
                sleep(1);
                ioctl(gMcbInst.am335x_gpio_fd, IOCTL_GPIO_SETVALUE, &gMcbInst.power_io_arg);

                gMcbInst.status_radar = STATUS_OFF;

                sendto(
                    gMcbInst.socket_arm_pc_for_radar,
                    buf_wrap.data(),
                    buf_wrap.size(),
                    0,
                    (struct sockaddr *)&gMcbInst.addr_pc_for_radar,
                    sizeof(gMcbInst.addr_pc_for_radar)
                );
            }
            //雷达控制信号controlradarreq
            else if (obj["type"] == "radarcontrolreq")
            {
                int send_size;
                //接收到雷达控制信号后，无需向显控反馈，解析attach部分并向雷达发送（attach部分base64解码的buffer就是原来的雷达控制信号）
                buf_wrap = MyUtils::TransParaFactoryUtil::Base64Decode(obj["attach"]);
                //udpClient.SendToRadar(bufPC);
                if(gMcbInst.status_radar)
                {
                    send_size = sendto(
                        gMcbInst.socket_arm_radar,
                        buf_wrap.data(),
                        buf_wrap.size(),
                        0,
                        (struct sockaddr *)&gMcbInst.addr_radar,
                        sizeof(gMcbInst.addr_radar)
                    );
                    printf("[DEBUG] [THREAD ctrlPc2Radar] :: rec : %lu, send : %d\n", buf_wrap.size(), send_size);
                }
            }
        }
    }
}


void* forwardRadar2Pc(void *pParams) {
    ssize_t size_rec, size_send;
    socklen_t len;
    len = sizeof(gMcbInst.addr_pc_for_radar);

    printf("[+] [THREAD forwardRadar2Pc] :: start listeing&forwarding RADAR to PC!\n");
    forever {
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
        printf("[DEBUG] [THREAD forwardRadar2Pc] :: receive %ld byte from radar, Transfer %ld byte to Pc\n", size_rec, size_send);
#endif
    }
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
            buf_pc2radar,
            BUFLEN_PC_RADAR,
            0,
            (struct sockaddr*)&gMcbInst.addr_pc_for_ais,
            &len
        );

        //先将buffer转换为vector<byte>，长度为length
        std::vector<byte> vec(recLen);
        ::memcpy(vec.data(), buf_pc2radar, vec.size());

        //首先调用TryParse函数，对vec进行拆包，obj为拆包得到的数据map
        std::map<std::string, std::string> obj;
        std::vector<byte> buf_wrap;
        if (MyUtils::TransParaFactoryUtil::TryParse(vec, obj))
        {
            // memcpy((struct sockaddr*)&target_addr_PC, (struct sockaddr*)&addrARM_PC, len_addr_PC);
            //ARM板接收到的数据有三种类型，雷达上电信号，雷达下电信号，雷达控制信号
            //雷达上电信号
            if (obj["type"] == "aispoweronreq")
            {
                //接收到雷达上电信号并成功上电后向显控发送上电成功信号
                buf_wrap = MyUtils::TransParaFactoryUtil::Wrap(MyUtils::TransParaFactoryUtil::ResType::AisPowerOnRes);

                // 做一些事情
                int baudrate = getBaudRate(obj["attach"]);

                configSerialPort(baudrate);
                gMcbInst.status_ais = STATUS_ON;

                char sb[4]={0x01, 0x02, 0x03, 0x04};
                //udpClient.SendToPc(bufPC); 反馈给远程pc
                int len = sendto(
                    gMcbInst.socket_arm_pc_for_ais,
                    buf_wrap.data(),
                    buf_wrap.size(),
                    0,
                    (struct sockaddr *)&gMcbInst.addr_pc_for_ais,
                    sizeof(gMcbInst.addr_pc_for_ais)
                );
                // int len = sendto(
                //     gMcbInst.socket_arm_pc_for_ais,
                //     sb,
                //     4,
                //     0,
                //     (struct sockaddr *)&gMcbInst.addr_pc_for_ais,
                //     sizeof(gMcbInst.addr_pc_for_ais)
                // );

                printf("\n[#]%d\n\n\n", len);



            }
            //雷达下电信号
            else if (obj["type"] == "aispoweroffreq")
            {
                //接收到雷达下电信号并成功上电后向显控发送下电成功信号
                buf_wrap = MyUtils::TransParaFactoryUtil::Wrap(MyUtils::TransParaFactoryUtil::ResType::AisPowerOffRes);

                gMcbInst.status_ais = STATUS_OFF;

                // 反馈给远程pc
                sendto(
                    gMcbInst.socket_arm_pc_for_ais,
                    buf_wrap.data(),
                    buf_wrap.size(),
                    0,
                    (struct sockaddr *)&gMcbInst.addr_pc_for_ais,
                    sizeof(gMcbInst.addr_pc_for_ais)
                );
            }
        }
    }
}


void* forwardAis2Pc(void *pParams) {
    // not inplement
    int cur_index=0;

    char *buf_send;

    ssize_t read_len = 0,
            send_len = 0;
    printf("[+] [THREAD forwardAis2Pc] :: start listening data from AIS to PC!\n");

    bzero(buf_ais, BUFLEN_AIS);
    buf_send = buf_send_ping;
    forever {
#ifndef DEBUG_NO_DEVICE
        if (gMcbInst.status_ais == STATUS_OFF) {
            sleep(POLLING_INTERVAL);
            continue;
        }
#endif

        if ( (read_len = read(gMcbInst.ais_serial_fd, buf_ais, BUFLEN_AIS)) > 0 ) {
#ifdef DEBUG_NO_DEVICE // 测试UDP用
            printf("[DEBUG] [THREAD forwardAis2Pc] :: read %ld bytes from searial port.\n", read_len);
            for (int i =0; i<read_len;i++) {
                printf("%c", buf_ais[i]);
            }
            fflush(stdout);

            if ( (BUFLEN_AIS - cur_index) >= read_len ) {
                memcpy(buf_send+cur_index, buf_ais, read_len);
                cur_index+=read_len;
            } else {
                for (int i = 0; i < cur_index; i++) {
                    printf("%c", buf_send[i]);
                }
                printf("\nbuff filled\n");
                cur_index = 0;
            }
            continue;
#endif

#ifdef DEBUG
            printf("[DEBUG] [THREAD forwardAis2Pc] :: ");
            for (int i = 0; i < read_len; i++) {
                    printf("%c", buf_ais[i]);
            }
#endif
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

                // printf("\n=======================================\n");
                // printf("\n[DEBUG] [SEND] ");
                // for (int i = 0; i < cur_index; i++) {
                //     printf("%c", buf_send[i]);
                // }
                // printf("\n=======================================\n");

                cur_index = 0;
                if (buf_send == buf_send_ping) {
                    buf_send = buf_send_pong;
                } else {
                    buf_send = buf_send_ping;
                }

                memcpy(buf_send+cur_index, buf_ais, read_len);
                bzero(buf_ais, BUFLEN_AIS);
                cur_index+=read_len;
            }
        }
    }
}

/**
 * 获取波特率
 * 前四个字节为 串口号com1， 无用
 */
int getBaudRate(std::string encrypt_attach) {
    std::vector<byte> attach = MyUtils::TransParaFactoryUtil::Base64Decode(encrypt_attach);
    return *(int*)(attach.data() + 4);
}

int configSerialPort(int baudrate) {

    struct termios new_serial_setting, cur_serial_setting;

    new_serial_setting.c_cflag |= (CLOCAL | CREAD);

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