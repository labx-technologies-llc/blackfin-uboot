/*
 * U-boot - serial.h Blackfin Serial Driver defines
 *
 * Copyright (c) 2005-2007 Analog Devices Inc.
 *
 * Copyright (C) 2003	Bas Vermeulen <bas@buyways.nl>
 * 			BuyWays B.V. (www.buyways.nl)
 *
 * Based heavily on:
 * blkfinserial.h: Definitions for the BlackFin DSP serial driver.
 *
 * Copyright (C) 2001	Tony Z. Kou	tonyko@arcturusnetworks.com
 * Copyright (C) 2001   Arcturus Networks Inc. <www.arcturusnetworks.com>
 *
 * Based on code from 68328serial.c which was:
 * Copyright (C) 1995       David S. Miller    <davem@caip.rutgers.edu>
 * Copyright (C) 1998       Kenneth Albanowski <kjahds@kjahds.com>
 * Copyright (C) 1998, 1999 D. Jeff Dionne     <jeff@uclinux.org>
 * Copyright (C) 1999       Vladimir Gurevich  <vgurevic@cisco.com>
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

#ifndef _BFIN_SERIAL_H
#define _BFIN_SERIAL_H

#include <linux/config.h>
#include <asm/blackfin.h>

/*
#define ACCESS_LATCH	*pUART1_LCR |= UART_LCR_DLAB;
#define ACCESS_PORT_IER	*pUART1_LCR &= (~UART_LCR_DLAB);
*/
#define ACCESS_LATCH	do {} while (0)
#define ACCESS_PORT_IER	do {} while (0)

static void local_put_char(char ch);
void calc_baud(void);
void serial_setbrg(void);
int serial_init(void);
void serial_putc(const char c);
int serial_tstc(void);
int serial_getc(void);
void serial_puts(const char *s);

int baud_table[5] = { 9600, 19200, 38400, 57600, 115200 };

struct {
	unsigned char dl_high;
	unsigned char dl_low;
} hw_baud_table[5];

#endif
