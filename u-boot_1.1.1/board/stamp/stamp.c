/*
 * (C) Copyright 2001
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

**********************************************************************************************************

                        PROJECT                 :       BFIN
                        VERISON                 :       2.0
                        FILE                    :       board.c
                        MODIFIED DATE           :       29 jun 2004
                        AUTHOR                  :       BFin Project-ADI
                        LOCATION                :       LG Soft India,Bangalore

***********************************************************************************************************/

#include <common.h>

int checkboard (void)
{
	return 0;
}

long int initdram (int board_type)
{
	DECLARE_GLOBAL_DATA_PTR;
	printf("SDRAM attributes:\n");
	printf("tRCD %d SCLK Cycles,tRP %d SCLK Cycles,tRAS %d SCLK Cycles,tWR %d SCLK Cycles,CAS Latency %d SCLK cycles \n",2,2,5,2,2);
	printf("SDRAM Begin: 0x%x\n",CFG_SDRAM_BASE);
	printf("Bank size = %d MB\n",128);

	gd->bd->bi_memstart = CFG_SDRAM_BASE;
	gd->bd->bi_memsize  = CFG_MAX_RAM_SIZE;
	return 0; 
}
