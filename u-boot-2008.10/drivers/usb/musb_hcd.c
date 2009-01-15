/*
 * Mentor USB OTG Core host controller driver.
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

#ifdef CONFIG_MUSB_HCD
#include "musb_hcd.h"

/* MSC control transfers */
#define USB_MSC_BBB_RESET 	0xFF
#define USB_MSC_BBB_GET_MAX_LUN	0xFE

/* speed negotiated with the connected device */
static u8 musb_speed;

/* Endpoint configuration information */
static struct musb_epinfo epinfo[2] = {
	{MUSB_BULK_EP, 1, 512}, /* EP1 - Bluk Out - 512 Bytes */
	{MUSB_BULK_EP, 0, 512}  /* EP1 - Bluk In  - 512 Bytes */
};

/*
 * This function writes the data toggle value.
 */
static void write_toggle(struct usb_device *dev, u8 ep, u8 dir_out)
{
	u16 toggle = usb_gettoggle(dev, ep, dir_out);
	u16 csr;

	if (dir_out) {
		if (!toggle)
			musb_writew(MUSB_INDEXED_OFFSET(MUSB_TXCSR),
#ifdef CONFIG_USB_BLACKFIN
						0
#else
						MUSB_TXCSR_CLRDATATOG
#endif
						);
		else {
			csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_TXCSR));
			csr |= MUSB_TXCSR_H_WR_DATATOGGLE;
			musb_writew(MUSB_INDEXED_OFFSET(MUSB_TXCSR), csr);
			csr |= (toggle << MUSB_TXCSR_H_DATATOGGLE_SHIFT);
			musb_writew(MUSB_INDEXED_OFFSET(MUSB_TXCSR), csr);
		}
	} else {
		if (!toggle)
			musb_writew(MUSB_INDEXED_OFFSET(MUSB_RXCSR),
#ifdef CONFIG_USB_BLACKFIN
						0
#else
						MUSB_RXCSR_CLRDATATOG
#endif
						);
		else {
			csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_RXCSR));
			csr |= MUSB_RXCSR_H_WR_DATATOGGLE;
			musb_writew(MUSB_INDEXED_OFFSET(MUSB_RXCSR), csr);
			csr |= (toggle << MUSB_S_RXCSR_H_DATATOGGLE);
			musb_writew(MUSB_INDEXED_OFFSET(MUSB_RXCSR), csr);
		}
	}
}

/*
 * This function checks if RxStall has occured on the endpoint. If a RxStall
 * has occured, the RxStall is cleared and 1 is returned. If RxStall has
 * not occured, 0 is returned.
 */
static u8 check_stall(u8 ep, u8 dir_out)
{
	u16 csr;

	/* For endpoint 0 */
	if (!ep) {
		csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_CSR0));
		if (csr & MUSB_CSR0_H_RXSTALL) {
			csr &= ~MUSB_CSR0_H_RXSTALL;
			musb_writew(MUSB_INDEXED_OFFSET(MUSB_CSR0), csr);
			return 1;
		}
	} else { /* For non-ep0 */
		if (dir_out) { /* is it tx ep */
			csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_TXCSR));
			if (csr & MUSB_TXCSR_H_RXSTALL) {
				csr &= ~MUSB_TXCSR_H_RXSTALL;
				musb_writew(MUSB_INDEXED_OFFSET(MUSB_TXCSR),
						csr);
				return 1;
			}
		} else { /* is it rx ep */
			csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_RXCSR));
			if (csr & MUSB_RXCSR_H_RXSTALL) {
				csr &= ~MUSB_RXCSR_H_RXSTALL;
				musb_writew(MUSB_INDEXED_OFFSET(MUSB_RXCSR),
						csr);
				return 1;
			}
		}
	}
	return 0;
}


/*
 * waits until ep0 is ready. Returns 0 if ep is ready, -1 for timeout
 * error and -2 for stall.
 */
