/*
 * U-boot - Configuration file for BF533 STAMP board
 */

#ifndef __CONFIG_BF533_STAMP_H__
#define __CONFIG_BF533_STAMP_H__

#include <asm/blackfin-config-pre.h>

#define BFIN_CPU             bf533-0.3
#define BFIN_BOOT_MODE       BFIN_BOOT_BYPASS

#define CONFIG_RTC_BFIN			1

#define CONFIG_PANIC_HANG 1


/* This sets the default state of the cache on U-Boot's boot */
#define CONFIG_ICACHE_ON
#define CONFIG_DCACHE_ON

/*
 * Board settings
 */
#define CONFIG_DRIVER_SMC91111	1
#define CONFIG_SMC91111_BASE	0x20300300

/* FLASH/ETHERNET uses the same address range */
#define SHARED_RESOURCES 	1

/* Is I2C bit-banged? */
#define CONFIG_SOFT_I2C		1

/*
 * Software (bit-bang) I2C driver configuration
 */
#define PF_SCL			PF3
#define PF_SDA			PF2

/*
 * Clock settings
 */

/* CONFIG_CLKIN_HZ is any value in Hz				*/
#define CONFIG_CLKIN_HZ		11059200
/* CONFIG_CLKIN_HALF controls what is passed to PLL 0=CLKIN	*/
/*						    1=CLKIN/2	*/
#define CONFIG_CLKIN_HALF	0
/* CONFIG_PLL_BYPASS controls if the PLL is used 0=don't bypass	*/
/*						 1=bypass PLL	*/
#define CONFIG_PLL_BYPASS	0
/* CONFIG_VCO_MULT controls what the multiplier of the PLL is.	*/
/* Values can range from 1-64					*/
#define CONFIG_VCO_MULT		36
/* CONFIG_CCLK_DIV controls what the core clock divider is	*/
/* Values can be 1, 2, 4, or 8 ONLY				*/
#define CONFIG_CCLK_DIV		1
/* CONFIG_SCLK_DIV controls what the peripheral clock divider is*/
/* Values can range from 1-15					*/
#define CONFIG_SCLK_DIV		5
/* CONFIG_SPI_BAUD controls the SPI peripheral clock divider	*/
/* Values can range from 2-65535				*/
/* SCK Frequency = SCLK / (2 * CONFIG_SPI_BAUD)			*/
#define CONFIG_SPI_BAUD		2

#if (BFIN_BOOT_MODE == BFIN_BOOT_SPI_MASTER)
#define CONFIG_SPI_BAUD_INITBLOCK	4
#endif

/*
 * Network settings
 */

#if (CONFIG_DRIVER_SMC91111)
#if 0
#define	CONFIG_MII
#endif

/* network support */
#define CONFIG_IPADDR		192.168.0.15
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_GATEWAYIP	192.168.0.1
#define CONFIG_SERVERIP		192.168.0.2
#define CONFIG_HOSTNAME		BFIN
#define CONFIG_ROOTPATH		/checkout/uClinux-dist/romfs

/* To remove hardcoding and enable MAC storage in EEPROM  */
/* #define CONFIG_ETHADDR		02:80:ad:20:31:b8 */
#endif /* CONFIG_DRIVER_SMC91111 */

/*
 * Flash settings
 */

#define CFG_FLASH_CFI		/* The flash is CFI compatible  */
#define CFG_FLASH_CFI_DRIVER	/* Use common CFI driver	*/
#define	CFG_FLASH_CFI_AMD_RESET

#define CFG_FLASH_BASE		0x20000000
#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CFG_MAX_FLASH_SECT	67	/* max number of sectors on one chip */

#if (BFIN_BOOT_MODE == BFIN_BOOT_SPI_MASTER)
#define CFG_ENV_IS_IN_EEPROM	1
#define CFG_ENV_OFFSET		0x4000
#else
#define CFG_ENV_IS_IN_FLASH	1
#define CFG_ENV_ADDR		0x20004000
#define	CFG_ENV_OFFSET		(CFG_ENV_ADDR - CFG_FLASH_BASE)
#endif
#define	CFG_ENV_SIZE		0x2000
#define CFG_ENV_SECT_SIZE 	0x2000	/* Total Size of Environment Sector */
#if (BFIN_BOOT_MODE == BFIN_BOOT_BYPASS)
#define	ENV_IS_EMBEDDED
#else
#define	ENV_IS_EMBEDDED_CUSTOM
#endif

#define CFG_FLASH_ERASE_TOUT	30000	/* Timeout for Chip Erase (in ms) */
#define CFG_FLASH_ERASEBLOCK_TOUT	5000	/* Timeout for Block Erase (in ms) */
#define CFG_FLASH_WRITE_TOUT	1	/* Timeout for Flash Write (in ms) */

/* JFFS Partition offset set  */
#define CFG_JFFS2_FIRST_BANK 0
#define CFG_JFFS2_NUM_BANKS  1
/* 512k reserved for u-boot */
#define CFG_JFFS2_FIRST_SECTOR 	11

/*
 * following timeouts shall be used once the
 * Flash real protection is enabled
 */
