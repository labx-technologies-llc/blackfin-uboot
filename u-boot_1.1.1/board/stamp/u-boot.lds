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

OUTPUT_FORMAT("elf32-bfin")
ENTRY(start)
MEMORY 
	{
<<<<<<< u-boot.lds
	ram		: ORIGIN = 0x7000024,  	LENGTH = 0x1000000
=======
	ram		: ORIGIN = 0x7000030,  	LENGTH = 0x1000000
>>>>>>> 1.2
	flash		: ORIGIN = 0x20000000, 	LENGTH = 0x80000
	sram		: ORIGIN = 0xFFA00000, 	LENGTH = 0xC000
	}

SECTIONS
{
	.text : {
		cpu/bf533/start.o (.text)
		flashdataend = ABSOLUTE(.);
	} > flash

        .text1 : {
                cpu/bf533/sram_func.o (.text)
        } > sram
	
        .text2 : {
		cpu/bf533/start1.o (.text)
                *(.text)
                *(.fixup)
                *(.got1)
                . = ALIGN(4);
                *(.rodata)
                *(.rodata1)
                *(.rodata.str1.4)
        } > ram


	.data :	{
		_sdata = . ;
		__data_start = . ;
		*(.rodata)
		*(.data)
		*(.data.*)
		. = ALIGN(16) ;
		ramdisk_begin = .;
		*(.ramdisk)
		ramdisk_end = .;

		*(.setup.init)
		*(.exitcall.exit)
		. = ALIGN(16) ;

		__u_boot_cmd_start = .;
		 *(.u_boot_cmd)
		__u_boot_cmd_end = .;
		. = ALIGN(16) ;

		_edata = . ;
		edata = . ;
	} > ram

	. = 0x7000000;	/* Ram data segment to use */
	_sram_in_sdram_start = ABSOLUTE(.);
	_bf533_data_dest = ABSOLUTE(.);
	_bf533_data_start = LOADADDR(.data);

	. = ALIGN(4);
	.got : AT ( LOADADDR(.data) + SIZEOF(.data) ) { *(.got) }
	_bf533_data_size = SIZEOF(.text1) + SIZEOF(.text2) + SIZEOF(.data) + SIZEOF(.got);
	_sram_inst_size = SIZEOF(.text1);

	.bss :	{
		. = ALIGN(16) ;
		_sbss = . ;
		__bss_start = . ;
		__data_end = . ;
		*(.bss)
		*(COMMON)
		. = ALIGN(16) ;
		end = . ;
		_ebss = . ;
		_end = . ;
	} > ram
 
	. = 0x7F00000;
	.eram :	{
		__ramend = . ;
	} > ram
}
