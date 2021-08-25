#! /bin/bash
# restore_user_leds.sh
# Shell program to restore user LEDs

# Restore user LED0
cd /sys/class/leds

cd beaglebone\:green\:usr0
echo heartbeat > trigger 
echo "Restore user LED0"

cd ../beaglebone\:green\:usr1
echo mmc0 > trigger
echo "Restore user LED1"

cd ../beaglebone\:green\:usr2
echo none > trigger
echo "Restore user LED2"

cd ../beaglebone\:green\:usr3
echo none > trigger
echo "Restore user LED3"