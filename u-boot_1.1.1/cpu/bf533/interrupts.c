/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
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

**********************************************************************************************************

                        PROJECT                 :       BFIN
                        VERISON                 :       2.0
                        FILE                    :       interrupts.c
                        MODIFIED DATE           :       29 jun 2004
                        AUTHOR                  :       BFin Project-ADI
                        LOCATION                :       LG Soft India,Bangalore

***********************************************************************************************************/
#include <common.h>
#include <asm/machdep.h>
#include <asm/irq.h>


extern void FRIO_enable_irq(unsigned int irq);
extern void FRIO_disable_irq(unsigned int irq);


/* enable IRQ/FIQ interrupts */
void enable_interrupts (void)
{
	FRIO_enable_irq(IRQ_UART);
}



/* disable IRQ/FIQ interrupts returns true if interrupts had been enabled before we disabled them */
int disable_interrupts (void)
{
	FRIO_disable_irq(IRQ_UART);
	return 0;
}

void bad_mode (void)
{

}

void show_regs (struct pt_regs *regs)
{

}

void do_undefined_instruction (struct pt_regs *pt_regs)
{

}

void do_software_interrupt (struct pt_regs *pt_regs)
{

}

void do_prefetch_abort (struct pt_regs *pt_regs)
{

}

void do_data_abort (struct pt_regs *pt_regs)
{

}

void do_not_used (struct pt_regs *pt_regs)
{

}

void do_fiq (struct pt_regs *pt_regs)
{

}

void do_irq (struct pt_regs *pt_regs)
{

}

int interrupt_init (void)
{
	return (0);
}

/* * timer without interrupts */

void reset_timer (void)
{

}

ulong get_timer (ulong base)
{
	return 0;
}

void set_timer (ulong t)
{
}

void udelay (unsigned long usec)
{
	unsigned long i;
	unsigned long j = usec/3;
	for(i = 0; i < j ; i++);
}

void reset_timer_masked (void)
{

}

ulong get_timer_masked (void)
{
	return 0;
}

void udelay_masked (unsigned long usec)
{

}
