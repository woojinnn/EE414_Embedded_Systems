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

volatile void *gpio_addr;
volatile unsigned int *gpio_datain;
volatile unsigned int *gpio_setdataout_addr;
volatile unsigned int *gpio_cleardataout_addr;

void init_LED() {
    fd = open("/dev/mem", O_RDWR);
    gpio_addr = mmap(0, GPIO1_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_START_ADDR);
    gpio_datain = gpio_addr + GPIO_DATAIN;
    gpio_setdataout_addr = gpio_addr + GPIO_SETDATAOUT;
    gpio_cleardataout_addr = gpio_addr + GPIO_CLEARDATAOUT;

    if (gpio_addr == MAP_FAILED) {
        printf("Unable to map GPIO\n");
        exit(1);
    }
    // printf("GPIO1 at %p is mapped to %p\n", (void*)GPIO1_START_ADDR, gpio_addr);
}

void play_LED(int number) {
    *gpio_setdataout_addr = number * USER0;
    fprintf(stdout, "%d", number);
    fflush(stdout);
}

void pause_LED(int number) {
    *gpio_cleardataout_addr = number * USER0;
}

void exit_LED() {
    munmap((void *)gpio_addr, GPIO1_SIZE);
    close(fd);
}