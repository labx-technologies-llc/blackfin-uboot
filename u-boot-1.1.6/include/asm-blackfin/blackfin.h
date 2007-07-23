/*
 * U-boot - blackfin.h
 *
 * Copyright (c) 2005-2007 Analog Devices Inc.
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

#ifndef _BLACKFIN_H_
#define _BLACKFIN_H_

#define LO(con32) ((con32) & 0xFFFF)
#define lo(con32) ((con32) & 0xFFFF)
#define HI(con32) (((con32) >> 16) & 0xFFFF)
#define hi(con32) (((con32) >> 16) & 0xFFFF)

#if !defined(__ASSEMBLY__)
# ifdef SHARED_RESOURCES
#  include <asm/shared_resources.h>
# endif

# include <linux/types.h>

extern u_long get_sclk(void);

#define bfin_revid() (*pCHIPID >> 28)

#endif

#include <asm/arch/anomaly.h>

#ifndef __ASSEMBLY__

/* SSYNC implementation for C file */
static inline void SSYNC(void)
{
	int _tmp;
	if (ANOMALY_05000312 && ANOMALY_05000244)
		__asm__ __volatile__(
			"cli %0;"
			"nop;"
			"nop;"
			"ssync;"
			"sti %0;"
			: "=d" (_tmp)
		);
	else if (ANOMALY_05000312 && !ANOMALY_05000244)
		__asm__ __volatile__(
			"cli %0;"
			"ssync;"
			"sti %0;"
			: "=d" (_tmp)
		);
	else if (!ANOMALY_05000312 && ANOMALY_05000244)
		__asm__ __volatile__(
			"nop;"
			"nop;"
			"nop;"
			"ssync;"
		);
	else
		__asm__ __volatile__("ssync;");
}

/* CSYNC implementation for C file */
static inline void CSYNC(void)
{
	int _tmp;
	if (ANOMALY_05000312 && ANOMALY_05000244)
		__asm__ __volatile__(
			"cli %0;"
			"nop;"
			"nop;"
			"csync;"
			"sti %0;"
			: "=d" (_tmp)
		);
	else if (ANOMALY_05000312 && !ANOMALY_05000244)
		__asm__ __volatile__(
			"cli %0;"
			"csync;"
			"sti %0;"
			: "=d" (_tmp)
		);
	else if (!ANOMALY_05000312 && ANOMALY_05000244)
		__asm__ __volatile__(
			"nop;"
			"nop;"
			"nop;"
			"ssync;"
		);
	else
		__asm__ __volatile__("csync;");
}

#else  /* __ASSEMBLY__ */

/* SSYNC & CSYNC implementations for assembly files */

#define ssync(x) SSYNC(x)
#define csync(x) CSYNC(x)

#if ANOMALY_05000312 && ANOMALY_05000244
#define SSYNC(scratch) cli scratch; nop; nop; SSYNC; sti scratch;
#define CSYNC(scratch) cli scratch; nop; nop; CSYNC; sti scratch;

#elif ANOMALY_05000312 && !ANOMALY_05000244
#define SSYNC(scratch) cli scratch; nop; nop; SSYNC; sti scratch;
#define CSYNC(scratch) cli scratch; nop; nop; CSYNC; sti scratch;

#elif !ANOMALY_05000312 && ANOMALY_05000244
#define SSYNC(scratch) nop; nop; nop; SSYNC;
#define CSYNC(scratch) nop; nop; nop; CSYNC;

#elif !ANOMALY_05000312 && !ANOMALY_05000244
#define SSYNC(scratch) SSYNC;
#define CSYNC(scratch) CSYNC;

#endif /* ANOMALY_05000312 & ANOMALY_05000244 handling */

#endif /* __ASSEMBLY__ */

#include <asm/arch-common/cdefBF5xx.h>

#endif
