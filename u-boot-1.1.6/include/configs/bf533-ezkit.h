/*
 * U-boot - Configuration file for BF533 EZKIT board
 */

#ifndef __CONFIG_BF533_EZKIT_H__
#define __CONFIG_BF533_EZKIT_H__

#include <asm/blackfin-config-pre.h>

#define BFIN_CPU             bf533-0.3
#define BFIN_BOOT_MODE       BFIN_BOOT_BYPASS

#define CONFIG_BAUDRATE		57600
#define CFG_AUTOLOAD		"no"	/*rarpb, bootp or dhcp commands will perform only a */

#define CFG_LONGHELP		1
#define CONFIG_CMDLINE_EDITING	1
#define CONFIG_AUTO_COMPLETE	1
#define CONFIG_ENV_OVERWRITE	1
#define CONFIG_DEBUG_DUMP	1
#define CONFIG_DEBUG_DUMP_SYMS	1

#define CONFIG_DRIVER_SMC91111	1
#define CONFIG_SMC91111_BASE	0x20310300

#if 0
#define	CONFIG_MII
#define CFG_DISCOVER_PHY
#endif

#define CONFIG_RTC_BFIN		1

#define CONFIG_PANIC_HANG 1

/* CONFIG_CLKIN_HZ is any value in Hz				*/
#define CONFIG_CLKIN_HZ		27000000
/* CONFIG_CLKIN_HALF controls what is passed to PLL 0=CLKIN	*/
/*						    1=CLKIN/2	*/
#define CONFIG_CLKIN_HALF	0
/* CONFIG_PLL_BYPASS controls if the PLL is used 0=don't bypass	*/
/*						 1=bypass PLL	*/
#define CONFIG_PLL_BYPASS	0
/* CONFIG_VCO_MULT controls what the multiplier of the PLL is.	*/
/* Values can range from 1-64					*/
#define CONFIG_VCO_MULT		22
/* CONFIG_CCLK_DIV controls what the core clock divider is	*/
/* Values can be 1, 2, 4, or 8 ONLY				*/
#define CONFIG_CCLK_DIV		1
/* CONFIG_SCLK_DIV controls what the peripheral clock divider is */
/* Values can range from 1-15					*/
#define CONFIG_SCLK_DIV		5
/* CONFIG_SPI_BAUD controls the SPI peripheral clock divider	*/
/* Values can range from 2-65535				*/
/* SCK Frequency = SCLK / (2 * CONFIG_SPI_BAUD)			*/
#define CONFIG_SPI_BAUD		2
#define CONFIG_SPI_BAUD_INITBLOCK	4

#define CONFIG_LOADS_ECHO	1

#define CONFIG_COMMANDS			(CONFIG_CMD_DFL	| \
					 CFG_CMD_PING	| \
					 CFG_CMD_ELF	| \
					 CFG_CMD_I2C	| \
					 CFG_CMD_JFFS2	| \
					 CFG_CMD_DATE)

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

#define CFG_PROMPT "bfin> "

#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define	CFG_CBSIZE		1024	/* Console I/O Buffer Size */
#else
#define	CFG_CBSIZE		256	/* Console I/O Buffer Size */
#endif
#define CONFIG_MEM_SIZE		32	/* 128, 64, 32, 16 */
#define	CFG_PBSIZE		(CFG_CBSIZE+sizeof(CFG_PROMPT)+16)	/* Print Buffer Size */
#define	CFG_MAXARGS		16	/* max number of command args */
#define CFG_BARGSIZE		CFG_CBSIZE	/* Boot Argument Buffer Size */
#define CFG_MEMTEST_START	0x00000000	/* memtest works on */
#define CFG_MEMTEST_END		( (CONFIG_MEM_SIZE - 1) * 1024 * 1024)	/* 1 ... 31 MB in DRAM */
#define	CFG_LOAD_ADDR		0x01000000	/* default load address */
#define	CFG_HZ			1000	/* decrementer freq: 10 ms ticks */
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
#define	CFG_SDRAM_BASE		0x00000000
#define CFG_MAX_RAM_SIZE	(CONFIG_MEM_SIZE * 1024 * 1024)
#define CFG_FLASH_BASE		0x20000000

#define CFG_MONITOR_LEN		(256 << 10)	/* Reserve 256 kB for Monitor	*/
#define CFG_MONITOR_BASE	(CFG_MAX_RAM_SIZE - CFG_MONITOR_LEN)
#define CFG_MALLOC_LEN		(128 << 10)	/* Reserve 128 kB for malloc()	*/
#define CFG_MALLOC_BASE		(CFG_MONITOR_BASE - CFG_MALLOC_LEN)
#define CFG_GBL_DATA_SIZE	0x4000
#define CFG_GBL_DATA_ADDR	(CFG_MALLOC_BASE - CFG_GBL_DATA_SIZE)
#define CONFIG_STACKBASE	(CFG_GBL_DATA_ADDR  - 4)

#define CFG_FLASH0_BASE		0x20000000
#define CFG_FLASH1_BASE		0x20200000
#define CFG_FLASH2_BASE		0x20280000
#define CFG_MAX_FLASH_BANKS	3	/* max number of memory banks */
#define CFG_MAX_FLASH_SECT	40	/* max number of sectors on one chip */

#define	CFG_ENV_IS_IN_FLASH	1
#define CFG_ENV_ADDR		0x20020000
#define	CFG_ENV_SECT_SIZE	0x10000	/* Total Size of Environment Sector */

/* JFFS Partition offset set  */
#define CFG_JFFS2_FIRST_BANK	0
#define CFG_JFFS2_NUM_BANKS	1
/* 512k reserved for u-boot */
#define CFG_JFFS2_FIRST_SECTOR	11

#define FLASH_TOT_SECT		40

/*
 * Initialize PSD4256 registers for using I2C
 */
#define	CONFIG_MISC_INIT_R

/*
 * I2C settings
 * By default PF1 is used as SDA and PF0 as SCL on the Stamp board
 */
#define CONFIG_SOFT_I2C		1	/* I2C bit-banged */
/*
 * Software (bit-bang) I2C driver configuration
 */
#define PF_SCL			PF0
#define PF_SDA			PF1

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
#define I2C_DELAY	udelay(5)	/* 1/4 I2C clock duration */

#define CFG_I2C_SPEED		50000
#define CFG_I2C_SLAVE		0xFE

#define CFG_BOOTM_LEN		0x4000000	/* Large Image Length, set to 64 Meg */

#define CONFIG_EBIU_SDRRC_VAL  0x398
#define CONFIG_EBIU_SDGCTL_VAL 0x91118d
#define CONFIG_EBIU_SDBCTL_VAL 0x13

#define CONFIG_EBIU_AMGCTL_VAL		0xFF
#define CONFIG_EBIU_AMBCTL0_VAL		0x7BB07BB0
#define CONFIG_EBIU_AMBCTL1_VAL		0xFFC27BB0

#include <asm/blackfin-config-post.h>

#endif
