/*
 * Blackfin MUSB HCD (Host Controller Driver) for u-boot
 *
 * Copyright (C) 2008 Bryan Wu <cooloney@kernel.org>
 * Copyright (c) 2008 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef __BLACKFIN_USB_H__
#define __BLACKFIN_USB_H__

/* Base address of Blackfin musb core */
#define MENTOR_USB_BASE		(USB_FADDR)

/* Blackfin uses DMA polling method of FIFO read/write */
#define USB_DMA_BASE		USB_DMA_INTERRUPT
#define USB_DMAx_CTRL		0x04
#define USB_DMAx_ADDR_LOW	0x08
#define USB_DMAx_ADDR_HIGH	0x0C
#define USB_DMAx_COUNT_LOW	0x10
#define USB_DMAx_COUNT_HIGH	0x14

#define USB_DMA_REG(ep, reg)	(USB_DMA_BASE + 0x20 * ep + reg)

#endif	/* __BLACKFIN_USB_H__ */
