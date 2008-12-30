/*
 * Blackfin MUSB HCD (Host Controller Driver) for u-boot
 *
 * Copyright (C) 2008 Bryan Wu <cooloney@kernel.org>
 * Copyright (c) 2008 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <common.h>

#ifdef CONFIG_USB_BLACKFIN

#include <usb.h>

#include <asm/blackfin.h>
#include <asm/mach-common/bits/usb.h>

#include "blackfin_usb.h"

/* This function writes to a 16-bit register of platform musb core */
inline void musb_writew(u32 offset, u16 value)
{
	bfin_write16(MENTOR_USB_BASE + offset, value);
}

/* This function writes to a 8-bit register of platform musb core */
inline void musb_writeb(u32 offset, u8 value)
{
	bfin_write16(MENTOR_USB_BASE + offset, (u16) value);
}

/* This function reads a 16-bit register of platform usb wrapper */
inline u16 musb_readw(u32 offset)
{
	return bfin_read16(MENTOR_USB_BASE + offset);
}

/* This function reads a 8-bit register of platform usb wrapper */
inline u8 musb_readb(u32 offset)
{
	return (u8) (bfin_read16(MENTOR_USB_BASE + offset));
}

/*
 * This function read or write data to endpoint fifo
 * Blackfin use DMA polling method to avoid buffer alignment issues
 *
 * ep		- Endpoint number
 * length	- Number of bytes to write to FIFO
 * fifo_data	- Pointer to data buffer to be read/write
 * is_write	- Flag for read or write
 */
void rw_fifo(u8 ep, u32 length, void *fifo_data, int is_write)
{
	u16 dma_reg = 0;

	blackfin_dcache_flush_invalidate_range((unsigned int)fifo_data,
		(unsigned int)(fifo_data + length));

	/* Setup DMA address register */
	dma_reg = (u16) ((u32) fifo_data & 0xFFFF);
	bfin_write16(USB_DMA_REG(ep, USB_DMAx_ADDR_LOW), dma_reg);
	SSYNC();

	dma_reg = (u16) (((u32) fifo_data >> 16) & 0xFFFF);
	bfin_write16(USB_DMA_REG(ep, USB_DMAx_ADDR_HIGH), dma_reg);
	SSYNC();

	/* Setup DMA count register */
	bfin_write16(USB_DMA_REG(ep, USB_DMAx_COUNT_LOW), length);
	bfin_write16(USB_DMA_REG(ep, USB_DMAx_COUNT_HIGH), 0);
	SSYNC();

	/* Enable the DMA */
	dma_reg = (ep << 4) | DMA_ENA | INT_ENA;
	if (is_write)
		dma_reg |= DIRECTION;
	bfin_write16(USB_DMA_REG(ep, USB_DMAx_CTRL), dma_reg);
	SSYNC();

	/* Wait for compelete */
	while (!(bfin_read_USB_DMA_INTERRUPT() & (1 << ep)));

	/* acknowledge dma interrupt */
	bfin_write_USB_DMA_INTERRUPT(1 << ep);
	SSYNC();

	/* Reset DMA */
	bfin_write16(USB_DMA_REG(ep, USB_DMAx_CTRL), 0);
	SSYNC();
}

void write_fifo(u8 ep, u32 length, void *fifo_data)
{
	rw_fifo(ep, length, fifo_data, 1);
}

void read_fifo(u8 ep, u32 length, void *fifo_data)
{
	rw_fifo(ep, length, fifo_data, 0);
}

int musb_platform_init(void)
{

#if defined(__ADSPBF54x__)
	/*
	 * Rev 1.0 BF549 EZ-KITs require PE7 to be high for both DEVICE
	 * and OTG HOST modes, while rev 1.1 and greater require PE7 to
	 * be low for DEVICE mode and high for HOST mode. We set it high
	 * here because we are in host mode
	 */
	*pPORTE_FER = *pPORTE_FER & ~(1 << 7);
	*pPORTE_DIR_SET |= (1 << 7);
	*pPORTE_SET |= (1 << 7);
	SSYNC();
#endif

	if (ANOMALY_05000346) {
		bfin_write_USB_APHY_CALIB(ANOMALY_05000346_value);
		SSYNC();
	}

	if (ANOMALY_05000347) {
		bfin_write_USB_APHY_CNTRL(0x0);
		SSYNC();
	}

	/* Configure PLL oscillator register */
	bfin_write_USB_PLLOSC_CTRL(0x30a8);
	SSYNC();

	bfin_write_USB_SRP_CLKDIV((get_sclk()/1000) / 32 - 1);
	SSYNC();

	bfin_write_USB_EP_NI0_RXMAXP(64);
	SSYNC();

	bfin_write_USB_EP_NI0_TXMAXP(64);
	SSYNC();

	/* Route INTRUSB/INTR_RX/INTR_TX to USB_INT0*/
	bfin_write_USB_GLOBINTR(0x7);
	SSYNC();

	bfin_write_USB_GLOBAL_CTL(GLOBAL_ENA | EP1_TX_ENA | EP2_TX_ENA |
				EP3_TX_ENA | EP4_TX_ENA | EP5_TX_ENA |
				EP6_TX_ENA | EP7_TX_ENA | EP1_RX_ENA |
				EP2_RX_ENA | EP3_RX_ENA | EP4_RX_ENA |
				EP5_RX_ENA | EP6_RX_ENA | EP7_RX_ENA);
	SSYNC();

	return 0;
}

/*
 * This function performs Blackfin platform specific deinitialization for usb.
*/
void musb_platform_deinit(void)
{
}

#endif /* CONFIG_USB_BLACKFIN */
