/*
 * Copyright (c) 2003 by BuyWays B.V. (www.buyways.nl)
 * Copyright (c) 2000, 2001 by Lineo, Inc./Lineo Canada Corp. (www.lineo.com),
 * Copyright (c) 2001,2002 by Arcturus Networks Inc. (www.arcturusnetworks.com),
 * Ported for Blackfin/Bfin Architecture by 
 *					Akbar Hussain  <akbar.hussain@arcturusnetworks.com>,
 *             Tony Kou <tony.ko@arcturusnetworks.com>
 *					MaTed <mated@sympatico.ca> <mated@arcturusnetworks.com>
 * Ported to the BlackFin BF533 by Bas Vermeulen <bas@buyways.nl>
 * Tab Size == 4 ....MaTed
 *
 */

#ifndef _BF533_H
#define _BF533_H_

#include <config.h>

#define LO(con32) ((con32) & 0xFFFF)
#define lo(con32) ((con32) & 0xFFFF)
#define HI(con32) (((con32) >> 16) & 0xFFFF)
#define hi(con32) (((con32) >> 16) & 0xFFFF)

#define OFFSET_( x ) ((x) & 0x0000FFFF) /* define macro for offset */

/* Event Vector Table Address */
#define EVT_EMULATION_ADDR	0xffe02000
#define EVT_RESET_ADDR		0xffe02004
#define EVT_NMI_ADDR		0xffe02008
#define EVT_EXCEPTION_ADDR	0xffe0200c
#define EVT_GLOBAL_INT_ENB_ADDR	0xffe02010
#define EVT_HARDWARE_ERROR_ADDR	0xffe02014
#define EVT_TIMER_ADDR		0xffe02018
#define EVT_IVG7_ADDR		0xffe0201c
#define EVT_IVG8_ADDR		0xffe02020
#define EVT_IVG9_ADDR		0xffe02024
#define EVT_IVG10_ADDR		0xffe02028
#define EVT_IVG11_ADDR		0xffe0202c
#define EVT_IVG12_ADDR		0xffe02030
#define EVT_IVG13_ADDR		0xffe02034
#define EVT_IVG14_ADDR		0xffe02038
#define EVT_IVG15_ADDR		0xffe0203c
#define EVT_OVERRIDE_ADDR	0xffe02100

/* PLL Registers */
#define PLL_CTL			0xFFC00000	/* PLL Control register (16-bit) */
#define PLL_DIV			0xFFC00004	/* PLL Divide Register (16-bit) */
#define VR_CTL			0xFFC00008	/* Voltage Regulator Control Register (16-bit) */
#define PLL_STAT		0xFFC0000C	/* PLL Status register (16-bit) */
#define PLL_LOCKCNT		0xFFC00010	/* PLL Lock Count register (16-bit) */
#define SWRST			0xFFC00100	/* Software Reset Register (16-bit) */
#define SYSCR			0xFFC00104	/* System Configuration register */

/* System Interrupt Controller */
#define SIC_RVECT		0xFFC00108	/* Interrupt Reset Vector Address Register */
#define SIC_IMASK		0xFFC0010C	/* Interrupt Mask Register */
#define SIC_IAR0		0xFFC00110	/* Interrupt Assignment Register 0 */
#define SIC_IAR1		0xFFC00114	/* Interrupt Assignment Register 1 */
#define SIC_IAR2		0xFFC00118	/* Interrupt Assignment Register 2 */
#define SIC_ISR			0xFFC00120	/* Interrupt Status Register */
#define SIC_IWR			0xFFC00124	/* Interrupt Wakeup Register */

/* Watch Dog Timer Register */
#define WDOG_CTL		0xFFC00200	/* Watchdog Control Register */
#define WDOG_CNT		0xFFC00204	/* Watchdog Count Register */
#define WDOG_STAT		0xFFC00208	/* Watchdog Status Register */

#define WDOG_TMR_DISABLE	(0xAD << 4)
#define ICTL_RST		0x00000000
#define ICTL_NMI		0x00000002
#define ICTL_GP			0x00000004
#define ICTL_DISABLE		0x00000003

/* RTC Registers */
#define RTC_STAT		0xFFC00300	/* RTC Status Register */
#define RTC_ICTL		0xFFC00304	/* RTC Interrupt Control egister */
#define RTC_ISTAT		0xFFC00308	/* RTC Interrupt Status Register */
#define RTC_SWCNT		0xFFC0030C	/* RTC Stopwatch Count Register */
#define RTC_ALARM		0xFFC00310	/* RTC Alarm Time Register */
#define RTC_FAST		0xFFC00314	/* RTC Prescaler Enable Register */
#define RTC_PREN		0xFFC00314	/* RTC Prescaler Enable Register (alternate macro) */

/* Serial Peripheral Interface */
#define SPI_CTL			0xFFC00500	/* SPI Control Register */
#define SPI_FLG			0xFFC00504	/* SPI Flag register */
#define SPI_STAT		0xFFC00508	/* SPI Status register */
#define SPI_TDBR		0xFFC0050C	/* SPI Transmit Data Buffer Register */
#define SPI_RDBR		0xFFC00510	/* SPI Receive Data Buffer Register */
#define SPI_BAUD		0xFFC00514	/* SPI Baud rate Register */
#define SPI_SHADOW		0xFFC00518	/* SPI_RDBR Shadow Register */

