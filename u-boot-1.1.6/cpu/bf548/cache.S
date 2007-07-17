/* cache.S - low level cache handling routines
 * Copyright (C) 2003-2007 Analog Devices Inc.
 * Licensed under the GPL-2 or later.
 */

#include <asm/linkage.h>
#include <config.h>
#include <asm/blackfin.h>

.text
.align 2
ENTRY(_blackfin_icache_flush_range)
	R2 = -32;
	R2 = R0 & R2;
	P0 = R2;
	P1 = R1;
	CSYNC;
1:
	IFLUSH[P0++];
	CC = P0 < P1(iu);
	IF CC JUMP 1b(bp);
	IFLUSH[P0];
	SSYNC;
	RTS;
ENDPROC(_blackfin_icache_flush_range)

ENTRY(_blackfin_dcache_flush_range)
	R2 = -32;
	R2 = R0 & R2;
	P0 = R2;
	P1 = R1;
	CSYNC;
1:
	FLUSH[P0++];
	CC = P0 < P1(iu);
	IF CC JUMP 1b(bp);
	FLUSH[P0];
	SSYNC;
	RTS;
ENDPROC(_blackfin_dcache_flush_range)

ENTRY(_icache_invalidate)
ENTRY(_invalidate_entire_icache)
	[--SP] = (R7:5);

	P0.L = LO(IMEM_CONTROL);
	P0.H = HI(IMEM_CONTROL);
	R7 =[P0];

	/*
	 * Clear the IMC bit , All valid bits in the instruction
	 * cache are set to the invalid state
	 */
	BITCLR(R7, IMC_P);
	CLI R6;
	/* SSYNC required before invalidating cache. */
	SSYNC;
	.align 8;
	[P0] = R7;
	SSYNC;
	STI R6;

	/* Configures the instruction cache agian */
	R6 = (IMC | ENICPLB);
	R7 = R7 | R6;

	CLI R6;
	SSYNC;
	.align 8;
	[P0] = R7;
	SSYNC;
	STI R6;

	(R7:5) =[SP++];
	RTS;
ENDPROC(_invalidate_entire_icache)
ENDPROC(_icache_invalidate)

/*
 * Invalidate the Entire Data cache by
 * clearing DMC[1:0] bits
 */
ENTRY(_invalidate_entire_dcache)
ENTRY(_dcache_invalidate)
	[--SP] = (R7:6);

	P0.L = LO(DMEM_CONTROL);
	P0.H = HI(DMEM_CONTROL);
	R7 =[P0];

	/*
	 * Clear the DMC[1:0] bits, All valid bits in the data
	 * cache are set to the invalid state
	 */
	BITCLR(R7, DMC0_P);
	BITCLR(R7, DMC1_P);
	CLI R6;
	SSYNC;
	.align 8;
	[P0] = R7;
	SSYNC;
	STI R6;
	/* Configures the data cache again */

	R6 = (ACACHE_BCACHE | ENDCPLB | PORT_PREF0);
	R7 = R7 | R6;

	CLI R6;
	SSYNC;
	.align 8;
	[P0] = R7;
	SSYNC;
	STI R6;

	(R7:6) =[SP++];
	RTS;
ENDPROC(_dcache_invalidate)
ENDPROC(_invalidate_entire_dcache)

ENTRY(_blackfin_dcache_invalidate_range)
	R2 = -32;
	R2 = R0 & R2;
	P0 = R2;
	P1 = R1;
	CSYNC;
1:
	FLUSHINV[P0++];
	CC = P0 < P1(iu);
	IF CC JUMP 1b(bp);

	/*
	 * If the data crosses a cache line, then we'll be pointing to
	 * the last cache line, but won't have flushed/invalidated it yet, so do
	 * one more.
	 */
	FLUSHINV[P0];
	SSYNC;
	RTS;
ENDPROC(_blackfin_dcache_invalidate_range)
