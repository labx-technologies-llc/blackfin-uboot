/*
 * U-boot - flash-defines.h
 *
 * Copyright (c) 2005 blackfin.uclinux.org
 *
 * (C) Copyright 2000-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * based on EZ-KIT flash driver
 * Edited by Thomas Tamandl, thomas.tamandl@bluetechnix.at
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

#define FLASH_TIMEOUT 0xffffff
#define FLASH_TIMEOUT 0xffffff
#define FLASH_SECTOR_SIZE 0x20000
#define MT_MANUFACT_CM_BF533 0x00890016	/* MT manuf. ID in D23..D16, D7..D0 */

int get_codes(void);
int erase_block_flash(int, unsigned long);
int write_data(long lStart, long lCount, long lStride, int *pnData);
int write_flash(long nOffset, unsigned short nValue);
int check_sector(unsigned short usSector);

flash_info_t flash_info[CFG_MAX_FLASH_BANKS];

#endif
