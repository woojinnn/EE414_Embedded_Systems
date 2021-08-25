
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>  // read()

#define PORT "3490"

#include "metro_client.h"

char *TimeStr[4] = {"2/4", "3/4", "4/4", "6/8"};
int TimeSig[4][6] = {{7, 1, 0, 0, 0, 0},
                     {7, 1, 1, 0, 0, 0},
                     {7, 1, 3, 1, 0, 0},
                     {7, 1, 1, 3, 1, 1}};

static struct timespec prev = {.tv_sec = 0, .tv_nsec = 0};

static int sockfd;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET)  // IPv4 Internet Protocol
        return &(((struct sockaddr_in *)sa)->sin_addr);

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void *recv_LED_signal(void *data) {
    char LED_signal[2];
    while (1) {
        recv(sockfd, LED_signal, 2, 0);
        printf("%s", LED_signal);
        fflush(stdout);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }

    int tempo = 90;
    int status = 3;
    int location = 0;
    int run = False;
    int on = True;

    char c;
    int thread_id;
    pthread_t pthread;
    char packet[30];

    char s[INET6_ADDRSTRLEN];
    int rv;

    struct addrinfo hints, *servinfo, *p;
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

    // Prologue
    thread_id = pthread_create(&pthread, NULL, recv_LED_signal, NULL);
    init_KEY();
    printf("Default: TimeSig %s, Tempo %d, Run %d\n", TimeStr[status], tempo, run);

    while (1) {
        while (!key_hit()) usleep(10000);
        c = getch();
        switch (c) {
            case 'z':
                status = (status + 1) % 4;
                break;

            case 'c':
                tempo -= 5;
                if (tempo < 30)
                    tempo = 30;
                break;

            case 'b':
                tempo += 5;
                if (tempo > 200)
                    tempo = 200;
                break;

            case 'q':
                send(sockfd, "Quit", 5, 0);
                break;

            case 'm':
                if (run == True) {
                    send(sockfd, "Stop", 5, 0);
                    run = False;
                } else {
                    sprintf(packet, "status %d , tempo %d", status, tempo);
                    send(sockfd, packet, 30, 0);
                    run = True;
                }
        }

        if (c == 'q') {
            printf("Quit\n");
            fflush(stdout);
            break;
        }

        location = 0;
        on = True;
        printf("Key %c: TimeSig %s, Tempo %d, Run %d\n", c, TimeStr[status], tempo, run);
        fflush(stdout);
    }

    // epilogue
    fflush(stdout);
    close(sockfd);
    exit_KEY();
    return 0;
}
