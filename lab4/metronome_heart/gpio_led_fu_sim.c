#include <fcntl.h>
#include <signal.h>  // Defines signal-handling functions (i.e. trap Ctrl-C)
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>  // close()

#include "userLEDmmap.h"

// Global variables
int fd;

int init_LED() {
}

void play_LED(int number) {
    printf("%d", number);
    fflush(stdout);
}

void pause_LED(int number) {
    printf(".");
    fflush(stdout);
}

void exit_LED() {
}