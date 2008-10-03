/*
 * U-boot - cm-bf548.c
 *
 * Copyright (c) 2005 blackfin.uclinux.org
 *
 * (C) Copyright 2000-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
#include <config.h>
#include <command.h>
#include <asm/blackfin.h>

DECLARE_GLOBAL_DATA_PTR;

int checkboard(void)
{
	printf("Board: Bluetechnix CM-BF548 board\n");
	printf("       Support: http://blackfin.uclinux.org/\n");
	return 0;
}

long int initdram(int board_type)
{
	gd->bd->bi_memstart = CFG_SDRAM_BASE;
	gd->bd->bi_memsize = CFG_MAX_RAM_SIZE;
	return CFG_MAX_RAM_SIZE;
}

int board_early_init_f(void)
{
	/* Port H: PH8 - PH13 == A4 - A9
	 * address lines of the parallel asynchronous memory interface
	 */

	/************************************************
	* configure GPIO 				*
	* set port H function enable register		*
	*  configure PH8-PH13 as peripheral (not GPIO) 	*
	*************************************************/
	bfin_write_PORTH_FER(0x3F03);

	/************************************************
	* set port H MUX to configure PH8-PH13		*
	*  1st Function (MUX = 00) (bits 16-27 == 0)	*
	*  Set to address signals A4-A9 		*
	*************************************************/
	bfin_write_PORTH_MUX(0);

	/************************************************
	* set port H direction register			*
	*  enable PH8-PH13 as outputs			*
	*************************************************/
	bfin_write_PORTH_DIR_SET(0x3F00);

	/* Port I: PI0 - PH14 == A10 - A24
	 * address lines of the parallel asynchronous memory interface
	 */

	/************************************************
	* set port I function enable register		*
	*  configure PI0-PI14 as peripheral (not GPIO) 	*
	*************************************************/
	bfin_write_PORTI_FER(0x7fff);

	/**************************************************
	* set PORT I MUX to configure PI14-PI0 as	  *
	* 1st Function (MUX=00) - address signals A10-A24 *
	***************************************************/
	bfin_write_PORTI_MUX(0);

	/****************************************
	* set PORT I direction register		*
	*  enable PI0 - PI14 as outputs		*
	*****************************************/
	bfin_write_PORTI_DIR_SET(0x7fff);

	return 0;
}
