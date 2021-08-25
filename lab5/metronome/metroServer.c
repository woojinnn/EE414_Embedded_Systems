#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>  // read()

#include "metro_server.h"

#define PORT "3490"  // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold

char *TimeStr[4] = {"2/4", "3/4", "4/4", "6/8"};

int TimeSig[4][6] = {{7, 1, 0, 0, 0, 0},
                     {7, 1, 1, 0, 0, 0},
                     {7, 1, 3, 1, 0, 0},
                     {7, 1, 1, 3, 1, 1}};

char *TimeSig_char[4][6] = {{"#", "!", "0", "0", "0", "0"},
                            {"#", "!", "!", "0", "0", "0"},
                            {"#", "!", "+", "!", "0", "0"},
                            {"#", "!", "!", "+", "!", "!"}};

static struct timespec prev = {.tv_sec = 0, .tv_nsec = 0};

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in *)sa)->sin_addr);

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

static int tempo;
static int status;
static int location;
static int on;
static int new_fd;

void handler(int signo) {
    if (on == True) {
        play_LED(TimeSig[status][location]);
        send(new_fd, TimeSig_char[status][location], 2, 0);
        on = False;
    } else {
        pause_LED(TimeSig[status][location]);
        on = True;
        location++;
        if ((TimeSig[status][location] == 0) || (location > 5))
            location = 0;
    }
}

int main(void) {
    char c;
    timer_t t_id;
    unsigned long long time;
    char packet[30], current_TimeSig[4];
    int recv_bytes;

    int sockfd;
    int yes = 1;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];
    int rv;

    init_LED();

    /* Socket setup */
    // load up address structs with getaddrinfo()
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;      // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;  // stream
    hints.ai_flags = AI_PASSIVE;      // use my IP
    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        // make a socket
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        // bind it to the port we passed in to getaddrinfo()
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }
    freeaddrinfo(servinfo);  // all done with this structure
    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    // start listening... Waiting for client's connect() request
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    /* SigALRM  control */
    struct sigaction act;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    act.sa_flags = 0;
    act.sa_mask = set;
    act.sa_handler = &handler;
    sigaction(SIGALRM, &act, NULL);

    while (1) {
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        printf("Connect\n");
        if (!fork()) {
            close(sockfd);

            while (1) {
                memset(packet, 0, 30);
                recv_bytes = recv(new_fd, packet, 30, 0);

                if (recv_bytes > 10) {
                    sscanf(packet, "status %d , tempo %d", &status, &tempo);

                    time = 60 * (NSEC_PER_SEC / (2 * tempo));
                    struct itimerspec tim_spec = {
                        .it_value =
                            {.tv_sec = time / NSEC_PER_SEC,
                             .tv_nsec = (time % NSEC_PER_SEC)},
                        .it_interval =
                            {.tv_sec = time / NSEC_PER_SEC,
                             .tv_nsec = (time % NSEC_PER_SEC)},
                    };

                    location = 0;
                    on = True;
                    if (timer_create(CLOCK_MONOTONIC, NULL, &t_id))
                        perror("timer_create\n");
                    if ((int)t_id > 0)
                        timer_settime(t_id, 0, &tim_spec, NULL);
                    printf("Current: TimeSig %s, Tempo %d, Run 1\n", TimeStr[status], tempo);
                }

                else if (strcmp(packet, "Stop") == 0) {
                    timer_delete(t_id);
                    pause_LED(7);
                }

                else if (strcmp(packet, "Quit") == 0) {
                    timer_delete(t_id);
                    pause_LED(7);
                    break;
                }

                fflush(stdout);
            }
            close(new_fd);
            fflush(stdout);
            exit(0);
        } else {
            wait(NULL);
            close(new_fd);
        }
    }

    close(sockfd);
    exit_LED();
}