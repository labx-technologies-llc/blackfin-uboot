/*
 * U-boot - traps.c Routines related to interrupts and exceptions
 *
 * Copyright (c) 2005-2007 Analog Devices Inc.
 *
 * This file is based on
 * No original Copyright holder listed,
 * Probabily original (C) Roman Zippel (assigned DJD, 1999)
 *
 * Copyright 2003 Metrowerks - for Blackfin
 * Copyright 2000-2001 Lineo, Inc. D. Jeff Dionne <jeff@lineo.ca>
 * Copyright 1999-2000 D. Jeff Dionne, <jeff@uclinux.org>
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
#include <linux/types.h>
#include <asm/errno.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/traps.h>
#include "cpu.h"
#include <asm/memory-map.h>
#include <asm/arch/anomaly.h>
#include <asm/cplb.h>
#include <asm/io.h>

void process_int(unsigned long vec, struct pt_regs *fp)
{
	printf("interrupt\n");
	return;
}

void trap_c(struct pt_regs *regs)
{
	uint32_t trapnr = (regs->seqstat) & SEQSTAT_EXCAUSE;
	bool data = false;

	switch (trapnr) {
	/* 0x26 - Data CPLB Miss */
	case VEC_CPLB_M:

		if (ANOMALY_05000261) {
			static uint32_t last_cplb_fault_retx;
			/*
			 * Work around an anomaly: if we see a new DCPLB fault,
			 * return without doing anything. Then,
			 * if we get the same fault again, handle it.
			 */
			if (last_cplb_fault_retx != regs->retx) {
				last_cplb_fault_retx = regs->retx;
				return;
			}
		}

		data = true;
		/* fall through */

	/* 0x27 - Instruction CPLB Miss */
	case VEC_CPLB_I_M: {
		volatile uint32_t *CPLB_ADDR_BASE, *CPLB_DATA_BASE, *CPLB_ADDR, *CPLB_DATA;
		uint32_t new_cplb_addr = 0, new_cplb_data = 0;
		static size_t last_evicted;
		size_t i;

		new_cplb_addr = (*(uint32_t *)(data ? pDCPLB_FAULT_ADDR : pICPLB_FAULT_ADDR)) & ~(4 * 1024 * 1024 - 1);

		for (i = 0; i < ARRAY_SIZE(bfin_memory_map); ++i) {
			/* if the exception is inside this range ... */
			if (new_cplb_addr >= bfin_memory_map[i].start &&
			    new_cplb_addr < bfin_memory_map[i].end)
				/* and it is the right type for this range ... */
				if ((data && bfin_memory_map[i].data) ||
				    (!data && bfin_memory_map[i].inst))
					/* lets use it */
					break;
		}
		if (i == ARRAY_SIZE(bfin_memory_map)) {
			printf("CPLB exception outside of memory map\n");
			do_reset(NULL, 0, 0, NULL);
		} else
			debug("CPLB addr %p matches map 0x%p - 0x%p\n", new_cplb_addr, bfin_memory_map[i].start, bfin_memory_map[i].end);
		new_cplb_data = (data ? bfin_memory_map[i].data_flags : bfin_memory_map[i].inst_flags);

		/* Turn the cache off */
		sync();
		if (data) {
			asm(" .align 8; ");
			*pDMEM_CONTROL &= ~ENDCPLB;
		} else {
			asm(" .align 8; ");
			*pIMEM_CONTROL &= ~ENICPLB;
		}
		sync();

		if (data) {
			CPLB_ADDR_BASE = (uint32_t *)DCPLB_ADDR0;
			CPLB_DATA_BASE = (uint32_t *)DCPLB_DATA0;
		} else {
			CPLB_ADDR_BASE = (uint32_t *)ICPLB_ADDR0;
			CPLB_DATA_BASE = (uint32_t *)ICPLB_DATA0;
		}

		/* find the next unlocked entry and evict it */
		i = last_evicted & 0xF;
		debug("last evicted = %i\n", i);
		CPLB_DATA = CPLB_DATA_BASE + i;
		while (*CPLB_DATA & CPLB_LOCK) {
			debug("skipping %i %p - %08X\n", i, CPLB_DATA, *CPLB_DATA);
			i = (i + 1) & 0xF;	/* wrap around */
			CPLB_DATA = CPLB_DATA_BASE + i;
		}
		CPLB_ADDR = CPLB_ADDR_BASE + i;

		debug("evicting entry %i: 0x%p 0x%08X\n", i, *CPLB_ADDR, *CPLB_DATA);
		last_evicted = i + 1;
		*CPLB_ADDR = new_cplb_addr;
		*CPLB_DATA = new_cplb_data;

		/* dump current table for debugging purposes */
		CPLB_ADDR = CPLB_ADDR_BASE;
		CPLB_DATA = CPLB_DATA_BASE;
		for (i = 0; i < 16; ++i)
			debug("%2i 0x%p 0x%08X\n", i, *CPLB_ADDR++, *CPLB_DATA++);

		/* Turn the cache back on */
		sync();
		if (data) {
			asm(" .align 8; ");
			*pDMEM_CONTROL |= ENDCPLB;
		} else {
			asm(" .align 8; ");
			*pIMEM_CONTROL |= ENICPLB;
		}
		sync();

		break;
	}

	default:
		/* All traps come here */
		blackfin_irq_panic(trapnr, regs);
		do_reset(NULL, 0, 0, NULL);
	}

	return;
}

