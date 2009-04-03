/*
 * U-boot - interrupts.c Interrupt related routines
 *
 * Copyright (c) 2005-2008 Analog Devices Inc.
 *
 * This file is based on interrupts.c
 * Copyright 1996 Roman Zippel
 * Copyright 1999 D. Jeff Dionne <jeff@uclinux.org>
 * Copyright 2000-2001 Lineo, Inc. D. Jefff Dionne <jeff@lineo.ca>
 * Copyright 2002 Arcturus Networks Inc. MaTed <mated@sympatico.ca>
 * Copyright 2003 Metrowerks/Motorola
 * Copyright 2003 Bas Vermeulen <bas@buyways.nl>,
 *			BuyWays B.V. (www.buyways.nl)
 *
 * (C) Copyright 2000-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * Licensed under the GPL-2 or later.
 */

#include <common.h>
#include <config.h>
#include <asm/blackfin.h>
#include "cpu.h"

static ulong timestamp;
static ulong last_time;
static int int_flag;

int irq_flags;			/* needed by asm-blackfin/system.h */

/* Functions just to satisfy the linker */

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On Blackfin it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return get_timer(0);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On Blackfin it returns the number of timer ticks per second.
 */
ulong get_tbclk(void)
{
	ulong tbclk;

	tbclk = CONFIG_SYS_HZ;
	return tbclk;
}

void enable_interrupts(void)
{
	local_irq_restore(int_flag);
}

int disable_interrupts(void)
{
	local_irq_save(int_flag);
	return 1;
}

void udelay(unsigned long usec)
{
	unsigned long delay, start, stop;
	unsigned long cclk;
	cclk = (CONFIG_CCLK_HZ);

	while (usec > 1) {
		/*
		 * how many clock ticks to delay?
		 *  - request(in useconds) * clock_ticks(Hz) / useconds/second
		 */
		if (usec < 1000) {
			delay = (usec * (cclk / 244)) >> 12;
			usec = 0;
		} else {
			delay = (1000 * (cclk / 244)) >> 12;
			usec -= 1000;
		}

		asm volatile (" %0 = CYCLES;" : "=r" (start));
		do {
			asm volatile (" %0 = CYCLES; " : "=r" (stop));
		} while (stop - start < delay);
	}

	return;
}

#define MAX_TIM_LOAD	0xFFFFFFFF
#define CLOCKS_PER_TICK	(CONFIG_CCLK_HZ / CONFIG_SYS_HZ)
#define TICKS_PER_LOAD	(MAX_TIM_LOAD / CLOCKS_PER_TICK)
int timer_init(void)
{
	*pTCNTL = 0x1;
	*pTSCALE = 0x0;
	*pTCOUNT = MAX_TIM_LOAD;
	*pTPERIOD = MAX_TIM_LOAD;
	*pTCNTL = 0x7;
	asm("CSYNC;");

	timestamp = 0;
	last_time = 0;

	return 0;
}

/*
 * Return the ticks since the last reset of the timer.
 * We may lose ticks, it is safe for timeout usage.
 */
ulong get_timer(ulong base)
{
	ulong milisec;

	/* Number of clocks elapsed */
	ulong clocks = (MAX_TIM_LOAD - (*pTCOUNT));

	/*
	 * Find if the TCOUNT is reset
	 * timestamp gives the number of times
	 * TCOUNT got reset
	 */
	if (clocks < last_time)
		timestamp++;
	last_time = clocks;

	/* Calculate the elapsed ticks */
	return clocks / CLOCKS_PER_TICK
			+ TICKS_PER_LOAD * timestamp - base;
}

void reset_timer(void)
{
	timer_init();
}
