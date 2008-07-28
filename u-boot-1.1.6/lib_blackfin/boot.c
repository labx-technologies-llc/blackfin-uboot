/*
 * U-boot - boot.c - misc boot helper functions
 *
 * Copyright (c) 2005-2008 Analog Devices Inc.
 *
 * (C) Copyright 2000-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * Licensed under the GPL-2 or later.
 */

#include <common.h>
#include <command.h>
#include <image.h>
#include <asm/byteorder.h>

#ifdef SHARED_RESOURCES
extern void swap_to(int device_id);
#endif

char *bfin_make_command_line(void)
{
	char *dest = (char *)CFG_LINUX_CMDLINE_ADDR;
	char *bootargs = getenv("bootargs");

	if (bootargs == NULL)
		return NULL;

	strncpy(dest, bootargs, CFG_LINUX_CMDLINE_SIZE);
	dest[CFG_LINUX_CMDLINE_SIZE - 1] = 0;
	return dest;
}

extern image_header_t header;

void do_bootm_linux(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[],
		    ulong addr, ulong *len_ptr, int verify)
{
	int (*appl) (char *cmdline);
	char *cmdline;

#ifdef SHARED_RESOURCES
	swap_to(FLASH);
#endif

	appl = (int (*)(char *))ntohl(header.ih_ep);
	printf("Starting Kernel at = %x\n", appl);
	cmdline = bfin_make_command_line();
	icache_disable();
	dcache_disable();
	(*appl) (cmdline);
}
