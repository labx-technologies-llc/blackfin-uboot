/* ax88180.h: ASIX AX88180 Non-PCI Gigabit Ethernet u-boot driver */
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

#include <asm/types.h>
#include <config.h>

#ifdef CONFIG_DRIVER_AX88180

#define PLATFORM_MEMBASE		0x08000000

#define ENABLE_JUMBO			1
#define	DISABLE_JUMBO			0

#define	DEFAULT_ETH_MTU			1500
#define	MAX_JUMBO_MTU			4072	/* Jumbo packet size 4086 bytes included 4 bytes CRC */
#define	MAX_TX_JUMBO_SIZE		4086	/* Max Tx Jumbo size 4086 bytes included 4 bytes CRC */
#define	MAX_RX_SIZE			0x3C00	/* Max Rx Jumbo size is 15K Bytes */

#define AX88180_MEMORY_SIZE		0x00010000

#define	MARVELL_88E1111_PHYADDR		0x18
#define	MARVELL_88E1111_PHYIDR0		0x0141

#define	CICADA_CIS8201_PHYADDR		0x01
#define	CICADA_CIS8201_PHYIDR0		0x000F

#define	MEDIA_AUTO			0
#define	MEDIA_1000FULL			1
#define	MEDIA_1000HALF			2
#define	MEDIA_100FULL			3
#define	MEDIA_100HALF			4
#define	MEDIA_10FULL			5
#define	MEDIA_10HALF			6

#define	AUTO_MEDIA			0
#define	FORCE_MEDIA			1

#define	TXDP_MASK			3
#define	TXDP0				0
#define	TXDP1				1
#define	TXDP2				2
#define	TXDP3				3

/* AX88180 Memory Mapping Definition */
#define	RXBUFFER_START			0x0000
#define	RX_PACKET_LEN_OFFSET		0
#define	RX_PAGE_NUM_MASK		0x7FF	/* RX pages 0~7FFh */
#define TXBUFFER_START			0x8000

/* AX88180 MAC Register Definition */
#define	CMD		0xFC00
#define	WAKEMOD				0x00000001
#define	TXEN				0x00000100
#define	RXEN				0x00000200
#define	DEFAULT_CMD			WAKEMOD
#define	IMR		0xFC04
#define	IMR_RXBUFFOVR			0x00000001
#define	IMR_WATCHDOG			0x00000002
#define	IMR_TX				0x00000008
#define	IMR_RX				0x00000010
#define	IMR_PHY				0x00000020
#define	CLEAR_IMR			0x00000000
#define	DEFAULT_IMR			(IMR_PHY | IMR_RX | IMR_RXBUFFOVR)
#define	ISR		0xFC08
#define	ISR_RXBUFFOVR			0x00000001
#define	ISR_WATCHDOG			0x00000002
#define	ISR_TX				0x00000008
#define	ISR_RX				0x00000010
#define	ISR_PHY				0x00000020
#define	TXCFG		0xFC10
#define	AUTOPAD_CRC			0x00000040
#define	DEFAULT_TXCFG			AUTOPAD_CRC
#define	TXCMD		0xFC14
#define	TXCMD_TXDP_MASK			0x00006000
#define	TXCMD_TXDP0			0x00000000
#define	TXCMD_TXDP1			0x00002000
#define	TXCMD_TXDP2			0x00004000
#define	TXCMD_TXDP3			0x00006000
#define	TX_START_WRITE			0x00008000
#define	TX_STOP_WRITE			0x00000000
#define	DEFAULT_TXCMD			0x00000000
#define	TXBS		0xFC18
#define	TXDP0_USED			0x00000001
#define	TXDP1_USED			0x00000002
#define	TXDP2_USED			0x00000004
#define	TXDP3_USED			0x00000008
#define	DEFAULT_TXBS			0x00000000
#define	TXDES0		0xFC20
#define	TXDPx_ENABLE			0x00008000
#define	TXDPx_LEN_MASK			0x00001FFF
#define	DEFAULT_TXDES0			0x00000000
#define	TXDES1		0xFC24
#define	TXDPx_ENABLE			0x00008000
#define	TXDPx_LEN_MASK			0x00001FFF
#define	DEFAULT_TXDES1			0x00000000
#define	TXDES2		0xFC28
#define	TXDPx_ENABLE			0x00008000
#define	TXDPx_LEN_MASK			0x00001FFF
#define	DEFAULT_TXDES2			0x00000000
#define	TXDES3		0xFC2C
#define	TXDPx_ENABLE			0x00008000
#define	TXDPx_LEN_MASK			0x00001FFF
#define	DEFAULT_TXDES3			0x00000000
#define	RXCFG		0xFC30
#define 	RXBUFF_PROTECT		0x00000001
#define	RXTCPCRC_CHECK			0x00000010
#define	RXFLOW_ENABLE			0x00000100
#define	DEFAULT_RXCFG			RXBUFF_PROTECT
#define	RXCURT		0xFC34
#define	DEFAULT_RXCURT			0x00000000
#define	RXBOUND		0xFC38
#define	DEFAULT_RXBOUND			0x000007FF
#define	MACCFG0		0xFC40
#define	MACCFG0_BIT3_0			0x00000007
#define	IPGT_VAL			0x00000150
#define	TXFLOW_ENABLE			0x00001000
#define	SPEED100			0x00008000
#define	DEFAULT_MACCFG0			(IPGT_VAL | MACCFG0_BIT3_0)
#define	MACCFG1		0xFC44
#define	RGMII_EN			0x00000002
#define	RXFLOW_EN			0x00000020
#define	FULLDUPLEX			0x00000040
#define	MAX_JUMBO_LEN_MASK		0x00000780
#define	RXJUMBO_EN			0x00000800
#define	GIGA_MODE_EN			0x00001000
#define	RXCRC_CHECK			0x00002000
#define	RXPAUSE_DA_CHECK		0x00004000

