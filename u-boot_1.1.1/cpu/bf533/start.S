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
 *
 * cpu/bf535/start.s:  A startup file for the Blackfin u-boot *
 * Copyright (c) 2003  Metrowerks/Motorola
 * Copyright (C) 1998  D. Jeff Dionne <jeff@ryeham.ee.ryerson.ca>,
 *                     Kenneth Albanowski <kjahds@kjahds.com>,
 *                     The Silver Hammer Group, Ltd.
 * (c) 1995, Dionne & Associates
 * (c) 1995, DKG Display Tech.
 */

#define ASSEMBLY

#include <linux/config.h>
#include <asm/blackfin.h>
#include <config.h>

.global _stext;
.global __bss_start;
.global start;
.global _start;
.global _rambase;
.global _ramstart;
.global _ramend;
.global _bf533_data_dest;
.global _bf533_data_size;
.global edata;
.global _initialize;
.global _exit;
.global flashdataend;

.text
_start:
start:
_stext:
        /* Check soft reset status */
        p0.h = SWRST >> 16;
        p0.l = SWRST & 0xFFFF;
        r0.l = w[p0];

        cc = bittst(r0, 15);
        if !cc jump no_soft_reset;

        /* Clear Soft reset */
        r0 = 0x0000;
        w[p0] = r0;
        ssync;

no_soft_reset:
	nop;
	r0 = 0x30;
	syscfg = r0;

	/* Clear EVT registers */
	r7 = 0;
	p0.h = (EVT_EMULATION_ADDR >> 16);
	p0.l = (EVT_EMULATION_ADDR & 0xFFFF);
	p0 += 8;
	p1 = 14;
	r1 = 0;
	LSETUP(4,4) lc0 = p1;
	[ p0 ++ ] = r1;

	/* Configure PLL */
	r0 = CONFIG_VCO;
	r1 = CONFIG_CRYSTAL_FREQ;
	r2 = 0;
	r3 = 1;
	r4 = -1;
dp_begin:
	r0 = r0 - r1;
	CC = r0 <= r4;
	if CC jump dp_skip;
	r2 = r2 + r3;
	jump dp_begin;
dp_skip:
	r0 = r2;
	r3 = r0;

	r1 = 0x3F;
	r0 = r0 & r1;
	r0 = r0 << 9;
	p0.h = (PLL_CTL >> 16);
        p0.l = (PLL_CTL & 0xFFFF);
	r1 = w[p0](z);
	r5 = 0x8E;
	r1 = r1 & r5;
	r1 = r1 | r0;
	cli r2;
	w[p0] = r1;
        idle;
        sti r2;
        ssync;

	/* Configure SCLK */
        r0 = PLL_DIV_FACTOR; 
	p0.h = (PLL_DIV >> 16);
	p0.l = (PLL_DIV & 0xFFFF);
	w[p0] = r0;
	ssync;

	p1.l = DELAY;
	p1.h = 0x0;
	lsetup ( del_start,del_end ) lc0 = p1;
del_start:
del_end: nop;

	/* Configure SDRAM if SDRAM is already
	   not enabled */
	p0.l = (EBIU_SDSTAT & 0xFFFF);
	p0.h = (EBIU_SDSTAT >> 16);
	r0.l = w[p0];
	cc = bittst(r0, 3);
	if !cc jump skip_sdram_enable;

        p0.l = (EBIU_SDRRC & 0xFFFF);
        p0.h = (EBIU_SDRRC >> 16);      /* SDRAM Refresh Rate Control Register */
        r0 = SDRRCVAL(Z);
        w[p0] = r0.l;
        ssync;

        p0.l = (EBIU_SDBCTL & 0xFFFF);
        p0.h = (EBIU_SDBCTL >> 16);     /* SDRAM Memory Bank Control Register */
        r0 = SDBCTL(Z);
        w[p0] = r0.l;
        ssync;


	/* SDRAM initialization */
	p0.l = (EBIU_SDGCTL & 0xFFFF);
	p0.h = (EBIU_SDGCTL >> 16);	/* SDRAM Memory Global Control Register */
	r0.h = (SDGCTL >> 16);
	r0.l = (SDGCTL & 0xFFFF);
	[p0] = r0;
	ssync;
	
skip_sdram_enable:
	nop;

	/*
	 * configure STACK
	 */
      
	sp.h = (STACKBASE >> 16);
	sp.l = (STACKBASE & 0xFFFF);

#if 0

	/*
	 * This next section keeps the processor in supervisor mode
	 * during kernel boot.  Switches to user mode at end of boot.
	 * See page 3-9 of Hardware Reference manual for documentation.
	 */

	/* To keep ourselves in the supervisor mode */
	p0.l = (EVT_IVG15_ADDR & 0xFFFF);
	p0.h = (EVT_IVG15_ADDR >> 16);
        
	p1.l = _real_start;
	p1.h = _real_start;
	[p0] = p1;
        
	p0.l = (IMASK & 0xFFFF);
	p0.h = (IMASK >> 16);
	r0 = IVG15_POS;
	[p0] = r0;
	raise 15;
	p0.l = WAIT_HERE;
	p0.h = WAIT_HERE;
	reti = p0;
	rti;

