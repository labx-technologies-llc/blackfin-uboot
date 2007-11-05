
/* ax88180: ASIX AX88180 Non-PCI Gigabit Ethernet u-boot driver */

/*
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 */

/*
 * ========================================================================
 * ASIX AX88180 Non-PCI 16/32-bit Gigabit Ethernet Linux Driver
 *
 * The AX88180 Ethernet controller is high performance and highly
 * integrated local CPU bus Ethernet controllers with embedded 40K bytes
 * SRAM and supports both 16-bit and 32-bit SRAM-Like interfaces
 * for any embedded systems.
 * The AX88180 is a single chip 10/100/1000Mbps Gigabit Ethernet controller
 * that supports both MII and RGMII interfaces and is compliant to
 * IEEE 802.3, IEEE 802.3u and IEEE 802.3z standards.
 *
 * Please visit ASIX's web site (http://www.asix.com.tw) for more details.
 *
 * Module Name : ax88180.c
 * Purpose     : This file is the main file.
 * Author      : Allan Chou <allan@asix.com.tw>
 * Date        : 2006-09-06
 * Notes       :
 * History     :
 * $Log:$
 * 1.0.0	2006-09-06
 * New release for AX88180 US2 chip.
 *
 * ========================================================================
 */

#include <common.h>
#include <command.h>
#include <net.h>

#if defined(CONFIG_S3C2440A_SMDK)
#include <s3c2440.h>
#endif

#include "ax88180.h"

#ifdef CONFIG_DRIVER_AX88180

#if (CONFIG_COMMANDS & CFG_CMD_NET)

/*
 * ===========================================================================
 * <<<<<<             Local SubProgram Declaration              >>>>>>
 * ===========================================================================
 */
static void ax88180_rx_handler(void);
static void ax88180_PHY_initial(void);
static void ax88180_meida_config(void);
static void get_CicadaPHY_meida_mode(void);
static void get_MarvellPHY_meida_mode(void);

/*
 * ===========================================================================
 * <<<<<<             Declare Macro/Structure Definition        >>>>>>
 * ===========================================================================
 */

typedef struct _AX88180_PRIVATE {
	unsigned long PhyAddr;
	unsigned long PhyID0;
	unsigned int MediaMode;
	unsigned int RealMediaMode;
	unsigned long RxFilterMode;
	unsigned long FirstTxDesc;
	unsigned long NextTxDesc;
	unsigned long rxbuf_overflow_count;
} AX88180_PRIVATE;

#define mdelay(n)       udelay((n)*1000)

#define	PRINTK(flag, args...) if (flag & DEBUG_FLAGS) printf(args)

/* Access RXBUFFER_START/TXBUFFER_START to read RX buffer/write TX buffer */

#ifdef CONFIG_DRIVER_AX88180_16BIT

#define MACREG_OFFSET_16BIT	(- 0xDD00)
#define RXBUF_OFFSET_16BIT	(0x2000)
#define TXBUF_OFFSET_16BIT	(- 0x7000)

#if defined(__bfin__)
#include <asm/io.h>
#define READ_RXBUF(data) data = readw(AX88180_BASE + RXBUFFER_START + RXBUF_OFFSET_16BIT)
#define WRITE_TXBUF(data) writew(data, AX88180_BASE + TXBUFFER_START + TXBUF_OFFSET_16BIT)
#define READ_MACREG(regaddr, regdata) regdata = readw(AX88180_BASE + MACREG_OFFSET_16BIT + regaddr)
#define WRITE_MACREG(regaddr, regdata) writew(regdata, AX88180_BASE + MACREG_OFFSET_16BIT + regaddr);
#else				/* defined(__bfin__) */
#define READ_RXBUF(data)	data = *(volatile unsigned short *)(AX88180_BASE + RXBUFFER_START)
#define WRITE_TXBUF(data)	*(volatile unsigned short *)(AX88180_BASE + TXBUFFER_START) = data
#define READ_MACREG(regaddr, regdata) regdata = *(volatile unsigned short*)(AX88180_BASE + regaddr)
#define WRITE_MACREG(regaddr, regdata) *(volatile unsigned short*)(AX88180_BASE + regaddr) = regdata;
#endif				/* defined(__bfin__) */

#else				/* CONFIG_DRIVER_AX88180_16BIT */

#if defined(__bfin__)
#include <asm/io.h>
#define READ_RXBUF(data) data = readl(AX88180_BASE + RXBUFFER_START)
#define WRITE_TXBUF(data) writel(data, AX88180_BASE + TXBUFFER_START)
#define READ_MACREG(regaddr, regdata) regdata = readl(AX88180_BASE + regaddr)
#define WRITE_MACREG(regaddr, regdata) writel(regdata, AX88180_BASE + regaddr);
#else				/* defined(__bfin__) */
#define READ_RXBUF(data)	data = *(volatile unsigned long *)(AX88180_BASE + RXBUFFER_START)
#define WRITE_TXBUF(data)	*(volatile unsigned long *)(AX88180_BASE + TXBUFFER_START) = data
#define READ_MACREG(regaddr, regdata) regdata = *(volatile unsigned long*)(AX88180_BASE + regaddr)
#define WRITE_MACREG(regaddr, regdata) *(volatile unsigned long*)(AX88180_BASE + regaddr) = regdata;
#endif				/* defined(__bfin__) */

