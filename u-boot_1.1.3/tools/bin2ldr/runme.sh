#!/bin/sh
if [ -e ../../u-boot.bin ];then
	make clean
	make all
	./bin2ldr
	bfin-elf-objcopy -I binary -O ihex app.ldr app.hex
	make distclean
else
	echo -e "ERR:Please build the u-boot package first"
fi


