#!/bin/sh

# strongly recommended to run this script to convert u-boot.bin to LDR format file

if [ ! -e bin2ldr ] ; then
	make all || exit 1
fi

[ -z "${OBJTREE}" ] && OBJTREE="../.."

if [ -e ${OBJTREE}/u-boot.bin ] ; then
	./bin2ldr --proc bf537 || exit 1
	bfin-uclinux-objcopy -I binary -O ihex app.ldr app.hex || exit 1
	exit 0
else
	echo "ERR: Please build the u-boot package first"
	exit 1
fi