/* General Purpose IO (0xFFC00700 - 0xFFC007FF) */
#define FIO_FLAG_D		0xFFC00700	/* Flag Mask to directly specify state of pins */
#define FIO_FLAG_C		0xFFC00704	/* Peripheral Interrupt Flag Register (clear) */
#define FIO_FLAG_S		0xFFC00708	/* Peripheral Interrupt Flag Register (set) */
#define FIO_FLAG_T		0xFFC0070C	/* Flag Mask to directly toggle state of pins */
#define FIO_MASKA_D		0xFFC00710	/* Flag Mask Interrupt A Register (set directly) */
#define FIO_MASKA_C		0xFFC00714	/* Flag Mask Interrupt A Register (clear) */
#define FIO_MASKA_S		0xFFC00718	/* Flag Mask Interrupt A Register (set) */
#define FIO_MASKA_T		0xFFC0071C	/* Flag Mask Interrupt A Register (toggle) */
#define FIO_MASKB_D		0xFFC00720	/* Flag Mask Interrupt B Register (set directly) */
#define FIO_MASKB_C		0xFFC00724	/* Flag Mask Interrupt B Register (clear) */
#define FIO_MASKB_S		0xFFC00728	/* Flag Mask Interrupt B Register (set) */
#define FIO_MASKB_T		0xFFC0072C	/* Flag Mask Interrupt B Register (toggle) */
#define FIO_DIR			0xFFC00730	/* Peripheral Flag Direction Register */
#define FIO_POLAR		0xFFC00734	/* Flag Source Polarity Register */
#define FIO_EDGE		0xFFC00738	/* Flag Source Sensitivity Register */
#define FIO_BOTH		0xFFC0073C	/* Flag Set on BOTH Edges Register */
#define FIO_INEN		0xFFC00740	/* Flag Input Enable Register */

/* Asynchronous Memory Controller - External Bus Interface Unit */
#define EBIU_AMGCTL		0xFFC00A00	/* Asynchronous Memory Global Control Register */
#define EBIU_AMBCTL0		0xFFC00A04	/* Asynchronous Memory Bank Control Register 0 */
#define EBIU_AMBCTL1		0xFFC00A08	/* Asynchronous Memory Bank Control Register 1 */

/* SDRAM Controller External Bus Interface Unit (0xFFC00A00 - 0xFFC00AFF) */
#define EBIU_SDGCTL		0xFFC00A10	/* SDRAM Global Control Register */
#define EBIU_SDBCTL		0xFFC00A14	/* SDRAM Bank Control Register */
#define EBIU_SDRRC		0xFFC00A18	/* SDRAM Refresh Rate Control Register */
#define EBIU_SDSTAT		0xFFC00A1C	/* SDRAM Status Register */

/* PLL_CTL Masks */
#define PLL_CLKIN              	0x00000000	/* Pass CLKIN to PLL */
#define PLL_CLKIN_DIV2         	0x00000001	/* Pass CLKIN/2 to PLL */
#define PLL_OFF                	0x00000002	/* Shut off PLL clocks */
#define STOPCK_OFF             	0x00000008	/* Core clock off */
#define PDWN                   	0x00000020	/* Put the PLL in a Deep Sleep state */
#define BYPASS                 	0x00000100	/* Bypass the PLL */

/* PLL_DIV Masks */
#define SCLK_DIV(x)		(x)	/* SCLK = VCO / x */
#define CCLK_DIV1		0x00000000	/* CCLK = VCO / 1 */
#define CCLK_DIV2		0x00000010	/* CCLK = VCO / 2 */
#define CCLK_DIV4		0x00000020	/* CCLK = VCO / 4 */
#define CCLK_DIV8		0x00000030	/* CCLK = VCO / 8 */

/* SWRST Mask */
#define SYSTEM_RESET		0x00000007	/* Initiates a system software reset */

/* RTC_STAT and RTC_ALARM register */
#define	RTSEC			0x0000003F	/* Real-Time Clock Seconds */
#define	RTMIN			0x00000FC0	/* Real-Time Clock Minutes */
#define	RTHR			0x0001F000	/* Real-Time Clock Hours */
#define	RTDAY			0xFFFE0000	/* Real-Time Clock Days */

/* RTC_FAST Mask (RTC_PREN Mask) */
#define ENABLE_PRESCALE		0x00000001	/* Enable prescaler so RTC runs at 1 Hz */
#define PREN			0x00000001

/* TIMER_ENABLE Register */
#define TIMEN0			0x0001
#define TIMEN1			0x0002
#define TIMEN2			0x0004
#define TIMEN0_P		0x00
#define TIMEN1_P		0x01
#define TIMEN2_P		0x02

/* TIMER_DISABLE Register */
#define TIMDIS0			0x0001
#define TIMDIS1			0x0002
#define TIMDIS2			0x0004
#define TIMDIS0_P		0x00
#define TIMDIS1_P		0x01
#define TIMDIS2_P		0x02

/* TIMER_STATUS Register */
#define TIMIL0			0x0001
#define TIMIL1			0x0002
#define TIMIL2			0x0004
#define TOVL_ERR0		0x0010
#define TOVL_ERR1		0x0020
#define TOVL_ERR2		0x0040
#define TRUN0			0x1000
#define TRUN1			0x2000
#define TRUN2			0x4000
#define TIMIL0_P		0x00
#define TIMIL1_P		0x01
#define TIMIL2_P		0x02
#define TOVL_ERR0_P		0x04
#define TOVL_ERR1_P		0x05
#define TOVL_ERR2_P		0x06
#define TRUN0_P			0x0C
#define TRUN1_P			0x0D
#define TRUN2_P			0x0E

/* TIMERx_CONFIG Registers */
#define PWM_OUT			0x0001
#define WDTH_CAP		0x0002
#define EXT_CLK			0x0003
#define PULSE_HI		0x0004
#define PERIOD_CNT		0x0008
#define IRQ_ENA			0x0010
#define TIN_SEL			0x0020
#define OUT_DIS			0x0040
#define CLK_SEL			0x0080
#define TOGGLE_HI		0x0100
#define EMU_RUN			0x0200
#define ERR_TYP(x)		((x & 0x03) << 14)
#define TMODE_P0		0x00
#define TMODE_P1		0x01
#define PULSE_HI_P		0x02
#define PERIOD_CNT_P		0x03
#define IRQ_ENA_P		0x04
#define TIN_SEL_P		0x05
#define OUT_DIS_P		0x06
#define CLK_SEL_P		0x07
#define TOGGLE_HI_P		0x08
#define EMU_RUN_P		0x09
#define ERR_TYP_P0		0x0E
#define ERR_TYP_P1		0x0F
#define PF0			0x0001
#define PF1			0x0002
#define PF2			0x0004
#define PF3			0x0008
#define PF4			0x0010
#define PF5			0x0020
#define PF6			0x0040
#define PF7			0x0080
#define PF8			0x0100
#define PF9			0x0200
#define PF10			0x0400
#define PF11			0x0800
#define PF12			0x1000
#define PF13			0x2000
#define PF14			0x4000
#define PF15			0x8000

/*  General Purpose IO (0xFFC00700 - 0xFFC007FF)  BIT POSITIONS */
#define PF0_P			0
#define PF1_P			1
#define PF2_P			2
#define PF3_P			3
#define PF4_P			4
#define PF5_P			5
#define PF6_P			6
#define PF7_P			7
#define PF8_P			8
#define PF9_P			9
#define PF10_P			10
#define PF11_P			11
#define PF12_P			12
#define PF13_P			13
#define PF14_P			14
#define PF15_P			15

/* ASYNCHRONOUS MEMORY CONTROLLER MASKS */
#define AMCKEN			0x00000001	/* Enable CLKOUT */
#define AMBEN_B0		0x00000002	/* Enable Asynchronous Memory Bank 0 only */
#define AMBEN_B0_B1		0x00000004	/* Enable Asynchronous Memory Banks 0 & 1 only */
#define AMBEN_B0_B1_B2		0x00000006	/* Enable Asynchronous Memory Banks 0, 1, and 2 */
#define AMBEN_ALL		0x00000008	/* Enable Asynchronous Memory Banks (all) 0, 1, 2, and 3 */

/* AMGCTL Bit Positions */
#define AMCKEN_P		0x00000000
#define AMBEN_P0		0x00000002
#define AMBEN_P1		0x00000004
#define AMBEN_P2		0x00000008

/* AMBCTL0 Masks */
#define B0RDYEN			0x00000001
#define B0RDYPOL		0x00000002
#define B0TT_1			0x00000004
#define B0TT_2			0x00000008
#define B0TT_3			0x0000000C
#define B0TT_4			0x00000000
#define B0ST_1			0x00000010
#define B0ST_2			0x00000020
#define B0ST_3			0x00000030
#define B0ST_4			0x00000000
#define B0HT_1			0x00000040
#define B0HT_2			0x00000080
#define B0HT_3			0x000000C0
#define B0HT_0			0x00000000
#define B0RAT_1			0x00000100
#define B0RAT_2			0x00000200
#define B0RAT_3			0x00000300
#define B0RAT_4			0x00000400
#define B0RAT_5			0x00000500
#define B0RAT_6			0x00000600
#define B0RAT_7			0x00000700
#define B0RAT_8			0x00000800
#define B0RAT_9			0x00000900
#define B0RAT_10		0x00000A00
#define B0RAT_11		0x00000B00
#define B0RAT_12		0x00000C00
#define B0RAT_13		0x00000D00
#define B0RAT_14		0x00000E00
#define B0RAT_15		0x00000F00
#define B0WAT_1			0x00001000
#define B0WAT_2			0x00002000
#define B0WAT_3			0x00003000
#define B0WAT_4			0x00004000
#define B0WAT_5			0x00005000
#define B0WAT_6			0x00006000
#define B0WAT_7			0x00007000
#define B0WAT_8			0x00008000
#define B0WAT_9			0x00009000
#define B0WAT_10		0x0000A000
#define B0WAT_11		0x0000B000
#define B0WAT_12		0x0000C000
#define B0WAT_13		0x0000D000
#define B0WAT_14		0x0000E000
#define B0WAT_15		0x0000F000
#define B1RDYEN			0x00010000
#define B1RDYPOL		0x00020000
#define B1TT_1			0x00040000
#define B1TT_2			0x00080000
#define B1TT_3			0x000C0000
#define B1TT_4			0x00000000
#define B1ST_1			0x00100000
#define B1ST_2			0x00200000
#define B1ST_3			0x00300000
#define B1ST_4			0x00000000
#define B1HT_1			0x00400000
#define B1HT_2			0x00800000
#define B1HT_3			0x00C00000
#define B1HT_0			0x00000000
#define B1RAT_1			0x01000000
#define B1RAT_2			0x02000000
#define B1RAT_3			0x03000000
#define B1RAT_4			0x04000000	/* Bank 1 Read Access Time = 4   cycles */
#define B1RAT_5			0x05000000	/* Bank 1 Read Access Time = 5   cycles */
#define B1RAT_6			0x06000000	/* Bank 1 Read Access Time = 6   cycles */
#define B1RAT_7			0x07000000	/* Bank 1 Read Access Time = 7   cycles */
#define B1RAT_8			0x08000000	/* Bank 1 Read Access Time = 8   cycles */
#define B1RAT_9			0x09000000	/* Bank 1 Read Access Time = 9   cycles */
#define B1RAT_10		0x0A000000	/* Bank 1 Read Access Time = 10  cycles */
#define B1RAT_11		0x0B000000	/* Bank 1 Read Access Time = 11  cycles */
#define B1RAT_12		0x0C000000	/* Bank 1 Read Access Time = 12  cycles */
#define B1RAT_13		0x0D000000	/* Bank 1 Read Access Time = 13  cycles */
#define B1RAT_14		0x0E000000	/* Bank 1 Read Access Time = 14  cycles */
#define B1RAT_15		0x0F000000	/* Bank 1 Read Access Time = 15  cycles */
#define B1WAT_1			0x10000000	/* Bank 1 Write Access Time = 1  cycles */
#define B1WAT_2			0x20000000	/* Bank 1 Write Access Time = 2  cycles */
#define B1WAT_3			0x30000000	/* Bank 1 Write Access Time = 3  cycles */
#define B1WAT_4			0x40000000	/* Bank 1 Write Access Time = 4  cycles */
#define B1WAT_5			0x50000000	/* Bank 1 Write Access Time = 5  cycles */
#define B1WAT_6			0x60000000	/* Bank 1 Write Access Time = 6  cycles */
#define B1WAT_7			0x70000000	/* Bank 1 Write Access Time = 7  cycles */
#define B1WAT_8			0x80000000	/* Bank 1 Write Access Time = 8  cycles */
#define B1WAT_9			0x90000000	/* Bank 1 Write Access Time = 9  cycles */
#define B1WAT_10		0xA0000000	/* Bank 1 Write Access Time = 10 cycles */
#define B1WAT_11		0xB0000000	/* Bank 1 Write Access Time = 11 cycles */
#define B1WAT_12		0xC0000000	/* Bank 1 Write Access Time = 12 cycles */
#define B1WAT_13		0xD0000000	/* Bank 1 Write Access Time = 13 cycles */
#define B1WAT_14		0xE0000000	/* Bank 1 Write Access Time = 14 cycles */
#define B1WAT_15		0xF0000000	/* Bank 1 Write Access Time = 15 cycles */

/* AMBCTL1 Masks */
#define B2RDYEN			0x00000001
#define B2RDYPOL		0x00000002
#define B2TT_1			0x00000004
#define B2TT_2			0x00000008
#define B2TT_3			0x0000000C
#define B2TT_4			0x00000000
#define B2ST_1			0x00000010
#define B2ST_2			0x00000020
#define B2ST_3			0x00000030
#define B2ST_4			0x00000000
#define B2HT_1			0x00000040
#define B2HT_2			0x00000080
#define B2HT_3			0x000000C0
#define B2HT_0			0x00000000
#define B2RAT_1			0x00000100
#define B2RAT_2			0x00000200
#define B2RAT_3			0x00000300
#define B2RAT_4			0x00000400
#define B2RAT_5			0x00000500
#define B2RAT_6			0x00000600
#define B2RAT_7			0x00000700
#define B2RAT_8			0x00000800
#define B2RAT_9			0x00000900
#define B2RAT_10		0x00000A00	/* Bank 2 Read Access  Time = 10 cycles */
#define B2RAT_11		0x00000B00	/* Bank 2 Read Access  Time = 11 cycles */
#define B2RAT_12		0x00000C00	/* Bank 2 Read Access  Time = 12 cycles */
#define B2RAT_13		0x00000D00	/* Bank 2 Read Access  Time = 13 cycles */
#define B2RAT_14		0x00000E00	/* Bank 2 Read Access  Time = 14 cycles */
#define B2RAT_15		0x00000F00	/* Bank 2 Read Access  Time = 15 cycles */
#define B2WAT_1			0x00001000	/* Bank 2 Write Access Time = 1  cycles */
#define B2WAT_2			0x00002000	/* Bank 2 Write Access Time = 2  cycles */
#define B2WAT_3			0x00003000	/* Bank 2 Write Access Time = 3  cycles */
#define B2WAT_4			0x00004000	/* Bank 2 Write Access Time = 4  cycles */
#define B2WAT_5			0x00005000	/* Bank 2 Write Access Time = 5  cycles */
#define B2WAT_6			0x00006000	/* Bank 2 Write Access Time = 6  cycles */
#define B2WAT_7			0x00007000	/* Bank 2 Write Access Time = 7  cycles */
#define B2WAT_8			0x00008000	/* Bank 2 Write Access Time = 8  cycles */
#define B2WAT_9			0x00009000	/* Bank 2 Write Access Time = 9  cycles */
#define B2WAT_10		0x0000A000	/* Bank 2 Write Access Time = 10 cycles */
#define B2WAT_11		0x0000B000	/* Bank 2 Write Access Time = 11 cycles */
#define B2WAT_12		0x0000C000	/* Bank 2 Write Access Time = 12 cycles */
#define B2WAT_13		0x0000D000	/* Bank 2 Write Access Time = 13 cycles */
#define B2WAT_14		0x0000E000	/* Bank 2 Write Access Time = 14 cycles */
#define B2WAT_15		0x0000F000	/* Bank 2 Write Access Time = 15 cycles */
#define B3RDYEN			0x00010000	/* Bank 3 RDY enable, 0=disable, 1=enable */
#define B3RDYPOL		0x00020000	/* Bank 3 RDY Active high, 0=active low,  1=active high */
#define B3TT_1			0x00040000	/* Bank 3 Transition Time from Read to Write = 1 cycles */
#define B3TT_2			0x00080000	/* Bank 3 Transition Time from Read to Write = 2 cycles */
#define B3TT_3			0x000C0000	/* Bank 3 Transition Time from Read to Write = 3 cycles */
#define B3TT_4			0x00000000	/* Bank 3 Transition Time from Read to Write = 4 cycles */
#define B3ST_1			0x00100000	/* Bank 3 Setup Time from AOE asserted to Read or Write asserted = 1 cycle */
#define B3ST_2			0x00200000
#define B3ST_3			0x00300000
#define B3ST_4			0x00000000
#define B3HT_1			0x00400000
#define B3HT_2			0x00800000
#define B3HT_3			0x00C00000
#define B3HT_0			0x00000000
#define B3RAT_1			0x01000000	/* Bank 3 Read Access  Time = 1  cycle  */
#define B3RAT_2			0x02000000	/* Bank 3 Read Access  Time = 2  cycles */
#define B3RAT_3			0x03000000	/* Bank 3 Read Access  Time = 3  cycles */
#define B3RAT_4			0x04000000	/* Bank 3 Read Access  Time = 4  cycles */
#define B3RAT_5			0x05000000	/* Bank 3 Read Access  Time = 5  cycles */
#define B3RAT_6			0x06000000	/* Bank 3 Read Access  Time = 6  cycles */
#define B3RAT_7			0x07000000	/* Bank 3 Read Access  Time = 7  cycles */
#define B3RAT_8			0x08000000	/* Bank 3 Read Access  Time = 8  cycles */
#define B3RAT_9			0x09000000	/* Bank 3 Read Access  Time = 9  cycles */
#define B3RAT_10		0x0A000000	/* Bank 3 Read Access  Time = 10 cycles */
#define B3RAT_11		0x0B000000	/* Bank 3 Read Access  Time = 11 cycles */
#define B3RAT_12		0x0C000000	/* Bank 3 Read Access  Time = 12 cycles */
#define B3RAT_13		0x0D000000	/* Bank 3 Read Access  Time = 13 cycles */
#define B3RAT_14		0x0E000000	/* Bank 3 Read Access  Time = 14 cycles */
#define B3RAT_15		0x0F000000	/* Bank 3 Read Access  Time = 15 cycles */
#define B3WAT_1			0x10000000	/* Bank 3 Write Access Time = 1  cycle  */
#define B3WAT_2			0x20000000	/* Bank 3 Write Access Time = 2  cycles */
#define B3WAT_3			0x30000000	/* Bank 3 Write Access Time = 3  cycles */
#define B3WAT_4			0x40000000	/* Bank 3 Write Access Time = 4  cycles */
#define B3WAT_5			0x50000000	/* Bank 3 Write Access Time = 5  cycles */
#define B3WAT_6			0x60000000	/* Bank 3 Write Access Time = 6  cycles */
#define B3WAT_7			0x70000000	/* Bank 3 Write Access Time = 7  cycles */
#define B3WAT_8			0x80000000	/* Bank 3 Write Access Time = 8  cycles */
#define B3WAT_9			0x90000000	/* Bank 3 Write Access Time = 9  cycles */
#define B3WAT_10		0xA0000000	/* Bank 3 Write Access Time = 10 cycles */
#define B3WAT_11		0xB0000000	/* Bank 3 Write Access Time = 11 cycles */
#define B3WAT_12		0xC0000000	/* Bank 3 Write Access Time = 12 cycles */
#define B3WAT_13		0xD0000000	/* Bank 3 Write Access Time = 13 cycles */
#define B3WAT_14		0xE0000000	/* Bank 3 Write Access Time = 14 cycles */
#define B3WAT_15		0xF0000000	/* Bank 3 Write Access Time = 15 cycles */