#endif				/* CONFIG_DRIVER_AX88180_16BIT */

#define READ_PHYREG(phyaddr, regaddr, regdata) { \
	unsigned long tmpval1, k1; \
	WRITE_MACREG(MDIOCTRL, READ_PHY | (regaddr << 8) | phyaddr); \
	for (k1 = 0; k1 < 10000; k1++) { \
		READ_MACREG(MDIOCTRL, tmpval1); \
		if ((tmpval1 & READ_PHY) == 0) { \
			break; \
		} \
		udelay(1); \
	} \
	READ_MACREG(MDIODP, regdata); \
}
#define WRITE_PHYREG(phyaddr, regaddr, regdata) { \
	unsigned long tmpval2, k2; \
	WRITE_MACREG(MDIODP, regdata); \
	WRITE_MACREG(MDIOCTRL, WRITE_PHY | (regaddr << 8) | phyaddr); \
	for (k2 = 0; k2 < 10000; k2++) { \
		READ_MACREG(MDIOCTRL, tmpval2); \
		if ((tmpval2 & WRITE_PHY) == 0) { \
			break; \
		} \
		udelay(1); \
	} \
}

#define RESET_MAC { \
	unsigned long tmpval3; \
	WRITE_MACREG(MISC, MISC_RESET_MAC); \
	READ_MACREG(MISC, tmpval3); \
	WRITE_MACREG(MISC, MISC_NORMAL); \
	WRITE_MACREG(RXINDICATOR, DEFAULT_RXINDICATOR); \
	WRITE_MACREG(TXCMD, DEFAULT_TXCMD); \
	WRITE_MACREG(TXBS, DEFAULT_TXBS); \
	WRITE_MACREG(TXDES0, DEFAULT_TXDES0); \
	WRITE_MACREG(TXDES1, DEFAULT_TXDES1); \
	WRITE_MACREG(TXDES2, DEFAULT_TXDES2); \
	WRITE_MACREG(TXDES3, DEFAULT_TXDES3); \
	WRITE_MACREG(TXCFG, DEFAULT_TXCFG); \
	WRITE_MACREG(MACCFG2, DEFAULT_MACCFG2); \
	WRITE_MACREG(MACCFG3, DEFAULT_MACCFG3); \
	WRITE_MACREG(TXLEN, DEFAULT_TXLEN); \
	WRITE_MACREG(TXPAUT, DEFAULT_TXPAUT); \
	WRITE_MACREG(RXBTHD0, DEFAULT_RXBTHD0); \
	WRITE_MACREG(RXBTHD1, DEFAULT_RXBTHD1); \
	WRITE_MACREG(RXFULTHD, DEFAULT_RXFULTHD); \
	WRITE_MACREG(DOGTHD0, DEFAULT_DOGTHD0); \
	WRITE_MACREG(DOGTHD1, DEFAULT_DOGTHD1); \
}
#define RESET_PHY { \
	unsigned long tmpval3a, k3a; \
	WRITE_PHYREG(axlocal.PhyAddr, BMCR, PHY_RESET); \
	for (k3a = 0; k3a < 500; k3a++) { \
		READ_PHYREG(axlocal.PhyAddr, BMCR, tmpval3a); \
		if (!(tmpval3a & PHY_RESET)) \
			break; \
		mdelay(1); \
	} \
}

#define	INIT_TXRX_VARIABLES { \
	axlocal.FirstTxDesc = TXDP0; \
	axlocal.NextTxDesc = TXDP0; \
	axlocal.rxbuf_overflow_count = 0; \
}

#define	ENABLE_INTERRUPT	WRITE_MACREG(IMR, DEFAULT_IMR)
#define	DISABLE_INTERRUPT	WRITE_MACREG(IMR, CLEAR_IMR)

#define	START_READ_RXBUFF 	WRITE_MACREG(RXINDICATOR, RX_START_READ)
#define	STOP_READ_RXBUFF 	WRITE_MACREG(RXINDICATOR, RX_STOP_READ)

/* Display all AX88180 MAC registers onto console screen */
#define	DISPLAY_ALLMACREG { \
	unsigned long tmpval4; \
	int k4; \
	PRINTK(DEBUG_MSG, "ax88180: AX88180 MAC Registers:\n"); \
	for (k4 = 0xFC00; k4 <= 0xFCFF; k4 += 4) { \
		READ_MACREG(k4, tmpval4); \
		PRINTK(DEBUG_MSG, "0x%04x=0x%08lx ", k4, tmpval4); \
		if ((k4 & 0xF) == 0xC) \
			PRINTK(DEBUG_MSG, "\n"); \
	} \
	PRINTK(DEBUG_MSG, "\n"); \
}

