/*
 * memory-map.h - map of the memory space on this Blackfin
 *
 * Copyright (c) 2007 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef __ASM_BLACKFIN_MEMORY_MAP_H__
#define __ASM_BLACKFIN_MEMORY_MAP_H__

#include <asm/posix_types.h>

struct memory_map {
	uint32_t start, end;
	bool data, inst;
	uint32_t data_flags, inst_flags;
};

#include <asm/cplb.h>
#include <asm/arch/memory-map.h>

#endif
