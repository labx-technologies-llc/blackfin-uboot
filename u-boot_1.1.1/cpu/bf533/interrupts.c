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
 */

#include <common.h>
#include <asm/machdep.h>
#include <asm/irq.h>
#include <asm/bf533.h>

static unsigned long timer_count;
void timer_int(void);
void timer_init(void);

/* Functions just to satisfy the linker */
void enable_interrupts(void)
{
}

int disable_interrupts(void)
{
	return 0;
}

int interrupt_init(void)
{
	return (0);
}

void udelay(unsigned long usec)
{
	unsigned long i=0;
	
	for(i=0;i<usec;i++) {
		i++;
		i--;
	}
}

void timer_int(void)
{
#if 0
	timer_count++;
#endif
}

void timer_init(void)
{
#if 0
	TCNTL = 0x1;
	TSCALE = 0x95;
	TCOUNT  = 0x105E;
	TPERIOD = 0x105E;
	TCNTL = 0x7;
#endif
}

ulong get_timer(ulong base)
{
	return timer_count;
}
