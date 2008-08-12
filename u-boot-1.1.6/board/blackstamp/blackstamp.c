/*
 * U-boot - blackstamp.c BlackStamp board specific routines
 * Most code stolen from boards/bf533-stamp/bf533-stamp.c
 * Edited to the BlackStamp by Ben Matthews for UR LLE
 *
 * Copyright (c) 2005-2008 Analog Devices Inc.
 *
 * (C) Copyright 2000-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * Licensed under the GPL-2 or later.
 */

#include <common.h>
#include <asm/io.h>

DECLARE_GLOBAL_DATA_PTR;

int checkboard(void)
{
	printf("Board: BlackStamp\n");
	printf("Support: http://blackfin.uclinux.org/gf/project/blackstamp/\n");
	return 0;
}

long int initdram(int board_type)
{
	gd->bd->bi_memstart = CFG_SDRAM_BASE;
	gd->bd->bi_memsize = CFG_MAX_RAM_SIZE;
	return gd->bd->bi_memsize;
}

#ifdef SHARED_RESOURCES
void swap_to(int device_id)
{
	bfin_write_FIO_DIR(bfin_read_FIO_DIR() | PF0);
	SSYNC();
	if (device_id == ETHERNET)
		bfin_write_FIO_FLAG_S(PF0);
	else if (device_id == FLASH)
		bfin_write_FIO_FLAG_C(PF0);
	else
		printf("Unknown device to switch\n");
	SSYNC();
}
#endif

#if defined(CONFIG_MISC_INIT_R)
/* miscellaneous platform dependent initialisations */
int misc_init_r(void)
{
}
#endif