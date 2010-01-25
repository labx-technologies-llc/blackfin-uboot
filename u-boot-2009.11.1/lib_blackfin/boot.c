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

#ifdef SHARED_RESOURCES
extern void swap_to(int device_id);
#endif

#ifdef CONFIG_VIDEO
extern void video_stop(void);
#endif

static char *make_command_line(void)
{
	char *dest = (char *)CONFIG_LINUX_CMDLINE_ADDR;
	char *bootargs = getenv("bootargs");

	if (bootargs == NULL)
		return NULL;

	strncpy(dest, bootargs, CONFIG_LINUX_CMDLINE_SIZE);
	dest[CONFIG_LINUX_CMDLINE_SIZE - 1] = 0;
	return dest;
}

extern ulong bfin_poweron_retx;

int do_bootm_linux(int flag, int argc, char *argv[], bootm_headers_t *images)
{
	int	(*appl) (char *cmdline);
	char	*cmdline;

	if ((flag != 0) && (flag != BOOTM_STATE_OS_GO))
		return 1;

#ifdef SHARED_RESOURCES
	swap_to(FLASH);
#endif

#ifdef CONFIG_VIDEO
	video_stop();
#endif

	appl = (int (*)(char *))images->ep;

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

	return 1;
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
