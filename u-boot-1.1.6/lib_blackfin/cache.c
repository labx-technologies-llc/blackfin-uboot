/*
 * U-boot - cache.c
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
#include <asm/mach-common/bits/mpu.h>

void flush_cache(unsigned long addr, unsigned long size)
{
	/* no need to flush stuff in on chip memory (L1/L2/etc...) */
	if (addr >= 0xE0000000)
		return;

	if (icache_status())
		blackfin_icache_flush_range((void *)addr, (void *)(addr + size));

	if (dcache_status())
		blackfin_dcache_flush_range((void *)addr, (void *)(addr + size));
}

#ifdef CONFIG_DCACHE_WB
static void flushinv_all_dcache(void)
{
	u32 way, bank, subbank, set;
	u32 status, addr;
	u32 dmem_ctl = bfin_read_DMEM_CONTROL();

	for (bank = 0; bank < 2; ++bank) {
		if (!(dmem_ctl & (1 << (DMC1_P - bank))))
			continue;

		for (way = 0; way < 2; ++way)
			for (subbank = 0; subbank < 4; ++subbank)
				for (set = 0; set < 64; ++set) {

					bfin_write_DTEST_COMMAND(
						way << 26 |
						bank << 23 |
						subbank << 16 |
						set << 5
					);
					CSYNC();
					status = bfin_read_DTEST_DATA0();

					/* only worry about valid/dirty entries */
					if ((status & 0x3) != 0x3)
						continue;

					/* construct the address using the tag */
					addr = (status & 0xFFFFC800) | (subbank << 12) | (set << 5);

					/* flush it */
					__asm__ __volatile__("FLUSHINV[%0];" : : "a"(addr));
				}
	}
}
#endif

void icache_enable(void)
{
	bfin_write_IMEM_CONTROL(IMC | ENICPLB);
	SSYNC();
}

void icache_disable(void)
{
	bfin_write_IMEM_CONTROL(0);
	SSYNC();
}

int icache_status(void)
{
	return bfin_read_IMEM_CONTROL() & ENICPLB;
}

void dcache_enable(void)
{
	bfin_write_DMEM_CONTROL(ACACHE_BCACHE | ENDCPLB | PORT_PREF0);
	SSYNC();
}

void dcache_disable(void)
{
#ifdef CONFIG_DCACHE_WB
	bfin_write_DMEM_CONTROL(bfin_read_DMEM_CONTROL() & ~(ENDCPLB));
	flushinv_all_dcache();
#endif
	bfin_write_DMEM_CONTROL(0);
	SSYNC();
}

int dcache_status(void)
{
	return bfin_read_DMEM_CONTROL() & ENDCPLB;
}
