/*
 * U-boot - ints.c Interrupt related routines
 *
 * Copyright (c) 2005-2007 Analog Devices Inc.
 *
 * This file is based on ints.c
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
#include <asm/system.h>
#include <asm/io.h>
#include <asm/blackfin.h>
#include "cpu.h"

void blackfin_irq_panic(int reason, struct pt_regs *regs)
{
	printf(
		"\n\n\n"
		"Exception: IRQ 0x%x entered\n"
		"seqstat=0x%x, stack frame=0x%p, bad PC=0x%p\n",
		reason,
		(unsigned int)regs->seqstat,
		(unsigned int)regs,
		(unsigned int)regs->pc
	);
	dump(regs);
	printf(
		"Unhandled IRQ or exceptions!\n"
		"Please reset the board\n"
		"\n\n"
	);
}

void blackfin_no_irqs(void)
{
	printf(
		"\n"
		"\n"
		"Unhandled IRQ (should not have been any IRQs)\n"
		"IMASK = 0x%08x\n"
		"ILAT  = 0x%08x\n"
		"IPEND = 0x%08x\n"
		"\n",
		bfin_read_IMASK(),
		bfin_read_ILAT(),
		bfin_read_IPEND()
	);
	bfin_reset_or_hang();
}

void irq_init(void)
{
#ifdef SIC_IMASK0
	bfin_write_SIC_IMASK0(0);
	bfin_write_SIC_IMASK1(0);
# ifdef SIC_IMASK2
	bfin_write_SIC_IMASK2(0);
# endif
#elif defined(SICA_IMASK0)
	bfin_write_SICA_IMASK0(0);
	bfin_write_SICA_IMASK1(0);
#else
	bfin_write_SIC_IMASK(0);
#endif
	bfin_write_EVT2(blackfin_no_irqs);	/* NMI */
	bfin_write_EVT3(trap);	/* exceptions */
	bfin_write_EVT5(blackfin_no_irqs);	/* hardware error */
	bfin_write_EVT6(blackfin_no_irqs);	/* core timer */
	bfin_write_EVT7(blackfin_no_irqs);
	bfin_write_EVT8(blackfin_no_irqs);
	bfin_write_EVT9(blackfin_no_irqs);
	bfin_write_EVT10(blackfin_no_irqs);
	bfin_write_EVT11(blackfin_no_irqs);
	bfin_write_EVT12(blackfin_no_irqs);
	bfin_write_EVT13(blackfin_no_irqs);
	bfin_write_EVT14(blackfin_no_irqs);
	bfin_write_EVT15(blackfin_no_irqs);
	bfin_write_ILAT(0);
	CSYNC();
	/* enable all interrupts except for core timer */
	irq_flags = 0xffffffbf;
	local_irq_enable();
	CSYNC();
}