static int wait_until_ep0_ready(struct usb_device *dev, u32 bit_mask)
{
	u16 csr;
	int result = 1;
	int timeout = CONFIG_MUSB_TIMEOUT;

	while (result > 0) {
		csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_CSR0));
		if (csr & MUSB_CSR0_H_ERROR) {
			csr &= ~MUSB_CSR0_H_ERROR;
			musb_writew(MUSB_INDEXED_OFFSET(MUSB_CSR0), csr);
			dev->status = USB_ST_CRC_ERR;
			result = -1;
			break;
		}

		switch (bit_mask) {
		case MUSB_CSR0_TXPKTRDY:
			if (!(csr & MUSB_CSR0_TXPKTRDY)) {
				if (check_stall(MUSB_CONTROL_EP, 0)) {
					dev->status = USB_ST_STALLED;
					result = -2;
				} else
					result = 0;
			}
			break;

		case MUSB_CSR0_RXPKTRDY:
			if (check_stall(MUSB_CONTROL_EP, 0)) {
				dev->status = USB_ST_STALLED;
				result = -2;
			} else
				if (csr & MUSB_CSR0_RXPKTRDY)
					result = 0;
			break;

		case MUSB_CSR0_H_REQPKT:
			if (!(csr & MUSB_CSR0_H_REQPKT)) {
				if (check_stall(MUSB_CONTROL_EP, 0)) {
					dev->status = USB_ST_STALLED;
					result = -2;
				} else
					result = 0;
			}
			break;
		}

		/* Check the timeout */
		if (--timeout)
			udelay(1);
		else {
			dev->status = USB_ST_CRC_ERR;
			result = -1;
			break;
		}
	}

	return result;
}

/*
 * waits until tx ep is ready. Returns 1 when ep is ready and 0 on error.
 */
static u8 wait_until_txep_ready(struct usb_device *dev, u8 ep)
{
	u16 csr;
	int timeout = CONFIG_MUSB_TIMEOUT;

	do {
		if (check_stall(ep, 1)) {
			dev->status = USB_ST_STALLED;
			return 0;
		}

		csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_TXCSR));
		if (csr & MUSB_TXCSR_H_ERROR) {
			dev->status = USB_ST_CRC_ERR;
			return 0;
		}

		/* Check the timeout */
		if (--timeout)
			udelay(1);
		else {
			dev->status = USB_ST_CRC_ERR;
			return -1;
		}

	} while (csr & MUSB_TXCSR_TXPKTRDY);
	return 1;
}

/*
 * waits until rx ep is ready. Returns 1 when ep is ready and 0 on error.
 */
static u8 wait_until_rxep_ready(struct usb_device *dev, u8 ep)
{
	u16 csr;
	int timeout = CONFIG_MUSB_TIMEOUT;

	do {
		if (check_stall(ep, 0)) {
			dev->status = USB_ST_STALLED;
			return 0;
		}

		csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_RXCSR));
		if (csr & MUSB_RXCSR_H_ERROR) {
			dev->status = USB_ST_CRC_ERR;
			return 0;
		}

		/* Check the timeout */
		if (--timeout)
			udelay(1);
		else {
			dev->status = USB_ST_CRC_ERR;
			return -1;
		}

	} while (!(csr & MUSB_RXCSR_RXPKTRDY));
	return 1;
}

/*
 * This function performs the setup phase of the control transfer
 */
static int ctrlreq_setup_phase(struct usb_device *dev, struct devrequest *setup)
{
	int result;
	u16 csr;

	/* write the control request to ep0 fifo */
	write_fifo(MUSB_CONTROL_EP, sizeof(struct devrequest), (void *)setup);

	/* enable transfer of setup packet */
	csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_CSR0));
	csr |= (MUSB_CSR0_TXPKTRDY|MUSB_CSR0_H_SETUPPKT);
	musb_writew(MUSB_INDEXED_OFFSET(MUSB_CSR0), csr);

	/* wait until the setup packet is transmitted */
	result = wait_until_ep0_ready(dev, MUSB_CSR0_TXPKTRDY);
	dev->act_len = 0;
	return result;
}

