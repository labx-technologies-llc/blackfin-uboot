/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
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
 *
 * cpu/bf535/start.s:  A startup file for the Blackfin u-boot *
 * Copyright (c) 2003  Metrowerks/Motorola
 * Copyright (C) 1998  D. Jeff Dionne <jeff@ryeham.ee.ryerson.ca>,
 *                     Kenneth Albanowski <kjahds@kjahds.com>,
 *                     The Silver Hammer Group, Ltd.
 * (c) 1995, Dionne & Associates
 * (c) 1995, DKG Display Tech.
 */

#define ASSEMBLY
#include <linux/config.h>
#include <asm/blackfin.h>
#include <config.h>

.global pll_set;
.global _pll_set;

.text
_start1:
start1:
	sp += -12;
	call	board_init_f;
	sp += 12;

_pll_set:
pll_set:
	LINK 0;
	[--sp] = r3;
	[--sp] = r4;
	[--sp] = r5;
	[--sp] = p0;
	[--sp] = p1;
	[--sp] = r2;

	r2 = 0;
	r3 = 1;
	r4 = -1;
dp_begin:
	r0 = r0 - r1;
	CC = r0 <= r4;
	if CC jump dp_skip;
	r2 = r2 + r3;
	jump dp_begin;
dp_skip:

	r0 = r2;
	r3 = r0;

	r1 = 0x3F;
	r0 = r0 & r1;
	r0 = r0 << 9;
	p0.h = (PLL_CTL >> 16);
	p0.l = (PLL_CTL & 0xFFFF);
	r1 = w[p0](z);
	r5 = 0x8E;
	r1 = r1 & r5;
	r1 = r1 | r0;
	cli r2;
	w[p0] = r1;
	idle;
	sti r2;
	ssync;

	r0 = [sp]; /* r0 = PLL_DIV_FACTOR */
	p0.h = (PLL_DIV >> 16);
	p0.l = (PLL_DIV & 0xFFFF);
	w[p0] = r0;
	ssync;

	p1.l = DELAY;
	p1.h = 0x0;
	lsetup ( del_start,del_end ) lc0 = p1;
del_start:
	nop;
del_end:
	nop;

	r2 = [sp++];
	p1 = [sp++];
	p0 = [sp++];
	r5 = [sp++];
	r4 = [sp++];
	r3 = [sp++];
	UNLINK;
	rts;
	nop;
	nop;
	nop;