/* Display all AX88180 PHY registers onto console screen */
#define	DISPLAY_ALLPHYREG { \
	unsigned long tmpval5; \
	READ_PHYREG(axlocal.PhyAddr, BMCR, tmpval5); \
	PRINTK(DEBUG_MSG, "BMCR=0x%04x ", (unsigned int)tmpval5); \
	READ_PHYREG(axlocal.PhyAddr, BMSR, tmpval5); \
	PRINTK(DEBUG_MSG, "BMSR=0x%04x ", (unsigned int)tmpval5); \
	READ_PHYREG(axlocal.PhyAddr, PHYIDR0, tmpval5); \
	PRINTK(DEBUG_MSG, "PHYIDR0=0x%04x ", (unsigned int)tmpval5); \
	READ_PHYREG(axlocal.PhyAddr, PHYIDR1, tmpval5); \
	PRINTK(DEBUG_MSG, "PHYIDR1=0x%04x ", (unsigned int)tmpval5); \
	READ_PHYREG(axlocal.PhyAddr, ANAR, tmpval5); \
	PRINTK(DEBUG_MSG, "ANAR=0x%04x ", (unsigned int)tmpval5); \
	READ_PHYREG(axlocal.PhyAddr, ANLPAR, tmpval5); \
	PRINTK(DEBUG_MSG, "ANLPAR=0x%04x \n", (unsigned int)tmpval5); \
	READ_PHYREG(axlocal.PhyAddr, ANER, tmpval5); \
	PRINTK(DEBUG_MSG, "ANER=0x%04x ", (unsigned int)tmpval5); \
	READ_PHYREG(axlocal.PhyAddr, AUX_1000_CTRL, tmpval5); \
	PRINTK(DEBUG_MSG, "1G_CTRL=0x%04x ", (unsigned int)tmpval5); \
	READ_PHYREG(axlocal.PhyAddr, AUX_1000_STATUS, tmpval5); \
	PRINTK(DEBUG_MSG, "1G_STATUS=0x%04x \n", (unsigned int)tmpval5); \
	if (axlocal.PhyID0 == MARVELL_88E1111_PHYIDR0) { \
		READ_PHYREG(axlocal.PhyAddr, M88_SSR, tmpval5); \
		PRINTK(DEBUG_MSG, "M88_SSR=0x%04x ", (unsigned int)tmpval5); \
		READ_PHYREG(axlocal.PhyAddr, M88_IER, tmpval5); \
		PRINTK(DEBUG_MSG, "M88_IER=0x%04x ", (unsigned int)tmpval5); \
		READ_PHYREG(axlocal.PhyAddr, M88_ISR, tmpval5); \
		PRINTK(DEBUG_MSG, "M88_ISR=0x%04x ", (unsigned int)tmpval5); \
		READ_PHYREG(axlocal.PhyAddr, M88_EXT_SCR, tmpval5); \
		PRINTK(DEBUG_MSG, "M88_EXT_SCR=0x%04x ", (unsigned int)tmpval5); \
		READ_PHYREG(axlocal.PhyAddr, M88_EXT_SSR, tmpval5); \
		PRINTK(DEBUG_MSG, "M88_EXT_SSR=0x%04x \n", (unsigned int)tmpval5); \
	} else if (axlocal.PhyID0 == CICADA_CIS8201_PHYIDR0) { \
		READ_PHYREG(axlocal.PhyAddr, CIS_IMR, tmpval5); \
		PRINTK(DEBUG_MSG, "CIS_IMR=0x%04x ", (unsigned int)tmpval5); \
		READ_PHYREG(axlocal.PhyAddr, CIS_ISR, tmpval5); \
		PRINTK(DEBUG_MSG, "CIS_ISR=0x%04x ", (unsigned int)tmpval5); \
		READ_PHYREG(axlocal.PhyAddr, CIS_AUX_CTRL_STATUS, tmpval5); \
		PRINTK(DEBUG_MSG, "CIS_AUX=0x%04x \n", (unsigned int)tmpval5); \
	} \
	READ_MACREG(RXCFG, tmpval5); \
	PRINTK(DEBUG_MSG, "RXCFG=0x%08lx ", tmpval5); \
	READ_MACREG(MACCFG0, tmpval5); \
	PRINTK(DEBUG_MSG, "MACCFG0=0x%08lx ", tmpval5); \
	READ_MACREG(MACCFG1, tmpval5); \
	PRINTK(DEBUG_MSG, "MACCFG1=0x%08lx ", tmpval5); \
	READ_MACREG(MACCFG2, tmpval5); \
	PRINTK(DEBUG_MSG, "MACCFG2=0x%08lx \n\n", tmpval5); \
}

/*
 * ===========================================================================
 * <<<<<<             Global Variable Definition                >>>>>>
 * ===========================================================================
 */

AX88180_PRIVATE axlocal;

/*
 * ===========================================================================
 * <<<<<<             Local SubProgram Bodies                   >>>>>>
 * ===========================================================================
 */

/*
 *****************************************************************************
 * ax88180_rx_handler()
 *
 * Handle packets received completion interrupt event.
 *
 *****************************************************************************
 */