/*
 * This function handles the control transfer in data phase
 */
static int ctrlreq_in_data_phase(struct usb_device *dev, u32 len, void *buffer)
{
	u16 csr;
	u32 rxlen = 0;
	u32 nextlen = 0;
	u8  maxpktsize = (1 << dev->maxpacketsize) * 8;
	u8  *rxbuff = (u8 *)buffer;
	u8  rxedlength;
	int result;

	while (rxlen < len) {
		/* Determine the next read length */
		nextlen = ((len-rxlen) > maxpktsize) ? maxpktsize : (len-rxlen);

		/* Set the ReqPkt bit */
		csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_CSR0));
		musb_writew(MUSB_INDEXED_OFFSET(MUSB_CSR0),
					csr | MUSB_CSR0_H_REQPKT);

		result = wait_until_ep0_ready(dev, MUSB_CSR0_RXPKTRDY);
		if (result < 0)
			return result;

		/* Actual number of bytes received by usb */
		rxedlength = musb_readb(MUSB_INDEXED_OFFSET(MUSB_COUNT0));

		/* Read the data from the RxFIFO */
		read_fifo(MUSB_CONTROL_EP, rxedlength, &rxbuff[rxlen]);

		/* Clear the RxPktRdy Bit */
		csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_CSR0));
		csr &= ~MUSB_CSR0_RXPKTRDY;
		musb_writew(MUSB_INDEXED_OFFSET(MUSB_CSR0), csr);

		/* short packet? */
		if (rxedlength != nextlen) {
			dev->act_len += rxedlength;
			break;
		}
		rxlen += nextlen;
		dev->act_len = rxlen;
	}
	return 0;
}

/*
 * This function handles the control transfer out data phase
 */
static int ctrlreq_out_data_phase(struct usb_device *dev, u32 len, void *buffer)
{
	u16 csr;
	u32 txlen = 0;
	u32 nextlen = 0;
	u8  maxpktsize = (1 << dev->maxpacketsize) * 8;
	u8  *txbuff = (u8 *)buffer;
	int result = 0;

	while (txlen < len) {
		/* Determine the next write length */
		nextlen = ((len-txlen) > maxpktsize) ? maxpktsize : (len-txlen);

		/* Load the data to send in FIFO */
		write_fifo(MUSB_CONTROL_EP, txlen, &txbuff[txlen]);

		/* Set TXPKTRDY bit */
		csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_CSR0));
		musb_writew(MUSB_INDEXED_OFFSET(MUSB_CSR0),
			csr | MUSB_CSR0_H_DIS_PING | MUSB_CSR0_TXPKTRDY);

		result = wait_until_ep0_ready(dev, MUSB_CSR0_TXPKTRDY);
		if (result < 0)
			break;

		txlen += nextlen;
		dev->act_len = txlen;
	}
	return result;
}


/*
 * This function handles the control transfer out status phase
 */
static int ctrlreq_out_status_phase(struct usb_device *dev)
{
	u16 csr;
	int result;

	/* Set the StatusPkt bit */
	csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_CSR0));
	csr |= (MUSB_CSR0_H_DIS_PING | MUSB_CSR0_TXPKTRDY |
			MUSB_CSR0_H_STATUSPKT);
	musb_writew(MUSB_INDEXED_OFFSET(MUSB_CSR0), csr);

	/* Wait until TXPKTRDY bit is cleared */
	result = wait_until_ep0_ready(dev, MUSB_CSR0_TXPKTRDY);
	return result;
}


/*
 * This function handles the control transfer in status phase
 */
