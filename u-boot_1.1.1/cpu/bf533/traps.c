/*
 *
 * No original Copyright holder listed,
 * Probabily original (C) Roman Zippel (assigned DJD, 1999)
 *
 * Copyright 2003 Metrowerks - for Blackfin
 * Copyright 2000-2001 Lineo, Inc. D. Jeff Dionne <jeff@lineo.ca>
 * Copyright 1999-2000 D. Jeff Dionne, <jeff@uclinux.org>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.

**********************************************************************************************************

                        PROJECT                 :       BFIN
                        VERISON                 :       2.0
                        FILE                    :       traps.c
                        MODIFIED DATE           :       29 jun 2004
                        AUTHOR                  :       BFin Project-ADI
                        LOCATION                :       LG Soft India,Bangalore

***********************************************************************************************************/
#include <common.h>	
#include <linux/types.h>
#include <asm/errno.h>
#include <asm/irq.h>

#include <asm/system.h>
#include <asm/traps.h>
#include <asm/page.h>
#include <asm/machdep.h>

#include "cpu.h"


void init_IRQ(void)
{
	FRIO_init_IRQ ();
	return;
}

irq_node_t *new_irq_node(void)
{
	irq_node_t *node;
	short i;

	for (node = nodes, i = NUM_IRQ_NODES-1; i >= 0; node++, i--)
		if (!node->handler)
			return node;
	return NULL;
}

int request_irq(unsigned int irq, void (*handler)(int, void *, struct pt_regs *),
                unsigned long flags, const char *devname, void *dev_id)
{
	if (irq)
		return FRIO_request_irq(irq, handler, flags, devname, dev_id);

	if (irq < IRQ_EMU || irq > IRQ_SW_INT2) {
		return -ENXIO;
	}

	if (!(irq_list[irq].flags & IRQ_FLG_STD)) {
		if (irq_list[irq].flags & IRQ_FLG_LOCK) {
			return -EBUSY;
		}
		if (flags & IRQ_FLG_REPLACE) {
			return -EBUSY;
		}
	}
	irq_list[irq].handler = handler;
	irq_list[irq].flags   = flags;
	irq_list[irq].dev_id  = dev_id;
	irq_list[irq].devname = devname;
	return 0;
}

void free_irq(unsigned int irq, void *dev_id)
{
	if (irq) {
		FRIO_free_irq(irq, dev_id);
		return;
	}

	if (irq < IRQ_EMU || irq > IRQ_SW_INT2) {
		return;
	}
}

unsigned long probe_irq_on (void)
{
	return 0;
}

int probe_irq_off (unsigned long irqs)
{
	return 0;
}

void process_int(unsigned long vec, struct pt_regs *fp)
{
	if (FRIO_do_irq)
        	FRIO_do_irq(vec, fp);
	else
		;
	return;
}

int get_irq_list(char *buf)
{
        return 0;
}

void dump(struct pt_regs *fp)
{
	printf("PC: %08lx\n", fp->pc);
	printf("SEQSTAT: %08lx    SP: %08lx\n", (long) fp->seqstat, (long) fp);
	printf("R0: %08lx    R1: %08lx    R2: %08lx    R3: %08lx\n",
		fp->r0, fp->r1, fp->r2, fp->r3);
	printf("R4: %08lx    R5: %08lx    R6: %08lx    R7: %08lx\n",
		fp->r4, fp->r5, fp->r6, fp->r7);
	printf("P0: %08lx    P1: %08lx    P2: %08lx    P3: %08lx\n",
		fp->p0, fp->p1, fp->p2, fp->p3);
	printf("P4: %08lx    P5: %08lx    FP: %08lx\n",
		fp->p4, fp->p5, fp->fp);
	printf("A0.w: %08lx    A0.x: %08lx    A1.w: %08lx    A1.x: %08lx\n",
		fp->a0w, fp->a0x, fp->a1w, fp->a1x);
	printf("\n");
}
