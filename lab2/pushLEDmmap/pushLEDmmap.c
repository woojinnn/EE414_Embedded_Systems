// From: http://stackoverflow.com/questions/13124271/driving-beaglebone-gpio
// -through-dev-mem
// user contributions licensed under cc by-sa 3.0 with attribution required
// http://creativecommons.org/licenses/by-sa/3.0/
// http://blog.stackoverflow.com/2009/06/attribution-required/
// Author: madscientist159 (http://stackoverflow.com/users/3000377/madscientist159)
//
// Read one gpio pin and write it out to another using mmap.
// Be sure to set -O3 when compiling.

#include "pushLEDmmap.h"

#include <fcntl.h>
#include <signal.h>  // Defines signal-handling functions (i.e. trap Ctrl-C)
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>  // close()

// Global variables
volatile int keepgoing = 1;

// Set to 0 when Ctrl-c is pressed
// Callback called when SIGINT is sent to the process (Ctrl-C)
void signal_handler(int sig) {
    printf("\nCtrl-C pressed, cleaning up and exiting...\n");
    keepgoing = 0;
}

int main(int argc, char *argv[]) {
    volatile void *gpio_addr;
    volatile unsigned int *gpio_datain;
    volatile unsigned int *gpio_setdataout_addr;
    volatile unsigned int *gpio_cleardataout_addr;

    // Set the signal callback for Ctrl-C
    // call signal_handler function when Ctrl-C comes in.
    signal(SIGINT, signal_handler);

    // file descripter for mmap
    int fd = open("/dev/mem", O_RDWR);
    printf("Mapping %X - %X (size: %X)\n", GPIO0_START_ADDR, GPIO0_END_ADDR, GPIO0_SIZE);

    // mmap: memory mapping
    // from GPIO0_START_ADDR, get GPIO0_SIZE amount of memory
    gpio_addr = mmap(0, GPIO0_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO0_START_ADDR);
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
    printf("Start copying GPIO_07 to GPIO_31\n");

    while (keepgoing) {
        // Since GPIO_07 is 1<<7, it only checks 7th bit.
        if (*gpio_datain & GPIO_07) {         //Check whether button is pushed
            *gpio_setdataout_addr = GPIO_31;  //LED on
        } else {
            *gpio_cleardataout_addr = GPIO_31;  //LED off
        }
        //usleep(1);
    }

    // To remove mapping made by mmap function.
    munmap((void *)gpio_addr, GPIO0_SIZE);
    close(fd);

    return 0;
}