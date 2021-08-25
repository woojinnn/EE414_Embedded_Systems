#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int main(void) {
    timer_t timer = timer_Create();

    struct itimerspec ts;
    ts.it_value.tv_sec = 1;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 2;
    ts.it_interval.tv_nsec = 0;

    int ret = timer_settime(timer, 0, &ts, NULL);

    if (ret == -1) {
        printf("Fail to timer_settimer(): %m\n");
        return -1;
    }

    return 0;
}