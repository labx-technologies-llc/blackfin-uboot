/*
 * (C) Copyright 2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2004
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <devices.h>
#include <version.h>
#include <net.h>
#include <environment.h>
#include "blackfin_board.h"

static void mem_malloc_init(void)
{
	ulong dest_addr = CFG_MALLOC_BASE;

	mem_malloc_end = dest_addr;
	mem_malloc_start = dest_addr - CFG_MALLOC_LEN;
	mem_malloc_brk = mem_malloc_start;
	memset((void *) mem_malloc_start, 0,
	       mem_malloc_end - mem_malloc_start);

}

void *sbrk(ptrdiff_t increment)
{
	ulong old = mem_malloc_brk;
	ulong new = old + increment;

	if ((new < mem_malloc_start) || (new > mem_malloc_end)) {
		return (NULL);
	}
	mem_malloc_brk = new;

	return ((void *) old);
}

static int display_banner(void)
{
	printf("%s\n", version_string);
	printf("%s\n", moreinfo_string);

	return (0);
}

static void display_flash_config(ulong size)
{
	puts("Flash Size ");
	print_size(size, "\n");
	return;
}


static int init_baudrate(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	uchar tmp[64];
	int i = getenv_r("baudrate", tmp, sizeof(tmp));
	gd->bd->bi_baudrate = gd->baudrate = (i > 0)
	    ? (int) simple_strtoul(tmp, NULL, 10)
	    : CONFIG_BAUDRATE;
	return (0);
}


/*
 * Breath some life into the board...
 *
 * The first part of initialization is running from Flash memory;
 * its main purpose is to initialize the RAM so that we
 * can relocate the monitor code to RAM.
 */

/*
 * All attempts to come up with a "common" initialization sequence
 * that works for all boards and architectures failed: some of the
 * requirements are just _too_ different. To get rid of the resulting
 * mess of board dependend #ifdef'ed code we now make the whole
 * initialization sequence configurable to the user.
 *
 * The requirements for any new initalization function is simple: it
 * receives a pointer to the "global data" structure as it's only
 * argument, and returns an integer return code, where 0 means
 * "continue" and != 0 means "fatal error, hang the system".
 */

void board_init_f(ulong bootflag)
{
	DECLARE_GLOBAL_DATA_PTR;
	gd_t gd_data;
	unsigned long cck;

	gd = &gd_data;
	memset((void *) gd, 0, sizeof(gd_t));

	/* Copy sram functions from sdram to sram */

	/* Setup destination start address */
	*(volatile unsigned long *)MDMA_D0_START_ADDR = L1_ISRAM;
	/* Setup destination xcount */
	*(volatile unsigned short *)MDMA_D0_X_COUNT = (unsigned short)&_sram_inst_size;
	/* Setup destination xmodify */
	*(volatile unsigned short *)MDMA_D0_X_MODIFY = 2;

	/* Setup Source start address */
	*(volatile unsigned long *)MDMA_S0_START_ADDR = (unsigned long)&_sram_in_sdram_start;
	/* Setup Source xcount */
	*(volatile unsigned short *)MDMA_S0_X_COUNT = (unsigned short)&_sram_inst_size;
	/* Setup Source xmodify */
	*(volatile unsigned short *)MDMA_S0_X_MODIFY = 2;

	/* Set word size to 16, set to read, enable interrupt for wakeup */
	/* Enable source DMA */
	*(volatile unsigned short *)MDMA_S0_CONFIG = (WDSIZE16 | DMAEN);
	asm("ssync;");
	/* Enable Destination DMA */
	*(volatile unsigned short *)MDMA_D0_CONFIG = (DI_EN | WDSIZE16 | WNR | DMAEN);
	
	/* Go into idle and wait for wakeup that indicates DMA is done */
	asm("idle;");
	/* Clear interrupt */
	*(volatile unsigned short *)MDMA_D0_IRQ_STATUS = 0x1;
  
	/* Initialize */
	init_IRQ();
	env_init();		/* initialize environment */
	cck = get_clock();
	get_sclk();
	init_baudrate();	/* initialze baudrate settings */
	serial_init();		/* serial communications setup */
	console_init_f();
	display_banner();	/* say that we are here */
	checkboard();
	rtc_init();
	initdram(0);
	timer_init();
	printf("Core Clock %d MHz\n",cck/1000000);
	printf("System Clock %d MHz\n",sclk/1000000);
	board_init_r((gd_t *) gd, 0x20000010);
}

