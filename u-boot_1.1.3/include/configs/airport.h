/*
 * (C) Copyright 2000-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * board/config.h - configuration options, board specific
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */

#define CONFIG_MPC860		1
#define CONFIG_MPC860T		1
#define CONFIG_MPC855T		1
#define	CONFIG_MPC855
#define CONFIG_AIRPORT		1	/* in an Apple Airport "snow"	*/

#undef	CONFIG_25MHz
#undef	CONFIG_50MHz
#define	CONFIG_75MHz				/* Original clock used by Apple */
#define	CONFIG_8xx_OSCLK	25000000	/* External clock input */

#define	CONFIG_8xx_CONS_SMC1	1	/* Console is on SMC1		*/
#undef	CONFIG_8xx_CONS_SMC2
#undef	CONFIG_8xx_CONS_NONE

// #define CONFIG_MAC_PARTITION
#define CONFIG_DOS_PARTITION

/* enable I2C and select the hardware/software driver */
#undef	CONFIG_HARD_I2C			/* I2C with hardware support	*/
#define CONFIG_SOFT_I2C		1	/* I2C bit-banged		*/

#define CFG_I2C_SPEED		93000	/* 93 kHz is supposed to work	*/
#define CFG_I2C_SLAVE		0xFE

#ifdef CONFIG_SOFT_I2C
/*
 * Software (bit-bang) I2C driver configuration
 */
#define PB_SCL		0x00000020	/* PB 26 */
#define PB_SDA		0x00000010	/* PB 27 */

#define I2C_INIT	(immr->im_cpm.cp_pbdir |=  PB_SCL)
#define I2C_ACTIVE	(immr->im_cpm.cp_pbdir |=  PB_SDA)
#define I2C_TRISTATE	(immr->im_cpm.cp_pbdir &= ~PB_SDA)
#define I2C_READ	((immr->im_cpm.cp_pbdat & PB_SDA) != 0)
#define I2C_SDA(bit)	if(bit) immr->im_cpm.cp_pbdat |=  PB_SDA; \
			else	immr->im_cpm.cp_pbdat &= ~PB_SDA
#define I2C_SCL(bit)	if(bit) immr->im_cpm.cp_pbdat |=  PB_SCL; \
			else	immr->im_cpm.cp_pbdat &= ~PB_SCL
#define I2C_DELAY	udelay(2)	/* 1/4 I2C clock duration */

#define CFG_EEPROM_PAGE_WRITE_ENABLE
#define CFG_EEPROM_PAGE_WRITE_BITS 3
#endif	/* CONFIG_SOFT_I2C */

#define CONFIG_DTT_LM75		1                /* ON Semi's LM75 */
#define CONFIG_DTT_SENSORS	{ 1}           /* Sensor addresses */
#define CFG_DTT_MAX_TEMP	70
#define CFG_DTT_LOW_TEMP	-30
#define CFG_DTT_HYSTERESIS	3

#define CONFIG_BAUDRATE		19200	/* console baudrate = 115kbps	*/
#define CONFIG_BOOTDELAY	5	/* autoboot after 5 seconds	*/


#define CONFIG_COMMANDS	      ( CONFIG_CMD_DFL	| \
				CFG_CMD_IMMAP   | \
				CFG_CMD_MII     | \
				CFG_CMD_I2C	| \
				CFG_CMD_ELF	| \
				CFG_CMD_DTT	| \
				CFG_CMD_PCMCIA	| \
				CFG_CMD_IDE	| \
				CFG_CMD_PING    )

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

/*
 * Miscellaneous configurable options
 */
#define	CFG_LONGHELP			/* undef to save memory		*/
#define	CFG_PROMPT		"airport> "	/* Monitor Command Prompt	*/


#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define	CFG_CBSIZE		1024	/* Console I/O Buffer Size	*/
#else
#define	CFG_CBSIZE		256	/* Console I/O Buffer Size	*/
#endif
#define	CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define	CFG_MAXARGS		16	/* max number of command args	*/
#define CFG_BARGSIZE	CFG_CBSIZE	/* Boot Argument Buffer Size	*/