static void ax88180_rx_handler(void)
{
	unsigned char *rxdata;
	unsigned long tmp_data;
	unsigned long rx_packet_len;
	unsigned int data_size;
	unsigned int dword_count, byte_count;
	unsigned long rxcurt_ptr, rxbound_ptr, next_ptr;
	int i;
	int j;

	READ_MACREG(RXCURT, rxcurt_ptr);
	READ_MACREG(RXBOUND, rxbound_ptr);
	next_ptr = (rxbound_ptr + 1) & RX_PAGE_NUM_MASK;

	PRINTK(RX_MSG,
	       "ax88180: RX original RXBOUND=0x%08lx, RXCURT=0x%08lx\n",
	       rxbound_ptr, rxcurt_ptr);

	while (next_ptr != rxcurt_ptr) {
		START_READ_RXBUFF;
		READ_RXBUF(rx_packet_len);
		if ((rx_packet_len == 0) || (rx_packet_len > MAX_RX_SIZE)) {
			STOP_READ_RXBUFF;
			RESET_MAC;
			PRINTK(ERROR_MSG,
			       "ax88180: Invalid Rx packet length!! (len=0x%08lx)\n",
			       rx_packet_len);
			PRINTK(ERROR_MSG,
			       "ax88180: RX RXBOUND=0x%08lx, RXCURT=0x%08lx\n",
			       rxbound_ptr, rxcurt_ptr);
			PRINTK(RX_MSG,
			       "ax88180: ax88180_rx_handler fail end ..........\n");
			return;
		}
		data_size = (unsigned int)rx_packet_len;
		rxbound_ptr += (((data_size + 0xF) & 0xFFF0) >> 4) + 1;
		rxbound_ptr &= RX_PAGE_NUM_MASK;

		rxdata = (unsigned char *)NetRxPackets[0];

#ifdef CONFIG_DRIVER_AX88180_16BIT
		dword_count = data_size >> 1;
		byte_count = data_size & 0x1;
#else
		dword_count = data_size >> 2;
		byte_count = data_size & 0x3;
#endif
		for (i = 0; i < dword_count; i++) {
			READ_RXBUF(tmp_data);
#ifdef CONFIG_DRIVER_AX88180_16BIT
			*((unsigned short *)rxdata + i) = tmp_data;
#else
			*((unsigned long *)rxdata + i) = tmp_data;
#endif
		}
		if (byte_count != 0) {
			READ_RXBUF(tmp_data);
			for (j = 0; j < byte_count; j++) {
				*(rxdata + (dword_count * 4) + j) =
				    (unsigned char)(tmp_data >> (j * 8));
			}
		}

		STOP_READ_RXBUFF;

		/* Pass the packet up to the protocol layers. */
		NetReceive(NetRxPackets[0], data_size);

		WRITE_MACREG(RXBOUND, rxbound_ptr);

		READ_MACREG(RXCURT, rxcurt_ptr);
		READ_MACREG(RXBOUND, rxbound_ptr);
		next_ptr = (rxbound_ptr + 1) & RX_PAGE_NUM_MASK;

		PRINTK(RX_MSG,
		       "ax88180: RX updated RXBOUND=0x%08lx, RXCURT=0x%08lx\n",
		       rxbound_ptr, rxcurt_ptr);
	}

	if (axlocal.rxbuf_overflow_count > 0)
		axlocal.rxbuf_overflow_count--;

	return;
}

/*
 *****************************************************************************
 * ax88180_PHY_initial()
 *
 * Initialize PHY registers.
 *
 *****************************************************************************
 */
static void ax88180_PHY_initial(void)
{
	unsigned long bmcr_val, anar_val, bmsr_val;
	unsigned long aux_1000_ctrl;
	unsigned long tmp_regval;
	unsigned int i;

	/* Check avaliable PHY chipset  */
	axlocal.PhyAddr = MARVELL_88E1111_PHYADDR;
	READ_PHYREG(axlocal.PhyAddr, PHYIDR0, axlocal.PhyID0);

	if (axlocal.PhyID0 == MARVELL_88E1111_PHYIDR0) {
		PRINTK(DRIVER_MSG,
		       "ax88180: Found Marvell 88E1111 PHY chipset. (PHY Addr=0x%x)\n",
		       (unsigned int)axlocal.PhyAddr);
		READ_PHYREG(axlocal.PhyAddr, M88_EXT_SSR, tmp_regval);
		if ((tmp_regval & HWCFG_MODE_MASK) == RGMII_COPPER_MODE) {
			WRITE_PHYREG(axlocal.PhyAddr, M88_EXT_SCR,
				     DEFAULT_EXT_SCR);
			RESET_PHY;
			WRITE_PHYREG(axlocal.PhyAddr, M88_IER, LINK_CHANGE_INT);
		}
	} else {
		axlocal.PhyAddr = CICADA_CIS8201_PHYADDR;
		READ_PHYREG(axlocal.PhyAddr, PHYIDR0, axlocal.PhyID0);
		if (axlocal.PhyID0 == CICADA_CIS8201_PHYIDR0) {
			PRINTK(DRIVER_MSG,
			       "ax88180: Found CICADA CIS8201 PHY chipset. (PHY Addr=0x%x)\n",
			       (unsigned int)axlocal.PhyAddr);
			WRITE_PHYREG(axlocal.PhyAddr, CIS_IMR,
				     (CIS_INT_ENABLE | LINK_CHANGE_INT));

			/* Set CIS_SMI_PRIORITY bit before force the media mode  */
			READ_PHYREG(axlocal.PhyAddr, CIS_AUX_CTRL_STATUS,
				    tmp_regval);
			tmp_regval &= ~CIS_SMI_PRIORITY;
			WRITE_PHYREG(axlocal.PhyAddr, CIS_AUX_CTRL_STATUS,
				     tmp_regval);
		} else {
			PRINTK(ERROR_MSG, "ax88180: Unknown PHY chipset!!\n");
		}
	}

	/* axlocal.ForceMedia = AUTO_MEDIA; */
	aux_1000_ctrl = DEFAULT_AUX_1000_CTRL;
	anar_val =
	    (ANAR_PAUSE | ANAR_100FULL | ANAR_100HALF | ANAR_10FULL |
	     ANAR_10HALF | ANAR_8023BIT);

	WRITE_PHYREG(axlocal.PhyAddr, AUX_1000_CTRL, aux_1000_ctrl);
	WRITE_PHYREG(axlocal.PhyAddr, ANAR, anar_val);

	/* Enable and restart auto-negotiation operation */
	bmcr_val = (AUTONEG_EN | RESTART_AUTONEG);
	WRITE_PHYREG(axlocal.PhyAddr, BMCR, bmcr_val);

	/* Waiting 5 secs for PHY link stable */
	PRINTK(DRIVER_MSG,
	       "ax88180: Waiting for auto-negotiation completion......\n");
	for (i = 0; i < 5000; i++) {
		READ_PHYREG(axlocal.PhyAddr, BMSR, bmsr_val);
		if (bmsr_val & LINKOK) {
			break;
		}
		mdelay(1);
	}

	return;
}

