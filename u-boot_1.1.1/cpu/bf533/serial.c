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
 * bf533_serial.c: Serial driver for BlackFin BF533 DSP internal UART.
 * Copyright (c) 2003	Bas Vermeulen <bas@buyways.nl>,
 * 			BuyWays B.V. (www.buyways.nl)
 *
 * Based heavily on blkfinserial.c
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
 */

#include <common.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/segment.h>
#include <asm/bitops.h>
#include <asm/delay.h>
#include <asm/uaccess.h>
#include "bf533_serial.h"

unsigned long pll_div_fact;

void calc_baud(void)
{
	unsigned char i;

	for(i = 0; i < sizeof(baud_table)/sizeof(int); i++) {
		hw_baud_table[i].dl_high = ((sclk/(baud_table[i]*16)) >> 8)& 0xFF;
		hw_baud_table[i].dl_low = (sclk/(baud_table[i]*16)) & 0xFF;
	}
}

void serial_setbrg(void)
{
	int i;
	DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_STAMP
	sclk = get_clock()/pll_div_fact;
#else
	sclk = get_clock()/PLL_DIV_FACTOR;
#endif
	calc_baud();

	for (i = 0; i < sizeof(baud_table) / sizeof(int); i++) {
		if (gd->baudrate == baud_table[i])
			break;
	}

        /* Enable UART */
        UART_GCTL |= UART_GCTL_UCEN;
        asm("ssync;");

        /* Set DLAB in LCR to Access DLL and DLH */
        ACCESS_LATCH;
        asm("ssync;");

	UART_DLL = hw_baud_table[i].dl_low;
	asm("ssync;");
	UART_DLH = hw_baud_table[i].dl_high;
	asm("ssync;");

	/* Clear DLAB in LCR to Access THR RBR IER */
	ACCESS_PORT_IER;
	asm("ssync;");

        /* Enable  ERBFI and ELSI interrupts
         * to poll SIC_ISR register*/
        UART_IER = UART_IER_ELSI | UART_IER_ERBFI | UART_IER_ETBEI;
        asm("ssync;");

	/* Set LCR to Word Lengh 8-bit word select */
	UART_LCR = UART_LCR_WLS8;
	asm("ssync;");

	return;
}

int serial_init(void)
{
#ifdef CONFIG_STAMP
	pll_div_fact = PLL_DIV_FACTOR;
#endif
	serial_setbrg();
	return (0);
}

void serial_putc(const char c)
{
	if (UART_LSR & UART_LSR_TEMT) {
		if (c == '\n') {
			local_put_char('\r');
		}
		local_put_char(c);
		if (c == '\r') {
			local_put_char('\n');
		}
	}

	while (!(UART_LSR & UART_LSR_TEMT))
		SYNC_ALL;

	return;
}

int serial_tstc(void)
{
	if (UART_LSR & UART_LSR_DR)
		return 1;
	else
		return 0;
}

int serial_getc(void)
{
	unsigned short uart_lsr_val, uart_rbr_val;
	unsigned long isr_val;
	int ret;

	/* Poll for RX Interrupt */
	while (!((isr_val = *(volatile unsigned long *)SIC_ISR) & IRQ_UART_RX_BIT)); 
	asm("csync;");

	uart_lsr_val = UART_LSR;	/* Clear status bit */
	uart_rbr_val = UART_RBR;	/* getc() */

	if (isr_val & IRQ_UART_ERROR_BIT) {
		ret =  -1; 
	}
	else
	{
		ret = uart_rbr_val & 0xff;
	}

	return ret;
}

void serial_puts(const char *s)
{
	while (*s) {
		serial_putc(*s++);
	}
}

static void local_put_char(char ch)
{
	int flags = 0;
	unsigned short uart_lsr_val;
        unsigned long isr_val;

	save_and_cli(flags);

        /* Poll for TX Interruput */
        while (!((isr_val = *(volatile unsigned long *)SIC_ISR) & IRQ_UART_TX_BIT));
        asm("csync;");

	UART_THR = ch;			/* putc() */

        if (isr_val & IRQ_UART_ERROR_BIT) {
                printf("?");
        }

	restore_flags(flags);

        return ;
}
