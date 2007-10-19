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
#include <asm/io.h>
#include <asm/mach-common/bits/core.h>
#include <asm/mach-common/bits/mpu.h>
#include "cpu.h"

#define CACHE_ON 1
#define CACHE_OFF 0

#if defined(__ADSPBF561__)
# define SYSCR_VAL 0x20	/* do not enable core b */
#else
# define SYSCR_VAL 0x0
#endif

/* A system soft reset makes external memory unusable
 * so force this function into L1.
 */
__attribute__ ((__l1_text__, __noreturn__))
void bfin_reset(void)
{
	/* force BMODE and disable Core B (as needed) */
	bfin_write_SYSCR(SYSCR_VAL);

	/* we use the asm(ssync) here rather than SSYNC() because
	 * it's safe (no interrupts and such) and we save some L1
	 */
	asm("ssync;");

	while (1) {
		/* initiate system soft reset with magic 0x7 */
		bfin_write_SWRST(0x7);
		asm("ssync;");
		/* clear system soft reset */
		bfin_write_SWRST(0);
		asm("ssync;");
		/* issue core reset */
		asm("raise 1");
	}
}

/* We need to trampoline ourselves up into L1 since our linker
 * does not have relaxtion support and will only generate a
 * PC relative call with a 25 bit immediate.  This is not enough
 * to get us from the top of SDRAM into L1.
 */
static inline void bfin_reset_trampoline(void)
{
	asm("jump (%0);" : : "qA" (bfin_reset));
}

__attribute__ ((__noreturn__))
void bfin_reset_or_hang(void)
{
	while (1)
#ifdef CONFIG_PANIC_HANG
		continue;
#else
		bfin_reset_trampoline();
#endif
}

int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	bfin_reset_trampoline();
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

__attribute__ ((__noreturn__))
void cpu_init_f(ulong bootflag, ulong loaded_from_ldr)
{
	/* Build a NOP slide over the LDR jump block.  Whee! */
	char nops[0xC];
	memset(nops, 0x00, sizeof(nops));
	extern char _stext_l1;
	memcpy(&_stext_l1 - sizeof(nops), nops, sizeof(nops));

	if (!loaded_from_ldr) {
		/* Relocate sections into L1 if the LDR didn't do it -- don't
		 * check length because the linker script does the size
		 * checking at build time.
		 */
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

	board_init_f(bootflag);
}
