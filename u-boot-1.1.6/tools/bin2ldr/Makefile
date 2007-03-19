OBJTREE ?= ../..
SRCTREE ?= ../..

CC = gcc
CFLAG = -Wall -g -I$(OBJTREE)/include -I$(SRCTREE)/include
APP_FILE = $(OBJTREE)/u-boot.bin

all: app.ldr

###############################################################
bin2ldr: bin2ldr.o
	$(CC) bin2ldr.o -o bin2ldr
bin2ldr.o: bin2ldr.c 
	$(CC) $(CFLAG) -c bin2ldr.c
###############################################################
jump: jump.o
	bfin-uclinux-ld -o jump -e 0x00000000 jump.o
jump.o: jump.S 
	bfin-uclinux-gcc $(CFLAG) -c jump.S
jump.bin: jump
	bfin-uclinux-objcopy -O binary jump jump.bin
################################################################
init_sdram_bootrom_initblock: init_sdram_bootrom_initblock.o
	bfin-uclinux-ld -o init_sdram_bootrom_initblock -e 0x00000000 init_sdram_bootrom_initblock.o
init_sdram_bootrom_initblock.o:
	cp ../../cpu/$(CPU)/init_sdram_bootrom_initblock.o .
init_sdram_bootrom_initblock.bin: init_sdram_bootrom_initblock
	bfin-uclinux-objcopy -O binary init_sdram_bootrom_initblock init_sdram_bootrom_initblock.bin
################################################################
app.bin:
	cp $(APP_FILE) app.bin
app.ldr: bin2ldr jump.bin init_sdram_bootrom_initblock.bin app.bin
	./runme.sh
################################################################
distclean:
	rm -f bin2ldr bin2ldr.o
	rm -f init_sdram_bootrom_initblock.o init_sdram_bootrom_initblock init_sdram_bootrom_initblock.bin
	rm -f jump.o jump jump.bin
clean:
	make distclean
	rm -f app.bin app.ldr app.hex

.PHONY: all distclean clean app.ldr app.bin
