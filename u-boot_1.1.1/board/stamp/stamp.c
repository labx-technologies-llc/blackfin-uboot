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
#include <asm/mem_init.h>
#include "stamp.h"

int checkboard(void)
{
	return 0;
}

long int initdram(int board_type)
{
	DECLARE_GLOBAL_DATA_PTR;
	printf("SDRAM attributes:\n");
	printf("  tRCD:%d Cycles; tRP:%d Cycles; tRAS:%d Cycles; tWR:%d Cycles; "
		"CAS Latency:%d cycles\n", (SDRAM_tRCD >> 15), (SDRAM_tRP >> 11),
		(SDRAM_tRAS >> 6), ( SDRAM_tWR >> 19), (SDRAM_CL >> 2 ) );
	printf("SDRAM Begin: 0x%x\n", CFG_SDRAM_BASE);
	printf("Bank size = %d MB\n", 128);
	gd->bd->bi_memstart = CFG_SDRAM_BASE;
	gd->bd->bi_memsize = CFG_MAX_RAM_SIZE;
	return 0;
}

void swap_to(int device_id)
{

	if (device_id == ETHERNET) {
		*pFIO_DIR = PF0;
		asm("ssync;");
		*pFIO_FLAG_S = PF0;
		asm("ssync;");
	}
	else if (device_id == FLASH) {
        	*pFIO_DIR = (PF4 | PF3 | PF2 | PF1 | PF0);
        	*pFIO_FLAG_S = (PF4 | PF3 | PF2 );
        	*pFIO_MASKA_D = (PF8 | PF6 | PF5);
        	*pFIO_MASKB_D = (PF7);
        	*pFIO_POLAR = (PF8 | PF6 | PF5 );
        	*pFIO_EDGE = (PF8 | PF7 | PF6 | PF5);
        	*pFIO_INEN = (PF8 | PF7 | PF6 | PF5);
        	*pFIO_FLAG_D = (PF4 | PF3 | PF2 );
        	asm("ssync;");
	}
	else {
		printf("Unknown bank to switch\n");
	}	
	
	return;
}

#if defined(CONFIG_MISC_INIT_R)
/* miscellaneous platform dependent initialisations */
int misc_init_r(void)
{
	int i;
	int cf_stat = 0;

	/* Check whether CF card is inserted */
	*pFIO_EDGE = FIO_EDGE_CF_BITS;
	*pFIO_POLAR = FIO_POLAR_CF_BITS;
	for (i=0; i < 0x300 ; i++) asm("nop;");
			
	if ( (*pFIO_FLAG_S) & CF_STAT_BITS) {
		cf_stat = 0;
	}
	else {
		cf_stat = 1;
	}

	*pFIO_EDGE  = FIO_EDGE_BITS;
	*pFIO_POLAR = FIO_POLAR_BITS;
	

	if (cf_stat) {
                printf ("Booting from COMPACT flash\n");

		/* Set cycle time for CF */
		*(volatile unsigned long *) ambctl1 = CF_AMBCTL1VAL;

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
        *pFIO_FLAG_S = CF_PF0;
        *pFIO_FLAG_C = CF_PF1;
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

	*pFIO_FLAG_S = CF_PF0;
	*pFIO_FLAG_C = CF_PF1;
	asm("ssync;");

	c = *(addr);
	asm("ssync;");

	*pFIO_FLAG_C = CF_PF1_PF0;
	asm("ssync;");

	return c;
}

void cf_insw(unsigned short *sect_buf, unsigned short *addr, int words)
{
        int i;

        *pFIO_FLAG_S = CF_PF0;
        *pFIO_FLAG_C = CF_PF1;
        asm("ssync;");

        for (i = 0;i < words; i++) {
                *(sect_buf + i) = *(addr);
                asm("ssync;");
        }

        *pFIO_FLAG_C = CF_PF1_PF0;
        asm("ssync;");
}

void cf_outsw(unsigned short *addr, unsigned short *sect_buf, int words)
{
        int i;

        *pFIO_FLAG_S = CF_PF0;
        *pFIO_FLAG_C = CF_PF1;
        asm("ssync;");

        for (i = 0;i < words; i++) {
                *(addr) = *(sect_buf + i);
                asm("ssync;");
        }

        *pFIO_FLAG_C = CF_PF1_PF0;
        asm("ssync;");
}
#endif
