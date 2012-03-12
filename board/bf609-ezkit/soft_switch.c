/*
 * U-boot - main board file
 *
 * Copyright (c) 2008-2011 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <common.h>
#include <asm/blackfin.h>
#include <asm/io.h>
#include <i2c.h>
#include "soft_switch.h"

#ifdef CONFIG_BFIN_BOARD_VERSION_1_2
#define SWITCH_ADDR	0x21
#else
#define SWITCH_ADDR	0x20
#endif

#define IODIRA		0x0
#define OLATA		0x14

int setup_soft_switch(int index, int level)
{
	int ret = 0;
	uchar data;
	int addr, pin, port;

	addr = index >> 4;
	if (addr > 2)
		return -1;
	addr += SWITCH_ADDR;
	pin = 1 << (index % 8);
	port = (index % 16) >> 3;

	ret = i2c_read(addr, IODIRA + port, 1, &data, 1);
	if (ret)
		return ret;
	data &= ~pin;
	ret = i2c_write(addr, IODIRA + port, 1, &data, 1);
	if (ret)
		return ret;
	ret = i2c_read(addr, OLATA + port, 1, &data, 1);
	if (ret)
		return ret;
	if (level)
		data |= pin;
	else
		data &= ~pin;
	ret = i2c_write(addr, OLATA + port, 1, &data, 1);
	if (ret)
		return ret;
}