/*
 *****************************************************************************
 * ax88180_meida_config()
 *
 * Configure MAC registers (RXCFG, MACCFG0, MACCFG1) to match the real PHY media mode.
 *
 *****************************************************************************
 */
static void ax88180_meida_config(void)
{
	unsigned long bmcr_val, bmsr_val;
	unsigned long rxcfg_val, maccfg0_val, maccfg1_val;
	int i;

	/* Waiting 200 msecs for PHY link stable */
	for (i = 0; i < 200; i++) {
		READ_PHYREG(axlocal.PhyAddr, BMSR, bmsr_val);
		if (bmsr_val & LINKOK) {
			break;
		}
		mdelay(1);
	}

	READ_PHYREG(axlocal.PhyAddr, BMSR, bmsr_val);
	if (bmsr_val & LINKOK) {
		READ_PHYREG(axlocal.PhyAddr, BMCR, bmcr_val);
		if (bmcr_val & AUTONEG_EN) {
			/* Waiting for Auto-negotiation completion */
			PRINTK(INIT_MSG,
			       "ax88180: Auto-negotiation is enabled. Waiting for NWay completion.....\n");

			for (i = 0; i < 5000; i++) {
				if (bmsr_val & AUTONEG_COMPLETE) {
					break;
				}
				mdelay(1);
				READ_PHYREG(axlocal.PhyAddr, BMSR, bmsr_val);
			}
			if (i >= 5000)
				PRINTK(INIT_MSG,
				       "ax88180: Auto-negotiation is NOT completed!!\n");
		} else
			PRINTK(INIT_MSG,
			       "ax88180: Auto-negotiation is disabled.\n");

		PRINTK(DEBUG_MSG, "ax88180: BMCR=0x%04x, BMSR=0x%04x\n",
		       (unsigned int)bmcr_val, (unsigned int)bmsr_val);

		/* Get real media mode here */
		if (axlocal.PhyID0 == MARVELL_88E1111_PHYIDR0) {
			get_MarvellPHY_meida_mode();
		} else if (axlocal.PhyID0 == CICADA_CIS8201_PHYIDR0) {
			get_CicadaPHY_meida_mode();
		} else {
			axlocal.RealMediaMode = MEDIA_1000FULL;
		}

		switch (axlocal.RealMediaMode) {
		default:
		case MEDIA_1000FULL:
			PRINTK(DRIVER_MSG,
			       "ax88180: Set to 1000Mbps Full-duplex mode.\n");
			rxcfg_val = RXFLOW_ENABLE | DEFAULT_RXCFG;
			maccfg0_val = TXFLOW_ENABLE | DEFAULT_MACCFG0;
			maccfg1_val =
			    GIGA_MODE_EN | RXFLOW_EN | FULLDUPLEX |
			    DEFAULT_MACCFG1;
			break;

		case MEDIA_1000HALF:
			PRINTK(DRIVER_MSG,
			       "ax88180: Set to 1000Mbps Half-duplex mode.\n");
			rxcfg_val = DEFAULT_RXCFG;
			maccfg0_val = DEFAULT_MACCFG0;
			maccfg1_val = GIGA_MODE_EN | DEFAULT_MACCFG1;
			break;

		case MEDIA_100FULL:
			PRINTK(DRIVER_MSG,
			       "ax88180: Set to 100Mbps Full-duplex mode.\n");
			rxcfg_val = RXFLOW_ENABLE | DEFAULT_RXCFG;
			maccfg0_val =
			    SPEED100 | TXFLOW_ENABLE | DEFAULT_MACCFG0;
			maccfg1_val = RXFLOW_EN | FULLDUPLEX | DEFAULT_MACCFG1;
			break;

		case MEDIA_100HALF:
			PRINTK(DRIVER_MSG,
			       "ax88180: Set to 100Mbps Half-duplex mode.\n");
			rxcfg_val = DEFAULT_RXCFG;
			maccfg0_val = SPEED100 | DEFAULT_MACCFG0;
			maccfg1_val = DEFAULT_MACCFG1;
			break;

		case MEDIA_10FULL:
			PRINTK(DRIVER_MSG,
			       "ax88180: Set to 10Mbps Full-duplex mode.\n");
			rxcfg_val = RXFLOW_ENABLE | DEFAULT_RXCFG;
			maccfg0_val = TXFLOW_ENABLE | DEFAULT_MACCFG0;
			maccfg1_val = RXFLOW_EN | FULLDUPLEX | DEFAULT_MACCFG1;
			break;

		case MEDIA_10HALF:
			PRINTK(DRIVER_MSG,
			       "ax88180: Set to 10Mbps Half-duplex mode.\n");
			rxcfg_val = DEFAULT_RXCFG;
			maccfg0_val = DEFAULT_MACCFG0;
			maccfg1_val = DEFAULT_MACCFG1;
			break;
		}
	} else {
		PRINTK(INIT_MSG, "ax88180: The cable is disconnected!!\n");
		/* Set to default media mode (1000FULL) */
		rxcfg_val = RXFLOW_ENABLE | DEFAULT_RXCFG;
		maccfg0_val = TXFLOW_ENABLE | DEFAULT_MACCFG0;
		maccfg1_val =
		    GIGA_MODE_EN | RXFLOW_EN | FULLDUPLEX | DEFAULT_MACCFG1;
	}

	WRITE_MACREG(RXCFG, rxcfg_val);
	WRITE_MACREG(MACCFG0, maccfg0_val);
	WRITE_MACREG(MACCFG1, maccfg1_val);

	return;
}

