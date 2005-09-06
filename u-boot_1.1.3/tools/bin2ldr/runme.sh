# strongly recommend to run this script to convert u-boot.bin to LDR format file
#!/bin/sh
if [ -e ../../u-boot.bin ];then
	make clean
	make all
	./bin2ldr
	bfin-uclinux-objcopy -I binary -O ihex app.ldr app.hex
	make distclean
else
	echo -e "ERR:Please build the u-boot package first"
fi