#define CFG_FLASH_LOCK_TOUT	5	/* Timeout for Flash Set Lock Bit (in ms) */
#define CFG_FLASH_UNLOCK_TOUT	10000	/* Timeout for Flash Clear Lock Bits (in ms) */

/*
 * SDRAM settings & memory map
 */

#define CONFIG_MEM_SIZE		128	/* 128, 64, 32, 16 */
#define CONFIG_MEM_ADD_WDTH     11	/* 8, 9, 10, 11    */
#define CONFIG_MEM_MT48LC64M4A2FB_7E	1

#if (BFIN_BOOT_MODE == BFIN_BOOT_BYPASS)
#define CFG_MEMTEST_START	0x00000000	/* memtest works on */
#elif (BFIN_BOOT_MODE == BFIN_BOOT_SPI_MASTER)
#define CFG_MEMTEST_START	0x00100000	/* memtest works on */
#endif

#define	CFG_SDRAM_BASE		0x00000000

#define CFG_MAX_RAM_SIZE	(CONFIG_MEM_SIZE * 1024 *1024)
#define CFG_MEMTEST_END		(CFG_MAX_RAM_SIZE - 0x80000 - 1)

#define CFG_LOAD_ADDR 		CONFIG_LOADADDR
#define CFG_MONITOR_LEN		(256 << 10)	/* Reserve 256 kB for Monitor	*/
#define CFG_MALLOC_LEN		(384 << 10)	/* Reserve 384 kB for malloc() (video/spi are big) */
#define CFG_GBL_DATA_SIZE	0x4000		/* Reserve 16k for Global Data  */

#define CFG_MONITOR_BASE		(CFG_MAX_RAM_SIZE - 0x40000)
#define CFG_MALLOC_BASE		(CFG_MONITOR_BASE - CFG_MALLOC_LEN)
#define CFG_GBL_DATA_ADDR	(CFG_MALLOC_BASE - CFG_GBL_DATA_SIZE)
#define CONFIG_STACKBASE	(CFG_GBL_DATA_ADDR  - 4)

/*
 * Command settings
 */

#define CFG_LONGHELP		1
#define CONFIG_CMDLINE_EDITING	1
#define CONFIG_AUTO_COMPLETE	1
#define CONFIG_ENV_OVERWRITE	1
#define CONFIG_DEBUG_DUMP	1
#define CONFIG_DEBUG_DUMP_SYMS	1

#if (BFIN_BOOT_MODE == BFIN_BOOT_BYPASS)
#define CFG_AUTOLOAD		"no"	/*rarpb, bootp or dhcp commands will perform only a */
#endif

/* configuration lookup from the BOOTP/DHCP server, */
/* but not try to load any image using TFTP	    */

#if (CONFIG_DRIVER_SMC91111)
#define CONFIG_COMMANDS1	(CONFIG_CMD_DFL | \
				 CFG_CMD_PING   | \
				 CFG_CMD_ELF    | \
				 CFG_CMD_CACHE  | \
				 CFG_CMD_JFFS2  | \
				 CFG_CMD_EEPROM | \
				 CFG_CMD_DATE)

#else
#define CONFIG_COMMANDS1	(CONFIG_CMD_DFL | \
				 CFG_CMD_ELF    | \
				 CFG_CMD_CACHE  | \
				 CFG_CMD_JFFS2  | \
				 CFG_CMD_EEPROM | \
				 CFG_CMD_DATE)
#endif

#ifdef CONFIG_SOFT_I2C
#if (!CONFIG_SOFT_I2C)
#undef CONFIG_SOFT_I2C
#endif
#endif

#if (CONFIG_SOFT_I2C)
#define CONFIG_COMMANDS2   CFG_CMD_I2C
#else
#define CONFIG_COMMANDS2 0
#endif /* CONFIG_SOFT_I2C */

#if (BFIN_BOOT_MODE == BFIN_BOOT_BYPASS)
#define CONFIG_COMMANDS  ( CONFIG_COMMANDS1 | CONFIG_COMMANDS2 | CFG_CMD_DHCP)
#elif (BFIN_BOOT_MODE == BFIN_BOOT_SPI_MASTER)
#define CONFIG_COMMANDS  ( CONFIG_COMMANDS1 | CONFIG_COMMANDS2)
#endif

/* This must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

#define CONFIG_BFIN_COMMANDS \
	( CFG_BFIN_CMD_CPLBINFO )

#define CONFIG_BOOTDELAY     5
#define CONFIG_BOOTCOMMAND   "run ramboot"
#define CONFIG_BOOTARGS      "root=/dev/mtdblock0 rw earlyprintk=serial,uart0,57600"
#define CONFIG_LOADADDR      0x1000000

#if (CONFIG_COMMANDS & CFG_CMD_NET)
# if (BFIN_BOOT_MODE == BFIN_BOOT_BYPASS)
#  define UBOOT_ENV_FILE "u-boot.bin"
# else
#  define UBOOT_ENV_FILE "u-boot.ldr"
# endif
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
	"ubootfile=" UBOOT_ENV_FILE "\0" \
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

/*
 * Console settings
 */

#define CONFIG_BAUDRATE		57600
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

