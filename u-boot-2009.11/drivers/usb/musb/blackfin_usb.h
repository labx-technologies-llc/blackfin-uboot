/*
 * Blackfin MUSB HCD (Host Controller Driver) for u-boot
 *
 * Copyright (c) 2008-2009 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef __BLACKFIN_USB_H__
#define __BLACKFIN_USB_H__

#include "musb_core.h"

/* Every register is 32bit aligned, but only 16bits in size */
#define ureg(name) u16 name; u16 __pad_##name;

struct bfin_musb_dma_regs {
	ureg(interrupt);
	ureg(control);
	ureg(addr_low);
	ureg(addr_high);
	ureg(count_low);
	ureg(count_high);
	ureg(pad);
};

#undef ureg

#endif
