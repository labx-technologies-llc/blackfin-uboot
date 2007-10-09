/*
 * U-boot - start.S Startup file for Blackfin u-boot
 *
 * Copyright (c) 2005-2007 Analog Devices Inc.
 *
 * This file is based on head.S
 * Copyright (c) 2003  Metrowerks/Motorola
 * Copyright (C) 1998  D. Jeff Dionne <jeff@ryeham.ee.ryerson.ca>,
 *                     Kenneth Albanowski <kjahds@kjahds.com>,
 *                     The Silver Hammer Group, Ltd.
 * (c) 1995, Dionne & Associates
 * (c) 1995, DKG Display Tech.
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

/*
 * Note: A change in this file subsequently requires a change in
 *       board/$(board_name)/config.mk for a valid u-boot.bin
 */

#include <config.h>
#include <asm/blackfin.h>
#include <asm/mach-common/bits/core.h>
#include <asm/mach-common/bits/dma.h>
#include <asm/mach-common/bits/pll.h>

#if defined(CONFIG_POST)
.global _memory_post_test;
.global _post_flag;
#endif

.text
ENTRY(_start)
	R0 = 0x32;
	SYSCFG = R0;
	SSYNC;

	/* As per HW reference manual DAG registers,
	 * DATA and Address resgister shall be zero'd
	 * in initialization, after a reset state
	 */
	r1 = 0;	/* Data registers zero'd */
	r2 = 0;
	r3 = 0;
	r4 = 0;
	r5 = 0;
	r6 = 0;
	r7 = 0;

	p0 = 0; /* Address registers zero'd */
	p1 = 0;
	p2 = 0;
	p3 = 0;
	p4 = 0;
	p5 = 0;

	i0 = 0; /* DAG Registers zero'd */
	i1 = 0;
	i2 = 0;
	i3 = 0;
	m0 = 0;
	m1 = 0;
	m3 = 0;
	m3 = 0;
	l0 = 0;
	l1 = 0;
	l2 = 0;
	l3 = 0;
	b0 = 0;
	b1 = 0;
	b2 = 0;
	b3 = 0;

	/* Set loop counters to zero, to make sure that
	 * hw loops are disabled.
	 */
	r0  = 0;
	lc0 = r0;
	lc1 = r0;

	SSYNC;

	/* Make sure we clear soft reset ... don't bother
	 * checking the value as reads are destructive here
	 * and double clearing the system reset field is OK
	 */
	p0.h = hi(SWRST);
	p0.l = lo(SWRST);
	r0 = 0x0000;
	w[p0] = r0;
	SSYNC;

	/* Clear EVT registers -- no point in clearing out
	 * EVT0 (EMU) or EVT1 (RESET), so skip them.
	 */
	p0.h = HI(EVT2);
	p0.l = LO(EVT2);
	p1 = (((EVT15 - EVT2) / 4) + 1);
	r1 = 0;
	LSETUP(4, 4) lc0 = p1;
	[p0++] = r1;

	sp.l = LO(L1_SRAM_SCRATCH + L1_SRAM_SCRATCH_SIZE);
	sp.h = HI(L1_SRAM_SCRATCH + L1_SRAM_SCRATCH_SIZE);

#if (BFIN_BOOT_MODE == BFIN_BOOT_BYPASS)
	/* In bypass mode, we don't have an LDR with an init block
	 * so we need to explicitly call it ourselves.  This will
	 * reprogram our clocks and setup our async banks.
	 */
	/* XXX: we should DMA this into L1, put external memory into
	 *      self refresh, and then jump there ...
	 */
	call _get_pc;
	r3 = 0x0 (x);
	r3.h = 0x2000;
	cc = r0 < r3 (iu);
	if cc jump .Lproc_initialized;

	call _initcode;

.Lproc_initialized:
#endif

#if defined(CONFIG_POST)
	/* DMA POST code to Hi of L1 SRAM */