#define	JUMBO_LEN_4K			0x00000200
#define	JUMBO_LEN_15K			0x00000780
#define	DEFAULT_MACCFG1			(RXCRC_CHECK | RXPAUSE_DA_CHECK | RGMII_EN)
#define	MACCFG2		0xFC48
#define	MACCFG2_BIT15_8			0x00000100
#define	JAM_LIMIT_MASK			0x000000FC
#define	DEFAULT_JAM_LIMIT		0x00000064
#define	DEFAULT_MACCFG2			MACCFG2_BIT15_8
#define	MACCFG3		0xFC4C
#define	IPGR2_VAL			0x0000000E
#define	IPGR1_VAL			0x00000600
#define	NOABORT				0x00008000
#define	DEFAULT_MACCFG3			(IPGR1_VAL | IPGR2_VAL)
#define	TXPAUT		0xFC54
#define	DEFAULT_TXPAUT			0x001FE000
#define	RXBTHD0		0xFC58
#define	DEFAULT_RXBTHD0			0x00000300
#define	RXBTHD1		0xFC5C
#define	DEFAULT_RXBTHD1			0x00000600
#define	RXFULTHD	0xFC60
#define	DEFAULT_RXFULTHD		0x00000100
#define	MISC		0xFC68
#define	MISC_NORMAL			0x00000003	/* Normal operation mode */
#define	MISC_RESET_MAC			0x00000002	/* Clear bit 0 to reset MAC */
#define	MISC_RESET_PHY			0x00000001	/* Clear bit 1 to reset PHY */
#define	MISC_RESET_MAC_PHY		0x00000000	/* Clear bit 0 and 1 to reset MAC and PHY */
#define	DEFAULT_MISC			MISC_NORMAL
#define	MACID0		0xFC70
#define	MACID1		0xFC74
#define	MACID2		0xFC78
#define	TXLEN		0xFC7C
#define	DEFAULT_TXLEN			0x000005FC
#define	RXFILTER	0xFC80
#define	RX_RXANY			0x00000001
#define	RX_MULTICAST			0x00000002
#define	RX_UNICAST			0x00000004
#define	RX_BROADCAST			0x00000008
#define	RX_MULTI_HASH			0x00000010
#define	DISABLE_RXFILTER		0x00000000
#define	DEFAULT_RXFILTER		(RX_BROADCAST + RX_UNICAST + RX_MULTICAST)
#define	MDIOCTRL	0xFC84
#define	PHY_ADDR_MASK			0x0000001F
#define	REG_ADDR_MASK			0x00001F00
#define	READ_PHY			0x00004000
#define WRITE_PHY			0x00008000
#define	MDIODP		0xFC88
#define	GPIOCTRL	0xFC8C
#define	RXINDICATOR	0xFC90
#define	RX_START_READ			0x00000001
#define	RX_STOP_READ			0x00000000
#define	DEFAULT_RXINDICATOR		RX_STOP_READ
#define	TXST		0xFC94
#define	MDCCLKPAT	0xFCA0
#define	RXIPCRCCNT	0xFCA4
#define	RXCRCCNT	0xFCA8
#define	TXFAILCNT	0xFCAC
#define	PROMDP		0xFCB0
#define	PROMCTRL	0xFCB4
#define	RELOAD_EEPROM			0x00000200
#define	MAXRXLEN	0xFCB8
#define	HASHTAB0	0xFCC0
#define	HASHTAB1	0xFCC4
#define	HASHTAB2	0xFCC8
#define	HASHTAB3	0xFCCC
#define	DOGTHD0		0xFCE0
#define	DEFAULT_DOGTHD0			0x0000FFFF
#define	DOGTHD1		0xFCE4
#define	START_WATCHDOG_TIMER		0x00008000
#define	DEFAULT_DOGTHD1			0x00000FFF
#define	SOFTRST		0xFCEC
#define	SOFTRST_NORMAL			0x00000003
#define	SOFTRST_RESET_MAC		0x00000002

