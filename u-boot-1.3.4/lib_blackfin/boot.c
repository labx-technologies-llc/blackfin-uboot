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
#include <asm/blackfin.h>

extern int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

#ifdef SHARED_RESOURCES
extern void swap_to(int device_id);
#endif

static char *make_command_line(void)
{
	char *dest = (char *)CFG_LINUX_CMDLINE_ADDR;
	char *bootargs = getenv("bootargs");

	if (bootargs == NULL)
		return NULL;

	strncpy(dest, bootargs, CFG_LINUX_CMDLINE_SIZE);
	dest[CFG_LINUX_CMDLINE_SIZE - 1] = 0;
	return dest;
}

extern ulong bfin_poweron_retx;

void do_bootm_linux(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[],
		    bootm_headers_t *images)
{
	int	(*appl) (char *cmdline);
	char	*cmdline;
	ulong	ep = 0;

#ifdef SHARED_RESOURCES
	swap_to(FLASH);
#endif

	/* find kernel entry point */
	if (images->legacy_hdr_valid) {
		ep = image_get_ep (&images->legacy_hdr_os_copy);
#if defined(CONFIG_FIT)
	} else if (images->fit_uname_os) {
		int ret = fit_image_get_entry (images->fit_hdr_os,
				images->fit_noffset_os, &ep);
		if (ret) {
			puts ("Can't get entry point property!\n");
			goto error;
		}
#endif
	} else {
		puts ("Could not find kernel entry point!\n");
		goto error;
	}
	appl = (int (*)(char *))ep;

	printf("Starting Kernel at = %p\n", appl);
	cmdline = make_command_line();
	icache_disable();
	dcache_disable();
	asm __volatile__(
		"RETX = %[retx];"
		"CALL (%0);"
		:
		: "p"(appl), "q0"(cmdline), [retx] "d"(bfin_poweron_retx)
	);
	/* does not return */
	return;

 error:
	do_reset (cmdtp, flag, argc, argv);
}

unsigned long do_go_exec(ulong (*entry)(int, char *[]), int argc, char *argv[])
{
	int d = dcache_status();
	int i = icache_status();

	dcache_disable();
	icache_disable();

	int ret = entry(argc, argv);

	if (i) icache_enable();
	if (d) dcache_enable();

	return ret;
}
unsigned long do_bootelf_exec(ulong (*entry)(int, char *[]), int argc, char *argv[])
{
	return do_go_exec(entry, argc, argv);
}
