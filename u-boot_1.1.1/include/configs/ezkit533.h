#ifndef __CONFIG_EZKIT533_H__
#define __CONFIG_EZKIT533_H__

#define CFG_LONGHELP		1
#define CONFIG_BAUDRATE		57600
#define CONFIG_EZKIT533		1
#define U_BOOT_BF533_RELEASE	"Release Version Beta"
#define CFG_ENV_IS_NOWHERE	1
#define CONFIG_BOOTDELAY	30
#define CONFIG_BOARD_TYPES	1
#undef	CONFIG_BOOTARGS
#define CONFIG_DRIVER_SMC91111	1
#define CONFIG_SMC91111_BASE	0x20300300
#define HARDCODE_MAC		1
#define CONFIG_RTC_BF533	1

#define CONFIG_CRYSTAL_FREQ	27
#define PLL_DIV_FACTOR		5
#define CONFIG_VCO		450
#define CONFIG_CCLK		CONFIG_VCO
#define CONFIG_SCLK		(CONFIG_VCO/PLL_DIV_FACTOR)

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

#define CONFIG_COMMANDS		(CFG_CMD_BDI | CFG_CMD_LOADB | CFG_CMD_IMI | CFG_CMD_MEMORY | CFG_CMD_NET |	\
				CFG_CMD_ELF | CFG_CMD_PING | CFG_CMD_DHCP | CFG_CMD_RUN | CFG_CMD_FLASH |	\
				CFG_CMD_ENV | CFG_CMD_SAVEENV | CFG_CMD_DATE | CFG_CMD_AUTOSCRIPT | CFG_CMD_MISC)

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

#define	CFG_PROMPT		"BOOT> "	/* Monitor Command Prompt */
#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define	CFG_CBSIZE		1024	/* Console I/O Buffer Size */
#else
#define	CFG_CBSIZE		256	/* Console I/O Buffer Size */
#endif
#define	CFG_PBSIZE		(CFG_CBSIZE+sizeof(CFG_PROMPT)+16)	/* Print Buffer Size */
#define	CFG_MAXARGS		16	/* max number of command args */
#define CFG_BARGSIZE		CFG_CBSIZE	/* Boot Argument Buffer Size */
#define CFG_MEMTEST_START	0x01000000	/* memtest works on */
#define CFG_MEMTEST_END		0x12000000	/* 4 ... 12 MB in DRAM */
#define	CFG_LOAD_ADDR		0x1000000	/* default load address */
#define	CFG_HZ			100	/* decrementer freq: 10 ms ticks */
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
#define	CFG_SDRAM_BASE		0x00000000
#define CFG_MAX_RAM_SIZE	0x2000000
#define CFG_FLASH_BASE		0x20000000
#define RAM_END			0x1800000
#define CFG_MONITOR_LEN		0x30000
#define CFG_MONITOR_BASE	RAM_END
#define CFG_MALLOC_LEN		0x20000
#define CFG_MALLOC_BASE		(CFG_MONITOR_BASE - CFG_MALLOC_LEN)
#define CFG_GBL_DATA_SIZE	0x4000
#define CFG_GBL_DATA_OFFSET	(CFG_MALLOC_BASE - CFG_GBL_DATA_SIZE)
#define	CFG_BOOTMAPSZ		(8 << 20)	/* Initial Memory map for Linux */
#define CFG_FLASH0_BASE		0x20000000
#define CFG_FLASH1_BASE		0x20200000
#define CFG_FLASH2_BASE		0x20280000
#define CFG_MAX_FLASH_BANKS	3	/* max number of memory banks */
#define CFG_MAX_FLASH_SECT	40	/* max number of sectors on one chip */
#define	CFG_ENV_IS_IN_FLASH	1
#define CFG_ENV_ADDR		0x20040000
#define	CFG_ENV_OFFSET		0x0	/* Offset of Environment Sector */
#define	CFG_ENV_SIZE		0x10000	/* Total Size of Environment Sector */
#define RAM_DATA		0x200001e8

/* General definitions */
#define STATUS_LED_BOOT		1
#define	CFG_ENV_IS_IN_FLASH	1
#define POLL_MODE		1
#define FLASH_TOT_SECT		40
#define FLASH_SIZE		0x220000
#define CFG_FLASH_SIZE		0x220000

/* network support */
#define CONFIG_IPADDR		10.100.4.50
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_GATEWAYIP	10.10.13.9
#define CONFIG_SERVERIP		10.100.4.174
#define CONFIG_HOSTNAME		EZKIT533
#define CONFIG_ETHADDR		02:80:ad:20:31:b8

#undef CONFIG_AUTOBOOT_KEYED
#ifdef CONFIG_AUTOBOOT_KEYED
#define CONFIG_AUTOBOOT_PROMPT	"Autobooting in %d seconds, press \" \" to stop\n"
#define CONFIG_AUTOBOOT_STOP_STR	" "
#undef  CONFIG_AUTOBOOT_DELAY_STR
#define DEBUG_BOOTKEYS	0
#endif

#undef	CONFIG_STATUS_LED

#define CONFIG_VDSP		1

#ifdef CONFIG_VDSP
#define ET_EXEC_VDSP		0x8
#define SHT_STRTAB_VDSP		0x1
#define ELFSHDRSIZE_VDSP	0x2C
#define VDSP_ENTRY_ADDR		0xFFA00000
#endif

#endif