/* External PHY Register Definition */
#define	BMCR				0x0000
#define	LINE_SPEED_MSB			0x0040
#define	DUPLEX_MODE			0x0100
#define	RESTART_AUTONEG			0x0200
#define	POWER_DOWN			0x0800
#define	AUTONEG_EN			0x1000
#define	LINE_SPEED_LSB			0x2000
#define PHY_RESET			0x8000

#define	MEDIAMODE_MASK			(LINE_SPEED_MSB | LINE_SPEED_LSB | DUPLEX_MODE)
#define	BMCR_SPEED_1000			LINE_SPEED_MSB
#define	BMCR_SPEED_100			LINE_SPEED_LSB
#define	BMCR_SPEED_10			0x0000

#define	BMCR_1000FULL			(BMCR_SPEED_1000 | DUPLEX_MODE)
#define	BMCR_100FULL			(BMCR_SPEED_100 | DUPLEX_MODE)
#define	BMCR_100HALF			BMCR_SPEED_100
#define	BMCR_10FULL			DUPLEX_MODE
#define	BMCR_10HALF			0x0000
#define	BMSR				0x0001
#define	LINKOK				0x0004
#define	AUTONEG_ENABLE_STS		0x0008
#define AUTONEG_COMPLETE		0x0020
#define	PHYIDR0				0x0002
#define PHYIDR1				0x0003
#define	ANAR				0x0004
#define ANAR_PAUSE			0x0400
#define ANAR_100FULL			0x0100
#define ANAR_100HALF			0x0080
#define ANAR_10FULL			0x0040
#define ANAR_10HALF			0x0020
#define	ANAR_8023BIT			0x0001
#define	ANLPAR				0x0005
#define	ANER				0x0006
#define	AUX_1000_CTRL			0x0009
#define	ENABLE_1000HALF			0x0100
#define	ENABLE_1000FULL			0x0200
#define DEFAULT_AUX_1000_CTRL	(ENABLE_1000HALF | ENABLE_1000FULL)
#define	AUX_1000_STATUS			0x000A
#define	LP_1000HALF			0x0400
#define	LP_1000FULL			0x0800

/* Marvell 88E1111 Gigabit PHY Register Definition */
#define	M88_SSR				0x0011
#define	SSR_SPEED_MASK			0xC000
#define	SSR_SPEED_1000			0x8000
#define	SSR_SPEED_100			0x4000
#define	SSR_SPEED_10			0x0000
#define	SSR_DUPLEX			0x2000
#define	SSR_MEDIA_RESOLVED_OK		0x0800

#define	SSR_MEDIA_MASK			(SSR_SPEED_MASK | SSR_DUPLEX)
#define SSR_1000FULL			(SSR_SPEED_1000 | SSR_DUPLEX)
#define SSR_1000HALF			SSR_SPEED_1000
#define SSR_100FULL			(SSR_SPEED_100 | SSR_DUPLEX)
#define SSR_100HALF			SSR_SPEED_100
#define SSR_10FULL			(SSR_SPEED_10 | SSR_DUPLEX)
#define SSR_10HALF			SSR_SPEED_10
#define	M88_IER				0x0012
#define	LINK_CHANGE_INT			0x0400
#define	M88_ISR				0x0013
#define	LINK_CHANGE_STATUS		0x0400
#define	M88_EXT_SCR			0x0014
#define	RGMII_RXCLK_DELAY		0x0080
#define	RGMII_TXCLK_DELAY		0x0002
#define	DEFAULT_EXT_SCR			(RGMII_TXCLK_DELAY | RGMII_RXCLK_DELAY)
#define	M88_EXT_SSR			0x001B
#define	HWCFG_MODE_MASK			0x000F
#define	RGMII_COPPER_MODE		0x000B

