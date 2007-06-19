/*
 * U-boot - u-boot.lds.S
 *
 * Copyright (c) 2005-2007 Analog Device Inc.
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

#include <config.h>

OUTPUT_ARCH(bfin)

SECTIONS
{
	. = CFG_MONITOR_BASE;
	.text :
	{
		/* WARNING - the following is hand-optimized to fit within
		 * the sector before the environment sector. If it throws
		 * an error during compilation remove an object here to get
		 * it linked after the configuration sector.
		 */

		cpu/bf561/start.o		(.text)
		cpu/bf561/start1.o		(.text)
		cpu/bf561/traps.o		(.text)
		cpu/bf561/interrupt.o	(.text)
		cpu/bf561/serial.o		(.text)
		common/dlmalloc.o		(.text)
		lib_generic/crc32.o		(.text)
		lib_generic/zlib.o		(.text)
		board/bf561-ezkit/bf561-ezkit.o		(.text)

		dxe_init_start = .;
		cpu/bf561/init_sdram.o	(.text)
		dxe_init_end = .;

		. = DEFINED(env_offset) ? env_offset : .;
		common/environment.o	(.text)

		*(.text)
	}
	PROVIDE (_etext = .);
	.rodata :
	{
		*(.rodata)
		*(.rodata1)
		*(.rodata.str1.4)
	}

	. = ALIGN(256);
	.data :
	{
		*(.data)
		*(.data1)
		*(.sdata)
		*(.sdata2)
		*(.dynamic)
		CONSTRUCTORS
	}
	PROVIDE (_edata = .);

	___u_boot_cmd_start = .;
	.u_boot_cmd : { *(.u_boot_cmd) }
	___u_boot_cmd_end = .;

	__start___ex_table = .;
	__ex_table : { *(__ex_table) }
	__stop___ex_table = .;

	. = ALIGN(256);
	__init_begin = .;
	.text.init : { *(.text.init) }
	.data.init : { *(.data.init) }
	. = ALIGN(256);
	__init_end = .;

	__bss_start = .;
	.bss :
	{
		*(.sbss) *(.scommon)
		*(.dynbss)
		*(.bss)
		*(COMMON)
	}
	PROVIDE (_end = .);
}