postcopy:
	/* P1 Points to the beginning of SYSTEM MMR Space */
	P1.H = hi(SYSMMR_BASE);
	P1.L = lo(SYSMMR_BASE);

	R0.H = _text_l1;
	R0.L = _text_l1;
	R1.H = _etext_l1;
	R1.L = _etext_l1;
	R2 = R1 - R0;           /* Count */
	R0.H = _etext;
	R0.L = _etext;
	R1.H = HI(CFG_MONITOR_BASE);
	R1.L = LO(CFG_MONITOR_BASE);
	R0 = R0 - R1;
	R1.H = HI(CFG_FLASH_BASE);
	R1.L = LO(CFG_FLASH_BASE);
	R0 = R0 + R1;		/* Source Address */
	R1.H = hi(L1_INST_SRAM);    /* Destination Address (high) */
	R1.L = lo(L1_INST_SRAM);    /* Destination Address (low) */
	R3.L = DMAEN;           /* Source DMAConfig Value (8-bit words) */
	/* Destination DMAConfig Value (8-bit words) */
	R4.L = (DI_EN | WNR | DMAEN);

	R6 = 0x1 (Z);
	W[P1+OFFSET_(MDMA_S0_X_MODIFY)] = R6;   /* Source Modify = 1 */
	W[P1+OFFSET_(MDMA_D0_X_MODIFY)] = R6;   /* Destination Modify = 1 */

	[P1+OFFSET_(MDMA_S0_START_ADDR)] = R0;  /* Set Source Base Address */
	W[P1+OFFSET_(MDMA_S0_X_COUNT)] = R2;    /* Set Source Count */
	/* Set Source  DMAConfig = DMA Enable,
	Memory Read,  8-Bit Transfers, 1-D DMA, Flow - Stop */
	W[P1+OFFSET_(MDMA_S0_CONFIG)] = R3;

	[P1+OFFSET_(MDMA_D0_START_ADDR)] = R1;  /* Set Destination Base Address */
	W[P1+OFFSET_(MDMA_D0_X_COUNT)] = R2;    /* Set Destination Count */
	/* Set Destination DMAConfig = DMA Enable,
	Memory Write, 8-Bit Transfers, 1-D DMA, Flow - Stop, IOC */
	W[P1+OFFSET_(MDMA_D0_CONFIG)] = R4;

POST_DMA_DONE:
	p0.h = hi(MDMA_D0_IRQ_STATUS);
	p0.l = lo(MDMA_D0_IRQ_STATUS);
	R0 = W[P0](Z);
	CC = BITTST(R0, 0);
	if ! CC jump POST_DMA_DONE

	R0 = 0x1;
	W[P1+OFFSET_(MDMA_D0_IRQ_STATUS)] = R0; /* Write 1 to clear DMA interrupt */

	/* DMA POST data to Hi of L1 SRAM */
	R0.H = _rodata_l1;
	R0.L = _rodata_l1;
	R1.H = _erodata_l1;
	R1.L = _erodata_l1;
	R2 = R1 - R0;           /* Count */
	R0.H = _erodata;
	R0.L = _erodata;
	R1.H = HI(CFG_MONITOR_BASE);
	R1.L = LO(CFG_MONITOR_BASE);
	R0 = R0 - R1;
	R1.H = HI(CFG_FLASH_BASE);
	R1.L = LO(CFG_FLASH_BASE);
	R0 = R0 + R1;           /* Source Address */
	R1.H = hi(DATA_BANKB_SRAM);    /* Destination Address (high) */
	R1.L = lo(DATA_BANKB_SRAM);    /* Destination Address (low) */
	R3.L = DMAEN;           /* Source DMAConfig Value (8-bit words) */
	R4.L = (DI_EN | WNR | DMAEN);   /* Destination DMAConfig Value (8-bit words) */

	R6 = 0x1 (Z);
	W[P1+OFFSET_(MDMA_S0_X_MODIFY)] = R6;   /* Source Modify = 1 */
	W[P1+OFFSET_(MDMA_D0_X_MODIFY)] = R6;   /* Destination Modify = 1 */

	[P1+OFFSET_(MDMA_S0_START_ADDR)] = R0;  /* Set Source Base Address */
	W[P1+OFFSET_(MDMA_S0_X_COUNT)] = R2;    /* Set Source Count */
	/* Set Source  DMAConfig = DMA Enable,
	Memory Read,  8-Bit Transfers, 1-D DMA, Flow - Stop */
	W[P1+OFFSET_(MDMA_S0_CONFIG)] = R3;

	[P1+OFFSET_(MDMA_D0_START_ADDR)] = R1;  /* Set Destination Base Address */
	W[P1+OFFSET_(MDMA_D0_X_COUNT)] = R2;    /* Set Destination Count */
	/* Set Destination DMAConfig = DMA Enable,
	Memory Write, 8-Bit Transfers, 1-D DMA, Flow - Stop, IOC */
	W[P1+OFFSET_(MDMA_D0_CONFIG)] = R4;

POST_DATA_DMA_DONE:
	p0.h = hi(MDMA_D0_IRQ_STATUS);
	p0.l = lo(MDMA_D0_IRQ_STATUS);
	R0 = W[P0](Z);
	CC = BITTST(R0, 0);
	if ! CC jump POST_DATA_DMA_DONE

	R0 = 0x1;
	W[P1+OFFSET_(MDMA_D0_IRQ_STATUS)] = R0; /* Write 1 to clear DMA interrupt */

	p0.l = _memory_post_test;
	p0.h = _memory_post_test;
	r0 = 0x0;
	call (p0);
	r7 = r0;				/* save return value */

	call _init_sdram;
#endif

	/* relocate from wherever are (FLASH/RAM/etc...) to the
	 * hardcoded monitor location in the end of RAM
	 */
	call _get_pc;
