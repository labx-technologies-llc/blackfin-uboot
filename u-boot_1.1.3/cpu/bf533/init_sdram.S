#define ASSEMBLY

#include <linux/config.h>
#include <config.h>
#include <asm/blackfin.h>
#include <asm/mem_init.h>
.global init_sdram;

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

init_sdram:
	[--SP] = ASTAT;
	[--SP] = RETS;
	[--SP] = (R7:0);
	[--SP] = (P5:0);
	
		/*
	 *  Set PLL_CTL
	 *   - [14:09] = MSEL[5:0] : CLKIN / VCO multiplication factors
	 *   - [8]     = BYPASS    : BYPASS the PLL, run CLKIN into CCLK/SCLK
	 *   - [7]     = output delay (add 200ps of delay to mem signals)
	 *   - [6]     = input delay (add 200ps of input delay to mem signals)
	 *   - [5]     = PDWN      : 1=All Clocks off
	 *   - [3]     = STOPCK    : 1=Core Clock off
	 *   - [1]     = PLL_OFF   : 1=Disable Power to PLL
	 *   - [0]     = DF        : 1=Pass CLKIN/2 to PLL / 0=Pass CLKIN to PLL
	 *   all other bits set to zero
	 */

	r0 = CONFIG_VCO_MULT;	/* Load the VCO multiplier */
	r0 = r0 << 9;		/* Shift it over */
	r1 =  CONFIG_CLKIN_HALF;	/* Do we need to divide CLKIN by 2? */
	r0 = r1 | r0;
	r1 = CONFIG_PLL_BYPASS;	/* Bypass the PLL?                 */
	r1 = r1 << 8;	/* Shift it over */
	r0 = r1 | r0;	/* add them all together */

	p0.h = (PLL_CTL >> 16);
	p0.l = (PLL_CTL & 0xFFFF);	/* Load the address */
	cli r2;				/* Disable interrupts */
	w[p0] = r0;			/* Set the value */
	idle;				/* Wait for the PLL to stablize */
	sti r2;				/* Enable interrupts */
	ssync;

	/*
	 * Turn on the CYCLES COUNTER
	 */
	r2 = SYSCFG;
	BITSET (r2,1);
	SYSCFG = r2;

	/* Configure SCLK & CCLK Dividers */
	r0 = CONFIG_CCLK_ACT_DIV | CONFIG_SCLK_DIV;
	p0.h = (PLL_DIV >> 16);
	p0.l = (PLL_DIV & 0xFFFF);
	w[p0] = r0;
	ssync;

wait_for_pll_stab:
	p0.h = (PLL_STAT >> 16);
	p0.l = (PLL_STAT & 0xFFFF);
	r0.l = w[p0];
	cc = bittst(r0,5);
	if !cc jump wait_for_pll_stab;

	/* Configure SDRAM if SDRAM is already not enabled */
	p0.l = (EBIU_SDSTAT & 0xFFFF);
	p0.h = (EBIU_SDSTAT >> 16);
	r0.l = w[p0];
	cc = bittst(r0, 3);
	if !cc jump skip_sdram_enable;

	/* SDRAM initialization */
	p0.l = (EBIU_SDGCTL & 0xFFFF);
	p0.h = (EBIU_SDGCTL >> 16);	/* SDRAM Memory Global Control Register */
	r0.h = (mem_SDGCTL >> 16);
	r0.l = (mem_SDGCTL & 0xFFFF);
	[p0] = r0;
	ssync;

	p0.l = (EBIU_SDBCTL & 0xFFFF);
	p0.h = (EBIU_SDBCTL >> 16);	/* SDRAM Memory Bank Control Register */
	r0 = mem_SDBCTL;
	w[p0] = r0.l;
	ssync;

	p0.l = (EBIU_SDRRC & 0xFFFF);
	p0.h = (EBIU_SDRRC >> 16);	/* SDRAM Refresh Rate Control Register */
	r0 = mem_SDRRC;
	w[p0] = r0.l;
	ssync;
	
	skip_sdram_enable:
	nop;
	
	(P5:0) = [SP++];
	(R7:0) = [SP++];
	RETS   = [SP++];
	ASTAT  = [SP++];
	RTS;
	