/* SDRAM CONTROLLER MASKS */
#define SCTLE			0x00000001	/* Enable SCLK[0], /SRAS, /SCAS, /SWE, SDQM[3:0] */
#define CL_2			0x00000008	/* SDRAM CAS latency = 2 cycles */
#define CL_3			0x0000000C	/* SDRAM CAS latency = 3 cycles */
#define PFE			0x00000010	/* Enable SDRAM prefetch */
#define PFP			0x00000020	/* Prefetch has priority over AMC requests */
#define TRAS_1			0x00000040	/* SDRAM tRAS = 1 cycle */
#define TRAS_2			0x00000080	/* SDRAM tRAS = 2 cycles */
#define TRAS_3			0x000000C0	/* SDRAM tRAS = 3 cycles */
#define TRAS_4			0x00000100	/* SDRAM tRAS = 4 cycles */
#define TRAS_5			0x00000140	/* SDRAM tRAS = 5 cycles */
#define TRAS_6			0x00000180	/* SDRAM tRAS = 6 cycles */
#define TRAS_7			0x000001C0	/* SDRAM tRAS = 7 cycles */
#define TRAS_8			0x00000200	/* SDRAM tRAS = 8 cycles */
#define TRAS_9			0x00000240	/* SDRAM tRAS = 9 cycles */
#define TRAS_10			0x00000280	/* SDRAM tRAS = 10 cycles */
#define TRAS_11			0x000002C0	/* SDRAM tRAS = 11 cycles */
#define TRAS_12			0x00000300	/* SDRAM tRAS = 12 cycles */
#define TRAS_13			0x00000340	/* SDRAM tRAS = 13 cycles */
#define TRAS_14			0x00000380	/* SDRAM tRAS = 14 cycles */
#define TRAS_15			0x000003C0	/* SDRAM tRAS = 15 cycles */
#define TRP_1			0x00000800	/* SDRAM tRP = 1 cycle */
#define TRP_2			0x00001000	/* SDRAM tRP = 2 cycles */
#define TRP_3			0x00001800	/* SDRAM tRP = 3 cycles */
#define TRP_4			0x00002000	/* SDRAM tRP = 4 cycles */
#define TRP_5			0x00002800	/* SDRAM tRP = 5 cycles */
#define TRP_6			0x00003000	/* SDRAM tRP = 6 cycles */
#define TRP_7			0x00003800	/* SDRAM tRP = 7 cycles */
#define TRCD_1			0x00008000	/* SDRAM tRCD = 1 cycle */
#define TRCD_2			0x00010000	/* SDRAM tRCD = 2 cycles */
#define TRCD_3			0x00018000	/* SDRAM tRCD = 3 cycles */
#define TRCD_4			0x00020000	/* SDRAM tRCD = 4 cycles */
#define TRCD_5			0x00028000	/* SDRAM tRCD = 5 cycles */
#define TRCD_6			0x00030000	/* SDRAM tRCD = 6 cycles */
#define TRCD_7			0x00038000	/* SDRAM tRCD = 7 cycles */
#define TWR_1			0x00080000	/* SDRAM tWR = 1 cycle */
#define TWR_2			0x00100000	/* SDRAM tWR = 2 cycles */
#define TWR_3			0x00180000	/* SDRAM tWR = 3 cycles */
#define PUPSD			0x00200000	/* Power-up start delay */
#define PSM			0x00400000	/* SDRAM powerup seq Precharge, mode register set, 8 CBR refresh cycles */
#define PSS			0x00800000	/* enable SDRAM power-up sequence on next SDRAM access */
#define SRFS			0x01000000	/* Start SDRAM self-refresh mode */
#define EBUFE			0x02000000	/* Enable external buffering timing */
#define FBBRW			0x04000000	/* Fast back-to-back read write enable */
#define EMREN			0x10000000	/* Extended mode register enable */
#define TCSR			0x20000000	/* Temp compensated self refresh value 85 deg C */
#define CDDBG			0x40000000	/* Tristate SDRAM controls during bus grant */
#define PASR_4			0x00000000
#define PASR_1			0x00000010
#define PASR_2			0x00000020
#define PASR_3			0x00000030

