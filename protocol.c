#include "protocol.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

int send_all(int sock, char *ptr, int total_size) {
    int bytes_sent = 0;
    while (total_size - bytes_sent > 0) {
        int rv = send(sock, ptr, total_size - bytes_sent, 0);
        if (rv <= 0) {
            perror("send");
            return -1;
        }

        bytes_sent += rv;
        ptr += rv;
    }

    return bytes_sent;
}

int send_hello_command(int sock, uint16_t udpPort) {
    int rv;
    uint8_t command_type = COMMAND_TYPE_HELLO;

    if ((rv = send_all(sock, (char *)&command_type, sizeof(uint8_t))) != sizeof(uint8_t)) {
        return -1;
    }

    uint16_t udpPort_to_send = htons(udpPort);
    if ((rv = send_all(sock, (char *)&udpPort_to_send, sizeof(uint16_t))) != sizeof(uint16_t)) {
        return -1;
    }

    return 0;
}

int send_welcome_reply(int sock, uint16_t numStation) {
    int rv;
    uint8_t reply_type = REPLY_TYPE_WELCOME;

    if ((rv = send_all(sock, (char *)&reply_type, sizeof(uint8_t))) != sizeof(uint8_t)) {
        return -1;
    }

    uint16_t numStation_to_send = htons(numStation);
    if ((rv = send_all(sock, (char *)&numStation_to_send, sizeof(uint16_t))) != sizeof(uint16_t)) {
        return -1;
    }

    return 0;
}

int recv_all(int sock, char *buffer, int total_size) {
    int bytes_read = 0;

    char *ptr = buffer;

    while (total_size - bytes_read > 0) {
        int rv = recv(sock, ptr, total_size - bytes_read, 0);
        if (rv <= 0) {
            perror("recv");
            return -1;
        }

        bytes_read += rv;
        ptr += rv;
    }

    return bytes_read;
}

int recv_hello(int sock, struct hello_command *cmd) {
    uint8_t command_type;
    int bytes_read = recv_all(sock, (char *)&command_type, sizeof(uint8_t));

    if (command_type != COMMAND_TYPE_HELLO || bytes_read != sizeof(uint8_t)) {
        return -1;
    }
    cmd->commandType = COMMAND_TYPE_HELLO;

    uint16_t reversed_udpPort;
    bytes_read = recv_all(sock, (char *)&reversed_udpPort, sizeof(uint16_t));
    if (bytes_read != sizeof(uint16_t)) {
        return -1;
    }
    cmd->udpPort = ntohs(reversed_udpPort);

    return 0;
}

int recv_welcome(int sock, struct welcome_reply *reply) {
    uint8_t reply_type;
    int bytes_read = recv_all(sock, (char *)&reply_type, sizeof(uint8_t));

    if (reply_type != REPLY_TYPE_WELCOME || bytes_read != sizeof(uint8_t)) {
        return -1;
    }
    reply->replyType = REPLY_TYPE_WELCOME;

    uint16_t reversed_numStations;
    bytes_read = recv_all(sock, (char *)&reversed_numStations, sizeof(uint16_t));
    if (bytes_read != sizeof(uint16_t)) {
        return -1;
    }
    reply->numStations = ntohs(reversed_numStations);

    return 0;
}