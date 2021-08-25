#ifndef _BEAGLEBONE_GPIO_H_
#define _BEAGLEBONE_GPIO_H_

// AM33xx GPIO Base addr & size
#define GPIO0_BASE 0x44E07000
#define GPIO1_BASE 0x4804C000
#define GPIO2_BASE 0x481AC000
#define GPIO3_BASE 0x481AE000

#define GPIO_SIZE 0x00001000

// AM33xx GPIO Register offsets
#define GPIO_OE 0x134 /* 0 output, 1 input */
#define GPIO_DATAIN 0x138
#define GPIO_DATAOUT 0x13C
#define GPIO_CLEARDATAOUT 0x190
#define GPIO_SETDATAOUT 0x194

// Four User LEDs bits
#define USR0_LED (1 << 21)
#define USR1_LED (1 << 22)
#define USR2_LED (1 << 23)
#define USR3_LED (1 << 24)

// GPIO Gr 0 pins
#define GPIO_03 (1 << 3)
#define GPIO_07 (1 << 7)
#define GPIO_31 (1 << 31)

// GPIO Gr 1 pins
#define GPIO_60 (1 << 28)

#define True 1
#define False 0

#define NSEC_PER_SEC 1000000000L

int init_LED();
void play_LED(int);
void pause_LED(int);
void exit_LED();

#endif