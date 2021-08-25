#! /bin/bash
# Shell program to control LED0 with on_time & off_time
#
# Check for two arguments
# $# means the number of arguments
if [ $# -lt 2 ]
  then
    echo "Usage:  control_led0.sh on_time off_time"
    exit
fi

# Control LED0
echo "Control LED0 with on_time $1 and off_time $2"
cd /sys/class/leds
cd beaglebone\:green\:usr0
echo none > trigger
echo timer > trigger
echo $1 > delay_on
echo $2 > delay_off