#define CFG_MEMTEST_START	0x0000000	/* memtest works on	*/
#define CFG_MEMTEST_END		0x077FFFF	/* 1 ... 7 MB in DRAM	*/
#define	CFG_ALT_MEMTEST

#define	CFG_LOAD_ADDR		0x0100000	/* default load address	*/

#define	CFG_HZ			1000	/* decrementer freq: 1 ms ticks	*/

#define CFG_BAUDRATE_TABLE	{ 1200, 2400, 4800, 9600, 19200, 57600, 115200 }

/*
 * Low Level Configuration Settings
 * (address mappings, register initial values, etc.)
 * You should know what you are doing if you make changes here.
 */
/*-----------------------------------------------------------------------
 * Internal Memory Mapped Register
 */
#define CFG_IMMR		0xFF000000

/*-----------------------------------------------------------------------
 * Definitions for initial stack pointer and data area (in DPRAM)
 */
#define CFG_INIT_RAM_ADDR	CFG_IMMR
#define	CFG_INIT_RAM_END	0x2F00	/* End of used area in DPRAM	*/
#define	CFG_GBL_DATA_SIZE	64  /* size in bytes reserved for initial data */
#define CFG_GBL_DATA_OFFSET	(CFG_INIT_RAM_END - CFG_GBL_DATA_SIZE)
#define	CFG_INIT_SP_OFFSET	CFG_GBL_DATA_OFFSET

/*-----------------------------------------------------------------------
 * Start addresses for the final memory configuration
 * (Set up by the startup code)
 * Please note that CFG_SDRAM_BASE _must_ start at 0
 */
#define	CFG_SDRAM_BASE		0x00000000
#define CFG_FLASH_BASE		0x40000000
#define	CFG_MONITOR_LEN		(256 << 10)	/* Reserve 256 kB for Monitor	*/
#define CFG_MONITOR_BASE	CFG_FLASH_BASE
#define	CFG_MALLOC_LEN		(128 << 10)	/* Reserve 128 kB for malloc()	*/

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define	CFG_BOOTMAPSZ		(8 << 20)	/* Initial Memory map for Linux	*/

/*-----------------------------------------------------------------------
 * FLASH organization
 */
#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks		*/
#define CFG_MAX_FLASH_SECT	39	/* max number of sectors on one chip	*/
#define	CONFIG_FLASH_16BIT

#define CFG_FLASH_ERASE_TOUT	120000	/* Timeout for Flash Erase (in ms)	*/
#define CFG_FLASH_WRITE_TOUT	500	/* Timeout for Flash Write (in ms)	*/

#define	CFG_ENV_IS_IN_FLASH	1
#define	CFG_ENV_OFFSET		0x40000	/*   Offset   of Environment Sector	*/
#define	CFG_ENV_SIZE		0x08000	/* Total Size of Environment Sector	*/
#define	CFG_ENV_SECT_SIZE	0x10000	/* Total Size of Environment Sector	*/
#define	CFG_ENV_ADDR		(CFG_FLASH_BASE + CFG_ENV_OFFSET)

/*-----------------------------------------------------------------------
 * Hardware Information Block
 */
#define CFG_HWINFO_OFFSET	0x0003FFC0	/* offset of HW Info block */
#define CFG_HWINFO_SIZE		0x00000040	/* size   of HW Info block */
#define CFG_HWINFO_MAGIC	0x534E4F57	/* 'SNOW' */

/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CFG_CACHELINE_SIZE	16	/* For all MPC8xx CPUs			*/
#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define CFG_CACHELINE_SHIFT	4	/* log base 2 of the above value	*/
#endif

/*-----------------------------------------------------------------------
 * SYPCR - System Protection Control				11-9
 * SYPCR can only be written once after reset!
 *-----------------------------------------------------------------------
 * Software & Bus Monitor Timer max, Bus Monitor enable, SW Watchdog freeze
 */
