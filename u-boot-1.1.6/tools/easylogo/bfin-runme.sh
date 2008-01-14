#!/bin/sh
make GZIP=1
./easylogo -r -g -b linux_bf.tga bfin_logo bfin_logo_230x230.h
mv bfin_logo_230x230.h ../../cpu/blackfin
