#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdint.h>

// #pragma pack(push, 1) // ensure compiler does not insert padding between the attributes
struct hello_command {
    uint8_t commandType;
    uint16_t udpPort;
};
//#pragma pack(pop)
#define COMMAND_TYPE_HELLO 0

struct welcome_reply {
    uint8_t replyType;
    uint16_t numStations;
};
#define REPLY_TYPE_WELCOME 0


int send_all(int sock, char *ptr, int total_size);
int send_hello_command(int sock, uint16_t udpPort);
int send_welcome_reply(int sock, uint16_t numStation);

int recv_all(int sock, char *buffer, int total_size);
int recv_hello(int sock, struct hello_command *cmd);
int recv_welcome(int sock, struct welcome_reply *reply);

#endif