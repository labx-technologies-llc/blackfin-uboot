/*
 * U-boot - stamp.h  Configuration file for STAMP board
 *			having BF533 processor
 *
 * Copyright (c) 2005 blackfin.uclinux.org
 *
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

#ifndef __CONFIG_STAMP_H__
#define __CONFIG_STAMP_H__

#define CFG_LONGHELP		1
#define CONFIG_BAUDRATE		57600
#define CONFIG_STAMP		1
#define U_BOOT_BF533_RELEASE	"Release Version Beta"
/* #define CFG_ENV_IS_NOWHERE	1 */
#define CONFIG_BOOTDELAY	30
#define CONFIG_BOARD_TYPES	1
#undef	CONFIG_BOOTARGS
#define CONFIG_DRIVER_SMC91111	1
#define CONFIG_SMC91111_BASE	0x20300300
/* Enabled below option for CF support */
/* #define CONFIG_STAMP_CF         1 */


/* To remove hardcoding and enable MAC storage in EEPROM */
/* #define HARDCODE_MAC		1 */

#define CONFIG_RTC_BF533	1
#define CONFIG_BOOT_RETRY_TIME	-1	/* Enable this if bootretry required, currently its disabled */

#ifndef CONFIG_STAMP_CF

/* CONFIG_CLKIN_HZ is any value in Hz                            */
#define CONFIG_CLKIN_HZ			11059200
/* CONFIG_CLKIN_HALF controls what is passed to PLL 0=CLKIN      */
/*                                                  1=CLKIN/2    */
#define CONFIG_CLKIN_HALF		0
/* CONFIG_PLL_BYPASS controls if the PLL is used 0=don't bypass  */
/*                                               1=bypass PLL    */
#define CONFIG_PLL_BYPASS		0
/* CONFIG_VCO_MULT controls what the multiplier of the PLL is.   */
/* Values can range from 1-64                                    */
#define CONFIG_VCO_MULT			36
/* CONFIG_CCLK_DIV controls what the core clock divider is       */
/* Values can be 1, 2, 4, or 8 ONLY                              */
#define CONFIG_CCLK_DIV			1
/* CONFIG_SCLK_DIV controls what the peripheral clock divider is */
/* Values can range from 1-15                                    */
#define CONFIG_SCLK_DIV			5

#else
/* For Compact Flash support Sclk must be less than 79MHz 	 */
#define CONFIG_CLKIN_HALF		0
#define CONFIG_PLL_BYPASS		0
#define CONFIG_VCO_MULT			36
#define CONFIG_CCLK_DIV			1
#define CONFIG_SCLK_DIV			5

#endif

#if ( CONFIG_CLKIN_HALF == 0 )
	#define CONFIG_VCO_HZ		( CONFIG_CLKIN_HZ * CONFIG_VCO_MULT )
#else
	#define CONFIG_VCO_HZ		(( CONFIG_CLKIN_HZ * CONFIG_VCO_MULT ) / 2 )
#endif

#if (CONFIG_PLL_BYPASS == 0)
	#define CONFIG_CCLK_HZ	( CONFIG_VCO_HZ / CONFIG_CCLK_DIV )
	#define CONFIG_SCLK_HZ	( CONFIG_VCO_HZ / CONFIG_SCLK_DIV )
#else
	#define CONFIG_CCLK_HZ	CONFIG_CLKIN_HZ
	#define CONFIG_SCLK_HZ	CONFIG_CLKIN_HZ
#endif

#define CONFIG_MEM_SIZE                128             /* 128, 64, 32, 16 */
#define CONFIG_MEM_ADD_WDTH     	11             /* 8, 9, 10, 11    */
#define CONFIG_MEM_MT48LC64M4A2FB_7E	1

