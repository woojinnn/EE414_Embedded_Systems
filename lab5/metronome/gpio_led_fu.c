#include <fcntl.h>
#include <signal.h>  // Defines signal-handling functions (i.e. trap Ctrl-C)
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>  // close()

#include "metro_server.h"

// Global variables
static int fd;

volatile void *gpio_addr;
volatile unsigned int *gpio_datain;
volatile unsigned int *gpio_setdataout_addr;
volatile unsigned int *gpio_cleardataout_addr;

int init_LED() {
    fd = open("/dev/mem", O_RDWR);
    printf("Mapping %X - %X (size: %X)\n", GPIO1_BASE, GPIO1_BASE + GPIO_SIZE, GPIO_SIZE);
    gpio_addr = mmap(0, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_BASE);
    gpio_datain = gpio_addr + GPIO_DATAIN;
    gpio_setdataout_addr = gpio_addr + GPIO_SETDATAOUT;
    gpio_cleardataout_addr = gpio_addr + GPIO_CLEARDATAOUT;

    if (gpio_addr == MAP_FAILED) {
        printf("Unable to map GPIO\n");
        exit(1);
    }
    printf("GPIO mapped to %p\n", gpio_addr);
    printf("GPIO SETDATAOUTADDR mapped to %p\n", gpio_setdataout_addr);
    printf("GPIO CLEARDATAOUT mapped to %p\n", gpio_cleardataout_addr);
}

void play_LED(int number) {
    *gpio_setdataout_addr = number * USR0_LED;
    fprintf(stdout, "%d", number);
    fflush(stdout);
}

void pause_LED(int number) {
    *gpio_cleardataout_addr = number * USR0_LED;
    fprintf(stdout, ".");
    fflush(stdout);
}

void exit_LED() {
    munmap((void *)gpio_addr, GPIO_SIZE);
    close(fd);
}