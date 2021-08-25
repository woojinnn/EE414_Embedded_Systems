CC = gcc
CFLAGS = -pthread
OBJS = metronome_hrt
LDFLAGS = -lrt


all: $(OBJS)

metronome_hrt: ./metronome_heart/metronome_hrt.c ./metronome_heart/gpio_led_fu.c ./metronome_heart/key_input_fu.c
	arm-linux-gnueabihf-gcc $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJS)