/*
 * U-boot - Configuration file for BF537 STAMP board
 */

#ifndef __CONFIG_BF527_EZKIT_H__
#define __CONFIG_BF527_EZKIT_H__

#include <asm/blackfin-config-pre.h>

#define BFIN_CPU             bf527-0.0
#define BFIN_BOOT_MODE       BFIN_BOOT_PARA

#define CFG_LONGHELP		1
#define CONFIG_CMDLINE_EDITING	1
#define CONFIG_AUTO_COMPLETE	1
#define CONFIG_ENV_OVERWRITE	1
#define CONFIG_DEBUG_DUMP	1
#define CONFIG_DEBUG_DUMP_SYMS	1
#define CONFIG_BAUDRATE		57600
#define CONFIG_UART_CONSOLE	1

#if !defined(__ADSPBF522__) && !defined(__ADSPBF523__) && \
    !defined(__ADSPBF524__) && !defined(__ADSPBF525__)
#define CONFIG_BFIN_MAC
#define CONFIG_BFIN_MAC_RMII
#define CONFIG_NETCONSOLE	1
#define CONFIG_NET_MULTI	1
#endif

#define CONFIG_PANIC_HANG 1

/* This sets the default state of the cache on U-Boot's boot */
#define CONFIG_ICACHE_ON
#define CONFIG_DCACHE_ON

#define CONFIG_RTC_BFIN		1

/* CONFIG_CLKIN_HZ is any value in Hz				*/
#define CONFIG_CLKIN_HZ		25000000
/* CONFIG_CLKIN_HALF controls what is passed to PLL 0=CLKIN	*/
/*						    1=CLKIN/2	*/
#define CONFIG_CLKIN_HALF	0
/* CONFIG_PLL_BYPASS controls if the PLL is used 0=don't bypass */
/*						    1=bypass PLL*/
#define CONFIG_PLL_BYPASS	0
/* CONFIG_VCO_MULT controls what the multiplier of the PLL is.	*/
/* Values can range from 1-64					*/
#define CONFIG_VCO_MULT			21
/* CONFIG_CCLK_DIV controls what the core clock divider is	*/
/* Values can be 1, 2, 4, or 8 ONLY				*/
#define CONFIG_CCLK_DIV			1
/* CONFIG_SCLK_DIV controls what the peripheral clock divider is*/
/* Values can range from 1-15					*/
#define CONFIG_SCLK_DIV			4
/* CONFIG_SPI_BAUD controls the SPI peripheral clock divider	*/
/* Values can range from 2-65535				*/
/* SCK Frequency = SCLK / (2 * CONFIG_SPI_BAUD)			*/
#define CONFIG_SPI_BAUD			2
#if (BFIN_BOOT_MODE == BFIN_BOOT_SPI_MASTER)
#define CONFIG_SPI_BAUD_INITBLOCK	4
#endif

#define CONFIG_MEM_SIZE			64	/* 128, 64, 32, 16 */
//#define CONFIG_MEM_ADD_WDTH		10	/* 8, 9, 10, 11 */
//#define CONFIG_MEM_MT48LC32M8A2_75	1
#define CONFIG_EBIU_SDRRC_VAL  0x03F6
#define CONFIG_EBIU_SDGCTL_VAL (SCTLE | CL_3 | PASR_ALL | TRAS_6 | TRP_3 | TRCD_3 | TWR_2 | PSSE)
#define CONFIG_EBIU_SDBCTL_VAL (EBE | EBSZ_64 | EBCAW_10)

#define CONFIG_EBIU_AMGCTL_VAL		(AMCKEN | AMBEN_ALL)
#define CONFIG_EBIU_AMBCTL0_VAL		(B1WAT_15 | B1RAT_15 | B1HT_3 | B1RDYPOL | B0WAT_15 | B0RAT_15 | B0HT_3 | B0RDYPOL)
#define CONFIG_EBIU_AMBCTL1_VAL		(B3WAT_15 | B3RAT_15 | B3HT_3 | B3RDYPOL | B2WAT_15 | B2RAT_15 | B2HT_3 | B2RDYPOL)

