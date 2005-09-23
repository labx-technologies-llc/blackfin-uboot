/*
 * U-boot - ezkit533.c
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
#include <asm/blackfin.h>

#define POST_WORD_ADDR 0xFF903FFC

int checkboard(void)
{
	printf("CPU:   ADSP BF537 Rev.: 0.%d\n", *pCHIPID >>28);
	printf("Board: ADI BF537 stamp board\n");
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
	printf("Serial Port initialized with Baud rate = %x\n",brate);
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
int misc_init_r(void)
{
	return 0;
}
#endif

#ifdef CONFIG_POST
/* Using sw10-PF5 as the hotkey */
int post_hotkeys_pressed(void)
{
	int delay = 3;
	int i;
	unsigned short value;
	
	*pPORTF_FER   &= ~PF5;
	*pPORTFIO_DIR &= ~PF5;
	*pPORTFIO_INEN|=  PF5;
	
	printf("########Press SW10 to enter POST########: %2d ",delay);	
	while(delay--){
		for(i=0;i<100;i++){
			value = *pPORTFIO & PF5;
			if(value != 0){
				break;
				}
			udelay(10000);		
			}
		printf("\b\b\b%2d ",delay);
	}
	printf("\b\b\b 0");
	printf("\n");
	if(value == 0)
		return 0;
	else
		{
		printf("Hotkey has been pressed, Enter POST . . . . . .\n");
		return 1;
		}
}
#endif

#if defined(CONFIG_POST) || defined(CONFIG_LOGBUFFER)
void post_word_store(ulong a)
{
	volatile ulong *save_addr =
		(volatile ulong *)POST_WORD_ADDR;
	*save_addr = a;
}

ulong post_word_load(void)
{
	volatile ulong *save_addr = 
		(volatile ulong *)POST_WORD_ADDR;
	return *save_addr;
}

int uart_post_test(int flags)
{
	return 0;
}

int ether_post_test(int flags)
{
	return 0;
}

int flash_post_test(int flags)
{
	return 0;
}

int blink_post_test(int flags)
{
	return 0;
}
#endif