static int ctrlreq_in_status_phase(struct usb_device *dev)
{
	u16 csr;
	int result;

	/* Set the StatusPkt bit and ReqPkt bit */
	csr = MUSB_CSR0_H_DIS_PING | MUSB_CSR0_H_REQPKT | MUSB_CSR0_H_STATUSPKT;
	musb_writew(MUSB_INDEXED_OFFSET(MUSB_CSR0), csr);
	result = wait_until_ep0_ready(dev, MUSB_CSR0_H_REQPKT);

	/* clear StatusPkt bit and RxPktRdy bit */
	csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_CSR0));
	csr &= ~(MUSB_CSR0_RXPKTRDY | MUSB_CSR0_H_STATUSPKT);
	musb_writew(MUSB_INDEXED_OFFSET(MUSB_CSR0), csr);
	return result;
}


/*
 * determines the speed of the device (High/Full/Slow)
 */
static u8 get_dev_speed(struct usb_device *dev)
{
	return (dev->speed & USB_SPEED_HIGH) ? MUSB_TYPE_SPEED_HIGH :
		((dev->speed & USB_SPEED_LOW) ? MUSB_TYPE_SPEED_LOW :
						MUSB_TYPE_SPEED_FULL);
}

/*
 * configure the hub address and the port address.
 */
static void config_hub_port(struct usb_device *dev, u8 ep)
{
	u8 chid;
	u8 hub;

	/* Find out the nearest parent which is high speed */
	while (dev->parent->parent != NULL)
		if (get_dev_speed(dev->parent) !=  MUSB_TYPE_SPEED_HIGH)
			dev = dev->parent;
		else
			break;

	/* determine the port address at that hub */
	hub = dev->parent->devnum;
	for (chid = 0; chid < USB_MAXCHILDREN; chid++)
		if (dev->parent->children[chid] == dev)
			break;

#ifndef CONFIG_BLACKFIN
	/* configure the hub address and the port address */
	musb_writeb(MUSB_BUSCTL_OFFSET(ep, MUSB_TXHUBADDR), hub);
	musb_writeb(MUSB_BUSCTL_OFFSET(ep, MUSB_TXHUBPORT), (chid + 1));
	musb_writeb(MUSB_BUSCTL_OFFSET(ep, MUSB_RXHUBADDR), hub);
	musb_writeb(MUSB_BUSCTL_OFFSET(ep, MUSB_RXHUBPORT), (chid + 1));
#endif
}

/*
 * do a control transfer
 */
int submit_control_msg(struct usb_device *dev, unsigned long pipe, void *buffer,
			int len, struct devrequest *setup)
{
#ifndef CONFIG_USB_BLACKFIN
	int devnum = usb_pipedevice(pipe);
#endif
	u16 csr;
	u8  devspeed;

	/* select control endpoint */
	musb_writeb(MUSB_INDEX, MUSB_CONTROL_EP);
	csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_CSR0));

#ifndef CONFIG_USB_BLACKFIN
	/* target addr and (for multipoint) hub addr/port */
	musb_writeb(MUSB_BUSCTL_OFFSET(MUSB_CONTROL_EP, MUSB_TXFUNCADDR),
				devnum);
	musb_writeb(MUSB_BUSCTL_OFFSET(MUSB_CONTROL_EP, MUSB_RXFUNCADDR),
				devnum);
