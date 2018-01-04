/*
 * HaierGateway.h
 *
 *  Created on: 2018年1月4日
 *      Author: Evan
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <pthread.h>
#include <errno.h>

#define CONNECTION_TIMEOUT      100
#define DISCOVER_TCP_PORT       2017
#define DISCOVER_UDP_PORT       49001
#define DISCOVER_BUFF_LEN       sizeof(discover_buff_t)


typedef uint8_t uint8;
typedef uint16_t uint16;

typedef struct {
    uint8 header[2];
    uint8 length;
    uint8 communication_type;
    uint8 unit_num;
    uint8 house_num[2];
    uint8 net_num;
    uint8 panel_num;
    uint8 extension_num;
    uint8 dev_mac[6];
    uint8 dev_ip[4];
    uint8 version[10];
    uint8 ack;
    uint8 end;
} discover_buff_t;


