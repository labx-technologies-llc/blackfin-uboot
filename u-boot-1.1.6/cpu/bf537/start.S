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

#include <linux/config.h>
#include <config.h>
#include <asm/blackfin.h>

.global __bss_start;
.global _start;
.global _exit;
#if defined(CONFIG_POST)
.global _memory_post_test;
.global _post_flag;
#endif

#if (BFIN_BOOT_MODE == BFIN_BOOT_UART)
#if (CONFIG_CCLK_DIV == 1)
#define CONFIG_CCLK_ACT_DIV   CCLK_DIV1
#endif
#if (CONFIG_CCLK_DIV == 2)
#define CONFIG_CCLK_ACT_DIV   CCLK_DIV2
#endif
#if (CONFIG_CCLK_DIV == 4)
#define CONFIG_CCLK_ACT_DIV   CCLK_DIV4
#endif
#if (CONFIG_CCLK_DIV == 8)
#define CONFIG_CCLK_ACT_DIV   CCLK_DIV8
#endif
#ifndef CONFIG_CCLK_ACT_DIV
#define CONFIG_CCLK_ACT_DIV   CONFIG_CCLK_DIV_not_defined_properly
#endif
#endif

.text
_start:

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

	/* Clear EVT registers */
	p0.h = HI(EVT_EMULATION_ADDR);
	p0.l = LO(EVT_EMULATION_ADDR);
	p0 += 8;
	p1 = 14;
	r1 = 0;
	LSETUP(4,4) lc0 = p1;
	[ p0 ++ ] = r1;

#if (BFIN_BOOT_MODE != BFIN_BOOT_SPI_MASTER)
	p0.h = hi(SIC_IWR);
	p0.l = lo(SIC_IWR);
	r0 = 0x1;
	[p0] = r0;
	SSYNC;
#endif

#if (BFIN_BOOT_MODE == BFIN_BOOT_UART)
	/*
	* PLL_LOCKCNT - how many SCLK Cycles to delay while PLL becomes stable
	*/
	p0.h = hi(PLL_LOCKCNT);
	p0.l = lo(PLL_LOCKCNT);
	r0 = 0x300(Z);
	w[p0] = r0.l;
	ssync;

	/*
	* Put SDRAM in self-refresh, incase anything is running
	*/
	P2.H = hi(EBIU_SDGCTL);
	P2.L = lo(EBIU_SDGCTL);
	R0 = [P2];
	BITSET (R0, 24);
	[P2] = R0;
	SSYNC;

	/*
	*  Set PLL_CTL with the value that we calculate in R0
	*   - [14:09] = MSEL[5:0] : CLKIN / VCO multiplication factors
	*   - [8]     = BYPASS    : BYPASS the PLL, run CLKIN into CCLK/SCLK
	*   - [7]     = output delay (add 200ps of delay to mem signals)
	*   - [6]     = input delay (add 200ps of input delay to mem signals)
	*   - [5]     = PDWN      : 1=All Clocks off
	*   - [3]     = STOPCK    : 1=Core Clock off
	*   - [1]     = PLL_OFF   : 1=Disable Power to PLL
	*   - [0]     = DF	  : 1=Pass CLKIN/2 to PLL / 0=Pass CLKIN to PLL
	*   all other bits set to zero
	*/

	r0 = CONFIG_VCO_MULT & 63;      /* Load the VCO multiplier         */
	r0 = r0 << 9;                   /* Shift it over,                  */
	r1 = CONFIG_CLKIN_HALF;        /* Do we need to divide CLKIN by 2?*/
	r0 = r1 | r0;
	r1 = CONFIG_PLL_BYPASS;         /* Bypass the PLL?                 */
	r1 = r1 << 8;                   /* Shift it over                   */
	r0 = r1 | r0;                   /* add them all together           */

	p0.h = hi(PLL_CTL);
	p0.l = lo(PLL_CTL);             /* Load the address                */
	cli r2;                         /* Disable interrupts              */
		ssync;
	w[p0] = r0.l;                   /* Set the value                   */
	idle;                           /* Wait for the PLL to stablize    */
	sti r2;                         /* Enable interrupts               */

