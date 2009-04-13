/*
 * flash.c - helper commands for working with GPIO-assisted flash
 *
 * Copyright (c) 2005-2009 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <common.h>
#include <command.h>
#include <asm/blackfin.h>
#include "gpio_cfi_flash.h"

int do_pf4(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong faddr = CONFIG_SYS_FLASH_BASE;
	ushort data;
	ulong dflg;

	if (argc > 1) {
		dflg = simple_strtoul(argv[1], NULL, 16);
		faddr |= (dflg << 21);
		gpio_cfi_flash_swizzle((void *)faddr);
	} else {
		data = bfin_read_PORTFIO();
		printf("PF4 data %04x\n", data);
	}

	return 0;
}

U_BOOT_CMD(pf4, 2, 0, do_pf4,
	"set/clear PF4 GPIO flash bank switch\n",
	"1 - set GPIO pins\n"
	"pf4 0 - clear GPIO pin\n");
