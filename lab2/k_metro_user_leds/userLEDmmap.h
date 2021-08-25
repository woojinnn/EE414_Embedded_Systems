// From: http://stackoverflow.com/questions/13124271/driving-beaglebone-gpio
// -through-dev-mem
// user contributions licensed under cc by-sa   3.0 with attribution required
// http://creativecommons.org/licenses/by-sa/3.0/ 
// http://blog.stackoverflow.com/2009/06/attribution-required/ 
// Author: madscientist159 (http://stackoverflow.com/users/3000377/madscientist159) 

#ifndef _BEAGLEBONE_GPIO_H_ 
#define _BEAGLEBONE_GPIO_H_ 
#define GPIO0_START_ADDR 0x44e07000 
#define GPIO0_END_ADDR   0x44e08000 
#define GPIO0_SIZE (GPIO0_END_ADDR - GPIO0_START_ADDR) 


// We will use GPIO1 since user LED is in GPIO1
#define GPIO1_START_ADDR 0x4804C000 
#define GPIO1_END_ADDR   0x4804D000 
#define GPIO1_SIZE (GPIO1_END_ADDR - GPIO1_START_ADDR) 

#define GPIO2_START_ADDR 0x41A4C000 
#define GPIO2_END_ADDR   0x41A4D000 
#define GPIO2_SIZE (GPIO2_END_ADDR - GPIO2_START_ADDR) 

#define GPIO3_START_ADDR 0x41A4E000 
#define GPIO3_END_ADDR   0x41A4F000 
#define GPIO3_SIZE (GPIO3_END_ADDR - GPIO3_START_ADDR) 

#define GPIO_DATAIN 0x138 
#define GPIO_SETDATAOUT 0x194 
#define GPIO_CLEARDATAOUT 0x190

// I was bit confused, because beaglebone black manual says
// USR0, USR1, USR2, USR3 are allocated to GPIO1_21, GPIO2_22, GPIO2_23, GPIO2_24
// However, in my case, I followed lab document, GPIO1_21 ~ GPIO1_24, and it worked.
#define USER0   1<<21
#define USER1   1<<22
#define USER2   1<<23
#define USER3   1<<24
#endif