/*
 * U-boot - flash-defines.h
 *
 * Copyright (c) 2005 blackfin.uclinux.org
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

#ifndef __FLASHDEFINES_H__
#define __FLASHDEFINES_H__

#include <common.h>

#define BUFFER_SIZE	0x100000
#define FLASH_TOT_SECT	67
#define FLASH_SIZE 	0x400000
#define FLASH_MAN_ST 	2
#define FLASH_DEV_ID	0x22CB
#define CFG_FLASH0_BASE	0x20000000

#define SECT1_ADDR	CFG_FLASH0_BASE
#define SECT2_ADDR	(CFG_FLASH0_BASE + 0x4000)
#define SECT3_ADDR	(SECT2_ADDR + 0x2000)
#define SECT4_ADDR	(SECT3_ADDR + 0x2000)

#define FLASH_SUCCESS	0
#define FLASH_FAIL	-1
#define ERASE_TIMEOUT	2

#define WRITE_CMD1	0x555
#define WRITE_CMD2	0x2AA
#define WRITE_CMD3	0x555
#define WRITE_DATA1	0xAA
#define WRITE_DATA2	0x55
#define WRITE_DATA3	0xA0

#define ANY_OFF		0x0
#define RESET_DATA	0xF0

#define ERASE_DATA1	0xAA
#define ERASE_DATA2	0x55
#define ERASE_DATA3	0x80
#define ERASE_DATA4	0xAA
#define ERASE_DATA5	0x55
#define ERASE_DATA6	0x10

#define GETCODE_DATA	0x90
#define FLASH_WIDTH	2

flash_info_t flash_info[CFG_MAX_FLASH_BANKS];
int GetCodes(void);
int ResetFlash(void);
int EraseFlash(void);
int write_buff (flash_info_t *info, uchar *src, ulong addr, ulong cnt);
int write_word(flash_info_t *info, ulong addr, ulong data);

unsigned long GetOffset(int sec_num);
int FLASH_Block_Erase(flash_info_t *info, unsigned long, unsigned long);
int FlashDataToggle(unsigned long);

int AFP_SectorSize1 = 0x10000;

volatile unsigned long *ambctl0 = (volatile unsigned long *) 0xffc00a04;
volatile unsigned long *ambctl1 = (volatile unsigned long *) 0xffc00a08;
volatile unsigned long *amgctl = (volatile unsigned long *) 0xffc00a00;

inline void LED6_On(void)
{
	*(volatile unsigned short *) pFIO_FLAG_C = 0x0004;
	asm("ssync;");
}
inline void LED6_Off(void)
{
	*(volatile unsigned short *) pFIO_FLAG_S = 0x0004;
	asm("ssync;");
}

volatile unsigned short *FLASH_Base = (unsigned short *) 0x20000000;
unsigned volatile long *FB = (unsigned long *) 0x20000002;

#endif