#endif

	/* configure the hub address and the port number as required */
	devspeed = get_dev_speed(dev);
	if ((musb_ishighspeed()) && (dev->parent != NULL) &&
		(devspeed != MUSB_TYPE_SPEED_HIGH)) {
		config_hub_port(dev, MUSB_CONTROL_EP);
		musb_writeb(MUSB_INDEXED_OFFSET(MUSB_TYPE0), devspeed << 6);
	} else {
		musb_writeb(MUSB_INDEXED_OFFSET(MUSB_TYPE0), musb_speed << 6);
#ifndef CONFIG_USB_BLACKFIN
		musb_writeb(MUSB_BUSCTL_OFFSET(MUSB_CONTROL_EP,
					MUSB_TXHUBADDR), 0);
		musb_writeb(MUSB_BUSCTL_OFFSET(MUSB_CONTROL_EP,
					MUSB_TXHUBPORT), 0);
		musb_writeb(MUSB_BUSCTL_OFFSET(MUSB_CONTROL_EP,
					MUSB_RXHUBADDR), 0);
		musb_writeb(MUSB_BUSCTL_OFFSET(MUSB_CONTROL_EP,
					MUSB_RXHUBPORT), 0);
#endif
	}

	/* Control transfer setup phase */
	if (ctrlreq_setup_phase(dev, setup) < 0)
		return 0;

	switch (setup->request) {
	case USB_REQ_GET_DESCRIPTOR:
	case USB_REQ_GET_CONFIGURATION:
	case USB_REQ_GET_INTERFACE:
	case USB_REQ_GET_STATUS:
	case USB_MSC_BBB_GET_MAX_LUN:
		/* control transfer in-data-phase */
		if (ctrlreq_in_data_phase(dev, len, buffer) < 0)
			return 0;
		/* control transfer out-status-phase */
		if (ctrlreq_out_status_phase(dev) < 0)
			return 0;
		break;

	case USB_REQ_SET_ADDRESS:
	case USB_REQ_SET_CONFIGURATION:
	case USB_REQ_SET_FEATURE:
	case USB_REQ_SET_INTERFACE:
	case USB_REQ_CLEAR_FEATURE:
	case USB_MSC_BBB_RESET:
		/* control transfer in status phase */
		if (ctrlreq_in_status_phase(dev) < 0)
			return 0;
		break;

	case USB_REQ_SET_DESCRIPTOR:
		/* control transfer out data phase */
		if (ctrlreq_out_data_phase(dev, len, buffer) < 0)
			return 0;
		/* control transfer in status phase */
		if (ctrlreq_in_status_phase(dev) < 0)
			return 0;
		break;

	default:
		/* unhandled control transfer */
		return -1;
	}

	dev->status = 0;
	dev->act_len = len;

#ifdef CONFIG_USB_BLACKFIN
	/* Set device address to USB_FADDR register */
	if (setup->request == USB_REQ_SET_ADDRESS)
		musb_writew(MUSB_FADDR, dev->devnum);
#endif

	return len;
}

/*
 * do a bulk transfer
 */
