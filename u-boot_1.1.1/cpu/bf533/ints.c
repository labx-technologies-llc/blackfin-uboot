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
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 *
 * Apr18 2003, Changed by HuTao to support interrupt cascading for Blackfin 
 *             drivers
 *
 * Copyright 1996 Roman Zippel
 * Copyright 1999 D. Jeff Dionne <jeff@uclinux.org>
 * Copyright 2000-2001 Lineo, Inc. D. Jefff Dionne <jeff@lineo.ca>
 * Copyright 2002 Arcturus Networks Inc. MaTed <mated@sympatico.ca>
 * Copyright 2003 Metrowerks/Motorola
 *
 *	PROJECT				:	BFIN
 *	VERSION				:	2.0
 *	FILE				:	ints.c
 *	MODIFIED DATE			:	29 jun 2004
 *	AUTHOR				:	BFin Project-ADI
 *	LOCATION			:	LG Soft India,Bangalore
 */

#include <common.h>
#include <linux/stddef.h>
#include <asm/system.h>
#include <asm/irq.h>
#include <asm/traps.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <asm/machdep.h>
#include <asm/setup.h>
#include <asm/blackfin.h>
#include "cpu.h"

void frio_irq_panic(int reason, struct pt_regs *regs)
{
	printf("\n\nException: IRQ 0x%x entered\n", reason);
	printf("code=[0x%x], ", (unsigned int) (regs->seqstat & 0x3f));
	printf("stack frame=0x%x, ", (unsigned int) regs);
	printf("bad PC=0x%04x\n", (unsigned int) regs->pc);
	dump(regs);
	printf("Unhandled IRQ or exceptions!\n");
	printf("Please reset the board \n");
}

#if 0
static void int_badint(int irq, void *dev_id, struct pt_regs *fp)
{
	num_spurious += 1;
}
#endif

void FRIO_init_IRQ(void)
{
	*(unsigned volatile long *) (SIC_IMASK) = SIC_MASK_ALL;
	cli();
#ifndef CONFIG_KGDB
	*(unsigned volatile long *) (EVT_EMULATION_ADDR) = 0x0;
#endif
	*(unsigned volatile long *) (EVT_NMI_ADDR) =
		(unsigned volatile long) evt_nmi;
	*(unsigned volatile long *) (EVT_EXCEPTION_ADDR) =
		(unsigned volatile long) trap;
	*(unsigned volatile long *) (EVT_HARDWARE_ERROR_ADDR) =
		(unsigned volatile long) evt_ivhw;
	*(unsigned volatile long *) (EVT_RESET_ADDR) =
		(unsigned volatile long) evt_rst;
	*(unsigned volatile long *) (EVT_TIMER_ADDR) =
		(unsigned volatile long) evt_timer;
	*(unsigned volatile long *) (EVT_IVG7_ADDR) =
		(unsigned volatile long) evt_evt7;
	*(unsigned volatile long *) (EVT_IVG8_ADDR) =
		(unsigned volatile long) evt_evt8;
	*(unsigned volatile long *) (EVT_IVG9_ADDR) =
		(unsigned volatile long) evt_evt9;
	*(unsigned volatile long *) (EVT_IVG10_ADDR) =
		(unsigned volatile long) evt_evt10;
	*(unsigned volatile long *) (EVT_IVG11_ADDR) =
		(unsigned volatile long) evt_evt11;
	*(unsigned volatile long *) (EVT_IVG12_ADDR) =
		(unsigned volatile long) evt_evt12;
	*(unsigned volatile long *) (EVT_IVG13_ADDR) =
		(unsigned volatile long) evt_evt13;
	*(unsigned volatile long *) (EVT_IVG14_ADDR) =
		(unsigned volatile long) evt_system_call;
	*(unsigned volatile long *) (EVT_IVG15_ADDR) =
		(unsigned volatile long) evt_soft_int1;
	*(volatile unsigned long *) IMASK = 0xffbf;
	asm("csync;");
	*(volatile unsigned long *) ILAT = 0;
	asm("csync;");
	sti();
	*(volatile unsigned long *) IMASK = 0xffbf;
	asm("csync;");
}