void board_init_r(gd_t * id, ulong dest_addr)
{
	DECLARE_GLOBAL_DATA_PTR;
	cmd_tbl_t *cmdtp;
	ulong size;
	ulong addr;
	extern void malloc_bin_reloc(void);
#ifndef CFG_ENV_IS_NOWHERE
	extern char *env_name_spec;
#endif
	char *s, *e;
	bd_t *bd;
	int i;
	gd = id;
	gd->flags |= GD_FLG_RELOC;	/* tell others: relocation done */

	monitor_flash_len = (ulong) &uboot_end_data - dest_addr;
	/*
	 * We have to relocate the command table manually
	 */
	for (cmdtp = &__u_boot_cmd_start; cmdtp != &__u_boot_cmd_end;
	     cmdtp++) {
		addr = (ulong) (cmdtp->cmd) + gd->reloc_off;
		cmdtp->cmd =
			(int (*)(struct cmd_tbl_s *, int, int, char *[])) addr;
		addr = (ulong) (cmdtp->name) + gd->reloc_off;
		cmdtp->name = (char *) addr;
		if (cmdtp->usage) {
			addr = (ulong) (cmdtp->usage) + gd->reloc_off;
			cmdtp->usage = (char *) addr;
		}
#ifdef CFG_LONGHELP
		if (cmdtp->help) {
			addr = (ulong) (cmdtp->help) + gd->reloc_off;
			cmdtp->help = (char *) addr;
		}
#endif
	}
	/* There are some other pointer constants we must deal with */
#ifndef CFG_ENV_IS_NOWHERE
	env_name_spec += gd->reloc_off;
#endif
	/* configure available FLASH banks */
	size = flash_init();
	display_flash_config(size);
	bd = gd->bd;
	bd->bi_flashstart = CFG_FLASH_BASE;
	bd->bi_flashsize = size;
	bd->bi_flashoffset = 0;

	/* initialize malloc() area */
	mem_malloc_init();
	malloc_bin_reloc();

	/* relocate environment function pointers etc. */
	env_relocate();

	/* board MAC address */
	s = getenv("ethaddr");
	for (i = 0; i < 6; ++i) {
		bd->bi_enetaddr[i] = s ? simple_strtoul(s, &e, 16) : 0;
		if (s)
			s = (*e) ? e + 1 : e;
	}

	/* IP Address */
	bd->bi_ip_addr = getenv_IPaddr("ipaddr");

	/* Initialize devices */
	devices_init();
	jumptable_init();

	/* Initialize the console (after the relocation and devices init) */
	console_init_r();

	/* Initialize from environment */
	if ((s = getenv("loadaddr")) != NULL) {
		load_addr = simple_strtoul(s, NULL, 16);
	}
#if (CONFIG_COMMANDS & CFG_CMD_NET)
	if ((s = getenv("bootfile")) != NULL) {
		copy_filename(BootFile, s, sizeof(BootFile));
	}
#endif
#if defined(CONFIG_MISC_INIT_R)
	/* miscellaneous platform dependent initialisations */
	misc_init_r();
#endif

#ifdef CONFIG_DRIVER_SMC91111
	printf("EZ-LAN Network card probed\n");
	printf("EZ-LAN memory mapped to 0x20300000 - 0x203fffff\n");
#endif
	/* main_loop() can return to retry autoboot, if so just run it again. */
	for (;;) {
		main_loop();
	}
}


void hang(void)
{
	puts("### ERROR ### Please RESET the board ###\n");
	for (;;);
}
