CC = gcc
CFLAGS = -pthread
OBJS = test_single_key test_single_key_nb algo_metro_tui_pc metronome_tui_thread 

all: $(OBJS)

test_single_key: ./get_single_key/test_single_key.c
	$(CC) -o $@ $^

test_single_key_nb: ./get_single_key/test_single_key_nb.c 
	$(CC) -o $@ $^

algo_metro_tui_pc: ./algo_metronome_tui/algo_metronome_tui.c
	$(CC) -o $@ $^

metronome_tui_thread: ./metronome_thread/metronome_tui_thread.c ./metronome_thread/gpio_led_fu.c ./metronome_thread/key_input_fu.c
	arm-linux-gnueabihf-gcc $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJS)