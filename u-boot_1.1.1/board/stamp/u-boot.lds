/*
 * (C) Copyright 2000
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

OUTPUT_FORMAT("elf32-little-nisa")
OUTPUT_ARCH(nisa)
ENTRY(start)

MEMORY 
	{
	ram		: ORIGIN = 0x7000000,  	LENGTH = 0x1000000
	flash		: ORIGIN = 0x20000000, 	LENGTH = 0x80000
	}

SECTIONS
{
	. = 0x20000000;

	. = ALIGN(4);
	.text      :
	{
	  cpu/bf533/start.o	(.text)
	  flashdataend = .;
	} > flash

	. = 0x7000000;
	. = ALIGN(4);
	.text1      :
	{
	  *(.text)
	} > ram

	. = ALIGN(4);
	.rodata : { *(.rodata) } > ram

	. = ALIGN(4);
	.data : { *(.data)
	__u_boot_cmd_start = .;
	*(.u_boot_cmd)
	__u_boot_cmd_end = .;
	 } > ram

	. = ALIGN(4);
	.got : { *(.got) } > ram


	. = ALIGN(4);
	__bss_start = .;
	.bss : { *(.bss) } > ram
	_end = .;

	. = 0x7000000;
	_bf533_data_dest = ABSOLUTE(.);
	_bf533_data_start = LOADADDR(.data);
	
	. = ALIGN(4);
	_bf533_data_size = SIZEOF(.text1) + SIZEOF(.data);

	. = 0x8000000;
	  .eram :
	{
	__ramend = . ;
	} > ram
}
