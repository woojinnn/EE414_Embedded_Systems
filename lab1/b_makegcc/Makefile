#	Embedded Systems Lab1 makegcc Makefile
#
# Source file: helloes.c

#Do everything
all:	nc cc

# Native Compile
nc:
	gcc -o helloes helloes.c

#Cross compile for Bone Ubuntu
cc:
	arm-linux-gnueabihf-gcc -o helloes.x helloes.c

# Clean up
clean:
	rm -f helloes helloes.x
