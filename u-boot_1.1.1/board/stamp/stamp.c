/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
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
 *
 *	PROJECT				:	BFIN
 *	VERSION				:	2.0
 *	FILE				:	stamp.c
 *	MODIFIED DATE			:	29 jun 2004
 *	AUTHOR				:	BFin Project-ADI
 *	LOCATION			:	LG Soft India,Bangalore
 */

#include <common.h>
#include "stamp.h"

int checkboard(void)
{
	return 0;
}

long int initdram(int board_type)
{
	DECLARE_GLOBAL_DATA_PTR;
	printf("SDRAM attributes:\n");
	printf("tRCD %d SCLK Cycles,tRP %d SCLK Cycles,tRAS %d SCLK Cycles,tWR %d "
	       "SCLK Cycles,CAS Latency %d SCLK cycles \n", 2, 2, 5, 2, 2);
	printf("SDRAM Begin: 0x%x\n", CFG_SDRAM_BASE);
	printf("Bank size = %d MB\n", 128);
	gd->bd->bi_memstart = CFG_SDRAM_BASE;
	gd->bd->bi_memsize = CFG_MAX_RAM_SIZE;
	return 0;
}

#if defined(CONFIG_MISC_INIT_R)
/* miscellaneous platform dependent initialisations */
int misc_init_r(void)
{
	int i;
	int cf_stat = 0;

	/* Check whether CF card is inserted */
	*(volatile unsigned short *) pFIO_EDGE = FIO_EDGE_CF_BITS;
	*(volatile unsigned short *) pFIO_POLAR = FIO_POLAR_CF_BITS;
	for (i=0; i < 0x300 ; i++) asm("nop;");
			
	if ( (*(volatile unsigned short *) pFIO_FLAG_S) & CF_STAT_BITS) {
		cf_stat = 0;
	}
	else {
		cf_stat = 1;
	}

	*(volatile unsigned short *) pFIO_EDGE  = FIO_EDGE_BITS;
	*(volatile unsigned short *) pFIO_POLAR = FIO_POLAR_BITS;
	

	if (cf_stat) {
                printf ("Booting from COMPACT flash\n");

		/* Set cycle time for CF */
		*(volatile unsigned long *) ambctl1 = CF_AMBCTL1VAL;


		pll_set(CF_CONFIG_VCO, CF_CONFIG_CRYSTAL_FREQ, CF_PLL_DIV_FACTOR);

		pll_div_fact = CF_PLL_DIV_FACTOR;

		for (i=0; i < 0x1000 ; i++) asm("nop;");
		for (i=0; i < 0x1000 ; i++) asm("nop;");
		for (i=0; i < 0x1000 ; i++) asm("nop;");

		serial_setbrg();
		ide_init();

                setenv ("bootargs", "");
                setenv ("bootcmd", "fatload ide 0:1 0x1000000 uImage-stamp;bootm 0x1000000;bootm 0x20100000" );
        } else {
                printf ("Booting from FLASH\n");
        }

	return 1;
}
#endif

#ifdef CONFIG_STAMP_CF

void cf_outb(unsigned char val, volatile unsigned char* addr)
{
	/* 
	 * Set PF1 PF0 respectively to 0 1 to divert address
	 * to the expansion memory banks  
	 */
        *(volatile unsigned short *) pFIO_FLAG_S = CF_PF0;
        *(volatile unsigned short *) pFIO_FLAG_C = CF_PF1;
        asm("ssync;");

        *(addr) = val;
        asm("ssync;");

	/* Setback PF1 PF0 to 0 0 to address external 
	 * memory banks  */
        *(volatile unsigned short *) pFIO_FLAG_C = CF_PF1_PF0;
        asm("ssync;");
}

unsigned char cf_inb(volatile unsigned char *addr)
{
	volatile unsigned char c;

	*(volatile unsigned short *) pFIO_FLAG_S = CF_PF0;
	*(volatile unsigned short *) pFIO_FLAG_C = CF_PF1;
	asm("ssync;");

	c = *(addr);
	asm("ssync;");

	*(volatile unsigned short *) pFIO_FLAG_C = CF_PF1_PF0;
	asm("ssync;");

	return c;
}

void cf_insw(unsigned short *sect_buf, unsigned short *addr, int words)
{
        int i;

        *(volatile unsigned short *) pFIO_FLAG_S = CF_PF0;
        *(volatile unsigned short *) pFIO_FLAG_C = CF_PF1;
        asm("ssync;");

        for (i = 0;i < words; i++) {
                *(sect_buf + i) = *(addr);
                asm("ssync;");
        }

        *(volatile unsigned short *) pFIO_FLAG_C = CF_PF1_PF0;
        asm("ssync;");
}

void cf_outsw(unsigned short *addr, unsigned short *sect_buf, int words)
{
        int i;

        *(volatile unsigned short *) pFIO_FLAG_S = CF_PF0;
        *(volatile unsigned short *) pFIO_FLAG_C = CF_PF1;
        asm("ssync;");

        for (i = 0;i < words; i++) {
                *(addr) = *(sect_buf + i);
                asm("ssync;");
        }

        *(volatile unsigned short *) pFIO_FLAG_C = CF_PF1_PF0;
        asm("ssync;");
}
#endif
