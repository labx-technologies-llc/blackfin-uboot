#!/bin/sh
CC = gcc
CFLAG = -g
APP_FILE = ../../u-boot.bin

###############################################################
bin2ldr: bin2ldr.o
	$(CC) bin2ldr.o -o bin2ldr
bin2ldr.o: bin2ldr.c bin2ldr.h
	$(CC) $(CFLAG) -c bin2ldr.c
###############################################################
jump:   jump.o
	bfin-uclinux-ld -o jump -e 0x00000000 jump.o
jump.o: jump.S bin2ldr.h
	bfin-uclinux-gcc $(CFLAG) -c jump.S
jump.bin: jump
	bfin-uclinux-objcopy -O binary jump jump.bin
################################################################
init_sdram: init_sdram.o
	bfin-uclinux-ld -o init_sdram -e 0x00000000 init_sdram.o
init_sdram.o:
	cp ../../cpu/bf533/init_sdram.o .
init_sdram.bin: init_sdram
	bfin-uclinux-objcopy -O binary init_sdram init_sdram.bin
################################################################
app.bin:
	cp $(APP_FILE) app.bin
################################################################
all:    bin2ldr jump.bin init_sdram.bin app.bin
distclean:
	rm bin2ldr bin2ldr.o -f
	rm init_sdram.o init_sdram init_sdram.bin -f
	rm jump.o jump jump.bin -f
clean:
	make distclean
	rm app.bin app.ldr app.hex -f
