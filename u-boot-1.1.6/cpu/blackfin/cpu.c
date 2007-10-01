/*
 * U-boot - cpu.c CPU specific functions
 *
 * Copyright (c) 2005-2007 Analog Devices Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <common.h>
#include <asm/blackfin.h>
#include <command.h>
#include <asm/entry.h>
#include <asm/cplb.h>
#include <asm/io.h>
#include "cpu.h"

#define CACHE_ON 1
#define CACHE_OFF 0

void bfin_reset(void)
{
	*pWDOG_CNT = 0x0010;
	*pWDOG_CTL = 0x0000;
	while (1)
		SSYNC();
}
int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	bfin_reset();
	return 0;
}

void icache_enable(void)
{
	local_irq_disable();
	SSYNC();
	asm(" .align 8; ");
	*pIMEM_CONTROL |= (IMC | ENICPLB);
	SSYNC();
	local_irq_enable();
}

void icache_disable(void)
{
	local_irq_disable();
	SSYNC();
	asm(" .align 8; ");
	*pIMEM_CONTROL &= ~(IMC | ENICPLB);
	SSYNC();
	local_irq_enable();
}

int icache_status(void)
{
	if (*pIMEM_CONTROL & (IMC | ENICPLB))
		return CACHE_ON;
	else
		return CACHE_OFF;
}

void dcache_enable(void)
{
	local_irq_disable();
	SSYNC();
	asm(" .align 8; ");
	*pDMEM_CONTROL |= (ACACHE_BCACHE | ENDCPLB | PORT_PREF0);
	SSYNC();
	local_irq_enable();
}

void dcache_disable(void)
{
	local_irq_disable();
	SSYNC();
	asm(" .align 8; ");
	*pDMEM_CONTROL &= ~(ACACHE_BCACHE | ENDCPLB | PORT_PREF0);
	SSYNC();
	local_irq_enable();
}

int dcache_status(void)
{
	if (*pDMEM_CONTROL & (ENDCPLB))
		return CACHE_ON;
	else
		return CACHE_OFF;
}