/*
 *****************************************************************************
 * get_MarvellPHY_meida_mode()
 *
 * Get real media mode of Marvell 88E1111 PHY.
 *
 *****************************************************************************
 */
static void get_MarvellPHY_meida_mode(void)
{
	unsigned long m88_ssr;
	int i;

	/* Get the real media mode */
	for (i = 0; i < 200; i++) {
		READ_PHYREG(axlocal.PhyAddr, M88_SSR, m88_ssr);
		if (m88_ssr & SSR_MEDIA_RESOLVED_OK) {
			break;
		}
		mdelay(1);
	}

	READ_PHYREG(axlocal.PhyAddr, M88_SSR, m88_ssr);
	switch (m88_ssr & SSR_MEDIA_MASK) {
	default:
	case SSR_1000FULL:
		axlocal.RealMediaMode = MEDIA_1000FULL;
		break;

	case SSR_1000HALF:
		axlocal.RealMediaMode = MEDIA_1000HALF;
		break;

	case SSR_100FULL:
		axlocal.RealMediaMode = MEDIA_100FULL;
		break;

	case SSR_100HALF:
		axlocal.RealMediaMode = MEDIA_100HALF;
		break;

	case SSR_10FULL:
		axlocal.RealMediaMode = MEDIA_10FULL;
		break;

	case SSR_10HALF:
		axlocal.RealMediaMode = MEDIA_10HALF;
		break;
	}

	PRINTK(INIT_MSG, "ax88180: get_MarvellPHY_meida_mode end ..........\n");
	return;
}

/*
 *****************************************************************************
 * get_CicadaPHY_meida_mode()
 *
 * Get real media mode of CICADA CIS8201 PHY.
 *
 *****************************************************************************
 */
static void get_CicadaPHY_meida_mode(void)
{
	unsigned long tmp_regval;

	READ_PHYREG(axlocal.PhyAddr, CIS_AUX_CTRL_STATUS, tmp_regval);
	switch (tmp_regval & CIS_MEDIA_MASK) {
	default:
	case CIS_1000FULL:
		axlocal.RealMediaMode = MEDIA_1000FULL;
		break;

	case CIS_1000HALF:
		axlocal.RealMediaMode = MEDIA_1000HALF;
		break;

	case CIS_100FULL:
		axlocal.RealMediaMode = MEDIA_100FULL;
		break;

	case CIS_100HALF:
		axlocal.RealMediaMode = MEDIA_100HALF;
		break;

	case CIS_10FULL:
		axlocal.RealMediaMode = MEDIA_10FULL;
		break;

	case CIS_10HALF:
		axlocal.RealMediaMode = MEDIA_10HALF;
		break;
	}

	return;
}

/*
 * ===========================================================================
 * <<<<<<             Exported SubProgram Bodies                   >>>>>>
 * ===========================================================================
 */

void eth_halt(void)
{
	/* Disable AX88180 TX/RX functions */
	WRITE_MACREG(CMD, WAKEMOD);
}

