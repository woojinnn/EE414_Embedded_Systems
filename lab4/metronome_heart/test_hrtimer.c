#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define NSEC_PER_SEC 1000000000L

#define timerdiff(a, b) ((float)((a)->tv_sec - (b)->tv_sec) + \
                         ((float)((a)->tv_nsec - (b)->tv_nsec)) / NSEC_PER_SEC)

static struct timespec prev = {.tv_sec = 0, .tv_nsec = 0};
static int count = 5;

void handler(int signo) {
    struct timespec now;

    clock_gettime(CLOCK_MONOTONIC, &now);
    printf("inside signal_handler:\nnow.tv_sec: %d\tnow.tv_nsec: %d\n", now.tv_sec, now.tv_nsec);
    printf("[%d]Diff time:%lf\n", count, timerdiff(&now, &prev));
    prev = now;
    count--;
}

int main(int argc, char *argv[]) {
    int i = 0;
    timer_t t_id;

    struct itimerspec tim_spec = {
        .it_interval = {.tv_sec = 1, .tv_nsec = 10000},
        .it_value = {.tv_sec = 2, .tv_nsec = 10000}};

    struct sigaction act;
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGALRM);

    act.sa_flags = 0;
    act.sa_mask = set;
    act.sa_handler = &handler;

    sigaction(SIGALRM, &act, NULL);

    if (timer_create(CLOCK_MONOTONIC, NULL, &t_id))
        perror("timer_create");

    if (timer_settime(t_id, 0, &tim_spec, NULL))
        perror("timer_settime");

    printf("current prev val:\ntv_sec:%d\ttv_nsec:%d\n", prev.tv_sec, prev.tv_nsec);
    clock_gettime(CLOCK_MONOTONIC, &prev);
    printf("after clock_gettime:\ntv_sec:%d\ttv_nsec:%d\n", prev.tv_sec, prev.tv_nsec);

    for (;;) {
        if (count == 0)
            break;
    }

    return 0;
}
