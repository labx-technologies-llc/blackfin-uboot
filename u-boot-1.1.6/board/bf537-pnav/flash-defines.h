/*
 * U-boot - flash-defines.h
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

#ifndef __FLASHDEFINES_H__
#define __FLASHDEFINES_H__

static int poll_toggle_bit(long lOffset);
static int get_codes(void);
static void reset_flash(void);
static int erase_flash(void);
static int erase_block_flash(int);
static void unlock_flash(long lOffset);
static int write_data(long lStart, long lCount, uchar *pnData);
static int read_flash(long nOffset, int *pnValue);
static int write_flash(long nOffset, int nValue);
static void get_sector_number(long lOffset, int *pnSector);

#define NUM_SECTORS		71
#define RESET_VAL		0xF0

#define WRITESEQ1		0x0AAA
#define WRITESEQ2		0x0554
#define WRITESEQ3		0x0AAA
#define WRITESEQ4		0x0AAA
#define WRITESEQ5		0x0554
#define WRITESEQ6		0x0AAA
#define WRITEDATA1		0xaa
#define WRITEDATA2		0x55
#define WRITEDATA3		0x80
#define WRITEDATA4		0xaa
#define WRITEDATA5		0x55
#define WRITEDATA6		0x10
#define PriFlashABegin		0
#define SecFlashABegin		8
#define SecFlashAOff		0x10000
#define SecFlashBOff		0x280000
#define BlockEraseVal		0x30
#define UNLOCKDATA1		0xaa
#define UNLOCKDATA2		0x55
#define UNLOCKDATA3		0xa0
#define GETCODEDATA1		0xaa
#define GETCODEDATA2		0x55
#define GETCODEDATA3		0x90
#define SecFlashASec1Off	0x200000
#define SecFlashASec2Off	0x204000
#define SecFlashASec3Off	0x206000
#define SecFlashASec4Off	0x208000
#define SecFlashBSec1Off	0x280000
#define SecFlashBSec2Off	0x284000
#define SecFlashBSec3Off	0x286000
#define SecFlashBSec4Off	0x288000

#endif
