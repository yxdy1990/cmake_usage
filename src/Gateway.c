/*
 * Gateway.c
 *
 *  Created on: 2017年12月29日
 *      Author: Evan
 */
#include "Gateway.h"
#include "mylib.h"


static unsigned char selfIp[4] = { 0 };
static char *voiceCommand[] = { "本房间", //
                "小U小U", //
                "车库", //
                "玄关", //
                "客厅", //
                "餐厅", //
                "厨房", //
                "主卧", //
                "客卧", //
                "儿童房", //
                "老人房", //
                "主卧卫生间", //
                "公用卫生间", //
                "阳台", //
                "酒窖", //
                "娱乐室", //
                "打开窗帘", //
                "关闭窗帘", //
                "停止窗帘", //
                "打开灯光", //
                "关闭灯光", //
                "打开投影机", //
                "关闭投影机", //
                "打开小帅影院", //
                "关闭小帅影院", //
                "打开家庭影院", //
                "关闭家庭影院", //
                "打开背景音乐", //
                "关闭背景音乐", //
                "打开机械手", //
                "关闭机械手", //
                "打开洗衣机", //
                "关闭洗衣机", //
                "打开油烟机", //
                "关闭油烟机", //
                "打开酒柜", //
                "关闭酒柜", //
                "打开热水器", //
                "关闭热水器", //
                "打开燃气灶", //
                "关闭燃气灶", //
                "打开晾衣机", //
                "关闭晾衣机", //
                "打开空气净化器", //
                "关闭空气净化器", //
                "打开消毒柜", //
                "关闭消毒柜", //
                "打开新风", //
                "关闭新风", //
                "打开电视", //
                "关闭电视", //
                "打开空调", //
                "关闭空调", //
                "十七度", //
                "十八度", //
                "十九度", //
                "二十度", //
                "二十一度", //
                "二十二度", //
                "二十三度", //
                "二十四度", //
                "二十五度", //
                "二十六度", //
                "二十七度", //
                "二十八度", //
                "二十九度", //
                "三十度", //
                "制热模式", //
                "制冷模式", //
                "送风模式", //
                "除湿模式", //
                "自动模式", //
                "高速风", //
                "中速风", //
                "低速风", //
                "自动风", //
                "回家模式", //
                "离家模式", //
                "娱乐模式", //
                "影院模式", //
                "安防模式", //
                "撤防模式", //
                "起床模式", //
                "就寝模式", //
                "小U再见" };

void fill_reply_buf(discover_buff_t *buf) {
    buf->header[0] = 0xFE;
    buf->header[1] = 0xFE;
    buf->length = 0x20;
    buf->communication_type = 0x01;
    buf->unit_num = 0x00;
    buf->house_num[0] = 0x00;
    buf->house_num[1] = 0x00;
    buf->net_num = 0x00;
    buf->panel_num = 0x00;
    buf->extension_num = 0x01;
    buf->dev_ip[0] = selfIp[0];
    buf->dev_ip[1] = selfIp[1];
    buf->dev_ip[2] = selfIp[2];
    buf->dev_ip[3] = selfIp[3];
    buf->ack = 0x00;
    buf->end = 0xFD;
}

static void discover_send_response(char *dst_ip) {
    discover_buff_t send_buf = { { 0 }, };
    struct sockaddr_in client_address;
    int i = 0, ret = 0, socketfd = 0;

    fill_reply_buf(&send_buf);

    socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    client_address.sin_family = AF_INET;
    client_address.sin_port = htons(DISCOVER_UDP_PORT);
    client_address.sin_addr.s_addr = inet_addr(dst_ip);

    ret = sendto(socketfd, &send_buf, DISCOVER_BUFF_LEN, 0, (struct sockaddr *) &client_address,
                    sizeof(client_address));
    if (ret <= 0) {
        printf("sendto error [%d] \n", errno);
    } else {
        printf("**************** 搜索网关IP ACK ****************\n");
        for (; i < DISCOVER_BUFF_LEN; i++)
            printf("0x%02X ", ((unsigned char *) &send_buf)[i]);
        printf("\n\n");
    }
    close(socketfd);
}

