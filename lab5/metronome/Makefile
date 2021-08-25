LDFLAGS = -lpthread -lrt
TARGET = server client

all: $(TARGET)

server: metroServer.c gpio_led_fu.c
	arm-linux-gnueabihf-gcc -o $(CFLAGS) $@ $^ $(LDFLAGS)

client: metroClient.c key_input_fu.c
	gcc -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)