int FRIO_request_irq(unsigned int irq,
		     void (*handler) (int, void *, struct pt_regs *),
		     unsigned long flags, const char *devname,
		     void *dev_id)
{
#if 0
	if (irq >= INTERNAL_IRQS) {
		printf("%s: Unknown IRQ %d from %s\n", __FUNCTION__, irq,
		       devname);
		return -ENXIO;
	}
	if (!int_irq_list[irq]) {
		int_irq_list[irq] = new_irq_node();
		int_irq_list[irq]->flags = IRQ_FLG_STD;
	}
	if (!(int_irq_list[irq]->flags & IRQ_FLG_STD)) {
		if (int_irq_list[irq]->flags & IRQ_FLG_LOCK) {
			printf("IRQ %d is already assigned.\n", irq);
			return -EBUSY;
		}
		if (flags & IRQ_FLG_REPLACE) {
			printf("Can't replace IRQ %d\n", irq);
			return -EBUSY;
		}
	}
	int_irq_list[irq]->handler = handler;
	int_irq_list[irq]->flags = flags;
	int_irq_list[irq]->dev_id = dev_id;
	int_irq_list[irq]->devname = devname;
#endif
	return 0;
}

void FRIO_free_irq(unsigned int irq, void *dev_id)
{
#if 0
	if (irq >= INTERNAL_IRQS) {
		printf("%s: Unknown IRQ %d\n", __FUNCTION__, irq);
		return;
	}
	if (int_irq_list[irq]->dev_id != dev_id)
		printf("%s: removing probably wrong IRQ %d from %s\n",
		       __FUNCTION__, irq, int_irq_list[irq]->devname);
	int_irq_list[irq]->handler = int_badint;
	int_irq_list[irq]->flags = IRQ_FLG_STD;
	int_irq_list[irq]->dev_id = NULL;
	int_irq_list[irq]->devname = NULL;
	FRIO_disable_irq(irq);
#endif
}

void FRIO_enable_irq(unsigned int irq)
{
#if 0
	unsigned long irq_val;
	cli();
	if (irq >= INTERNAL_IRQS) {
		printf("%s: Unknown IRQ %d\n", __FUNCTION__, irq);
		return;
	}
	if (irq <= IRQ_CORETMR) {
		irq_flags |= 1 << irq;
		sti();
		return;
	}
	if (irq == IRQ_SPORT0)
		irq_val = 0x600;
	else if (irq == IRQ_SPORT1)
		irq_val = 0x1800;
	else if (irq == IRQ_UART) {
		irq_val = 0xC000;
	} else
		irq_val = (1 << (irq - IRQ_CORETMR));
	*(unsigned volatile long *) (SIC_MASK_ADDR) |= irq_val;
	sti();
#endif
}

void FRIO_disable_irq(unsigned int irq)
{
#if 0
	unsigned long irq_val;
	if (irq >= INTERNAL_IRQS) {
		printf("%s: Unknown IRQ %d\n", __FUNCTION__, irq);
		return;
	}
	if (irq < IRQ_CORETMR) {
		cli();
		irq_flags &= ~(1 << irq);
		sti();
		return;
	}
	if (irq == IRQ_SPORT0)
		irq_val = 0x600;
	else if (irq == IRQ_SPORT1)
		irq_val = 0x1800;
	else if (irq == IRQ_UART)
		irq_val = 0xC000;
	else
		irq_val = (1 << (irq - IRQ_CORETMR));
	cli();
	*(unsigned volatile long *) (SIC_MASK_ADDR) &= (~irq_val);
	sti();
#endif
}

