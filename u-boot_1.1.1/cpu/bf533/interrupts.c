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
 * along  with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/machdep.h>
#include <asm/irq.h>
#include <asm/bf533.h>
#include "cpu.h"

static ulong timestamp;
static ulong last_time;
static int int_flag;

int irq_flags; /* needed by asm-blackfin/system.h */

/* Functions just to satisfy the linker */

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On BF533 it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return get_timer(0);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On BF533 it returns the number of timer ticks per second.
 */
ulong get_tbclk (void)
{
	ulong tbclk;

	tbclk = CFG_HZ;
	return tbclk;
}

void enable_interrupts(void)
{
	restore_flags(int_flag);
}

int disable_interrupts(void)
{
	save_and_cli(int_flag);
	return 1;
}

int interrupt_init(void)
{
	return (0);
}

void udelay(unsigned long usec)
{
        unsigned long delay;

        /* system clk  in MHZ*/
        delay = get_clock();

        /* system clk is 1 uSec */
        delay = delay /1000000;

        /* delay = ssytem clk * usecs *
         * delay is the number of sclk's
         */
        delay = delay * usec;

        asm("lsetup(cp_s,cp_e) LC0 = %0;\n"
           "cp_s: \n "
           "cp_e: nop; \n"
           :
           : "a" (delay)
           );
        return;
}

void timer_init(void)
{
	TCNTL = 0x1;
	TSCALE = 0x0;
	TCOUNT  = MAX_TIM_LOAD;
	TPERIOD = MAX_TIM_LOAD;
	TCNTL = 0x7;
	asm("CSYNC;");

	timestamp = 0;
	last_time = 0;
}

/* Any network command or flash
 * command is started get_timer shall  
 * be called before TCOUNT gets reset,
 * to implement the accurate timeouts. 
 *
 * How ever milliconds doesn't return
 * the number that has been elapsed from
 * the last reset.
 *
 *  As get_timer is used in the u-boot
 *  only for timeouts this should be 
 *  sufficient
 */
ulong get_timer(ulong base)
{
	ulong milisec;

	/* Number of clocks elapsed */
	ulong clocks = (MAX_TIM_LOAD - TCOUNT);

	/* Find if the TCOUNT is reset
	   timestamp gives the number of times
	   TCOUNT got reset */
        if(clocks < last_time)
		timestamp++;
	last_time = clocks;

	/* Get the number of milliseconds */
	milisec = (clocks/ VCO) * 1000;

	/* Find the number of millisonds 
	   that got elapsed before this TCOUNT
	   cycle */
	milisec += timestamp * (MAX_TIM_LOAD/VCO) * 1000;

	return (milisec - base);
}