#if defined(CONFIG_WATCHDOG)
#define CFG_SYPCR	(SYPCR_SWTC | SYPCR_BMT | SYPCR_BME | SYPCR_SWF | \
			 SYPCR_SWE  | SYPCR_SWRI| SYPCR_SWP)
#else
#define CFG_SYPCR	(SYPCR_SWTC | SYPCR_BMT | SYPCR_BME | SYPCR_SWF | SYPCR_SWP)
#endif

/*-----------------------------------------------------------------------
 * SIUMCR - SIU Module Configuration				11-6
 *-----------------------------------------------------------------------
 * PCMCIA config., multi-function pin tri-state
 */
#ifndef	CONFIG_CAN_DRIVER
#define CFG_SIUMCR	(SIUMCR_DBGC00 | SIUMCR_DBPC00 | SIUMCR_MLRC01)
#else	/* we must activate GPL5 in the SIUMCR for CAN */
#define CFG_SIUMCR	(SIUMCR_DBGC11 | SIUMCR_DBPC00 | SIUMCR_MLRC01)
#endif	/* CONFIG_CAN_DRIVER */

/*-----------------------------------------------------------------------
 * TBSCR - Time Base Status and Control				11-26
 *-----------------------------------------------------------------------
 * Clear Reference Interrupt Status, Timebase freezing enabled
 */
#define CFG_TBSCR	(TBSCR_REFA | TBSCR_REFB | TBSCR_TBF)

/*-----------------------------------------------------------------------
 * RTCSC - Real-Time Clock Status and Control Register		11-27
 *-----------------------------------------------------------------------
 */
#define CFG_RTCSC	(RTCSC_SEC | RTCSC_ALR | RTCSC_RTF| RTCSC_RTE)

/*-----------------------------------------------------------------------
 * PISCR - Periodic Interrupt Status and Control		11-31
 *-----------------------------------------------------------------------
 * Clear Periodic Interrupt Status, Interrupt Timer freezing enabled
 */
#define CFG_PISCR	(PISCR_PS | PISCR_PITF)

/*-----------------------------------------------------------------------
 * PLPRCR - PLL, Low-Power, and Reset Control Register		15-30
 *-----------------------------------------------------------------------
 * Reset PLL lock status sticky bit, timer expired status bit and timer
 * interrupt status bit
 */
#if defined(CONFIG_50MHz)
#define	CFG_PLPRCR	(((0x1 << PLPRCR_MF_SHIFT) & PLPRCR_MF_MSK) | \
			PLPRCR_SPLSS | PLPRCR_TEXPS | PLPRCR_TMIST)
#elif defined(CONFIG_75MHz)
#define	CFG_PLPRCR	(((0x2 << PLPRCR_MF_SHIFT) & PLPRCR_MF_MSK) | \
			PLPRCR_SPLSS | PLPRCR_TEXPS | PLPRCR_TMIST)
#else
#define CFG_PLPRCR	(PLPRCR_SPLSS | PLPRCR_TEXPS | PLPRCR_TMIST)
#endif
/*-----------------------------------------------------------------------
 * SCCR - System Clock and reset Control Register		15-27
 *-----------------------------------------------------------------------
 * Set clock output, timebase and RTC source and divider,
 * power management and some other internal clocks
 */
#define SCCR_MASK	SCCR_EBDF11
#define CFG_SCCR	(SCCR_COM00   | SCCR_DFSYNC00 | SCCR_DFBRG00  | \
			 SCCR_DFNL000 | SCCR_DFNH000  | SCCR_DFLCD000 | \
			 SCCR_DFALCD00)

/*-----------------------------------------------------------------------
 * PCMCIA stuff
 *-----------------------------------------------------------------------
 *
 */
#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA)
#define	CONFIG_PCMCIA_SLOT_A

#define CFG_PCMCIA_MEM_ADDR	(0xE0000000)
#define CFG_PCMCIA_MEM_SIZE	( 64 << 20 )
#define CFG_PCMCIA_DMA_ADDR	(0xE4000000)
#define CFG_PCMCIA_DMA_SIZE	( 64 << 20 )
#define CFG_PCMCIA_ATTRB_ADDR	(0xE8000000)
#define CFG_PCMCIA_ATTRB_SIZE	( 64 << 20 )
#define CFG_PCMCIA_IO_ADDR	(0xEC000000)
#define CFG_PCMCIA_IO_SIZE	( 64 << 20 )
#endif
/*-----------------------------------------------------------------------
 * IDE/ATA stuff (Supports IDE harddisk on PCMCIA Adapter)
 *-----------------------------------------------------------------------
 */
#if (CONFIG_COMMANDS & CFG_CMD_PCMCIA)
#define	CONFIG_IDE_8xx_PCCARD		/* Use IDE with PC Card	Adapter	*/

#undef	CONFIG_IDE_8xx_DIRECT		/* Direct IDE    not supported	*/
#undef	CONFIG_IDE_LED			/* LED   for ide not supported	*/
#undef	CONFIG_IDE_RESET		/* reset for ide not supported	*/

#define CFG_IDE_MAXBUS		1	/* max. 1 IDE bus		*/
#define CFG_IDE_MAXDEVICE	1	/* max. 1 drive per IDE bus	*/

#define CFG_ATA_IDE0_OFFSET	0x0000

#define CFG_ATA_BASE_ADDR	CFG_PCMCIA_MEM_ADDR

/* Offset for data I/O			*/
#define CFG_ATA_DATA_OFFSET	(CFG_PCMCIA_MEM_SIZE + 0x320)

/* Offset for normal register accesses	*/
#define CFG_ATA_REG_OFFSET	(2 * CFG_PCMCIA_MEM_SIZE + 0x320)

/* Offset for alternate registers	*/
#define CFG_ATA_ALT_OFFSET	0x0100
#endif
/*-----------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------
 *
 */
#define CFG_DER	0

/*
 * Init Memory Controller:
 *
 * BR0 and OR0 (FLASH)
 */

#define FLASH_BASE0_PRELIM	0x40000000	/* FLASH bank #0	*/

/* used to re-map FLASH both when starting from SRAM or FLASH:
 * restrict access enough to keep SRAM working (if any)
 * but not too much to meddle with FLASH accesses
 */
#define CFG_REMAP_OR_AM		0x80000000	/* OR addr mask */
#define CFG_PRELIM_OR_AM	0xE0000000	/* OR addr mask */

/*
 * FLASH timing:
 */
#define CFG_OR_TIMING_FLASH	(OR_ACS_DIV1  | OR_TRLX | OR_CSNT_SAM | \
				 OR_SCY_3_CLK | OR_EHTR | OR_BI)
#if 0
#define CFG_OR0_REMAP	(CFG_REMAP_OR_AM  | CFG_OR_TIMING_FLASH)
#define CFG_OR0_PRELIM	(CFG_PRELIM_OR_AM | CFG_OR_TIMING_FLASH)
#define CFG_BR0_PRELIM	((FLASH_BASE0_PRELIM & BR_BA_MSK) | BR_V )
#else
#define CFG_OR0_PRELIM	(0xFFE00FF4)
#define CFG_BR0_PRELIM	(0x40000801)
#endif
/*
 * BR2 and OR2 (SDRAM)
 *
 */
#define SDRAM_BASE1_PRELIM	0x00000000	/* SDRAM bank #0	*/
#define	SDRAM_MAX_SIZE		0x00800000	/* max 8 MB		*/

/* SDRAM timing: Multiplexed addresses, GPL5 output to GPL5_A (don't care)	*/
#define CFG_OR_TIMING_SDRAM	0x00000A00

#define CFG_OR1_PRELIM	(0xFF800800)
#define CFG_BR1_PRELIM	(0x00000081)