check_again:
	p0.h = hi(PLL_STAT);
	p0.l = lo(PLL_STAT);
	R0 = W[P0](Z);
	CC = BITTST(R0,5);
	if ! CC jump check_again;

	/* Configure SCLK & CCLK Dividers */
	r0 = (CONFIG_CCLK_ACT_DIV | CONFIG_SCLK_DIV);
	p0.h = hi(PLL_DIV);
	p0.l = lo(PLL_DIV);
	w[p0] = r0.l;
	ssync;
#endif

	/*
	 * We now are running at speed, time to set the Async mem bank wait states
	 * This will speed up execution, since we are normally running from FLASH.
	 * we need to read MAC address from FLASH
	 */
	p2.h = HI(EBIU_AMBCTL1);
	p2.l = LO(EBIU_AMBCTL1);
	r0.h = HI(AMBCTL1VAL);
	r0.l = LO(AMBCTL1VAL);
	[p2] = r0;
	ssync;

	p2.h = HI(EBIU_AMBCTL0);
	p2.l = LO(EBIU_AMBCTL0);
	r0.h = HI(AMBCTL0VAL);
	r0.l = LO(AMBCTL0VAL);
	[p2] = r0;
	ssync;

	p2.h = HI(EBIU_AMGCTL);
	p2.l = LO(EBIU_AMGCTL);
	r0 = AMGCTLVAL;
	w[p2] = r0;
	ssync;

#if (BFIN_BOOT_MODE == BFIN_BOOT_BYPASS)
	/* In bypass mode, we don't have an LDR with an init block
	 * so we need to explicitly call it ourselves.
	 */
	sp.l = LO(L1_SRAM_SCRATCH + L1_SRAM_SCRATCH_SIZE);
	sp.h = HI(L1_SRAM_SCRATCH + L1_SRAM_SCRATCH_SIZE);
	call _init_sdram;
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
	R1.H = hi(L1_ISRAM);    /* Destination Address (high) */
	R1.L = lo(L1_ISRAM);    /* Destination Address (low) */
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

	/* relocate into to RAM */
	call get_pc;
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
loop1:
	r1 = [p1 ++ p3];
	[p2 ++ p3] = r1;
	cc=p2==p4;
	if !cc jump loop1;
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
	p0.l = LO(EVT_IVG15_ADDR);
	p0.h = HI(EVT_IVG15_ADDR);

	p1.l = _real_start;
	p1.h = _real_start;
	[p0] = p1;

	p0.l = LO(IMASK);
	p0.h = HI(IMASK);
	r0.l = LO(IVG15_POS);
	r0.h = HI(IVG15_POS);
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

/* Initialise General-Purpose I/O Modules on BF537
 * Rev 0.0 Anomaly 05000212 - PORTx_FER,
 * PORT_MUX Registers Do Not accept "writes" correctly
 */
	p0.h = hi(PORTF_FER);
	p0.l = lo(PORTF_FER);
	R0.L = W[P0]; /* Read */
	nop;
	nop;
	nop;
	ssync;
	R0 = 0x000F(Z);
	W[P0] = R0.L; /* Write */
	nop;
	nop;
	nop;
	ssync;
	W[P0] = R0.L; /* Enable peripheral function of PORTF for UART0 and UART1 */
	nop;
	nop;
	nop;
	ssync;

	p0.h = hi(PORTH_FER);
	p0.l = lo(PORTH_FER);
	R0.L = W[P0]; /* Read */
	nop;
	nop;
	nop;
	ssync;
	R0 = 0xFFFF(Z);
	W[P0] = R0.L; /* Write */
	nop;
	nop;
	nop;
	ssync;
	W[P0] = R0.L; /* Enable peripheral function of PORTH for MAC */
	nop;
	nop;
	nop;
	ssync;

	/* DMA reset code to Hi of L1 SRAM */
