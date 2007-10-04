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
#include <asm/blackfin.h>
#include <asm/mach-common/bits/uart.h>

#if defined(pUART_LSR) && (CONFIG_UART_CONSOLE != 0)
# error CONFIG_UART_CONSOLE must be 0 on parts with only one UART
#endif

#ifndef pUART_LSR
# if (CONFIG_UART_CONSOLE == 1)
#  define pUART_DLH  pUART1_DLH
#  define pUART_DLL  pUART1_DLL
#  define pUART_GCTL pUART1_GCTL
#  define pUART_LCR  pUART1_LCR
#  define pUART_LSR  pUART1_LSR
#  define pUART_RBR  pUART1_RBR
#  define pUART_THR  pUART1_THR
# elif (CONFIG_UART_CONSOLE == 0)
#  define pUART_DLH  pUART0_DLH
#  define pUART_DLL  pUART0_DLL
#  define pUART_GCTL pUART0_GCTL
#  define pUART_LCR  pUART0_LCR
#  define pUART_LSR  pUART0_LSR
#  define pUART_RBR  pUART0_RBR
#  define pUART_THR  pUART0_THR
# endif
#endif

#ifdef __ADSPBF54x__
# define ACCESS_LATCH()
# define ACCESS_PORT_IER()
#else
# define ACCESS_LATCH()    (*pUART_LCR |= DLAB)
# define ACCESS_PORT_IER() (*pUART_LCR &= ~DLAB)
#endif

static void serial_do_portmux(void)
{
#ifdef __ADSPBF52x__
	if (CONFIG_UART_CONSOLE == 0) {
		bfin_write_PORTG_MUX((bfin_read_PORTG_MUX() & ~PORT_x_MUX_2_MASK) | PORT_x_MUX_2_FUNC_3);
		bfin_write_PORTG_FER(bfin_read_PORTG_FER() | PG7 | PG8);
	} else if (CONFIG_UART_CONSOLE == 1) {
		bfin_write_PORTF_MUX((bfin_read_PORTF_MUX() & ~PORT_x_MUX_5_MASK) | PORT_x_MUX_5_FUNC_3);
		bfin_write_PORTF_FER(bfin_read_PORTF_FER() | PF14 | PF15);
	}
	SSYNC();
#elif defined(__ADSPBF537__) || defined(__ADSPBF536__) || defined(__ADSPBF534__)
	if (CONFIG_UART_CONSOLE == 0) {
		bfin_write_PORT_MUX(bfin_read_PORT_MUX() & ~PFDE);
		bfin_write_PORTF_FER(bfin_read_PORTF_FER() | PF0 | PF1);
	} else if (CONFIG_UART_CONSOLE == 1) {
		bfin_write_PORT_MUX(bfin_read_PORT_MUX() & ~PFTE);
		bfin_write_PORTF_FER(bfin_read_PORTF_FER() | PF2 | PF3);
	}
	SSYNC();
#endif
}

static int baud_table[5] = { 9600, 19200, 38400, 57600, 115200 };
static struct {
	unsigned char dl_high;
	unsigned char dl_low;
} hw_baud_table[5];

static void calc_baud(void)
{
	unsigned char i;
	int temp;
	u_long sclk = get_sclk();

	for (i = 0; i < ARRAY_SIZE(baud_table); ++i) {
		temp = sclk / (baud_table[i] * 8);
		if ((temp & 0x1) == 1)
			++temp;
		temp = temp / 2;
		hw_baud_table[i].dl_high = (temp >> 8) & 0xFF;
		hw_baud_table[i].dl_low = (temp) & 0xFF;
	}
}

void serial_setbrg(void)
{
	int i;
	DECLARE_GLOBAL_DATA_PTR;

	calc_baud();

	/* XXX: what if baudrate is not in baud_table ? */
	for (i = 0; i < ARRAY_SIZE(baud_table); ++i)
		if (gd->baudrate == baud_table[i])
			break;

	/* handle portmux crap on different Blackfins */
	serial_do_portmux();

	/* Enable UART */
	*pUART_GCTL |= UCEN;
	SSYNC();

	/* Set DLAB in LCR to Access DLL and DLH */
	ACCESS_LATCH();
	SSYNC();

	*pUART_DLL = hw_baud_table[i].dl_low;
	SSYNC();
	*pUART_DLH = hw_baud_table[i].dl_high;
	SSYNC();

	/* Clear DLAB in LCR to Access THR RBR IER */
	ACCESS_PORT_IER();

	/* Set LCR to Word Lengh 8-bit word select */
	*pUART_LCR = WLS_8;
	SSYNC();
}

int serial_init(void)
{
	serial_setbrg();
	return 0;
}

void serial_putc(const char c)
{
	/* send a \r for compatibility */
	if (c == '\n')
		serial_putc('\r');

	/* wait for the THR to clear up */
	while (!(*pUART_LSR & THRE))
		continue;

	/* queue the character for transmission */
	*pUART_THR = c;
	SSYNC();

	/* wait for both the THR and the TSR to empty
	 * XXX: do we really care ?  the THRE check above
	 * prevents transmit overflows, and if we're worried
	 * about baud changes, we can move the TEMT check to
	 * serial_setbrg() ...
	 */
	while (!(*pUART_LSR & TEMT))
		SSYNC();
}

int serial_tstc(void)
{
	return (*pUART_LSR & DR) ? 1 : 0;
}

int serial_getc(void)
{
	uint16_t uart_lsr_val, uart_rbr_val;

	/* wait for data ! */
	while (!serial_tstc())
		continue;

	/* clear the status and grab the new byte */
	uart_lsr_val = *pUART_LSR;
	uart_rbr_val = *pUART_RBR;

	if (uart_lsr_val & (OE|PE|FE|BI))
		return -1;
	else
		return uart_rbr_val & 0xFF;
}

void serial_puts(const char *s)
{
	while (*s)
		serial_putc(*s++);
}