void discover_recv_thread() {
    socklen_t sock_len = sizeof(struct sockaddr_in);
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    int recv_len, result, opt = 1;
    int server_socketfd = 0;
    fd_set cli_msgfd;
    int count = 0;

    server_socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(DISCOVER_UDP_PORT);
    setsockopt(server_socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    bind(server_socketfd, (struct sockaddr*) &server_address, sock_len);
    printf("********** Udp Server now Listenning Search on Port 49001 **********\n\n");
    while (1) {
        struct timeval to = { 10, 0 };
        FD_ZERO(&cli_msgfd);
        FD_SET(server_socketfd, &cli_msgfd);
        result = select(server_socketfd + 1, &cli_msgfd, NULL, NULL, &to);
        if (result > 0) {
            if (FD_ISSET(server_socketfd, &cli_msgfd)) {
                discover_buff_t buffer = { { 0 }, };
                recv_len = recvfrom(server_socketfd, &buffer, DISCOVER_BUFF_LEN, 0,
                                (struct sockaddr *) &client_address, &sock_len);
                if (recv_len == DISCOVER_BUFF_LEN && buffer.header[0] == 0xFE && buffer.header[1] == 0xFE
                                && buffer.end == 0xFD) {
                    if (buffer.communication_type == 0x01) {
                        printf("\nRecv Search IP Request from %s\n\n", inet_ntoa(client_address.sin_addr));
                        discover_send_response(inet_ntoa(client_address.sin_addr));
                    }
                }
            }
        }
    }
    close(server_socketfd);
}

void parse_mini_U_packet(int sockfd, unsigned char *packet, int len) {
    if (len <= 5 || packet[0] != 0x18 || packet[len - 1] != 0x06) {
        printf("无效的报文：数据 长度/头/尾 错误！\n");
    } else if (packet[1] == 0x00) {
        unsigned char heart_ack[6] = { 0x16, 0x0b, 0x01, 0xff, 0x21, 0x06 };
        printf("********************* 心跳报文，MAC --> %02X%02X%02X%02X%02X%02X *********************", packet[3],
                        packet[4], packet[5], packet[6], packet[7], packet[8]);
        if (send(sockfd, heart_ack, 6, 0) <= 0) {
            printf("心跳ACK失败: %s\n", strerror(errno));
        }
    } else if (packet[1] == 0x11) {
        unsigned char pos = packet[9];
        unsigned char index = packet[10];
        if (index >= 1 && index <= 84) {
            printf("语音指令%d: %s, 位置: %s, MAC --> %02X%02X%02X%02X%02X%02X\n", index, voiceCommand[index],
                            voiceCommand[pos], packet[3], packet[4], packet[5], packet[6], packet[7],
                            packet[8]);
        } else {
            printf("!!!!!! 未知指令 !!!!!!\n");
        }
    }
    printf("\n\n");
}

void mini_U_connection_handler(void *arg) {
    int sockfd = *(int *) arg;
    struct timeval time_v = { CONNECTION_TIMEOUT, 0 };

    printf("Mini U connection handler, socket id: %d\n", sockfd);

    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &time_v, sizeof(time_v));
    while (1) {
        unsigned char data[128] = { 0 };
        int i = 0, length = 0;

        if ((length = recv(sockfd, data, 128, 0)) > 0) {
            printf("小U传过来 %d 个字节的数据:\n", length);
            for (; i < length; i++)
                printf("0x%02X ", data[i]);
            printf("\n");
            parse_mini_U_packet(sockfd, data, length);
        } else { // 超时或客户端已掉电，断开连接
            printf("!!!!!! 小U已挂，断开本次连接(code:%d, id:%d) !!!!!!\n\n", length, sockfd);
            close(sockfd);
            break;
        }
    }
    free(arg); arg = NULL;
}