#define CONFIG_LOADS_ECHO		1

/*
 * rarpb, bootp or dhcp commands will perform only a
 * configuration lookup from the BOOTP/DHCP server
 * but not try to load any image using TFTP
 */
#define CFG_AUTOLOAD			"no"

/* Don't waste time transferring a logo over the UART */
#if (BFIN_BOOT_MODE != BFIN_BOOT_UART)
# define CONFIG_VIDEO
#endif

/*
 * Network Settings
 */
/* network support */
#ifdef CONFIG_BFIN_MAC
#define CONFIG_IPADDR		192.168.0.15
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_GATEWAYIP	192.168.0.1
#define CONFIG_SERVERIP		192.168.0.2
#define CONFIG_HOSTNAME		BFIN
#endif

#define CONFIG_ROOTPATH		/romfs
/* Uncomment next line to use fixed MAC address */
/* #define CONFIG_ETHADDR	02:80:ad:20:31:e8 */
/* This is the routine that copies the MAC in Flash to the 'ethaddr' setting */

#ifdef CONFIG_BFIN_MAC
# define CONFIG_BFIN_CMD		(CONFIG_CMD_DFL | CFG_CMD_PING)
#else
# define CONFIG_BFIN_CMD		(CONFIG_CMD_DFL & ~CFG_CMD_NET)
#endif

#define CONFIG_COMMANDS		(CONFIG_BFIN_CMD| \
				 CFG_CMD_ELF	| \
				 CFG_CMD_I2C	| \
				 CFG_CMD_CACHE  | \
				 CFG_CMD_JFFS2	| \
				 CFG_CMD_EEPROM | \
				 CFG_CMD_DHCP   | \
				 ADD_IDE_CMD	| \
				 ADD_NAND_CMD	| \
				 CFG_CMD_DATE)

/* This must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

#define CONFIG_BFIN_COMMANDS \
	( CFG_BFIN_CMD_BOOTLDR | \
	  CFG_BFIN_CMD_CPLBINFO | \
	  CFG_BFIN_CMD_OTP )

#if (BFIN_BOOT_MODE == BFIN_BOOT_UART)
# define CONFIG_BOOTDELAY    -1
#else
# define CONFIG_BOOTDELAY    5
#endif

#define CONFIG_BOOTCOMMAND   "run ramboot"
#define CONFIG_BOOTARGS      "root=/dev/mtdblock0 rw earlyprintk=serial,uart1,57600 console=tty0 console=ttyBF0,57600"
#define CONFIG_LOADADDR      0x1000000

#if (CONFIG_COMMANDS & CFG_CMD_NET)
# if (BFIN_BOOT_MODE == BFIN_BOOT_SPI_MASTER)
#  define UBOOT_ENV_UPDATE \
		"eeprom write $(loadaddr) 0x0 $(filesize)"
# else
#  define UBOOT_ENV_UPDATE \
		"protect off 0x20000000 0x2003FFFF;" \
		"erase 0x20000000 0x2003FFFF;" \
		"cp.b $(loadaddr) 0x20000000 $(filesize)"
# endif
# define NETWORK_ENV_SETTINGS \
	"ubootfile=u-boot.ldr\0" \
	"update=" \
		"tftp $(loadaddr) $(ubootfile);" \
		UBOOT_ENV_UPDATE \
		"\0" \
	"addip=set bootargs $(bootargs) ip=$(ipaddr):$(serverip):$(gatewayip):$(netmask):$(hostname):eth0:off\0" \
	"ramargs=set bootargs " CONFIG_BOOTARGS "\0" \
	"ramboot=" \
		"tftp $(loadaddr) uImage;" \
		"run ramargs;" \
		"run addip;" \
		"bootm" \
		"\0" \
	"nfsargs=set bootargs root=/dev/nfs rw nfsroot=$(serverip):$(rootpath),tcp,nfsvers=3\0" \
	"nfsboot=" \
		"tftp $(loadaddr) vmImage;" \
		"run nfsargs;" \
		"run addip;" \
		"bootm" \
		"\0"
#else
# define NETWORK_ENV_SETTINGS
#endif
#define CONFIG_EXTRA_ENV_SETTINGS \
	NETWORK_ENV_SETTINGS \
	"flashboot=bootm 0x20100000\0"

#define CFG_PROMPT "bfin> "

#define	CFG_CBSIZE		256	/* Console I/O Buffer Size */
#define CFG_MAX_RAM_SIZE       	(CONFIG_MEM_SIZE * 1024*1024)
#define	CFG_PBSIZE		(CFG_CBSIZE+sizeof(CFG_PROMPT)+16)	/* Print Buffer Size */
#define	CFG_MAXARGS		16	/* max number of command args */
#define CFG_BARGSIZE		CFG_CBSIZE	/* Boot Argument Buffer Size */
#define CFG_MEMTEST_START	0x0	/* memtest works on */
#define CFG_MEMTEST_END		( (CONFIG_MEM_SIZE - 1) * 1024*1024)	/* 1 ... 63 MB in DRAM */
#define	CFG_LOAD_ADDR		CONFIG_LOADADDR	/* default load address */
#define	CFG_HZ			1000	/* decrementer freq: 10 ms ticks */
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
#define	CFG_SDRAM_BASE		0x00000000

