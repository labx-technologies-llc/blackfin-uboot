/*
 * dma.h - Blackfin DMA defines/structures/etc...
 *
 * Copyright 2004-2008 Analog Devices Inc.
 * Licensed under the GPL-2 or later.
 */

#ifndef _BLACKFIN_DMA_H_
#define _BLACKFIN_DMA_H_

#include <linux/types.h>
#ifdef __ADSPBF60x__
#include <asm/mach-common/bits/dde.h>
#else
#include <asm/mach-common/bits/dma.h>
#endif

struct dmasg_large {
	void *next_desc_addr;
	u32 start_addr;
	u16 cfg;
	u16 x_count;
	s16 x_modify;
	u16 y_count;
	s16 y_modify;
} __attribute__((packed));

struct dmasg {
	u32 start_addr;
	u16 cfg;
	u16 x_count;
	s16 x_modify;
	u16 y_count;
	s16 y_modify;
} __attribute__((packed));

/*
 * All Blackfin system MMRs are padded to 32bits even if the register
 * itself is only 16bits.  So use a helper macro to streamline this.
 */
#define __BFP(m) m; u16 __pad_##m
struct dma_register {
#ifdef __ADSPBF60x__
	void *next_desc_ptr;
	u32 start_addr;
	u32 config;
	u32 x_count;
	s32 x_modify;
	u32 y_count;
	s32 y_modify;
	u32 __pad0[2];
	void *curr_desc_ptr;
	void *prev_desc_ptr;
	void *curr_addr;
	u32 status;
	u32 curr_x_count;
	u32 curr_y_count;
	u32 __pad1[2];
	u32 bw_limit;
	u32 curr_bw_limit;
	u32 bw_monitor;
	u32 curr_bw_monitor;
#else
	void *next_desc_ptr;
	u32 start_addr;
	u16 __BFP(config);
	u32 __pad0;
	u16 __BFP(x_count);
	s16 __BFP(x_modify);
	u16 __BFP(y_count);
	s16 __BFP(y_modify);
	void *curr_desc_ptr;
	u32 curr_addr_ptr;
	u16 __BFP(status);
	u16 __BFP(peripheral_map);
	u16 __BFP(curr_x_count);
	u32 __pad1;
	u16 __BFP(curr_y_count);
	u32 __pad2;
#endif
};
#undef __BFP

#endif