void ax88180_get_enetaddr(uchar * addr)
{
	unsigned long macid0_val, macid1_val, macid2_val;
	unsigned long tmp_regval;
	unsigned char env_enetaddr[6];
	unsigned char *tmp = getenv("ethaddr");
	char *end;
	int i;

#if defined(CONFIG_S3C2440A_SMDK)
	/* 32-bit mode */
	BWSCON = (BWSCON & ~(0xf << 4)) | (0x02 << 4);
	BANKCON1 = BANKCON1_14CLKS;
#endif

	for (i = 0; i < 6; i++) {
		env_enetaddr[i] = tmp ? simple_strtoul(tmp, &end, 16) : 0;
		if (tmp)
			tmp = (*end) ? end + 1 : end;
	}

	RESET_MAC;

	/* Disable AX88180 interrupt */
	DISABLE_INTERRUPT;

	/* Disable AX88180 TX/RX functions */
	WRITE_MACREG(CMD, WAKEMOD);

	/* Reload MAC address from EEPROM */
	WRITE_MACREG(PROMCTRL, RELOAD_EEPROM);
	for (i = 0; i < 500; i++) {
		READ_MACREG(PROMCTRL, tmp_regval);
		if ((tmp_regval & RELOAD_EEPROM) == 0)
			break;
		mdelay(1);
	}

	/* Get MAC addresses */
	READ_MACREG(MACID0, macid0_val);
	READ_MACREG(MACID1, macid1_val);
	READ_MACREG(MACID2, macid2_val);
	if ((macid0_val | macid1_val | macid2_val) != 0) {
		*addr = (unsigned char)macid0_val;
		*(addr + 1) = (unsigned char)(macid0_val >> 8);
		*(addr + 2) = (unsigned char)macid1_val;
		*(addr + 3) = (unsigned char)(macid1_val >> 8);
		*(addr + 4) = (unsigned char)macid2_val;
		*(addr + 5) = (unsigned char)(macid2_val >> 8);
	} else {
		/* No EEPROM here!! Set MAC address from environment. */
		for (i = 0; i < 6; i++)
			addr[i] = env_enetaddr[i];
	}

	printf("MAC: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x",
	       *addr, *(addr + 1), *(addr + 2), *(addr + 3), *(addr + 4),
	       *(addr + 5));
	printf("\n");

}

int eth_init(bd_t *bd)
{
	unsigned long tmp_regval;
	unsigned long macid0_val, macid1_val, macid2_val;
	unsigned short tmp16;
#if defined(CONFIG_S3C2440A_SMDK)
	/* 32-bit mode */
	BWSCON = (BWSCON & ~(0xf << 4)) | (0x02 << 4);
	BANKCON1 = BANKCON1_14CLKS;
#endif

#ifdef CONFIG_DRIVER_AX88180_16BIT
	WRITE_MACREG(0xDD00, 0);
	WRITE_MACREG(0xDD06, 0x10);
	WRITE_MACREG(0xDD00, 1);
#endif
	memset(&axlocal, 0, sizeof(AX88180_PRIVATE));

	RESET_MAC;

	/* Disable AX88180 interrupt */
	DISABLE_INTERRUPT;

	/* Disable AX88180 TX/RX functions */
	WRITE_MACREG(CMD, WAKEMOD);

	tmp16 = bd->bi_enetaddr[1];
	macid0_val = (tmp16 << 8) | bd->bi_enetaddr[0];

	tmp16 = bd->bi_enetaddr[3];
	macid1_val = (tmp16 << 8) | bd->bi_enetaddr[2];

	tmp16 = bd->bi_enetaddr[5];
	macid2_val = (tmp16 << 8) | bd->bi_enetaddr[4];

	WRITE_MACREG(MACID0, macid0_val);
	WRITE_MACREG(MACID1, macid1_val);
	WRITE_MACREG(MACID2, macid2_val);

	/* Initial PHY registers */
	ax88180_PHY_initial();

	/* Configure MAC media mode registers */
	ax88180_meida_config();

	WRITE_MACREG(RXFILTER, DEFAULT_RXFILTER);

	/* Initial variables here */
	INIT_TXRX_VARIABLES;

	DISPLAY_ALLMACREG;

	/* Check if there is any invalid interrupt status. If yes, clear it. */
	READ_MACREG(ISR, tmp_regval);
	PRINTK(INIT_MSG, "ax88180: The interrupt status = 0x%08lx\n",
	       tmp_regval);
	if (tmp_regval)
		WRITE_MACREG(ISR, tmp_regval);

	/* Start AX88180 TX/RX functions */
	WRITE_MACREG(CMD, RXEN | TXEN | WAKEMOD);

	return 0;
}