#define CFG_FLASH_BASE		0x20000000
#define CFG_FLASH_CFI		/* The flash is CFI compatible */
#define CFG_FLASH_CFI_DRIVER	/* Use common CFI driver */
#define CFG_FLASH_PROTECTION
#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CFG_MAX_FLASH_SECT	259	/* max number of sectors on one chip */

#define CFG_MONITOR_LEN		(384 << 10)	/* Reserve 384 kB for Monitor	*/
#define CFG_MONITOR_BASE	(CFG_MAX_RAM_SIZE - CFG_MONITOR_LEN)
#define CFG_MALLOC_LEN		(384 << 10)	/* Reserve 384 kB for malloc() (video/spi are big) */
#define CFG_MALLOC_BASE		(CFG_MONITOR_BASE - CFG_MALLOC_LEN)
#define CFG_GBL_DATA_SIZE	0x4000
#define CFG_GBL_DATA_ADDR	(CFG_MALLOC_BASE - CFG_GBL_DATA_SIZE)
#define CONFIG_STACKBASE	(CFG_GBL_DATA_ADDR  - 4)

#if (BFIN_BOOT_MODE == BFIN_BOOT_SPI_MASTER)
#define CFG_ENV_IS_IN_EEPROM	1
#define CFG_ENV_OFFSET		0x4000
#else
#define	CFG_ENV_IS_IN_FLASH	1
#define CFG_ENV_ADDR		0x20004000
#define CFG_ENV_OFFSET		(CFG_ENV_ADDR - CFG_FLASH_BASE)
#endif
#define CFG_ENV_SIZE		0x2000
#define	CFG_ENV_SECT_SIZE	0x2000	/* Total Size of Environment Sector */
#define ENV_IS_EMBEDDED_CUSTOM

/* JFFS Partition offset set  */
#define CFG_JFFS2_FIRST_BANK	0
#define CFG_JFFS2_NUM_BANKS	1
/* 512k reserved for u-boot */
#define CFG_JFFS2_FIRST_SECTOR	15

#define CONFIG_SPI

#define FLASH_TOT_SECT		71

/*
 * Initialize PSD4256 registers for using I2C
 */
#define CONFIG_MISC_INIT_R

#define CFG_BOOTM_LEN		0x4000000	/* Large Image Length, set to 64 Meg */

/*
 * I2C settings
 */
#define CONFIG_HARD_I2C		1	/* I2C TWI */
#if defined CONFIG_HARD_I2C
#define CONFIG_TWICLK_KHZ	50
#endif

#define CFG_I2C_SPEED		50000
#define CFG_I2C_SLAVE		0xFE

#include <asm/blackfin-config-post.h>

#endif
