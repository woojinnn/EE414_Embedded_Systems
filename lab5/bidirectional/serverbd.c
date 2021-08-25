#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PORT "3490"  // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold

void sigchld_handler(int s) {
    (void)s;  // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

static int new_fd;  // for pthread and main thread
static unsigned int rate;

void *sending(void *data) {
    char packet[20] = "server: from thread";
    for (int i = 0; i < 10; i++) {
        send(new_fd, packet, 20, 0);
        usleep(rate);
    }
}

int main(void) {
    int sockfd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;  // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    char tempo[10];
    int rv;

    pthread_t pthread;
    int thread_id;
    char packet[20];

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

    sa.sa_handler = sigchld_handler;  // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while (1) {  // main accept() loop
        sin_size = sizeof their_addr;

        // got new connect() request from child
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);

        recv(new_fd, tempo, 10, 0);
        printf("input tempo: %s\n", tempo);
        rate = 1000000 / atoi(tempo);

        if (!fork()) {      // this is the child process
            close(sockfd);  // child doesn't need the listener

            thread_id = pthread_create(&pthread, NULL, sending, NULL);

            // process for main thread
            for (int i = 0; i < 5; i++) {
                recv(new_fd, packet, 20, 0);
                printf("received from client fd %d: %s\n", new_fd, packet);
                usleep(rate * 2);
            }

            // epilogue
            pthread_join(pthread, NULL);

            printf("Done\n");
            close(new_fd);
            exit(0);
        }

        close(new_fd);  // parent doesn't need this
    }

    return 0;
}
