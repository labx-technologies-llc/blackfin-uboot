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

MEMORY
{
	ram     : ORIGIN = (CFG_MONITOR_BASE), LENGTH = (256 * 1024)
	l1_code : ORIGIN = 0xFFA00000, LENGTH = 0xC000
	l1_data : ORIGIN = 0xFF900000, LENGTH = 0x4000
}

SECTIONS
{
	. = CFG_MONITOR_BASE;
	.text :
	{
		*(.text)
	} > ram
	PROVIDE (_etext = .);

	.rodata :
	{
		. = ALIGN(4);
		*(EXCLUDE_FILE (board/bf548-ezkit/post-memory.o) .rodata)
		*(EXCLUDE_FILE (board/bf548-ezkit/post-memory.o) .rodata1)
		*(EXCLUDE_FILE (board/bf548-ezkit/post-memory.o) .rodata.str1.4)
		*(.eh_frame)
		. = ALIGN(4);
	} > ram

	. = ALIGN(256);
	.data :
	{
		*(.data)
		*(.data1)
		*(.sdata)
		*(.sdata2)
		*(.dynamic)
		CONSTRUCTORS
	} > ram
	PROVIDE (_edata = .);

	___u_boot_cmd_start = .;
	.u_boot_cmd : { *(.u_boot_cmd) } > ram
	___u_boot_cmd_end = .;

	__start___ex_table = .;
	__ex_table : { *(__ex_table) } > ram
	__stop___ex_table = .;

	. = ALIGN(256);
	__init_begin = .;
	.text.init : { *(.text.init) } > ram
	.data.init : { *(.data.init) } > ram
	. = ALIGN(256);
	__init_end = .;

	.bss :
	{
		__bss_start = .;
		*(.sbss) *(.scommon)
		*(.dynbss)
		*(.bss)
		*(COMMON)
	} > ram
	PROVIDE (_end = .);
}

