/*
 * U-boot - serial.c Blackfin Serial Driver
 *
 * Copyright (c) 2005-2007 Analog Devices Inc.
 *
 * Copyright (c) 2003	Bas Vermeulen <bas@buyways.nl>,
 * 			BuyWays B.V. (www.buyways.nl)
 *
 * Based heavily on:
 * blkfinserial.c: Serial driver for BlackFin DSP internal USRTs.
 * Copyright(c) 2003	Metrowerks	<mwaddel@metrowerks.com>
 * Copyright(c)	2001	Tony Z. Kou	<tonyko@arcturusnetworks.com>
 * Copyright(c)	2001-2002 Arcturus Networks Inc. <www.arcturusnetworks.com>
 *
 * Based on code from 68328 version serial driver imlpementation which was:
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

#include <common.h>
#include <watchdog.h>
#include <asm/blackfin.h>
#include <asm/mach-common/bits/uart.h>

#if defined(UART_LSR) && (CONFIG_UART_CONSOLE != 0)
# error CONFIG_UART_CONSOLE must be 0 on parts with only one UART
#endif

#include "serial.h"

#ifdef CONFIG_DEBUG_SERIAL
uint16_t cached_lsr[256];
uint16_t cached_rbr[256];
size_t cache_count;

/* The LSR is read-to-clear on some parts, so we have to make sure status
 * bits aren't inadvertently lost when doing various tests.
 */
static uint16_t uart_lsr_save;
static uint16_t uart_lsr_read(void)
{
	uint16_t lsr = *pUART_LSR;
	uart_lsr_save |= (lsr & (OE|PE|FE|BI));
	return lsr | uart_lsr_save;
}
/* Just do the clear for everyone since it can't hurt. */
static void uart_lsr_clear(void)
{
	uart_lsr_save = 0;
	*pUART_LSR |= -1;
}
#else
static inline uint16_t uart_lsr_read(void) { return *pUART_LSR; }
static inline void uart_lsr_clear(void) { *pUART_LSR = -1; }
#endif

/* Symbol for our assembly to call. */
void serial_set_baud(uint32_t baud)
{
	serial_early_set_baud(baud);
}

/* Symbol for common u-boot code to call.
 * Setup the baudrate (brg: baudrate generator).
 */
void serial_setbrg(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	serial_set_baud(gd->baudrate);
}

/* Symbol for our assembly to call. */
void serial_initialize(void)
{
	serial_early_init();
}

/* Symbol for common u-boot code to call. */
int serial_init(void)
{
	serial_initialize();
	serial_setbrg();
	uart_lsr_clear();
#ifdef CONFIG_DEBUG_SERIAL
	cache_count = 0;
	memset(cached_lsr, 0x00, sizeof(cached_lsr));
	memset(cached_rbr, 0x00, sizeof(cached_rbr));
#endif
	return 0;
}

void serial_putc(const char c)
{
	/* send a \r for compatibility */
	if (c == '\n')
		serial_putc('\r');

	WATCHDOG_RESET();

	/* wait for the hardware fifo to clear up */
	while (!(uart_lsr_read() & THRE))
		continue;

	/* queue the character for transmission */
	*pUART_THR = c;
	SSYNC();

	WATCHDOG_RESET();

	/* wait for the byte to be shifted over the line */
	while (!(uart_lsr_read() & TEMT))
		continue;
}

int serial_tstc(void)
{
	WATCHDOG_RESET();
	return (uart_lsr_read() & DR) ? 1 : 0;
}

int serial_getc(void)
{
	uint16_t uart_rbr_val;

	/* wait for data ! */
	while (!serial_tstc())
		continue;

	/* grab the new byte */
	uart_rbr_val = *pUART_RBR;

#ifdef CONFIG_DEBUG_SERIAL
	/* grab & clear the LSR */
	uint16_t uart_lsr_val = uart_lsr_read();
	uart_lsr_clear();

	cached_lsr[cache_count] = uart_lsr_val;
	cached_rbr[cache_count] = uart_rbr_val;
	cache_count = (cache_count + 1) % ARRAY_SIZE(cached_lsr);

	if (uart_lsr_val & (OE|PE|FE|BI)) {
		printf("\n[SERIAL ERROR]\n");
		do {
			--cache_count;
			printf("\t%3i: RBR=0x%02x LSR=0x%02x\n", cache_count,
				cached_rbr[cache_count], cached_lsr[cache_count]);
		} while (cache_count > 0);
		return -1;
	}
#endif

	return uart_rbr_val;
}

void serial_puts(const char *s)
{
	while (*s)
		serial_putc(*s++);
}