#define	CONFIG_EXTRA_ENV_SETTINGS					\
	"netdev=eth0\0"							\
	"tftp_boot=tftp 0x1000000 uImage;bootm 0x1000000;echo\0"	\
	"dhcp_boot=dhcp;bootelf 0x1000000;echo\0"			\
	"serial_boot=loadb;bootm 0x1000000;echo\0"			\
	"linuxrun=bootm 0x20100000;echo\0"				\
	"bootfile=uImage\0"						\
	"autoload=yes\0"						\
	"autostart=no\0"						\
	"netretry=no\0"							\
	""

#define CONFIG_BOOTCOMMAND	"run linuxrun"	/* For autoboot */
#define CONFIG_LOADS_ECHO	1
#undef	CFG_LOADS_BAUD_CHANGE

#ifdef CONFIG_STAMP_CF
#define CONFIG_COMMANDS		(CFG_CMD_BDI | CFG_CMD_LOADB | CFG_CMD_IMI | CFG_CMD_MEMORY | CFG_CMD_NET | \
				CFG_CMD_ELF | CFG_CMD_PING | CFG_CMD_DHCP | CFG_CMD_RUN | \
				CFG_CMD_ENV | CFG_CMD_FLASH | CFG_CMD_DATE | CFG_CMD_AUTOSCRIPT | CFG_CMD_MISC | \
				CFG_CMD_IDE | CFG_CMD_FAT)

#else
#define CONFIG_COMMANDS		(CFG_CMD_BDI | CFG_CMD_LOADB | CFG_CMD_IMI | CFG_CMD_MEMORY | CFG_CMD_NET | \
				CFG_CMD_ELF | CFG_CMD_PING | CFG_CMD_DHCP | CFG_CMD_RUN | \
				CFG_CMD_ENV | CFG_CMD_FLASH | CFG_CMD_DATE | CFG_CMD_AUTOSCRIPT | CFG_CMD_MISC)
#endif
/* This must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

/*
 * Miscellaneous configurable options
 */
#define	CFG_PROMPT		"BOOT> "	/* Monitor Command Prompt */
#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define	CFG_CBSIZE		1024	/* Console I/O Buffer Size */
#else
#define	CFG_CBSIZE		256	/* Console I/O Buffer Size */
#endif
#define	CFG_PBSIZE		(CFG_CBSIZE+sizeof(CFG_PROMPT)+16)	/* Print Buffer Size */
#define	CFG_MAXARGS		16	/* max number of command args */
#define CFG_BARGSIZE		CFG_CBSIZE	/* Boot Argument Buffer Size */
#define CFG_MEMTEST_START	0x1000000	/* memtest works on */
#define CFG_MEMTEST_END		0x12000000	/* 4 ... 12 MB in DRAM */
#define	CFG_LOAD_ADDR		0x1000000	/* default load address */
#define	CFG_HZ			1000	/* 1ms time tick */ 
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
#define	CFG_SDRAM_BASE		0x00000000
#define CFG_MAX_RAM_SIZE	0x8000000
#define CFG_FLASH_BASE		0x20000000

#define CFG_MONITOR_BASE	0x7f80000
#define CFG_MONITOR_LEN		0x80000
#define CFG_MALLOC_LEN		0x40000
#define CFG_MALLOC_BASE		(CFG_MONITOR_BASE - CFG_MALLOC_LEN)
#define CFG_GBL_DATA_SIZE	0x4000
#define CFG_GBL_DATA_ADDR	(CFG_MALLOC_BASE - CFG_GBL_DATA_SIZE)
#define CONFIG_STACKBASE	(CFG_GBL_DATA_ADDR  - 4)
/*
 * Stack sizes
 */
#define CONFIG_STACKSIZE	(128*1024)      /* regular stack */

#define CFG_ENV_IS_IN_FLASH	1

/*
 * FLASH organization and environment definitions
 */