/* Get a data block via Ethernet */
extern int eth_rx(void)
{
	unsigned long ISR_Status;
	unsigned long rxcurt_ptr, rxbound_ptr;
	unsigned long bmsr_val;
	unsigned long tmp_regval;
	int i;

	/* Read and check interrupt status here...... */
	READ_MACREG(ISR, ISR_Status);

	while (ISR_Status) {
		/* Clear the interrupt status */
		WRITE_MACREG(ISR, ISR_Status);

		PRINTK(INT_MSG,
		       "\n\rax88180: The interrupt status = 0x%08lx\n\r",
		       ISR_Status);

		if (ISR_Status & ISR_PHY) {
			/* Read ISR register once to clear Marvell PHY interrupt bit */
			READ_PHYREG(axlocal.PhyAddr, M88_ISR, tmp_regval);

			/* Waiting 200 msecs for PHY link stable */
			for (i = 0; i < 200; i++) {
				READ_PHYREG(axlocal.PhyAddr, BMSR, bmsr_val);
				if (bmsr_val & LINKOK) {
					break;
				}
				mdelay(1);
			}

			if (bmsr_val & LINKOK) {
				PRINTK(WARNING_MSG,
				       "ax88180: The cable is connected.\n");
				ax88180_meida_config();
			} else {
				PRINTK(WARNING_MSG,
				       "ax88180: The cable is disconnected.\n");
			}
			DISPLAY_ALLPHYREG;
		}

		if (ISR_Status & ISR_RXBUFFOVR) {
			axlocal.rxbuf_overflow_count++;
			READ_MACREG(RXCURT, rxcurt_ptr);
			READ_MACREG(RXBOUND, rxbound_ptr);
			PRINTK(ERROR_MSG,
			       "ax88180: RX Buffer overflow!! (count=%d, RXBOUND=0x%08lx, RXCURT=0x%08lx)\n",
			       (int)axlocal.rxbuf_overflow_count, rxbound_ptr,
			       rxcurt_ptr);
			PRINTK(ERROR_MSG,
			       "ax88180: The interrupt status = 0x%08lx\n",
			       ISR_Status);

			if (axlocal.rxbuf_overflow_count > 10) {
				RESET_MAC;
				INIT_TXRX_VARIABLES;
			}
		}

		if (ISR_Status & ISR_RX) {
			ax88180_rx_handler();
		}

		/* Read and check interrupt status here...... */
		READ_MACREG(ISR, ISR_Status);
	}

	return 0;
}

/* Send a data block via Ethernet. */
extern int eth_send(volatile void *packet, int length)
{
	volatile unsigned char *txdata;
	unsigned long TXDES_addr;
	unsigned long txcmd_txdp, txbs_txdp;
	/* unsigned long txdes0_val, txdes1_val, txdes2_val, txdes3_val; */
	unsigned long tmp_data;
	int i;

	txdata = (volatile unsigned char *)packet;

	axlocal.FirstTxDesc = axlocal.NextTxDesc;
	txbs_txdp = 1 << axlocal.FirstTxDesc;

	/* allan9 add to make sure TX machine is OK */
	i = 0;
	READ_MACREG(TXBS, tmp_data);
	READ_MACREG(TXBS, tmp_data);
	PRINTK(TX_MSG, "ax88180: Checking available TXDP (TXBS=0x%08lx)\n",
	       tmp_data);
	while (tmp_data & txbs_txdp) {
		axlocal.NextTxDesc++;
		axlocal.NextTxDesc &= TXDP_MASK;
		axlocal.FirstTxDesc = axlocal.NextTxDesc;
		txbs_txdp = 1 << axlocal.FirstTxDesc;
		READ_MACREG(TXBS, tmp_data);
		i++;

		if (i > 1000) {
			RESET_MAC;
			axlocal.NextTxDesc = TXDP0;
			axlocal.FirstTxDesc = axlocal.NextTxDesc;
			txbs_txdp = 1 << axlocal.FirstTxDesc;
			READ_MACREG(TXBS, tmp_data);
			i = 0;
			PRINTK(ERROR_MSG, "ax88180: No available TXDP!!\n");
		}
	}

	PRINTK(TX_MSG, "ax88180: TXDP%d is available, i=%d\n",
	       (int)axlocal.FirstTxDesc, i);
	/* allan9 end */

	txcmd_txdp = axlocal.FirstTxDesc << 13;
	TXDES_addr = TXDES0 + (axlocal.FirstTxDesc << 2);

	WRITE_MACREG(TXCMD, txcmd_txdp | length | TX_START_WRITE);

#ifdef CONFIG_DRIVER_AX88180_16BIT
	for (i = 0; i < length; i += 2) {
		tmp_data =
		    (unsigned short)*(txdata + i) +
		    (unsigned short)(*(txdata + i + 1) << 8);
		WRITE_TXBUF(tmp_data);
	}
#else
	for (i = 0; i < length; i += 4) {
		tmp_data =
		    (unsigned long)*(txdata + i) +
		    (unsigned long)(*(txdata + i + 1) << 8) +
		    (unsigned long)(*(txdata + i + 2) << 16) +
		    (unsigned long)(*(txdata + i + 3) << 24);
		WRITE_TXBUF(tmp_data);
	}
#endif

	WRITE_MACREG(TXCMD, txcmd_txdp | length);
	WRITE_MACREG(TXBS, txbs_txdp);
	WRITE_MACREG(TXDES_addr, TXDPx_ENABLE | length);

	axlocal.NextTxDesc++;

	axlocal.NextTxDesc &= TXDP_MASK;

	return 0;
}

#endif				/* COMMANDS & CFG_NET */
#endif				/* CONFIG_DRIVER_AX88180 */