.Loffset:
	r2.l = .Loffset;
	r2.h = .Loffset;
	r3.l = _start;
	r3.h = _start;
	r1 = r2 - r3;

	r0 = r0 - r1;
	p1 = r0;

	p2.l = LO(CFG_MONITOR_BASE);
	p2.h = HI(CFG_MONITOR_BASE);

	p3 = 0x04;
	p4.l = LO(CFG_MONITOR_BASE + CFG_MONITOR_LEN);
	p4.h = HI(CFG_MONITOR_BASE + CFG_MONITOR_LEN);
.Lloop1:
	r1 = [p1 ++ p3];
	[p2 ++ p3] = r1;
	cc=p2==p4;
	if !cc jump .Lloop1;
	/*
	 * configure STACK
	 */
	r0.h = HI(CONFIG_STACKBASE);
	r0.l = LO(CONFIG_STACKBASE);
	sp = r0;
	fp = sp;

	/*
	 * This next section keeps the processor in supervisor mode
	 * during kernel boot.  Switches to user mode at end of boot.
	 * See page 3-9 of Hardware Reference manual for documentation.
	 */

	/* To keep ourselves in the supervisor mode */
	p0.l = LO(EVT15);
	p0.h = HI(EVT15);

	p1.l = _real_start;
	p1.h = _real_start;
	[p0] = p1;

	p0.l = LO(IMASK);
	p0.h = HI(IMASK);
	r0.l = LO(EVT_IVG15);
	r0.h = HI(EVT_IVG15);
	[p0] = r0;
	raise 15;
	p0.l = .LWAIT_HERE;
	p0.h = .LWAIT_HERE;
	reti = p0;
	rti;

.LWAIT_HERE:
	jump .LWAIT_HERE;
ENDPROC(_start)

ENTRY(_real_start)
	[--SP] = reti;

#ifdef __ADSPBF54x__
	/* XXX: this needs to get moved to early-board init code */

	/* Port H: PH8 - PH13 == A4 - A9
	 * address lines of the parallel asynchronous memory interface
	 */

	/************************************************
	* configure GPIO 				*
	* set port H function enable register		*
	*  configure PH8-PH13 as peripheral (not GPIO) 	*
	*************************************************/
	p0.l = lo(PORTH_FER);
	p0.h = hi(PORTH_FER);
	r0.l = 0x3F03;
	w[p0] = r0;

	/************************************************
	* set port H MUX to configure PH8-PH13		*
	*  1st Function (MUX = 00) (bits 16-27 == 0)	*
	*  Set to address signals A4-A9 		*
	*************************************************/
	p0.l = lo(PORTH_MUX);
	p0.h = hi(PORTH_MUX);
	r0.l = 0x0000;
	r0.h = 0x0000;
	[p0] = r0;

	/************************************************
	* set port H direction register			*
	*  enable PH8-PH13 as outputs			*
	*************************************************/
	p0.l = lo(PORTH_DIR_SET);
	p0.h = hi(PORTH_DIR_SET);
	r0.l = 0x3F00;
	w[p0] = r0;

	/* Port I: PI0 - PH14 == A10 - A24
	 * address lines of the parallel asynchronous memory interface
	 */

	/************************************************
	* set port I function enable register		*
	*  configure PI0-PI14 as peripheral (not GPIO) 	*
	*************************************************/
	p0.l = lo(PORTI_FER);
	p0.h = hi(PORTI_FER);
	r0.l = 0x7fff;
	w[p0] = r0;

	/**************************************************
	* set PORT I MUX to configure PI14-PI0 as	  *
	* 1st Function (MUX=00) - address signals A10-A24 *
	***************************************************/
	p0.l = lo(PORTI_MUX);
	p0.h = hi(PORTI_MUX);
	r0.l = 0x0000;
	r0.h = 0x0000;
	w[p0] = r0;

	/****************************************
	* set PORT I direction register		*
	*  enable PI0 - PI14 as outputs		*
	*****************************************/
	p0.l = lo(PORTI_DIR_SET);
	p0.h = hi(PORTI_DIR_SET);
	r0.l = 0x7fff;
	w[p0] = r0;
#endif

	/* Initialize BSS section ... we know that memset()
	 * does not use the BSS, so it is safe to call here
	 */
	r0.l = __bss_start;
	r0.h = __bss_start;
	r1 = 0 (x);
	r2.l = _end;
	r2.h = _end;
	r2 = r2 - r0;
	call _memset;

#if defined(CONFIG_POST)
	p0.l = _post_flag;
	p0.h = _post_flag;
	r0   = r7;
	[p0] = r0;
#endif

	p0.l = _board_init_f;
	p0.h = _board_init_f;
	jump (p0);
ENDPROC(_real_start)

ENTRY(_get_pc)
	r0 = rets;
	rts;
ENDPROC(_get_pc)