/*
 * Memory Periodic Timer Prescaler
 *
 * The Divider for PTA (refresh timer) configuration is based on an
 * example SDRAM configuration (64 MBit, one bank). The adjustment to
 * the number of chip selects (NCS) and the actually needed refresh
 * rate is done by setting MPTPR.
 *
 * PTA is calculated from
 *	PTA = (gclk * Trefresh) / ((2 ^ (2 * DFBRG)) * PTP * NCS)
 *
 *	gclk	  CPU clock (not bus clock!)
 *	Trefresh  Refresh cycle * 4 (four word bursts used)
 *
 * 4096  Rows from SDRAM example configuration
 * 1000  factor s -> ms
 *   32  PTP (pre-divider from MPTPR) from SDRAM example configuration
 *    4  Number of refresh cycles per period
 *   64  Refresh cycle in ms per number of rows
 * --------------------------------------------
 * Divider = 4096 * 32 * 1000 / (4 * 64) = 512000
 *
 * 50 MHz => 50.000.000 / Divider =  98
 * 66 Mhz => 66.000.000 / Divider = 129
 * 80 Mhz => 80.000.000 / Divider = 156
 */


/*
 * Apple is using MT48LC2M32B2-TG7 which has
 * 4 banks of 512k * 32
 * 64msec 4096 cycle refresh
 * 2048 rows	11 bits
 * 256 columns	8 bits
 *
 */

#define CFG_PTA_PER_CLK	((4096 * 32 * 1000) / (4 * 64))
#define CFG_MAMR_PTA	98

/*
 * For 16 MBit, refresh rates could be 31.3 us
 * (= 64 ms / 2K = 125 / quad bursts).
 * For a simpler initialization, 15.6 us is used instead.
 *
 * #define CFG_MPTPR_2BK_2K	MPTPR_PTP_DIV32		for 2 banks
 * #define CFG_MPTPR_1BK_2K	MPTPR_PTP_DIV64		for 1 bank
 */
#define CFG_MPTPR_2BK_4K	MPTPR_PTP_DIV16		/* setting for 2 banks	*/
#define CFG_MPTPR_1BK_4K	MPTPR_PTP_DIV32		/* setting for 1 bank	*/

/* refresh rate 7.8 us (= 64 ms / 8K = 31.2 / quad bursts) for 256 MBit		*/
#define CFG_MPTPR_2BK_8K	MPTPR_PTP_DIV8		/* setting for 2 banks	*/
#define CFG_MPTPR_1BK_8K	MPTPR_PTP_DIV16		/* setting for 1 bank	*/

/*
 * MAMR settings for SDRAM
 */

/* 8 column SDRAM */
#define CFG_MAMR_8COL	((CFG_MAMR_PTA << MAMR_PTA_SHIFT)  | MAMR_PTAE	    |	\
			 MAMR_AMA_TYPE_0 | MAMR_DSA_1_CYCL | MAMR_G0CLA_A11 |	\
			 MAMR_RLFA_1X	 | MAMR_WLFA_1X	   | MAMR_TLFA_4X)
/* 9 column SDRAM */
#define CFG_MAMR_9COL	((CFG_MAMR_PTA << MAMR_PTA_SHIFT)  | MAMR_PTAE	    |	\
			 MAMR_AMA_TYPE_1 | MAMR_DSA_1_CYCL | MAMR_G0CLA_A10 |	\
			 MAMR_RLFA_1X	 | MAMR_WLFA_1X	   | MAMR_TLFA_4X)


/*
 * Internal Definitions
 *
 * Boot Flags
 */
#define	BOOTFLAG_COLD	0x01		/* Normal Power-On: Boot from FLASH	*/
#define BOOTFLAG_WARM	0x02		/* Software reboot			*/

#if 1
#define	CONFIG_FEC_ENET		1		/* Ethernet is on FEC		*/
#define CFG_DISCOVER_PHY
#define	CONFIG_MII
#define FEC_ENET
#define	CONFIG_ETHER_ON_FEC1
#else
#define CONFIG_SCC1_ENET
#define CONFIG_FEC_ENET
#define	FEC_ENET
#define	SCC_ENET
#define CONFIG_ETHPRIME		"SCC ETHERNET"
#endif

#endif	/* __CONFIG_H */