int submit_bulk_msg(struct usb_device *dev, unsigned long pipe,
					void *buffer, int len)
{
	int dir_out = usb_pipeout(pipe);
	int ep = usb_pipeendpoint(pipe);
#ifndef CONFIG_USB_BLACKFIN
	int devnum = usb_pipedevice(pipe);
#endif
	u8  type;
	u16 csr;
	u32 txlen = 0;
	u32 nextlen = 0;
	u8  devspeed;

	/* select bulk endpoint */
	musb_writeb(MUSB_INDEX, MUSB_BULK_EP);

#ifndef CONFIG_USB_BLACKFIN
	/* write the address of the device */
	if (dir_out)
		musb_writeb(MUSB_BUSCTL_OFFSET(MUSB_BULK_EP,
					MUSB_TXFUNCADDR), devnum);
	else
		musb_writeb(MUSB_BUSCTL_OFFSET(MUSB_BULK_EP,
					MUSB_RXFUNCADDR), devnum);
#endif

	/* configure the hub address and the port number as required */
	devspeed = get_dev_speed(dev);
	if ((musb_ishighspeed()) && (dev->parent != NULL) &&
		(devspeed != MUSB_TYPE_SPEED_HIGH)) {
		/*
		 * MUSB is in high speed and the destination device is full
		 * speed device. So configure the hub address and port
		 * address registers.
		 */
		config_hub_port(dev, MUSB_BULK_EP);
	} else {
#ifndef CONFIG_USB_BLACKFIN
		if (dir_out) {
			musb_writeb(MUSB_BUSCTL_OFFSET(MUSB_BULK_EP,
						MUSB_TXHUBADDR), 0);
			musb_writeb(MUSB_BUSCTL_OFFSET(MUSB_BULK_EP,
						MUSB_TXHUBPORT), 0);
		} else {
			musb_writeb(MUSB_BUSCTL_OFFSET(MUSB_BULK_EP,
						MUSB_RXHUBADDR), 0);
			musb_writeb(MUSB_BUSCTL_OFFSET(MUSB_BULK_EP,
						MUSB_RXHUBPORT), 0);
		}
#endif
		devspeed = musb_speed;
	}

	/* Write the saved toggle bit value */
	write_toggle(dev, ep, dir_out);

	if (dir_out) { /* bulk-out transfer */
		/* Program the TxType register */
		type = (devspeed << MUSB_TYPE_SPEED_SHIFT) |
			   (MUSB_TYPE_PROTO_BULK << MUSB_TYPE_PROTO_SHIFT) |
			   (ep & MUSB_TYPE_REMOTE_END);
		musb_writeb(MUSB_INDEXED_OFFSET(MUSB_TXTYPE), type);

		/* Write maximum packet size to the TxMaxp register */
		musb_writew(MUSB_INDEXED_OFFSET(MUSB_TXMAXP),
				dev->epmaxpacketout[ep]);

		while (txlen < len) {
			nextlen = ((len-txlen) < dev->epmaxpacketout[ep]) ?
					(len-txlen) : dev->epmaxpacketout[ep];

#ifdef CONFIG_USB_BLACKFIN
			/* Set the transfer data size */
			musb_writew(MUSB_INDEXED_OFFSET(MUSB_TXCOUNT), nextlen);
#endif

			/* Write the data to the FIFO */
			write_fifo(MUSB_BULK_EP, nextlen,
					(void *)(((u8 *)buffer) + txlen));

			/* Set the TxPktRdy bit */
			csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_TXCSR));
			musb_writew(MUSB_INDEXED_OFFSET(MUSB_TXCSR),
					csr | MUSB_TXCSR_TXPKTRDY);

			/* Wait until the TxPktRdy bit is cleared */
			if (!wait_until_txep_ready(dev, MUSB_BULK_EP)) {
				csr = musb_readw(
					MUSB_INDEXED_OFFSET(MUSB_TXCSR));
				usb_settoggle(dev, ep, dir_out,
				(csr >> MUSB_TXCSR_H_DATATOGGLE_SHIFT) & 1);
				dev->act_len = txlen;
				return 0;
			}
			txlen += nextlen;
		}

		/* Keep a copy of the data toggle bit */
		csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_TXCSR));
		usb_settoggle(dev, ep, dir_out,
				(csr >> MUSB_TXCSR_H_DATATOGGLE_SHIFT) & 1);
	} else { /* bulk-in transfer */

		/* Write the saved toggle bit value */
		write_toggle(dev, ep, dir_out);

		/* Program the RxType register */
		type = (devspeed << MUSB_TYPE_SPEED_SHIFT) |
			   (MUSB_TYPE_PROTO_BULK << MUSB_TYPE_PROTO_SHIFT) |
			   (ep & MUSB_TYPE_REMOTE_END);
		musb_writeb(MUSB_INDEXED_OFFSET(MUSB_RXTYPE), type);

		/* Write the maximum packet size to the RxMaxp register */
		musb_writew(MUSB_INDEXED_OFFSET(MUSB_RXMAXP),
					dev->epmaxpacketin[ep]);

		while (txlen < len) {
			nextlen = ((len-txlen) < dev->epmaxpacketin[ep]) ?
					(len-txlen) : dev->epmaxpacketin[ep];

			/* Set the ReqPkt bit */
			csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_RXCSR));
			musb_writew(MUSB_INDEXED_OFFSET(MUSB_RXCSR),
						csr | MUSB_RXCSR_H_REQPKT);

			/* Wait until the RxPktRdy bit is set */
			if (!wait_until_rxep_ready(dev, MUSB_BULK_EP)) {
				csr = musb_readw(
					MUSB_INDEXED_OFFSET(MUSB_RXCSR));
				usb_settoggle(dev, ep, dir_out,
				(csr >> MUSB_S_RXCSR_H_DATATOGGLE) & 1);
				csr &= ~MUSB_RXCSR_RXPKTRDY;
				musb_writew(MUSB_INDEXED_OFFSET(MUSB_RXCSR),
					       csr);
				dev->act_len = txlen;
				return 0;
			}

			/* Read the data from the FIFO */
			read_fifo(MUSB_BULK_EP, nextlen,
					(void *)(((u8 *)buffer) + txlen));

			/* Clear the RxPktRdy bit */
			csr =  musb_readw(MUSB_INDEXED_OFFSET(MUSB_RXCSR));
			csr &= ~MUSB_RXCSR_RXPKTRDY;
			musb_writew(MUSB_INDEXED_OFFSET(MUSB_RXCSR), csr);
			txlen += nextlen;
		}

		/* Keep a copy of the data toggle bit */
		csr = musb_readw(MUSB_INDEXED_OFFSET(MUSB_RXCSR));
		usb_settoggle(dev, ep, dir_out,
				(csr >> MUSB_S_RXCSR_H_DATATOGGLE) & 1);
	}

	/* bulk transfer is complete */
	dev->status = 0;
	dev->act_len = len;
	return 0;
}