/* EBIU_SDBCTL Masks */
#define EBE			0x00000001	/* Enable SDRAM external bank */
#define EBSZ_16			0x00000000	/* SDRAM external bank size = 16MB */
#define EBSZ_32			0x00000002	/* SDRAM external bank size = 32MB */
#define EBSZ_64			0x00000004	/* SDRAM external bank size = 64MB */
#define EBSZ_128		0x00000006	/* SDRAM external bank size = 128MB */
#define EBCAW_8			0x00000000	/* SDRAM external bank column address width = 8 bits */
#define EBCAW_9			0x00000010	/* SDRAM external bank column address width = 9 bits */
#define EBCAW_10		0x00000020	/* SDRAM external bank column address width = 9 bits */
#define EBCAW_11		0x00000030	/* SDRAM external bank column address width = 9 bits */

/* EBIU_SDSTAT Masks */
#define SDCI			0x00000001	/* SDRAM controller is idle */
#define SDSRA			0x00000002	/* SDRAM SDRAM self refresh is active */
#define SDPUA			0x00000004	/* SDRAM power up active */
#define SDRS			0x00000008	/* SDRAM is in reset state */
#define SDEASE			0x00000010	/* SDRAM EAB sticky error status - W1C */
#define BGSTAT			0x00000020	/* Bus granted */

/* Core Interrupt Controller */
#define IMASK			0xFFE02104	/* Interrupt Mask Register */
#define IPEND			0xFFE02108	/* Interrupt Pending Register */
#define ILAT			0xFFE0210C	/* Interrupt Latch Register */
#define IPRIO			0xFFE02110	/* Core Interrupt Priority Register */

/* IMASK Bit values */
#define IVG15_POS		0x00008000
#define IVG14_POS		0x00004000
#define IVG13_POS		0x00002000
#define IVG12_POS		0x00001000
#define IVG11_POS		0x00000800
#define IVG10_POS		0x00000400
#define IVG9_POS		0x00000200
#define IVG8_POS		0x00000100
#define IVG7_POS		0x00000080
#define IVGTMR_POS		0x00000040
#define IVGHW_POS		0x00000020

#define TCNTL_ADDR		0xffe03000 /*  register 32 bit */
#define TCNTL			WORD_REF(TCNTL_ADDR)
#define TPERIOD_ADDR		0xffe03004 /*  register 32 bit */
#define TPERIOD			WORD_REF(TPERIOD_ADDR)
#define TSCALE_ADDR		0xffe03008 /*  register 32 bit */
#define TSCALE			WORD_REF(TSCALE_ADDR)
#define TCOUNT_ADDR		0xffe0300c /*  register 32 bit */
#define TCOUNT			WORD_REF(TCOUNT_ADDR)

#define MDMA_D1_CONFIG		0xFFC00E88	/* MemDMA Stream 1 Destination Configuration Register */
#define MDMA_D1_NEXT_DESC_PTR	0xFFC00E80	/* MemDMA Stream 1 Destination Next Descriptor Pointer Register */
#define MDMA_D1_START_ADDR	0xFFC00E84	/* MemDMA Stream 1 Destination Start Address Register */
#define MDMA_D1_X_COUNT		0xFFC00E90	/* MemDMA Stream 1 Destination X Count Register */
#define MDMA_D1_Y_COUNT		0xFFC00E98	/* MemDMA Stream 1 Destination Y Count Register */
#define MDMA_D1_X_MODIFY	0xFFC00E94	/* MemDMA Stream 1 Destination X Modify Register */
#define MDMA_D1_Y_MODIFY	0xFFC00E9C	/* MemDMA Stream 1 Destination Y Modify Register */
#define MDMA_D1_CURR_DESC_PTR	0xFFC00EA0	/* MemDMA Stream 1 Destination Current Descriptor Pointer Register */
#define MDMA_D1_CURR_ADDR	0xFFC00EA4	/* MemDMA Stream 1 Destination Current Address Register */
#define MDMA_D1_CURR_X_COUNT	0xFFC00EB0	/* MemDMA Stream 1 Destination Current X Count Register */
#define MDMA_D1_CURR_Y_COUNT	0xFFC00EB8	/* MemDMA Stream 1 Destination Current Y Count Register */
#define MDMA_D1_IRQ_STATUS	0xFFC00EA8	/* MemDMA Stream 1 Destination Interrupt/Status Register */
#define MDMA_D1_PERIPHERAL_MAP	0xFFC00EAC	/* MemDMA Stream 1 Destination Peripheral Map Register */

#define MDMA_S1_CONFIG		0xFFC00EC8	/* MemDMA Stream 1 Source Configuration Register */
#define MDMA_S1_NEXT_DESC_PTR	0xFFC00EC0	/* MemDMA Stream 1 Source Next Descriptor Pointer Register */
#define MDMA_S1_START_ADDR	0xFFC00EC4	/* MemDMA Stream 1 Source Start Address Register */
#define MDMA_S1_X_COUNT		0xFFC00ED0	/* MemDMA Stream 1 Source X Count Register */
#define MDMA_S1_Y_COUNT		0xFFC00ED8	/* MemDMA Stream 1 Source Y Count Register */
#define MDMA_S1_X_MODIFY	0xFFC00ED4	/* MemDMA Stream 1 Source X Modify Register */
#define MDMA_S1_Y_MODIFY	0xFFC00EDC	/* MemDMA Stream 1 Source Y Modify Register */
#define MDMA_S1_CURR_DESC_PTR	0xFFC00EE0	/* MemDMA Stream 1 Source Current Descriptor Pointer Register */
#define MDMA_S1_CURR_ADDR	0xFFC00EE4	/* MemDMA Stream 1 Source Current Address Register */
#define MDMA_S1_CURR_X_COUNT	0xFFC00EF0	/* MemDMA Stream 1 Source Current X Count Register */
#define MDMA_S1_CURR_Y_COUNT	0xFFC00EF8	/* MemDMA Stream 1 Source Current Y Count Register */
#define MDMA_S1_IRQ_STATUS	0xFFC00EE8	/* MemDMA Stream 1 Source Interrupt/Status Register */
#define MDMA_S1_PERIPHERAL_MAP	0xFFC00EEC	/* MemDMA Stream 1 Source Peripheral Map Register */
                                          
#define MDMA_D0_CONFIG		0xFFC00E08	/* MemDMA Stream 0 Destination Configuration Register */
#define MDMA_D0_NEXT_DESC_PTR	0xFFC00E00	/* MemDMA Stream 0 Destination Next Descriptor Pointer Register */
#define MDMA_D0_START_ADDR	0xFFC00E04	/* MemDMA Stream 0 Destination Start Address Register */
#define MDMA_D0_X_COUNT		0xFFC00E10	/* MemDMA Stream 0 Destination X Count Register */
#define MDMA_D0_Y_COUNT		0xFFC00E18	/* MemDMA Stream 0 Destination Y Count Register */
#define MDMA_D0_X_MODIFY	0xFFC00E14	/* MemDMA Stream 0 Destination X Modify Register */
#define MDMA_D0_Y_MODIFY	0xFFC00E1C	/* MemDMA Stream 0 Destination Y Modify Register */
#define MDMA_D0_CURR_DESC_PTR	0xFFC00E20	/* MemDMA Stream 0 Destination Current Descriptor Pointer Register */
#define MDMA_D0_CURR_ADDR	0xFFC00E24	/* MemDMA Stream 0 Destination Current Address Register */
#define MDMA_D0_CURR_X_COUNT	0xFFC00E30	/* MemDMA Stream 0 Destination Current X Count Register */
#define MDMA_D0_CURR_Y_COUNT	0xFFC00E38	/* MemDMA Stream 0 Destination Current Y Count Register */
#define MDMA_D0_IRQ_STATUS	0xFFC00E28	/* MemDMA Stream 0 Destination Interrupt/Status Register */
#define MDMA_D0_PERIPHERAL_MAP	0xFFC00E2C	/* MemDMA Stream 0 Destination Peripheral Map Register */
                                          
#define MDMA_S0_CONFIG		0xFFC00E48	/* MemDMA Stream 0 Source Configuration Register */
#define MDMA_S0_NEXT_DESC_PTR	0xFFC00E40	/* MemDMA Stream 0 Source Next Descriptor Pointer Register */
#define MDMA_S0_START_ADDR	0xFFC00E44	/* MemDMA Stream 0 Source Start Address Register */
#define MDMA_S0_X_COUNT		0xFFC00E50	/* MemDMA Stream 0 Source X Count Register */
#define MDMA_S0_Y_COUNT		0xFFC00E58	/* MemDMA Stream 0 Source Y Count Register */
#define MDMA_S0_X_MODIFY	0xFFC00E54	/* MemDMA Stream 0 Source X Modify Register */
#define MDMA_S0_Y_MODIFY	0xFFC00E5C	/* MemDMA Stream 0 Source Y Modify Register */
#define MDMA_S0_CURR_DESC_PTR	0xFFC00E60	/* MemDMA Stream 0 Source Current Descriptor Pointer Register */
#define MDMA_S0_CURR_ADDR	0xFFC00E64	/* MemDMA Stream 0 Source Current Address Register */
#define MDMA_S0_CURR_X_COUNT	0xFFC00E70	/* MemDMA Stream 0 Source Current X Count Register */
#define MDMA_S0_CURR_Y_COUNT	0xFFC00E78	/* MemDMA Stream 0 Source Current Y Count Register */
#define MDMA_S0_IRQ_STATUS	0xFFC00E68	/* MemDMA Stream 0 Source Interrupt/Status Register */
#define MDMA_S0_PERIPHERAL_MAP	0xFFC00E6C	/* MemDMA Stream 0 Source Peripheral Map Register */

/* **********  DMA CONTROLLER MASKS  ********************* */

/* DMAx_CONFIG, MDMA_yy_CONFIG Masks */
#define DMAEN           0x00000001  /* Channel Enable */
#define WNR             0x00000002  /* Channel Direction (W/R*) */
#define WDSIZE_8        0x00000000  /* Word Size 8 bits */
#define WDSIZE_16       0x00000004  /* Word Size 16 bits */
#define WDSIZE_32       0x00000008  // Word Size 32 bits
#define DMA2D           0x00000010  /* 2D/1D* Mode */
#define RESTART         0x00000020  /* Restart */
#define DI_SEL          0x00000040  /* Data Interrupt Select */
#define DI_EN           0x00000080  /* Data Interrupt Enable */
#define NDSIZE          0x00000900  /* Next Descriptor Size */
#define FLOW            0x00007000  /* Flow Control */


#define DMAEN_P                 0  /* Channel Enable */
#define WNR_P                   1  /* Channel Direction (W/R*) */
#define DMA2D_P                 4  /* 2D/1D* Mode */
#define RESTART_P               5  /* Restart */
#define DI_SEL_P                6  /* Data Interrupt Select */
#define DI_EN_P                 7  /* Data Interrupt Enable */

