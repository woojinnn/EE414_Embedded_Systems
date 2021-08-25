#	Embedded Systems Lab1 c_taylor Makefile

# Source file: taylor_ce.c,	taylor_re.c,	taylor_ok.c

# Do everything
all:	nce nre nok cok

# Native Compile
# Note: Be sure to add '-lm' to include math library.
nce:
	gcc -o taylor_ce_pc taylor_ce.c -lm

nre:
	gcc -o taylor_re_pc taylor_re.c -lm

nok:
	gcc -g -O0 -o taylor_ok_pc taylor_ok.c -lm

# Cross Compile
cok:
	arm-linux-gnueabihf-gcc -o taylor_ok taylor_ok.c -lm
