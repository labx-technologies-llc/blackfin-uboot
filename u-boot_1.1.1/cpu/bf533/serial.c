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
 *
 *	PROJECT				:	BFIN
 *	VERSION				:	2.0
 *	FILE				:	serial.c
 *	MODIFIED DATE			:	29 jun 2004
 *	AUTHOR				:	BFin Project-ADI
 *	LOCATION			:	LG Soft India,Bangalore
 */

#include <common.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/segment.h>
#include <asm/bitops.h>
#include <asm/delay.h>
#include <asm/uaccess.h>
#include "bf533_serial.h"

void serial_setbrg(void)
{
	int i;
	DECLARE_GLOBAL_DATA_PTR;

	UART_GCTL |= UART_GCTL_UCEN;
	ACCESS_PORT_IER;
	UART_IER &= ~UART_IER_ETBEI;
	ACCESS_LATCH;

	for (i = 0; i < sizeof(baud_table) / sizeof(int); i++) {
		if (gd->baudrate == baud_table[i])
			break;
	}

	UART_DLL = hw_baud_table[i].dl_low;
	UART_DLH = hw_baud_table[i].dl_high;

	UART_LCR |= UART_LCR_WLS8;
	UART_LCR &= ~UART_LCR_PEN;

	ACCESS_PORT_IER;
	printf("Serial Port initialized with baud rate = %d\n",
	       CONFIG_BAUDRATE);

	return;
}

int serial_init(void)
{
	serial_setbrg();	/* Currently supports polling method - BFIN */
	return (0);
}

void serial_putc(const char c)
{
	while (!(UART_LSR & UART_LSR_TEMT))
		SYNC_ALL;

	if (UART_LSR & UART_LSR_TEMT) {
		if (c == '\n') {
			local_put_char('\r');
		}
		local_put_char(c);
		if (c == '\r') {
			local_put_char('\n');
		}
	}
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
	while (!(UART_LSR & UART_LSR_DR));
	return (UART_RBR & 0xff);
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
	save_flags(flags);
	cli();
	while (!(UART_LSR & UART_LSR_THRE)) {
		SYNC_ALL;
	}
	ACCESS_PORT_IER UART_THR = ch;
	SYNC_ALL;
	restore_flags(flags);
}


void serial_isr(int irq, void *dev_id, struct pt_regs *regs)
{
	struct bf533_serial *info;
	unsigned short iir;
	unsigned short rx;
	unsigned int sic_status = SIC_ISR;

	if (sic_status & 0xC000) {
		iir = UART_IIR;
		info = &bf533_soft;

		if (!(iir & UART_IIR_NOINT)) {
			switch (iir & UART_IIR_STATUS) {
			case UART_IIR_LSR:
				printf("[%x,%x]", iir, UART_LSR);
				break;
			case UART_IIR_RBR:
				ACCESS_PORT_IER if (UART_LSR & UART_LSR_DR) {
					rx = UART_RBR;
					receive_chars(info, regs, rx);
				}
				break;
			case UART_IIR_THR:
				ACCESS_PORT_IER
				    if (UART_LSR & UART_LSR_THRE) {
					transmit_chars(info);
				}
				break;
			case UART_IIR_MSR:
				break;
			}
		}
	}
	return;
}

static _INLINE_ void receive_chars(struct bf533_serial *info,
				   struct pt_regs *regs, unsigned short rx)
{
	unsigned char ch;

	ch = (unsigned char) rx;
	serial_putc(ch);

	return;
}

static _INLINE_ void transmit_chars(struct bf533_serial *info)
{
#if 0
	if (info->x_char) {
		local_put_char(info->x_char);
		info->x_char = 0;
		goto clear_and_return;
	}
	if (info->xmit_cnt <= 0) {
		ACCESS_PORT_IER UART_IER &= ~UART_IER_ETBEI;
		goto clear_and_return;
	}
	local_put_char(info->xmit_buf[info->xmit_tail++]);
	info->xmit_tail = info->xmit_tail & (SERIAL_XMIT_SIZE - 1);
	info->xmit_cnt--;

	if (info->xmit_cnt <= 0) {
		ACCESS_PORT_IER UART_IER &= ~UART_IER_ETBEI;
		goto clear_and_return;
	}
      clear_and_return:
	return;
#endif
}

#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
void kgdb_serial_init(void)
{
	serial_printf("[on serial] ");
}

void putDebugChar(int c)
{
	serial_putc(c);
}

void putDebugStr(const char *str)
{
	serial_puts(str);
}

int getDebugChar(void)
{
	return serial_getc();
}

void kgdb_interruptible(int yes)
{
	return;
}
#endif	/* CFG_CMD_KGDB */
