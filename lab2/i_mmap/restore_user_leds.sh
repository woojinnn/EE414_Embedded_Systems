#! /bin/bash
# restore_user_leds.sh
# Shell program to Restore userLEDs

cd /sys/class/leds

cd beaglebone\:green\:usr0
echo hearbeat > trigger
echo "Restore user LED0"

cd ../beaglebone\:green\:usr1
echo mmc0 > trigger
echo "Restore user LED1"

cd ../beaglebone\:green\:usr2
echo cpu0 > trigger
echo "Restore user LED2"

cd ../beaglebone\:green\:usr3
echo mmc1 > trigger
echo "Restore user LED3"