#define	CFG_BOOTMAPSZ		(8 << 20)	/* Initial Memory map for Linux */
#define CFG_FLASH0_BASE		0x20000000
#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CFG_MAX_FLASH_SECT	67	/* max number of sectors on one chip */
#define CFG_ENV_ADDR		0x20020000

#define CFG_ENV_SECT_SIZE	0x10000	/* Total Size of Environment Sector */

/* General definitions */
#define STATUS_LED_BOOT		1
#define POLL_MODE		1
#define FLASH_TOT_SECT		67
#define FLASH_SIZE		0x400000
#define CFG_FLASH_SIZE		0x400000

#define CFG_FLASH_ERASE_TOUT		200000	/* Timeout for Chip Erase (in ms) */
#define CFG_FLASH_ERASEBLOCK_TOUT	6000	/* Timeout for Block Erase (in ms) */
#define CFG_FLASH_WRITE_TOUT		1	/* Timeout for Flash Write (in ms) */

/* 
 * following timeouts shall be used once the 
 * Flash real protection is enabled
 */
#define CFG_FLASH_LOCK_TOUT		5	/* Timeout for Flash Set Lock Bit (in ms) */
#define CFG_FLASH_UNLOCK_TOUT		10000	/* Timeout for Flash Clear Lock Bits (in ms) */

/* network support */
#define CONFIG_IPADDR		10.100.4.50
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_GATEWAYIP	10.10.13.9
#define CONFIG_SERVERIP		10.100.4.174
#define CONFIG_HOSTNAME		STAMP

/* To remove hardcoding and enable MAC storage in EEPROM */
/* #define CONFIG_ETHADDR		02:80:ad:20:31:b8 */

#undef CONFIG_AUTOBOOT_KEYED
#ifdef CONFIG_AUTOBOOT_KEYED
#define CONFIG_AUTOBOOT_PROMPT	"Autobooting in %d seconds, press \" \" to stop\n"
#define CONFIG_AUTOBOOT_STOP_STR	" "
#undef  CONFIG_AUTOBOOT_DELAY_STR
#define DEBUG_BOOTKEYS	0
#endif

#undef	CONFIG_STATUS_LED

#define CONFIG_VDSP	1

/* FLASH/ETHERNET uses the same address range */
#define SHARED_RESOURCES	1

#define __ADSPLPBLACKFIN__	1
#define __ADSPBF533__		1

#define AMGCTLVAL		0xFF
#define AMBCTL0VAL		0xBBC3BBC3
#define AMBCTL1VAL		0x99B39983
#define CF_AMBCTL1VAL		0x99B3ffc2

#ifdef CONFIG_VDSP
#define ET_EXEC_VDSP		0x8
#define SHT_STRTAB_VDSP		0x1
#define ELFSHDRSIZE_VDSP	0x2C
#define VDSP_ENTRY_ADDR		0xFFA00000
#endif

#ifdef CONFIG_STAMP_CF

#define CONFIG_MISC_INIT_R	1
#define CONFIG_DOS_PARTITION	1

/*
 * IDE/ATA stuff
 */
#undef  CONFIG_IDE_8xx_DIRECT	/* no pcmcia interface required */
#undef  CONFIG_IDE_LED		/* no led for ide supported */
#undef  CONFIG_IDE_RESET	/* no reset for ide supported */

#define CFG_IDE_MAXBUS	1	/* max. 1 IDE busses */
#define CFG_IDE_MAXDEVICE	(CFG_IDE_MAXBUS*1) /* max. 1 drives per IDE bus */

#define CFG_ATA_BASE_ADDR	0x20200000
#define CFG_ATA_IDE0_OFFSET	0x0000

#define CFG_ATA_DATA_OFFSET	0x0020  /* Offset for data I/O */
#define CFG_ATA_REG_OFFSET	0x0020  /* Offset for normal register accesses */
#define CFG_ATA_ALT_OFFSET	0x0007  /* Offset for alternate registers */

#define CFG_ATA_STRIDE	2

#endif

#endif