copy:
	P1.H = hi(SYSMMR_BASE);	/* P1 Points to the beginning of SYSTEM MMR Space */
	P1.L = lo(SYSMMR_BASE);

	R0.H = reset_start;	/* Source Address (high) */
	R0.L = reset_start;	/* Source Address (low) */
	R1.H = reset_end;
	R1.L = reset_end;
	R2 = R1 - R0;		/* Count */
	R1.H = hi(L1_ISRAM);	/* Destination Address (high) */
	R1.L = lo(L1_ISRAM);	/* Destination Address (low) */
	R3.L = DMAEN;		/* Source DMAConfig Value (8-bit words) */
	R4.L = (DI_EN | WNR | DMAEN);	/* Destination DMAConfig Value (8-bit words) */

DMA:
	R6 = 0x1 (Z);
	W[P1+OFFSET_(MDMA_S0_X_MODIFY)] = R6;	/* Source Modify = 1 */
	W[P1+OFFSET_(MDMA_D0_X_MODIFY)] = R6;	/* Destination Modify = 1 */

	[P1+OFFSET_(MDMA_S0_START_ADDR)] = R0;	/* Set Source Base Address */
	W[P1+OFFSET_(MDMA_S0_X_COUNT)] = R2;	/* Set Source Count */
	/* Set Source  DMAConfig = DMA Enable,
	Memory Read,  8-Bit Transfers, 1-D DMA, Flow - Stop */
	W[P1+OFFSET_(MDMA_S0_CONFIG)] = R3;

	[P1+OFFSET_(MDMA_D0_START_ADDR)] = R1;	/* Set Destination Base Address */
	W[P1+OFFSET_(MDMA_D0_X_COUNT)] = R2;	/* Set Destination Count */
	/* Set Destination DMAConfig = DMA Enable,
	Memory Write, 8-Bit Transfers, 1-D DMA, Flow - Stop, IOC */
	W[P1+OFFSET_(MDMA_D0_CONFIG)] = R4;

WAIT_DMA_DONE:
	p0.h = hi(MDMA_D0_IRQ_STATUS);
	p0.l = lo(MDMA_D0_IRQ_STATUS);
	R0 = W[P0](Z);
	CC = BITTST(R0, 0);
	if ! CC jump WAIT_DMA_DONE

	R0 = 0x1;
	W[P1+OFFSET_(MDMA_D0_IRQ_STATUS)] = R0;	/* Write 1 to clear DMA interrupt */

	/* Initialize BSS Section with 0 s */
	p1.l = __bss_start;
	p1.h = __bss_start;
	p2.l = _end;
	p2.h = _end;
	r1 = p1;
	r2 = p2;
	r3 = r2 - r1;
	r3 = r3 >> 2;
	p3 = r3;
	lsetup (_clear_bss, _clear_bss_end ) lc1 = p3;
	CC = p2<=p1;
	if CC jump _clear_bss_skip;
	r0 = 0;
_clear_bss:
_clear_bss_end:
	[p1++] = r0;
_clear_bss_skip:

#if defined(CONFIG_POST)
	p0.l = _post_flag;
	p0.h = _post_flag;
	r0   = r7;
	[p0] = r0;
#endif

	p0.l = _board_init_f;
	p0.h = _board_init_f;
	jump (p0);

reset_start:
	p0.h = HI(WDOG_CNT);
	p0.l = LO(WDOG_CNT);
	r0 = 0x0010;
	w[p0] = r0;
	p0.h = HI(WDOG_CTL);
	p0.l = LO(WDOG_CTL);
	r0 = 0x0000;
	w[p0] = r0;
reset_wait:
	jump reset_wait;

reset_end:
	nop;

_exit:
	jump.s	_exit;
get_pc:
	r0 = rets;
	rts;
