/*
 * Mentor USB OTG Core functionality common for both host and Device
 * functionality.
 *
 * Copyright (c) 2008 Texas Instruments
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Author: Thomas Abraham t-abraham <at> ti.com, Texas Instruments
 */

#include <common.h>

#if defined(CONFIG_MUSB_HCD) || defined(CONFIG_MUSB_DEVICE)
#include "musb_core.h"

/*
 * program the mentor core to start (enable interrupts, dma, etc.)
 */
void musb_start(void)
{
	u8 devctl;

	/* disable all interrupts */
	musb_writew(MUSB_INTRTXE, 0);
	musb_writew(MUSB_INTRRXE, 0);
	musb_writeb(MUSB_INTRUSBE, 0);
	musb_writeb(MUSB_TESTMODE, 0);

	/* put into basic highspeed mode and start session */
	musb_writeb(MUSB_POWER, MUSB_POWER_HSENAB);
#if defined(CONFIG_MUSB_HCD)
	devctl = musb_readb(MUSB_DEVCTL);
	musb_writeb(MUSB_DEVCTL, devctl | MUSB_DEVCTL_SESSION);
#endif
}

/*
 * This function configures the endpoint configuration. The musb hcd or musb
 * device implementation can use this function to configure the Endpoints
 * and set the FIFO sizes. Note: The summation of FIFO sizes of all endpoints
 * should not be more than the available FIFO size.
 *
 * epinfo	- Pointer to EP configuration table
 * cnt		- Number of entries in the EP conf table.
 */
void musb_configure_ep(struct musb_epinfo *epinfo, u8 cnt)
{
	u16 csr;
	u16 fifoaddr = 64; /* First 64 bytes of FIFO reserved for EP0 */
	u32 fifosize;
	u8  idx;

	while (cnt--) {
		/* prepare fifosize to write to register */
		fifosize = epinfo->epsize >> 3;
		idx = ffs(fifosize) - 1;

		musb_writeb(MUSB_INDEX, epinfo->epnum);
		if (epinfo->epdir) {
#ifndef CONFIG_USB_BLACKFIN
			/* Configure fifo size and fifo base address */
			musb_writeb(MUSB_TXFIFOSZ, idx);
			musb_writew(MUSB_TXFIFOADD, fifoaddr >> 3);
#endif

#if defined(CONFIG_MUSB_HCD)
			/* clear the data toggle bit */
			csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_TXCSR));
			musb_writew(MUSB_INDEXED_OFFSET(MUSB_TXCSR),
					csr | MUSB_TXCSR_CLRDATATOG);
#endif

			/* Flush fifo if required */
			if (csr & MUSB_TXCSR_TXPKTRDY) {
				musb_writew(MUSB_INDEXED_OFFSET(MUSB_TXCSR),
						csr | MUSB_TXCSR_FLUSHFIFO);
			}
		} else {
#ifndef CONFIG_USB_BLACKFIN
			/* Configure fifo size and fifo base address */
			musb_writeb(MUSB_RXFIFOSZ, idx);
			musb_writew(MUSB_RXFIFOADD, fifoaddr >> 3);
#endif

#if defined(CONFIG_MUSB_HCD)
			/* clear the data toggle bit */
			csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_RXCSR));
			musb_writew(MUSB_INDEXED_OFFSET(MUSB_RXCSR),
						csr | MUSB_RXCSR_CLRDATATOG);
#endif

			/* Flush fifo if required */
			if (csr & MUSB_RXCSR_RXPKTRDY) {
				musb_writew(MUSB_INDEXED_OFFSET(MUSB_RXCSR),
						csr | MUSB_RXCSR_FLUSHFIFO);
			}
		}
		fifoaddr += epinfo->epsize;
		epinfo++;
	}
}

#ifndef CONFIG_USB_BLACKFIN
/*
 * This function writes data to endpoint fifo
 *
 * ep		- Endpoint number
 * length	- Number of bytes to write to FIFO
 * fifo_data	- Pointer to data buffer that contains the data to write
 */
void write_fifo(u8 ep, u32 length, void *fifo_data)
{
	u32 address;
	u8  *data = (u8 *)fifo_data;

	/* select the endpoint index */
	musb_writeb(MUSB_INDEX, ep);
	address = MUSB_FIFO_OFFSET(ep);

	/* write the data to the fifo */
	while (length--)
		musb_writeb(address, *data++);
}
#endif

#ifndef CONFIG_USB_BLACKFIN
/*
 * This function reads data from endpoint fifo
 *
 * ep           - Endpoint number
 * length       - Number of bytes to read from FIFO
 * fifo_data    - Pointer to data buffer into which data is read
 */
void read_fifo(u8 ep, u32 length, void *fifo_data)
{
	u32 address;
	u8  *data = (u8 *)fifo_data;

	/* select the endpoint index */
	musb_writeb(MUSB_INDEX, ep);
	address = MUSB_FIFO_OFFSET(ep);

	/* read the data to the fifo */
	while (length--)
		*data++ = musb_readb(address);
}
#endif

#endif	/* CONFIG_MUSB_HCD */



