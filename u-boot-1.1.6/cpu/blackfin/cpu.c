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
#include <command.h>
#include <asm/blackfin.h>
#include <asm/cplb.h>
#include <asm/mach-common/bits/core.h>
#include <asm/mach-common/bits/mpu.h>

#include "cpu.h"
#include "serial.h"

void icache_enable(void)
{
	bfin_write_IMEM_CONTROL(bfin_read_IMEM_CONTROL() | (IMC | ENICPLB));
	SSYNC();
}

void icache_disable(void)
{
	bfin_write_IMEM_CONTROL(bfin_read_IMEM_CONTROL() & ~(IMC | ENICPLB));
	SSYNC();
}

int icache_status(void)
{
	return bfin_read_IMEM_CONTROL() & ENICPLB;
}

void dcache_enable(void)
{
	bfin_write_DMEM_CONTROL(bfin_read_DMEM_CONTROL() | (ACACHE_BCACHE | ENDCPLB | PORT_PREF0));
	SSYNC();
}

void dcache_disable(void)
{
	bfin_write_DMEM_CONTROL(bfin_read_DMEM_CONTROL() & ~(ACACHE_BCACHE | ENDCPLB | PORT_PREF0));
	SSYNC();
}

int dcache_status(void)
{
	return bfin_read_DMEM_CONTROL() & ENDCPLB;
}

#ifdef CONFIG_HW_WATCHDOG
void hw_watchdog_reset(void)
{
	bfin_write_WDOG_STAT(0);
}

void hw_watchdog_init(void)
{
	bfin_write_WDOG_CNT(5 * get_sclk());	/* 5 second timeout */
	hw_watchdog_reset();
	bfin_write_WDOG_CTL(0x0);
}
#endif

__attribute__ ((__noreturn__))
void cpu_init_f(ulong bootflag, ulong loaded_from_ldr)
{
	/* Build a NOP slide over the LDR jump block.  Whee! */
	serial_early_puts("NOP Slide\n");
	char nops[0xC];
	memset(nops, 0x00, sizeof(nops));
	extern char _stext_l1;
	memcpy(&_stext_l1 - sizeof(nops), nops, sizeof(nops));

	if (!loaded_from_ldr) {
		/* Relocate sections into L1 if the LDR didn't do it -- don't
		 * check length because the linker script does the size
		 * checking at build time.
		 */
		serial_early_puts("L1 Relocate\n");
		extern char _stext_l1, _etext_l1, _stext_l1_lma;
		memcpy(&_stext_l1, &_stext_l1_lma, (&_etext_l1 - &_stext_l1));
		extern char _sdata_l1, _edata_l1, _sdata_l1_lma;
		memcpy(&_sdata_l1, &_sdata_l1_lma, (&_edata_l1 - &_sdata_l1));
	}

#ifndef CONFIG_PANIC_HANG
	/* Reset upon a double exception rather than just hanging.
	 * Do not do bfin_read on SWRST as that will reset status bits.
	 */
	bfin_write_SWRST(DOUBLE_FAULT);
#endif

	serial_early_puts("Board init flash\n");
	board_init_f(bootflag);
}