#if 0
void mini_U_connection_handler(void *arg) {
    fd_set read_fd;
    int sockfd = *(int *) arg;
    while (1) {
        struct timeval to = {CONNECTION_TIMEOUT, 0};
        FD_ZERO(&read_fd);
        FD_SET(sockfd, &read_fd);
        int result = select(sockfd + 1, &read_fd, NULL, NULL, &to);
        if (result > 0) {
            if (FD_ISSET(sockfd, &read_fd) > 0) {
                unsigned char data[128] = {0};
                int i = 0, length = 0;
                length = recv(sockfd, data, 128, 0);
                if (length > 0) {
                    printf("小U传过来 %d 个字节的数据:\n", length);
                    for (; i < length; i++)
                    printf("0x%02X ", data[i]);
                    printf("\n");
                    parse_mini_U_packet(sockfd, data, length);
                } else
                goto exit;
            } else
            goto exit;
        } else { // 超时或客户端已掉电，断开连接
            exit: printf("!!!!!! 小U已挂，断开本次连接 !!!!!!\n\n");
            close(sockfd);
            return;
        }
    }
}
#endif

void gateway_tcp_server_thread() {
    struct sockaddr_in server_sockaddr, client_sockaddr;
    int recvbytes = 0, reuse = 1, sockfd = 0;
    fd_set read_fd;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Tcp socket create error: %s!\n", strerror(errno));
        return;
    }
    bzero(&server_sockaddr, sizeof(struct sockaddr_in));
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(DISCOVER_TCP_PORT);
    server_sockaddr.sin_addr.s_addr = INADDR_ANY;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
    if (bind(sockfd, (struct sockaddr *) &server_sockaddr, sizeof(struct sockaddr)) < 0) {
        printf("Tcp socket bind error: %s!\n", strerror(errno));
        goto end;
    }
    if (listen(sockfd, 10) < 0) {
        printf("Tcp listen error: %s!\n", strerror(errno));
        goto end;
    }
    printf("********** Tcp Server now Listenning Connect on Port 2017 **********\n\n");
    while (1) {
        socklen_t addr_size = sizeof(struct sockaddr_in);
        struct timeval to = { 10, 0 };
        FD_ZERO(&read_fd);
        FD_SET(sockfd, &read_fd);
        int result = select(sockfd + 1, &read_fd, NULL, NULL, &to);
        if (result == 0) {
            continue;
        } else if (result > 0) {
            if (FD_ISSET(sockfd, &read_fd) > 0) {
                int clientfd = 0;
                if ((clientfd = accept(sockfd, (struct sockaddr *) &client_sockaddr, &addr_size)) < 0) {
                    printf("Tcp accept error: %s!\n", strerror(errno));
                    sleep(1);
                } else {
                    printf("Tcp Server Accept from: %s\n\n", inet_ntoa(client_sockaddr.sin_addr));
                    pthread_t thread = 0;
                    int *thread_arg = (int *) malloc(sizeof(int));
                    *thread_arg = clientfd;
                    if (pthread_create(&thread, NULL, (void*) mini_U_connection_handler, (void*) thread_arg)
                                == 0) {
                        pthread_detach(thread);
                    }
                }
            }
        }
    }
    end: close(sockfd);
}

/*
 * 使用说明：./ExeName 本机IP
 */
int main(int argc, char** argv) {
    test_cmake_function();
    
    if (argc != 2) {
        printf("参数错误！\n");
        printf("Usage: %s\n", "./Gateway 本机IP");
        return -1;
    }
    char *split = strtok(argv[1], ".");
    unsigned int counter = 0;

    while (split != NULL && counter < 4) {
        selfIp[counter] = atoi(split);
        split = strtok(NULL, ".");
        counter++;
    }
    pthread_t thread1 = 0, thread2 = 0;
    if (pthread_create(&thread1, NULL, (void*) discover_recv_thread, NULL) != 0) {
        printf("Create discover thread failed, exit!\n");
        return -1;
    } else
        pthread_detach(thread1);
    if (pthread_create(&thread2, NULL, (void*) gateway_tcp_server_thread, NULL) != 0) {
        printf("Create tcp server thread failed, exit!\n");
        return -1;
    } else
        pthread_detach(thread2);
    printf("\n******************* 网关主机已启动 @%d.%d.%d.%d ******************\n\n", selfIp[0], selfIp[1], selfIp[2],
                    selfIp[3]);
    while (1) {
        sleep(600);
    }
    return 0;
}
