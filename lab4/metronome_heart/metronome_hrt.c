#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>  // read()

#include "userLEDmmap.h"

char *TimeStr[4] = {"2/4", "3/4", "4/4", "6/8"};

int TimeSig[4][6] = {{7, 1, 0, 0, 0, 0},
                     {7, 1, 1, 0, 0, 0},
                     {7, 1, 3, 1, 0, 0},
                     {7, 1, 1, 3, 1, 1}};

static struct timespec prev = {.tv_sec = 0, .tv_nsec = 0};

unsigned int tempo;
unsigned int status;
unsigned int location;
unsigned int run;
unsigned int on;

void handler(int signo) {
    // change on True and False so that we can blink LED
    if (on == True) {
        play_LED(TimeSig[status][location]);
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

    struct sigaction act;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    act.sa_flags = 0;
    act.sa_mask = set;
    act.sa_handler = &handler;
    sigaction(SIGALRM, &act, NULL);

    status = 3;
    location = 0;
    tempo = 90;
    run = False;
    on = True;

    fprintf(stdout, "Default: TimeSig %d, Tempo %d, Run %d\n", status, tempo, run);
    fflush(stdout);

    init_LED();
    init_KEY();

    // Test loop
    while (1) {
        while (!key_hit()) usleep(10000);

        time = 60 * (NSEC_PER_SEC / (tempo));
        struct itimerspec tim_spec = {
            .it_value =
                {.tv_sec = time / NSEC_PER_SEC,
                 .tv_nsec = (time % NSEC_PER_SEC)},
            .it_interval =
                {.tv_sec = time / NSEC_PER_SEC,
                 .tv_nsec = (time % NSEC_PER_SEC)},
        };

        c = getch();
        switch (c) {
            case 'q':
                break;
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
            case 'm':
                if (run == True) {
                    timer_delete(t_id);
                    run = False;
                } else {
                    if (timer_create(CLOCK_MONOTONIC, NULL, &t_id))
                        perror("timer_create");
                    run = True;
                }
                break;
        }
        if (c == 'q') {
            printf("q: Quit!\n");
            break;
        }

        location = 0;
        on = True;
        if ((int)t_id > 0)
            timer_settime(t_id, 0, &tim_spec, NULL);

        printf("Key %c: TimeSig %s, Tempo %u, Run %u", c, TimeStr[status], tempo, run);
        fflush(stdout);
    }
    fflush(stdout);

    exit_KEY();
    exit_LED();
    return 0;
}