/* CICADA CIS8201 Gigabit PHY Register Definition */
#define	CIS_IMR				0x0019
#define	CIS_INT_ENABLE			0x8000
#define	CIS_LINK_CHANGE_INT		0x2000
#define	CIS_ISR				0x001A
#define	CIS_INT_PENDING			0x8000
#define	CIS_LINK_CHANGE_STATUS		0x2000
#define	CIS_AUX_CTRL_STATUS		0x001C
#define	CIS_AUTONEG_COMPLETE		0x8000
#define	CIS_SPEED_MASK			0x0018
#define	CIS_SPEED_1000			0x0010
#define	CIS_SPEED_100			0x0008
#define	CIS_SPEED_10			0x0000
#define	CIS_DUPLEX			0x0020

#define	CIS_MEDIA_MASK			(CIS_SPEED_MASK | CIS_DUPLEX)
#define CIS_1000FULL			(CIS_SPEED_1000 | CIS_DUPLEX)
#define CIS_1000HALF			CIS_SPEED_1000
#define CIS_100FULL			(CIS_SPEED_100 | CIS_DUPLEX)
#define CIS_100HALF			CIS_SPEED_100
#define CIS_10FULL			(CIS_SPEED_10 | CIS_DUPLEX)
#define CIS_10HALF			CIS_SPEED_10
#define	CIS_SMI_PRIORITY		0x0004

/* SMDK2440 Registers Definition */
/* SMDK2440 default clocks: FCLK=400MHZ, HCLK=125MHZ, PCLK=62.5MHZ */
#define	CLKDIVN_125MHZ			0x0000000F	/* Set HCLK=FCLK/3, PCLK=HCLK/2 when CAMDIVN[8]=0 */
#define	CAMDIVN_125MHZ			0x00000000	/* Set HCLK=FCLK/3, PCLK=HCLK/2 when CAMDIVN[8]=0 */
#define UBRDIV0_125MHZ			0x00000023	/* Set UART Baud Rate divisor for 125MHZ HCLK */
#define	CLKDIVN_100MHZ			0x0000000D	/* Set HCLK=FCLK/4, PCLK=HCLK/2 when CAMDIVN[9]=0 */
#define	CAMDIVN_100MHZ			0x00000000	/* Set HCLK=FCLK/4, PCLK=HCLK/2 when CAMDIVN[9]=0 */
#define UBRDIV0_100MHZ			0x0000001B	/* Set UART Baud Rate divisor for 100MHZ HCLK */
#define	CLKDIVN_50MHZ			0x0000000D	/* Set HCLK=FCLK/8, PCLK=HCLK/2 when CAMDIVN[9]=1 */
#define	CAMDIVN_50MHZ			0x00000200	/* Set HCLK=FCLK/8, PCLK=HCLK/2 when CAMDIVN[9]=1 */
#define UBRDIV0_50MHZ			0x0000000D	/* Set UART Baud Rate divisor for 50MHZ HCLK */
#define	BANKCON1_4CLKS			0x00000300	/* Set Bank 1 access timing 4 clocks for AX88180 US1 */
#define	BANKCON1_6CLKS			0x00000400	/* Set Bank 1 access timing 6 clocks for AX88180 US2 */
#define	BANKCON1_6CLKS_PAGE		0x0000040F	/* Set Bank 1 access timing 6 clocks with page access for AX88180 US2 */
#define	BANKCON1_14CLKS			0x00000700	/* Set Bank 1 access timing 14 clocks (SMDK2440 H/W default) */

/* EINTMASK Register Bit Definition */
#define	EINT11_MASK			0x00000800	/* Clear this bit to enable EINT11 interrupt */

/* EXTINT1 Register Bit Definition */
#define	FLTEN11				0x00008000	/* Enable EINT11 signal noise filter */

/* Debug Message Display Level Definition */
#define	DRIVER_MSG			0x0001
#define	INIT_MSG			0x0002
#define	TX_MSG				0x0004
#define	RX_MSG				0x0008
#define INT_MSG				0x0010
#define	ERROR_MSG			0x0020
#define	WARNING_MSG			0x0040
#define	DEBUG_MSG			0x0080
#define	OTHERS_MSG			0x0100
#define	ALL_MSG				0x01FF
#define	NO_MSG				0x0000
#define	DEBUG_FLAGS			(ERROR_MSG)

#endif	/*end of CONFIG_DRIVER_AX88180 */