#define CFG_PROMPT "bfin> "

#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define CFG_CBSIZE	1024		/* Console I/O Buffer Size */
#else
#define CFG_CBSIZE	256		/* Console I/O Buffer Size */
#endif
#define CFG_PBSIZE	(CFG_CBSIZE+sizeof(CFG_PROMPT)+16)	/* Print Buffer Size */
#define CFG_MAXARGS	16		/* max number of command args */
#define CFG_BARGSIZE	CFG_CBSIZE	/* Boot Argument Buffer Size */

#define CONFIG_LOADS_ECHO	1

/*
 * I2C settings
 * By default PF2 is used as SDA and PF3 as SCL on the Stamp board
 */
#if (CONFIG_SOFT_I2C)

#define I2C_INIT       do { *pFIO_DIR |= PF_SCL; SSYNC(); } while (0)
#define I2C_ACTIVE     do { *pFIO_DIR |= PF_SDA; *pFIO_INEN &= ~PF_SDA; SSYNC(); } while (0)
#define I2C_TRISTATE   do { *pFIO_DIR &= ~PF_SDA; *pFIO_INEN |= PF_SDA; SSYNC(); } while (0)
#define I2C_READ       ((*pFIO_FLAG_D & PF_SDA) != 0)
#define I2C_SDA(bit) \
	do { \
		if (bit) \
			*pFIO_FLAG_S = PF_SDA; \
		else \
			*pFIO_FLAG_C = PF_SDA; \
		SSYNC(); \
	} while (0)
#define I2C_SCL(bit) \
	do { \
		if (bit) \
			*pFIO_FLAG_S = PF_SCL; \
		else \
			*pFIO_FLAG_C = PF_SCL; \
		SSYNC(); \
	} while (0)
#define I2C_DELAY		udelay(5)	/* 1/4 I2C clock duration */

#define CFG_I2C_SPEED		50000
#define CFG_I2C_SLAVE		0xFE
#endif /* CONFIG_SOFT_I2C */

/*
 * Compact Flash settings
 */

/* Enabled below option for CF support */
/* #define CONFIG_STAMP_CF	1 */

#if defined(CONFIG_STAMP_CF) && (CONFIG_COMMANDS & CFG_CMD_IDE)

#define CONFIG_MISC_INIT_R	1
#define CONFIG_DOS_PARTITION	1
/*
 * IDE/ATA stuff
 */
#undef  CONFIG_IDE_8xx_DIRECT		/* no pcmcia interface required */
#undef  CONFIG_IDE_LED			/* no led for ide supported */
#undef  CONFIG_IDE_RESET		/* no reset for ide supported */

#define CFG_IDE_MAXBUS		1	/* max. 1 IDE busses */
#define CFG_IDE_MAXDEVICE	(CFG_IDE_MAXBUS*1) /* max. 1 drives per IDE bus */

#define CFG_ATA_BASE_ADDR	0x20200000
#define CFG_ATA_IDE0_OFFSET	0x0000

#define CFG_ATA_DATA_OFFSET	0x0020	/* Offset for data I/O */
#define CFG_ATA_REG_OFFSET	0x0020	/* Offset for normal register accesses */
#define CFG_ATA_ALT_OFFSET	0x0007	/* Offset for alternate registers */

#define CFG_ATA_STRIDE		2
#endif

/*
 * Miscellaneous configurable options
 */

#define	CFG_HZ			1000	/* 1ms time tick */

#define CFG_BOOTM_LEN		0x4000000/* Large Image Length, set to 64 Meg */

#define CONFIG_SHOW_BOOT_PROGRESS 1	/* Show boot progress on LEDs */

/*
 * Serial Flash Infomation
 */
#define CONFIG_SPI

/*
 * FLASH organization and environment definitions
 */

/* 0xFF, 0xBBC3BBc3, 0x99B39983 */
/*#define CONFIG_EBIU_AMGCTL_VAL		(AMBEN_P0 | AMBEN_P1 | AMBEN_P2 | AMCKEN)
#define CONFIG_EBIU_AMBCTL0_VAL		(B1WAT_11 | B1RAT_11 | B1HT_3 | B1ST_4 | B1TT_4 | B1RDYPOL | \
				B1RDYEN | B0WAT_11 | B0RAT_11 | B0HT_3 | B0ST_4 | B0TT_4 | B0RDYPOL | B0RDYEN)
#define CONFIG_EBIU_AMBCTL1_VAL   		(B3WAT_9 | B3RAT_9 | B3HT_2 | B3ST_3 | B3TT_4 | B3RDYPOL | \
				B3RDYEN | B2WAT_9 | B2RAT_9 | B2HT_2 | B2ST_4 | B2TT_4 | B2RDYPOL | B2RDYEN)
*/
#define CONFIG_EBIU_AMGCTL_VAL		0xFF
#define CONFIG_EBIU_AMBCTL0_VAL		0xBBC3BBC3
#define CONFIG_EBIU_AMBCTL1_VAL		0x99B39983
#define CF_CONFIG_EBIU_AMBCTL1_VAL		0x99B3ffc2

#include <asm/blackfin-config-post.h>

#endif
