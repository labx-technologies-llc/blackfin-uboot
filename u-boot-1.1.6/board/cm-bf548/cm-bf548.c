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

#define POST_WORD_ADDR 0xFF903FFC

int checkboard(void)
{
	printf("Board: Bluetechnix CM-BF548 board\n");
	printf("       Support: http://blackfin.uclinux.org/\n");
	return 0;
}

long int initdram(int board_type)
{
	DECLARE_GLOBAL_DATA_PTR;
#ifdef DEBUG
	int brate;
	char *tmp = getenv("baudrate");
	brate = simple_strtoul(tmp, NULL, 16);
	printf("Serial Port initialized with Baud rate = %x\n", brate);
	printf("SDRAM attributes:\n");
	printf("tRCD %d SCLK Cycles,tRP %d SCLK Cycles,tRAS %d SCLK Cycles"
	       "tWR %d SCLK Cycles,CAS Latency %d SCLK cycles \n",
	       3, 3, 6, 2, 3);
	printf("SDRAM Begin: 0x%x\n", CFG_SDRAM_BASE);
	printf("Bank size = %d MB\n", CFG_MAX_RAM_SIZE >> 20);
#endif
	gd->bd->bi_memstart = CFG_SDRAM_BASE;
	gd->bd->bi_memsize = CFG_MAX_RAM_SIZE;
	return CFG_MAX_RAM_SIZE;
}

#if defined(CONFIG_MISC_INIT_R)
/* miscellaneous platform dependent initialisations */
#if (CONFIG_BFIN_BOOT_MODE == BFIN_BOOT_BYPASS)
int misc_init_r(void)
{
#if defined(CONFIG_BFIN_IDE)
#if defined(CONFIG_BFIN_CF_IDE)
	/*Disable ATASEL when we're in Common Memory Mode */
	cf_outb(0, CONFIG_CF_ATASEL_DIS);
	udelay(1000);
#endif
	ide_init();
#endif
	return 0;
}
#else
int misc_init_r(void)
{
	return 0;
}
#endif
#endif

#ifdef CONFIG_POST
#if (CONFIG_BFIN_BOOT_MODE == BFIN_BOOT_BYPASS)
/* Using sw10-PF5 as the hotkey */
int post_hotkeys_pressed(void)
{
	return 0;
}
#else
/* Using sw10-PF5 as the hotkey */
int post_hotkeys_pressed(void)
{
	int delay = 3;
	int i;
	unsigned short value;

	*pPORTF_FER &= ~PF5;
	*pPORTFIO_DIR &= ~PF5;
	*pPORTFIO_INEN |= PF5;

	printf("########Press SW10 to enter Memory POST########: %2d ", delay);
	while (delay--) {
		for (i = 0; i < 100; i++) {
			value = *pPORTFIO & PF5;
			if (value != 0)
				break;

			udelay(10000);
		}
		printf("\b\b\b%2d ", delay);
	}
	printf("\b\b\b 0");
	printf("\n");
	if (value == 0)
		return 0;
	else {
		printf("Hotkey has been pressed, Enter POST . . . . . .\n");
		return 1;
	}
}
#endif
#endif

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
