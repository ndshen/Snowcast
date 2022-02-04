#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "protocol.h"

#define SERVER_PORT "8888"
#define LISTEN_MAX 15

struct client_data {
    int sock;
    struct sockaddr_storage addr;
    socklen_t addr_size;

    pthread_t thread;
};

void *client_handler(void *data) {
    struct client_data *cd = (struct client_data *)data;
    printf("Client %ld connected!\n", cd->thread);

    struct hello_command hello;
    recv_hello(cd->sock, &hello);
    printf("Client %ld has udpPort %d!\n", cd->thread, (int)hello.udpPort);

    uint16_t temp_num_stations = 10;
    send_welcome_reply(cd->sock, temp_num_stations);

    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    int rv;        // return value
    int s_listen;  // the socket on listen port

    // construct hints
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;      // can be IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;  // tcp
    hints.ai_flags = AI_PASSIVE;      // localhost

    // getaddrinfo uses the hints we provide and return a list of results
    if ((rv = getaddrinfo(NULL, SERVER_PORT, &hints, &res)) < 0) {
        perror("getaddrinfo");
        exit(1);
    }

    // use the first result of getaddrinfo and construct a socket for it
    if ((s_listen = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        perror("socket");
        exit(1);
    }

    // bind the socket with the port
    if ((rv = bind(s_listen, res->ai_addr, res->ai_addrlen)) < 0) {
        perror("bind");
        exit(1);
    }

    if ((rv = listen(s_listen, LISTEN_MAX)) < 0) {
        perror("listen");
        exit(1);
    }

    while (1) {
        int s_client;
        struct sockaddr_storage client_addr;
        socklen_t addr_size;

        if ((s_client = accept(s_listen, (struct sockaddr *)&client_addr, &addr_size)) < 0) {
            perror("accept");
            continue;
        }

        struct client_data *cd = (struct client_data *)malloc(sizeof(struct client_data));
        memset(cd, 0, sizeof(struct client_data));
        cd->sock = s_client;
        memcpy(&cd->addr, &client_addr, sizeof(struct sockaddr_storage));
        cd->addr_size = addr_size;

        pthread_create(&cd->thread, NULL, client_handler, (void *)cd);
    }

    return 0;
}