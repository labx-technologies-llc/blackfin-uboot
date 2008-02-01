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

static char *make_command_line(void)
{
	char *dest = (char *)CMD_LINE_ADDR;
	char *bootargs = getenv("bootargs");

	if (bootargs == NULL)
		return NULL;

	strncpy(dest, bootargs, 0x1000);
	dest[0xfff] = 0;
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
	cmdline = make_command_line();
	if (icache_status())
		icache_disable();
	if (dcache_status())
		dcache_disable();
	(*appl) (cmdline);
}
