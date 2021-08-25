#! /bin/bash
# stop_user_leds.sh
# Shell program to stop user LEDs

# Stop user LEd0
cd /sys/class/leds

cd beaglebone\:green\:usr0
echo none > trigger
echo "Stop user LED0"

cd ../beaglebone\:green\:usr1
echo none > trigger
echo "Stop user LED1"

cd ../beaglebone\:green\:usr2
echo none > trigger
echo "Stop user LED2"

cd ../beaglebone\:green\:usr3
echo none > trigger
echo "Stop user LED3"
