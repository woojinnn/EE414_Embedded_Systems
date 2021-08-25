#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT "3490"      // the port client will be connecting to
#define MAXDATASIZE 100  // max number of bytes we can get at once

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET)  // IPv4 Internet Protocol
        return &(((struct sockaddr_in *)sa)->sin_addr);

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

static int sockfd;
static unsigned rate;

void *receiving(void *data) {
    // thread routine: receive and print
    char packet[20];
    for (int i = 0; i < 10; i++) {
        recv(sockfd, packet, 20, 0);
        printf("sockfd: %d\npacket: %s\n\n", sockfd, packet);
        usleep(rate);
    }
}

int main(int argc, char *argv[]) {
    int numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    int thread_id;
    pthread_t pthread;

    if (argc != 3) {
        fprintf(stderr, "usage: client hostname and tempo in Hz\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    // get the host information
    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        // open a steram socket
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo);  // all done with this structure

    send(sockfd, argv[2], 10, 0);
    rate = 1000000 / atoi(argv[2]);

    char packet[20] = "client: from main";
    thread_id = pthread_create(&pthread, NULL, receiving, NULL);

    // main thread routine: send
    for (int i = 0; i < 5; i++) {
        send(sockfd, packet, 20, 0);
        usleep(rate * 2);
    }

    pthread_join(pthread, NULL);
    close(sockfd);
    return 0;
}