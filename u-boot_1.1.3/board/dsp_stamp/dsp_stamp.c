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
#include "dsp_stamp.h"

int checkboard(void)
{
	printf("CPU:   ADSP BF533-SBBC600 0.2\n");
	printf("Board: CSP BF533 Stamp board\n");
	printf("       cleanup by Richard Klingler <richard@uclinux.net>\n");
	return 0;
}

long int initdram(int board_type)
{
	DECLARE_GLOBAL_DATA_PTR;
	gd->bd->bi_memstart = CFG_SDRAM_BASE;
	gd->bd->bi_memsize = CFG_MAX_RAM_SIZE;
	return (gd->bd->bi_memsize);
}

