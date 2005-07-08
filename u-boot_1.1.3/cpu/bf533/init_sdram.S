#define ASSEMBLY

#include <linux/config.h>
#include <config.h>
#include <asm/blackfin.h>
#include <asm/mem_init.h>
.global init_sdram;

init_sdram:
	[--SP] = ASTAT;
	[--SP] = RETS;
	[--SP] = (R7:0);
	[--SP] = (P5:0);

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
	
	(P5:0) = [SP++];
	(R7:0) = [SP++];
	RETS   = [SP++];
	ASTAT  = [SP++];
	RTS;
	
