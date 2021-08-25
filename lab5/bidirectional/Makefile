CC = gcc
LDFLAGS = -lpthread
TARGET = server client

all: $(TARGET)

server: serverbd.c
	arm-linux-gnueabihf-gcc -o $@ $^ $(LDFLAGS)

client: clientbd.c
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o
	rm -f $(TARGET)