/*
 * This function initializes the usb controller module.
 */
int usb_lowlevel_init(void)
{
	u8  power;
	u32 timeout;

	if (musb_platform_init() == -1)
		return -1;

	/* Configure all the endpoint FIFO's and start usb controller */
	musb_configure_ep(&epinfo[0],
			sizeof(epinfo) / sizeof(struct musb_epinfo));
	musb_start();

	/*
	 * Wait until musb is enabled in host mode with a timeout. There
	 * should be a usb device connected.
	 */
	timeout = MUSB_HCD_TIMEOUT;
	while (timeout--)
		if (musb_readb(MUSB_DEVCTL) & MUSB_DEVCTL_HM)
			break;

	/* if musb core is not in host mode, then return */
	if (!timeout)
		return -1;

	/* start usb bus reset */
	power = musb_readb(MUSB_POWER);
	musb_writeb(MUSB_POWER, power | MUSB_POWER_RESET);

	/* After initiating a usb reset, wait for about 20ms to 30ms */
	udelay(30000);

	/* stop usb bus reset */
	power = musb_readb(MUSB_POWER);
	power &= ~MUSB_POWER_RESET;
	musb_writeb(MUSB_POWER, power);

	/* Determine if the connected device is a high/full/low speed device */
	musb_speed = (musb_readb(MUSB_POWER) & MUSB_POWER_HSMODE) ?
			MUSB_TYPE_SPEED_HIGH :
			((musb_readb(MUSB_DEVCTL) & MUSB_DEVCTL_FSDEV) ?
			MUSB_TYPE_SPEED_FULL : MUSB_TYPE_SPEED_LOW);
	return 0;
}

/*
 * This function stops the operation of the davinci usb module.
 */
int usb_lowlevel_stop(void)
{
	/* Reset the USB module */
	musb_platform_deinit();
	musb_writeb(MUSB_DEVCTL, 0);
	return 0;
}

/*
 * This function supports usb interrupt transfers. Currently, usb interrupt
 * transfers are not supported.
 */
int submit_int_msg(struct usb_device *dev, unsigned long pipe,
				void *buffer, int len, int interval)
{
	return -1;
}

#endif	/* CONFIG_MUSB_HCD */



