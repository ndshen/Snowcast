#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Usage: client <server address> <server port> <udpPort>\n");
        exit(1);
    }

    char *server_address = argv[1];
    char *server_port = argv[2];
    uint16_t udp_port = (uint16_t)atoi(argv[3]);

    int rv;
    struct addrinfo hints, *res, *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(server_address, server_port, &hints, &servinfo)) != 0) {
        perror(gai_strerror(rv));  // translate the error code to readable string
        exit(1);
    }

    int sock;
    for (res = servinfo; res != NULL; res = res->ai_next) {
        if ((sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
            continue;
        }

        if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
            close(sock);
            continue;
        }

        break;
    }

    if (send_hello_command(sock, udp_port) != 0) {
        printf("fail to send hello\n");
        exit(1);
    }

    struct welcome_reply welcome;
    if (recv_welcome(sock, &welcome) != 0) {
        printf("fail to recv welcome\n");
        exit(1);
    }

    printf("Server has %d stations\n", (int)welcome.numStations);

    return 0;
}