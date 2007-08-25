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
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/segment.h>
#include <asm/bitops.h>
#include <asm/delay.h>
#include <asm/io.h>
#include "serial.h"

#ifdef __ADSPBF54x__
#define IRQ_UART1_RX_BIT		0x0200 //SIC_ISR1
#define IRQ_UART1_TX_BIT		0x0400 //SIC_ISR1
#define IRQ_UART1_ERROR_BIT	0x40 //SIC_ISR1
#endif

void calc_baud(void)
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

#ifdef PORTF_FER
	/* Enable UART peripheral pins */
	*pPORTF_FER |= 0xF;
	SSYNC();
#endif

#ifdef __ADSPBF54x__
	/* Enable UART */
	*pUART1_GCTL |= UCEN;
	SSYNC();

	/* Set DLAB in LCR to Access DLL and DLH */
	ACCESS_LATCH;
	SSYNC();

	*pUART1_DLL = hw_baud_table[i].dl_low;
	SSYNC();
	*pUART1_DLH = hw_baud_table[i].dl_high;
	SSYNC();

	/* Clear DLAB in LCR to Access THR RBR IER */
	ACCESS_PORT_IER;
	SSYNC();

	/* Enable ERBFI and ELSI interrupts
	 * to poll SIC_ISR register*/
	*pUART1_IER_SET = ELSI_S | ERBFI_S | ETBEI_S;
	SSYNC();

	/* Set LCR to Word Lengh 8-bit word select */
	*pUART1_LCR = WLS_8;
	SSYNC();
#else
	/* Enable UART */
	*pUART_GCTL |= UART_GCTL_UCEN;
	SSYNC();

	/* Set DLAB in LCR to Access DLL and DLH */
	ACCESS_LATCH;
	SSYNC();

	*pUART_DLL = hw_baud_table[i].dl_low;
	SSYNC();
	*pUART_DLH = hw_baud_table[i].dl_high;
	SSYNC();

	/* Clear DLAB in LCR to Access THR RBR IER */
	ACCESS_PORT_IER;
	SSYNC();

	/* Enable ERBFI and ELSI interrupts
	 * to poll SIC_ISR register*/
	*pUART_IER = UART_IER_ELSI | UART_IER_ERBFI | UART_IER_ETBEI;
	SSYNC();

	/* Set LCR to Word Lengh 8-bit word select */
	*pUART_LCR = UART_LCR_WLS8;
	SSYNC();
#endif
}

int serial_init(void)
{
	serial_setbrg();
	return 0;
}

void serial_putc(const char c)
{
#ifdef __ADSPBF54x__
	if (*pUART1_LSR & TEMT) {
#else
	if (*pUART_LSR & UART_LSR_TEMT) {
#endif
		if (c == '\n')
			serial_putc('\r');

		local_put_char(c);
	}

#ifdef __ADSPBF54x__
	while (!(*pUART1_LSR & TEMT))
#else
	while (!(*pUART_LSR & UART_LSR_TEMT))
#endif
		SSYNC();
}

int serial_tstc(void)
{
#ifdef __ADSPBF54x__
	return (*pUART1_LSR & DR) ? 1 : 0;
#else
	return (*pUART_LSR & UART_LSR_DR) ? 1 : 0;
#endif
}

int serial_getc(void)
{
	unsigned short uart_lsr_val, uart_rbr_val;
	uint32_t isr_val;
	int ret;

	/* Poll for RX Interrupt */
#ifdef __ADSPBF54x__
	do {
		isr_val = *pSIC_ISR1;
	} while (!(isr_val & IRQ_UART1_RX_BIT));
#else
	do {
		isr_val = *pSIC_ISR;
	} while (!(isr_val & IRQ_UART_RX_BIT));
#endif
	CSYNC();

#ifdef __ADSPBF54x__
	uart_lsr_val = *pUART1_LSR;	/* Clear status bit */
	uart_rbr_val = *pUART1_RBR;	/* getc() */
#else
	uart_lsr_val = *pUART_LSR;	/* Clear status bit */
	uart_rbr_val = *pUART_RBR;	/* getc() */
#endif

#ifdef __ADSPBF54x__
	if (isr_val & IRQ_UART1_ERROR_BIT)
#else
	if (isr_val & IRQ_UART_ERROR_BIT)
#endif
		ret = -1;
	else
		ret = uart_rbr_val & 0xff;

	return ret;
}

void serial_puts(const char *s)
{
	while (*s)
		serial_putc(*s++);
}

static void local_put_char(char ch)
{
	uint32_t isr_val;

	/* Poll for TX Interruput */
#ifdef __ADSPBF54x__
	do {
		isr_val = *pSIC_ISR1;
	} while (!(isr_val & IRQ_UART1_TX_BIT));
#else
	do {
		isr_val = *pSIC_ISR;
	} while (!(isr_val & IRQ_UART_TX_BIT));
#endif
	CSYNC();

#ifdef __ADSPBF54x__
	*pUART1_THR = ch;	/* putc() */

	if (isr_val & IRQ_UART1_ERROR_BIT)
#else
	*pUART_THR = ch;	/* putc() */

	if (isr_val & IRQ_UART_ERROR_BIT)
#endif
		printf("?");
}
