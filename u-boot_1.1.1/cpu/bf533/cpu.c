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
 *	FILE				:	cpu.c
 *	MODIFIED DATE			:	29 jun 2004
 *	AUTHOR				:	BFin Project-ADI
 *	LOCATION			:	LG Soft India,Bangalore
 */

#include <common.h>
#include <asm/blackfin.h>
#include <command.h>
#include <asm/entry.h>

unsigned long sclk;

int cpu_init(void)
{
	return 0;
}

int cleanup_before_linux(void)
{
	return 0;
}

int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	asm("raise 1;");
#if 0
	void (*f)(void) = (void *) 0x20000000;
	f();	
#endif
	return 0;
}

void icache_enable(void)
{

}

void icache_disable(void)
{

}

int icache_status(void)
{
	return 0;
}

void dcache_enable(void)
{

}

void dcache_disable(void)
{

}

int dcache_status(void)
{
	return 0;
}

void asyncbank_init(void)
{
#ifdef CONFIG_STAMP
	asm("p2.h = 0xFFC0;");
	asm("p2.l = 0x0730;");
	asm("r0 = 0x01;");
	asm("w[p2] = r0;");
	asm("ssync;");

	asm("p2.h = 0xffc0;");
	asm("p2.l = 0x0708;");
	asm("r0 = 0x01;");
	asm("w[p2] = r0;");
	asm("ssync;");
#endif
}

int GetClock(void)	{
	unsigned long val;
	volatile unsigned long *pllctl = 0xFFC00000;
	
	val = *(volatile unsigned short *)pllctl;
	val = (val >> 9) & 0x3F;
	val = val * CONFIG_CRYSTAL_FREQ;
	val = val * 1000000;
	
	return val;
}

int Getsclk(void)	{
	sclk = GetClock()/PLL_DIV_FACTOR;
}

