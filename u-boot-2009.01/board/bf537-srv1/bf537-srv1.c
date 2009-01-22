/*
 * U-boot - main board file
 *
 * Copyright (c) 2005-2008 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <common.h>
#include <config.h>
#include <command.h>
#include <asm/blackfin.h>
#include <asm/io.h>

DECLARE_GLOBAL_DATA_PTR;

int checkboard(void)
{
	printf("Board: Surveyor SRV1 board\n");
	printf("       Support: http://www.surveyor.com/\n");
	return 0;
}

phys_size_t initdram(int board_type)
{
	gd->bd->bi_memstart = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_memsize = CONFIG_SYS_MAX_RAM_SIZE;
	return gd->bd->bi_memsize;
}