void dump(struct pt_regs *fp)
{
	debug("RETE:  %08lx  RETN: %08lx  RETX: %08lx  RETS: %08lx\n",
		 fp->rete, fp->retn, fp->retx, fp->rets);
	debug("IPEND: %04lx  SYSCFG: %04lx\n", fp->ipend, fp->syscfg);
	debug("SEQSTAT: %08lx    SP: %08lx\n", (long)fp->seqstat, (long)fp);
	debug("R0: %08lx    R1: %08lx    R2: %08lx    R3: %08lx\n",
		 fp->r0, fp->r1, fp->r2, fp->r3);
	debug("R4: %08lx    R5: %08lx    R6: %08lx    R7: %08lx\n",
		 fp->r4, fp->r5, fp->r6, fp->r7);
	debug("P0: %08lx    P1: %08lx    P2: %08lx    P3: %08lx\n",
		 fp->p0, fp->p1, fp->p2, fp->p3);
	debug("P4: %08lx    P5: %08lx    FP: %08lx\n",
		 fp->p4, fp->p5, fp->fp);
	debug("A0.w: %08lx    A0.x: %08lx    A1.w: %08lx    A1.x: %08lx\n",
		 fp->a0w, fp->a0x, fp->a1w, fp->a1x);

	debug("LB0: %08lx  LT0: %08lx  LC0: %08lx\n",
		 fp->lb0, fp->lt0, fp->lc0);
	debug("LB1: %08lx  LT1: %08lx  LC1: %08lx\n",
		 fp->lb1, fp->lt1, fp->lc1);
	debug("B0: %08lx  L0: %08lx  M0: %08lx  I0: %08lx\n",
		 fp->b0, fp->l0, fp->m0, fp->i0);
	debug("B1: %08lx  L1: %08lx  M1: %08lx  I1: %08lx\n",
		 fp->b1, fp->l1, fp->m1, fp->i1);
	debug("B2: %08lx  L2: %08lx  M2: %08lx  I2: %08lx\n",
		 fp->b2, fp->l2, fp->m2, fp->i2);
	debug("B3: %08lx  L3: %08lx  M3: %08lx  I3: %08lx\n",
		 fp->b3, fp->l3, fp->m3, fp->i3);

	debug("DCPLB_FAULT_ADDR=%p\n", *pDCPLB_FAULT_ADDR);
	debug("ICPLB_FAULT_ADDR=%p\n", *pICPLB_FAULT_ADDR);
}