/* DMAx_IRQ_STATUS, MDMA_yy_IRQ_STATUS Masks */

#define DMA_DONE                0x00000001  /* DMA Done Indicator */
#define DMA_ERR                 0x00000002  /* DMA Error Indicator */
#define DFETCH                  0x00000004  /* Descriptor Fetch Indicator */
#define DMA_RUN                 0x00000008  /* DMA Running Indicator */

#define DMA_DONE_P              0  /* DMA Done Indicator */
#define DMA_ERR_P               1 /* DMA Error Indicator */
#define DFETCH_P                2  /* Descriptor Fetch Indicator */
#define DMA_RUN_P               3  /* DMA Running Indicator */


/* L1 Memory */
#define DATASRAM_A		0xFF800000
#define DATASRAM_CACHE_A	0xFF804000
#define DATASRAM_B		0xFF900000
#define DATASRAM_CACHE_B	0xFF904000

/*
 * Values for registers, stack etc
 */

/* Default PLL selection for sclk and cclk */
#define PLL_CSEL		0x0
#define PLL_SSEL		0x5

/* SDRRC values to be used for Ezkit and Stamp */
#define EZKIT_RDIV		0x817
#define STAMP_RDIV		0x305

/* Stack Pointer setup */
#define STACKBASE		0x1900000	

/* Watch Dog timer values setup */
#define WATCHDOG_DISABLE	WDOG_TMR_DISABLE | ICTL_DISABLE

/* SDRAM Settings for Ezkit and Stamp */
#ifdef CONFIG_EZKIT533
#define SDRRCVAL 		EZKIT_RDIV
#define SDBCTL			(EBCAW_9 | EBSZ_32 | EBE)	/* SDBCTL value for Ezkit = 0x0013 */
/* #define SDGCTL		(~CDDBG | ~TCSR | ~EMREN | ~FBBRW | ~EBUFE | ~SRFS | PSS |	\
 *				~PSM | ~PUPSD | TWR_2 | TRCD_3 | TRP_3 | TRAS_6 | PASR_4 | CL_3 | SCTLE )
 */
#define SDGCTL			0x0091998D
/* SDGCTL value for Ezkit = 0x0091998D */
#endif

#ifdef CONFIG_STAMP
#define SDRRCVAL		STAMP_RDIV
#define SDBCTL			(EBCAW_11 | EBSZ_128 | EBE)	/* SDBCTL value for Stamp = 0x0037 */
/* #define SDGCTL		(~CDDBG | ~TCSR | ~EMREN | ~FBBRW | ~EBUFE | ~SRFS | PSS | ~PSM |	\
 *				~PUPSD | TWR_2 | TRCD_2 | TRP_2 | TRAS_5 | PASR_4 | CL_2 | SCTLE)
 */
#define SDGCTL			0x00911149
/* SDGCTL value for Stamp = 0x00911149 */
#endif

/* Async Mem Bank Settings for Ezkit and Stamp */
#ifdef CONFIG_EZKIT533
/* 0xFF, 0x7BB07BB0, 0x22547BB0 */
/* #define AMGCTLVAL		(AMBEN_P0 | AMBEN_P1 | AMBEN_P2 | AMCKEN)
#define AMBCTL0VAL		(B1WAT_7 | B1RAT_11 | B1HT_2 | B1ST_3 | B1TT_4 | ~B1RDYPOL |	\
				~B1RDYEN | B0WAT_7 | B0RAT_11 | B0HT_2 | B0ST_3 | B0TT_4 | ~B0RDYPOL | ~B0RDYEN)
#define AMBCTL1VAL		(B3WAT_2 | B3RAT_2 | B3HT_1 | B3ST_1 | B3TT_4 | B3RDYPOL | ~B3RDYEN |	\
				B2WAT_7 | B2RAT_11 | B2HT_2 | B2ST_3 | B2TT_4 | ~B2RDYPOL | ~B2RDYEN)
*/
#define AMGCTLVAL		0xFF
#define AMBCTL0VAL		0x7BB07BB0
#define AMBCTL1VAL		0x22547BB0
	
#endif

#if CONFIG_STAMP
/* 0xFF, 0xBBC3BBc3, 0x99B39983 */
/*#define AMGCTLVAL		(AMBEN_P0 | AMBEN_P1 | AMBEN_P2 | AMCKEN)
#define AMBCTL0VAL		(B1WAT_11 | B1RAT_11 | B1HT_3 | B1ST_4 | B1TT_4 | B1RDYPOL |	\
				B1RDYEN | B0WAT_11 | B0RAT_11 | B0HT_3 | B0ST_4 | B0TT_4 | B0RDYPOL | B0RDYEN)
#define AMBCTL1VAL		(B3WAT_9 | B3RAT_9 | B3HT_2 | B3ST_3 | B3TT_4 | B3RDYPOL |	\
				B3RDYEN | B2WAT_9 | B2RAT_9 | B2HT_2 | B2ST_4 | B2TT_4 | B2RDYPOL | B2RDYEN)
*/
#define AMGCTLVAL		0xFF
#define AMBCTL0VAL		0xBBC3BBC3
#define AMBCTL1VAL		0x99B39983
#define CF_AMBCTL1VAL		0x99B3ffc2

#endif

/* Clock Settings for PLL */
#define STAMP_CCLK_396MHZ	0x4800
#define STAMP_SCLK_132MHZ	0x3

/* Delay inserted for PLL transition */
#define DELAY			0x1000

/* System Interrupt Controller Values */
#define SIC_MASK_ALL		0x00000000

/* Misc Values */
#define TEMP_STACK		0xFF902000

#define L1_ISRAM		0xFFA00000
#define L1_ISRAM_END		0xFFA10000
#define SYSMMR_BASE		0xFFC00000
#define WDSIZE16		0x00000004

#endif
