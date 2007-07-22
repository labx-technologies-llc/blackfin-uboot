/*
 * memory-map.h - map of the memory space on this Blackfin
 *
 * Copyright (c) 2007 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef __ASM_BLACKFIN_ARCH_MEMORY_MAP_H__
#define __ASM_BLACKFIN_ARCH_MEMORY_MAP_H__

#ifndef __ASM_BLACKFIN_MEMORY_MAP_H__
# error "do not include this file directly, include asm/memory-map.h"
#endif

/* so we dont need to hardcode addresses into our .c files */
#define BFIN_MM_ASYNC    (bfin_memory_map[1].start)
#define BFIN_MM_L1_DATA  (bfin_memory_map[3].start)
#define BFIN_MM_L1_INST  (bfin_memory_map[5].start)

const struct memory_map const bfin_memory_map[] = {
	{	/* external memory */
		.start = 0x00000000,
		.end   = 0x20000000,
		.data  = true,
		.inst  = true,
		.data_flags = SDRAM_DGENERIC,
		.inst_flags = SDRAM_IGENERIC,
	},
	{	/* async banks */
		.start = 0x20000000,
		.end   = 0x30000000,
		.data  = true,
		.inst  = true,
		.data_flags = SDRAM_EBIU,
		.inst_flags = SDRAM_INON_CHBL,
	},
	{	/* boot rom */
		.start = 0xEF000000,
		.end   = 0xEF000800,
		.data  = true,
		.inst  = true,
		.data_flags = SDRAM_DGENERIC,
		.inst_flags = SDRAM_IGENERIC,
	},
	{	/* L2 SRAM */
		.start = 0xFEB00000,
		.end   = 0xFEB20000,
		.data  = true,
		.inst  = true,
		.data_flags = L1_DMEMORY,
		.inst_flags = L1_IMEMORY,
	},
	{	/* L1 Data Bank A SRAM */
		.start = 0xFF800000,
		.end   = 0xFF808000,
		.data  = true,
		.inst  = false,
		.data_flags = L1_DMEMORY,
		.inst_flags = L1_IMEMORY,
	},
	{	/* L1 Data Bank B SRAM */
		.start = 0xFF900000,
		.end   = 0xFF908000,
		.data  = true,
		.inst  = false,
		.data_flags = L1_DMEMORY,
		.inst_flags = L1_IMEMORY,
	},
	{	/* L1 Inst SRAM */
		.start = 0xFFA00000,
		.end   = 0xFFA08000,
		.data  = false,
		.inst  = true,
		.data_flags = L1_DMEMORY,
		.inst_flags = L1_IMEMORY,
	},
	{	/* L1 Inst SRAM/Cache */
		.start = 0xFFA10000,
		.end   = 0xFFA14000,
		.data  = false,
		.inst  = true,
		.data_flags = L1_DMEMORY,
		.inst_flags = L1_IMEMORY,
	},
	{	/* L1 Inst ROM */
		.start = 0xFFA14000,
		.end   = 0xFFA24000,
		.data  = false,
		.inst  = true,
		.data_flags = L1_DMEMORY,
		.inst_flags = L1_IMEMORY,
	},
#if 0
	/* these regions are automatically covered */
	{	/* L1 Scratchpad SRAM */
		.start = 0xFFB00000,
		.end   = 0xFFB01000,
		.data  = true,
		.inst  = false,
		.data_flags = L1_DMEMORY,
		.inst_flags = L1_IMEMORY,
	},
	{	/* System MMRs */
		.start = 0xFFC00000,
		.end   = 0xFFE00000,
		.data  = true,
		.inst  = false,
	},
	{	/* Core MMRs */
		.start = 0xFFE00000,
		.end   = 0xFFFFFFFF,
		.data  = true,
		.inst  = false,
	},
#endif
};

#endif
