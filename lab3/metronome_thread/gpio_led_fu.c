#include <fcntl.h>
#include <pthread.h>
#include <signal.h>  // Defines signal-handling functions (i.e. trap Ctrl-C)
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>  // read, close()

#include "userLEDmmap.h"

volatile void* gpio_addr;
volatile unsigned int* gpio_datain;
volatile unsigned int* gpio_setdataout_addr;
volatile unsigned int* gpio_cleardataout_addr;
int timeSig[4][6] = {{7, 1, 0, 0, 0, 0},
                     {7, 1, 1, 0, 0, 0},
                     {7, 1, 3, 1, 0, 0},
                     {7, 1, 1, 3, 1, 1}};
pthread_t pthread;
int fd;

void* turn_LED(void* data) {
    __useconds_t time;

    while (keepgoing) {
        pthread_mutex_lock(&mutex_lock);
        if (run == True) {
            // tempo: bpm: beats per minute(=60s=60000000us)
            time = 60000000 / tempo;
            printf("%d", timeSig[timeSig_idx][timeSig_idx2]);
            fflush(stdout);
            *gpio_setdataout_addr = timeSig[timeSig_idx][timeSig_idx2] * USER0;
            usleep(time);

            *gpio_cleardataout_addr = timeSig[timeSig_idx][timeSig_idx2] * USER0;
            usleep(time);

            timeSig_idx2++;
            if (timeSig[timeSig_idx][timeSig_idx2] == 0 || timeSig_idx2 > 5)
                timeSig_idx2 = 0;
        } else
            usleep(1);
        pthread_mutex_unlock(&mutex_lock);
    }
    pthread_exit(0);
}

void init_LED() {
    int thread_result;
    fd = open("/dev/mem", O_RDWR);
    // printf("Mapping %X - %X (size: %X)\n", GPIO1_START_ADDR, GPIO1_END_ADDR, GPIO1_SIZE);

    gpio_addr = mmap(0, GPIO1_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_START_ADDR);
    gpio_datain = gpio_addr + GPIO_DATAIN;
    gpio_setdataout_addr = gpio_addr + GPIO_SETDATAOUT;
    gpio_cleardataout_addr = gpio_addr + GPIO_CLEARDATAOUT;

    if (gpio_addr == MAP_FAILED) {
        printf("Unable to map GPIO\n");
        exit(1);
    }
    // printf("GPIO mapped to %p\n", gpio_addr);
    // printf("GPIO SETDATAOUTADDR mapped to %p\n", gpio_setdataout_addr);
    // printf("GPIO CLEARDATAOUT mapped to %p\n", gpio_cleardataout_addr);

    pthread_mutex_init(&mutex_lock, NULL);
    thread_result = pthread_create(&pthread, NULL, turn_LED, NULL);
    if (thread_result < 0) {
        munmap((void*)gpio_addr, GPIO1_SIZE);
        close(fd);
        perror("init_LED(): cannot make thread\n");
    } else {
        printf("Thread created successfully\n");
    }
}

void exit_LED() {
    keepgoing = 0;
    pthread_mutex_destroy(&mutex_lock);
    pthread_join(pthread, NULL);
    munmap((void*)gpio_addr, GPIO1_SIZE);
    close(fd);
}