WAIT_HERE:
	jump WAIT_HERE;

.global _real_start;
_real_start:
	[ -- sp ] = reti;
#endif
	
#ifdef CONFIG_EZKIT533
	p0.l = (WDOG_CTL & 0xFFFF);
        p0.h = (WDOG_CTL >> 16);
        r0 = WATCHDOG_DISABLE(z);
        w[p0] = r0;
#endif

	/* Code for initializing Async mem banks */
	p2.h = (EBIU_AMBCTL1 >> 16);
	p2.l = (EBIU_AMBCTL1 & 0xFFFF);
	r0.h = (AMBCTL1VAL >> 16);
	r0.l = (AMBCTL1VAL & 0xFFFF);
	[p2] = r0;
	ssync;

	p2.h = (EBIU_AMBCTL0 >> 16);
	p2.l = (EBIU_AMBCTL0 & 0xFFFF);
	r0.h = (AMBCTL0VAL >> 16);
	r0.l = (AMBCTL0VAL & 0xFFFF);
	[p2] = r0;
	ssync;
	
	p2.h = (EBIU_AMGCTL >> 16);
	p2.l = (EBIU_AMGCTL & 0xffff);
	r0 = AMGCTLVAL;
	w[p2] = r0;
	ssync;

	/* DMA reset code to Hi of L1 SRAM */
copy:
	P1.H = hi(SYSMMR_BASE);		/* P1 Points to the beginning of SYSTEM MMR Space */
	P1.L = lo(SYSMMR_BASE);

	R0.H = reset_start;		/* Source Address (high) */
	R0.L = reset_start;		/* Source Address (low) */
	R1.H = reset_end;
	R1.L = reset_end;
	R2 = R1 - R0;			/* Count */
	R1.H = hi(L1_ISRAM);		/* Destination Address (high) */
	R1.L = lo(L1_ISRAM);		/* Destination Address (low) */
	R3.L = DMAEN;			/* Source DMAConfig Value (8-bit words) */
	R4.L = (DI_EN | WNR | DMAEN);	/* Destination DMAConfig Value (8-bit words) */

DMA:
	R6 = 0x1 (Z);
	W[P1+OFFSET_(MDMA_S0_X_MODIFY)] = R6;	/* Source Modify = 1 */
	W[P1+OFFSET_(MDMA_D0_X_MODIFY)] = R6;	/* Destination Modify = 1 */

	[P1+OFFSET_(MDMA_S0_START_ADDR)] = R0;	/* Set Source Base Address */
	W[P1+OFFSET_(MDMA_S0_X_COUNT)] = R2;	/* Set Source Count */
	/* Set Source      DMAConfig = DMA Enable, 
	Memory Read,  8-Bit Transfers, 1-D DMA, Flow - Stop */
	W[P1+OFFSET_(MDMA_S0_CONFIG)] = R3;	

	[P1+OFFSET_(MDMA_D0_START_ADDR)] = R1;	/* Set Destination Base Address */
	W[P1+OFFSET_(MDMA_D0_X_COUNT)] = R2;	/* Set Destination Count */
	/* Set Destination DMAConfig = DMA Enable, 
	Memory Write, 8-Bit Transfers, 1-D DMA, Flow - Stop, IOC */
	W[P1+OFFSET_(MDMA_D0_CONFIG)] = R4;	

	IDLE;					//Wait for DMA to Complete

	R0 = 0x1;
	W[P1+OFFSET_(MDMA_D0_IRQ_STATUS)] = R0;	//Write 1 to clear DMA interrupt

copy_end: nop;

        /* Initialize BSS Section with 0 s */
        p1.l = _sbss;
        p1.h = _sbss;
        p2.l = _ebss;
        p2.h = _ebss;
	r1 = p1;
	r2 = p2;
	r3 = r2 - r1;
	p3 = r3;
        lsetup (_clear_bss, _clear_bss_end ) lc1 = p3;
        CC = p2<=p1;
        if CC jump _clear_bss_skip;
        r0 = 0;
_clear_bss:
_clear_bss_end:
        [p1++] = r0;
_clear_bss_skip:

	/* copy data section to RAM */
	p1.l = flashdataend;
	p1.h = flashdataend;
	p2.l = _bf533_data_dest;
	p2.h = _bf533_data_dest;
	r0.l = _bf533_data_size;
	r0.h = _bf533_data_size;
loop:
	r1 = [p1];
	[p2] = r1;
	p3=0x4;
	p1=p1+p3;	
	p2=p2+p3;
	r2=0x4;	
	r0=r0-r2;
	cc=r0==0x0;
	if !cc jump loop;	
	r4.l = (CFG_MONITOR_BASE & 0xFFFF);
	r4.h = (CFG_MONITOR_BASE >> 16);
	r6 = _sram_inst_size;
	r6 = r4 + r6;
	p0 = r6;
	jump (p0);

reset_start:
        /* Soft Reset */
        p0.h = SWRST >> 16;
        p0.l = SWRST & 0xFFFF;
        r0 = 0x0007;
        w[p0] = r0;
        ssync;

        /* Core reset - forces reboot */
        RAISE 1 ;
reset_end: nop;

delay:
	rts;
	nop;
_exit:
	jump.s	_exit;
