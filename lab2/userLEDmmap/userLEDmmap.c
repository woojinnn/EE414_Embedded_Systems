// From: http://stackoverflow.com/questions/13124271/driving-beaglebone-gpio
// -through-dev-mem
// user contributions licensed under cc by-sa 3.0 with attribution required
// http://creativecommons.org/licenses/by-sa/3.0/
// http://blog.stackoverflow.com/2009/06/attribution-required/
// Author: madscientist159 (http://stackoverflow.com/users/3000377/madscientist159)
//
// Read one gpio pin and write it out to another using mmap.
// Be sure to set -O3 when compiling.

// Change both .c and .h files to control four user LEDs, instead of the push button and the external LED, used in the tutorial example.
// You may decide not to change the .h file.

// Check four user LEDs can be controlled (in an arbitrary manner).
// For example, you may try to blink the first user LED. In this step, just check the mapping works.

#include "userLEDmmap.h"

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
    signal(SIGINT, signal_handler);

    int fd = open("/dev/mem", O_RDWR);
    printf("Mapping %X - %X (size: %X)\n", GPIO1_START_ADDR, GPIO1_END_ADDR, GPIO1_SIZE);

    gpio_addr = mmap(0, GPIO1_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_START_ADDR);
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

    printf("Manipulating LEDs\n");
    int time = 1;
    // pushLEDmmap uses usleep function, but usleep is in micro_second unit, so I modified it to sleep function.
    // turns on, turns off one by one
    // should run stop_user_leds.sh first.
    while (keepgoing) {
        // turn on LEDs
        printf("turning on LEDs\n");
        *gpio_setdataout_addr = USER0;
        sleep(time);
        *gpio_setdataout_addr = USER1;
        sleep(time);
        *gpio_setdataout_addr = USER2;
        sleep(time);
        *gpio_setdataout_addr = USER3;
        sleep(time);

        // turn off LEDs
        printf("turning off LEDs\n");
        *gpio_cleardataout_addr = USER0;
        sleep(time);
        *gpio_cleardataout_addr = USER1;
        sleep(time);
        *gpio_cleardataout_addr = USER2;
        sleep(time);
        *gpio_cleardataout_addr = USER3;
        sleep(time);
    }

    munmap((void *)gpio_addr, GPIO1_SIZE);
    close(fd);

    return 0;
}