void call_isr(int irq, struct pt_regs *fp)
{
#if 0
	if (int_irq_list[irq]->handler)
		int_irq_list[irq]->handler(irq, int_irq_list[irq]->dev_id,
					   fp);
	else
		printf("unregistered interrupt %d\n", irq);
	return;
#endif
}

void FRIO_do_irq(int vec, struct pt_regs *fp)
{
#if 0
	if (vec <= IRQ_CORETMR) {
		printf("less than IRQ_CORETMR\n");
		call_isr(vec, fp);
		return;
	}
	switch (vec) {
	case IVG7:
		if ((SIC_ISR & 0x1) && (SIC_MASK & 0x1)) {
			call_isr(IRQ_PLL_WAKEUP, fp);
		}
		if ((SIC_ISR & 0x2) && (SIC_MASK & 0x2)) {
			call_isr(IRQ_DMA_ERROR, fp);
		}
		if ((SIC_ISR & 0x4) && (SIC_MASK & 0x4)) {
			call_isr(IRQ_PPI_ERROR, fp);
		}
		if ((SIC_ISR & 0x8) && (SIC_MASK & 0x8)) {
			call_isr(IRQ_SPORT0_ERROR, fp);
		}
		if ((SIC_ISR & 0x10) && (SIC_MASK & 0x10)) {
			call_isr(IRQ_SPORT1_ERROR, fp);
		}
		if ((SIC_ISR & 0x20) && (SIC_MASK & 0x20)) {
			call_isr(IRQ_SPI_ERROR, fp);
		}
		if ((SIC_ISR & 0x40) && (SIC_MASK & 0x40)) {
			call_isr(IRQ_UART_ERROR, fp);
		}
		break;
	case IVG8:
		if ((SIC_ISR & 0x80) && (SIC_MASK & 0x80))
			call_isr(IRQ_RTC, fp);
		if ((SIC_ISR & 0x100) && (SIC_MASK & 0x100))
			call_isr(IRQ_PPI, fp);
		break;
	case IVG9:
		if ((SIC_ISR & 0x600) && (SIC_MASK & 0x600))
			call_isr(IRQ_SPORT0, fp);
		if ((SIC_ISR & 0x1800) && (SIC_MASK & 0x1800))
			call_isr(IRQ_SPORT1, fp);
		break;
	case IVG10:
		if ((SIC_ISR & 0x2000) && (SIC_MASK & 0x2000))
			call_isr(IRQ_SPI, fp);
		if ((SIC_ISR & 0xC000) && (SIC_MASK & 0xC000)) {
			call_isr(IRQ_UART, fp);
		}
		break;
	case IVG11:
		if ((SIC_ISR & 0x10000) && (SIC_MASK & 0x10000))
			call_isr(IRQ_TMR0, fp);
		if ((SIC_ISR & 0x20000) && (SIC_MASK & 0x20000))
			call_isr(IRQ_TMR1, fp);
		if ((SIC_ISR & 0x40000) && (SIC_MASK & 0x40000))
			call_isr(IRQ_TMR2, fp);
		break;
	case IVG12:
		if ((SIC_ISR & 0x80000) && (SIC_MASK & 0x80000))
			call_isr(IRQ_PROG_INTA, fp);
		if ((SIC_ISR & 0x100000) && (SIC_MASK & 0x100000))
			call_isr(IRQ_PROG_INTB, fp);
		break;
	case IVG13:
		if ((SIC_ISR & 0x200000) && (SIC_MASK & 0x200000))
			call_isr(IRQ_MEM_DMA0, fp);
		if ((SIC_ISR & 0x400000) && (SIC_MASK & 0x400000))
			call_isr(IRQ_MEM_DMA1, fp);
		if ((SIC_ISR & 0x800000) && (SIC_MASK & 0x800000))
			call_isr(IRQ_WATCH, fp);
		break;
	}
#endif
}

int FRIO_get_irq_list(char *buf)
{
	return 0;
}

void display_excp(void)
{
	printf("Exception!\n");
}
