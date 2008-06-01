/*
 * U-boot - Configuration file for BF537 STAMP board
 */

#ifndef __CONFIG_BF537_STAMP_H__
#define __CONFIG_BF537_STAMP_H__

#include <asm/blackfin-config-pre.h>


/*
 * Processor Settings
 */
#define CONFIG_BFIN_CPU             bf537-0.2
#define CONFIG_BFIN_BOOT_MODE       BFIN_BOOT_BYPASS


/*
 * Clock Settings
 *	CCLK = (CLKIN * VCO_MULT) / CCLK_DIV
 *	SCLK = (CLKIN * VCO_MULT) / SCLK_DIV
 */
/* CONFIG_CLKIN_HZ is any value in Hz					*/
#define CONFIG_CLKIN_HZ			25000000
/* CLKIN_HALF controls the DF bit in PLL_CTL      0 = CLKIN		*/
/*                                                1 = CLKIN / 2		*/
#define CONFIG_CLKIN_HALF		0
/* PLL_BYPASS controls the BYPASS bit in PLL_CTL  0 = do not bypass	*/
/*                                                1 = bypass PLL	*/
#define CONFIG_PLL_BYPASS		0
/* VCO_MULT controls the MSEL (multiplier) bits in PLL_CTL		*/
/* Values can range from 0-63 (where 0 means 64)			*/
#define CONFIG_VCO_MULT			20
/* CCLK_DIV controls the core clock divider				*/
/* Values can be 1, 2, 4, or 8 ONLY					*/
#define CONFIG_CCLK_DIV			1
/* SCLK_DIV controls the system clock divider				*/
/* Values can range from 1-15						*/
#define CONFIG_SCLK_DIV			5


/*
 * Memory Settings
 */
#define CONFIG_MEM_ADD_WDTH	10
#define CONFIG_MEM_SIZE		64

#define CONFIG_EBIU_SDRRC_VAL	0x306
#define CONFIG_EBIU_SDGCTL_VAL	0x91114d
#define CONFIG_EBIU_SDBCTL_VAL	(EBSZ_64 | EBCAW_10 | EBE)

#define CONFIG_EBIU_AMGCTL_VAL	0xFF
#define CONFIG_EBIU_AMBCTL0_VAL	0x7BB07BB0
#define CONFIG_EBIU_AMBCTL1_VAL	0xFFC27BB0

#define CFG_MONITOR_LEN		(256 * 1024)	/* Reserve 256 kB for monitor */
#define CFG_MALLOC_LEN		(384 * 1024)	/* Reserve 384 kB for malloc() (video/spi are big) */
#define CFG_GBL_DATA_SIZE	0x4000


/*
 * Network Settings
 */
#ifndef __ADSPBF534__
#define ADI_CMDS_NETWORK	1
#define CONFIG_BFIN_MAC
#define CONFIG_NETCONSOLE	1
#define CONFIG_NET_MULTI	1
#endif
#define CONFIG_HOSTNAME		bf537-stamp
/* Uncomment next line to use fixed MAC address */
/* #define CONFIG_ETHADDR	02:80:ad:20:31:e8 */


/*
 * Flash Settings
 */
#define CFG_FLASH_BASE		0x20000000
#define CFG_FLASH_CFI		/* The flash is CFI compatible */
#define CFG_FLASH_CFI_DRIVER	/* Use common CFI driver */
#define CFG_FLASH_PROTECTION
#define CFG_MAX_FLASH_BANKS	1
#define CFG_MAX_FLASH_SECT	71	/* some have 67 sectors (M29W320DB), but newer have 71 (M29W320EB) */

#if (CONFIG_BFIN_BOOT_MODE == BFIN_BOOT_SPI_MASTER)
#define CFG_ENV_IS_IN_EEPROM	1
#define CFG_ENV_OFFSET		0x4000
#define CFG_ENV_HEADER		(CFG_ENV_OFFSET + 0x16e) /* 0x12A is the length of LDR file header */
#else
#define	CFG_ENV_IS_IN_FLASH	1
#define CFG_ENV_ADDR		0x20004000
#define CFG_ENV_OFFSET		(CFG_ENV_ADDR - CFG_FLASH_BASE)
#endif
#define CFG_ENV_SIZE		0x2000
#define	CFG_ENV_SECT_SIZE	0x2000	/* Total Size of Environment Sector */
#if (CONFIG_BFIN_BOOT_MODE == BFIN_BOOT_BYPASS)
#define ENV_IS_EMBEDDED
#else
#define ENV_IS_EMBEDDED_CUSTOM
#endif

/* CONFIG_SPI_BAUD controls the SPI peripheral clock divider		*/
/* Values can range from 2-65535					*/
/* SCK Frequency = SCLK / (2 * CONFIG_SPI_BAUD)				*/
#define CONFIG_SPI
#define CONFIG_SPI_BAUD			2
#define CONFIG_SPI_BAUD_INITBLOCK	4


/*
 * I2C Settings
 */
#define CONFIG_HARD_I2C		1	/* I2C TWI */
#define CFG_I2C_SPEED		50000
#define CFG_I2C_SLAVE		0


/*
 * NAND Settings
 */
/* #define CONFIG_BF537_NAND */
#ifdef CONFIG_BF537_NAND
# define ADD_NAND_CMD		CFG_CMD_NAND
#else
# define ADD_NAND_CMD		0
#endif

#define CFG_NAND_ADDR		0x20212000
#define CFG_NAND_BASE		CFG_NAND_ADDR
#define CFG_MAX_NAND_DEVICE	1
#define SECTORSIZE		512
#define ADDR_COLUMN		1
#define ADDR_PAGE		2
#define ADDR_COLUMN_PAGE	3
#define NAND_ChipID_UNKNOWN	0x00
#define NAND_MAX_FLOORS		1
#define NAND_MAX_CHIPS		1
#define BFIN_NAND_READY		PF3

#define NAND_WAIT_READY(nand)  			\
	do { 					\
		int timeout = 0; 		\
		while(!(*pPORTFIO & PF3)) 	\
			if (timeout++ > 100000)	\
				break;		\
	} while (0)

#define BFIN_NAND_CLE		(1<<2)	/* A2 -> Command Enable */
#define BFIN_NAND_ALE		(1<<1)	/* A1 -> Address Enable */

#define WRITE_NAND_COMMAND(d, adr) do{ *(volatile __u8 *)((unsigned long)adr | BFIN_NAND_CLE) = (__u8)(d); } while(0)
#define WRITE_NAND_ADDRESS(d, adr) do{ *(volatile __u8 *)((unsigned long)adr | BFIN_NAND_ALE) = (__u8)(d); } while(0)
#define WRITE_NAND(d, adr) do{ *(volatile __u8 *)((unsigned long)adr) = (__u8)d; } while(0)
#define READ_NAND(adr) ((volatile unsigned char)(*(volatile __u8 *)(unsigned long)adr))


/*
 * CF-CARD IDE-HDD Support
 */
/* #define CONFIG_BFIN_TRUE_IDE */	/* Add CF flash card support */
/* #define CONFIG_BFIN_CF_IDE */	/* Add CF flash card support */
/* #define CONFIG_BFIN_HDD_IDE */	/* Add IDE Disk Drive (HDD) support */

#if defined(CONFIG_BFIN_CF_IDE) || defined(CONFIG_BFIN_HDD_IDE) || defined(CONFIG_BFIN_TRUE_IDE)
# define CONFIG_BFIN_IDE	1
# define ADD_IDE_CMD		CFG_CMD_IDE
#else
# define ADD_IDE_CMD		0
#endif

#if defined(CONFIG_BFIN_IDE)

#define CONFIG_DOS_PARTITION	1
/*
 * IDE/ATA stuff
 */
#undef  CONFIG_IDE_8xx_DIRECT	/* no pcmcia interface required */
#undef  CONFIG_IDE_LED		/* no led for ide supported */
#undef  CONFIG_IDE_RESET	/* no reset for ide supported */

#define CFG_IDE_MAXBUS		1	/* max. 1 IDE busses */
#define CFG_IDE_MAXDEVICE	(CFG_IDE_MAXBUS*1)	/* max. 1 drives per IDE bus */

#undef  CONFIG_EBIU_AMBCTL1_VAL
#define CONFIG_EBIU_AMBCTL1_VAL		0xFFC3FFC3

#define CONFIG_CF_ATASEL_DIS	0x20311800
#define CONFIG_CF_ATASEL_ENA	0x20311802

#if defined(CONFIG_BFIN_TRUE_IDE)
/*
 * Note that these settings aren't for the most part used in include/ata.h
 * when all of the ATA registers are setup
 */
#define CFG_ATA_BASE_ADDR	0x2031C000
#define CFG_ATA_IDE0_OFFSET	0x0000
#define CFG_ATA_DATA_OFFSET	0x0020	/* Offset for data I/O */
#define CFG_ATA_REG_OFFSET	0x0020	/* Offset for normal register accesses */
#define CFG_ATA_ALT_OFFSET	0x001C	/* Offset for alternate registers */
#define CFG_ATA_STRIDE		2	/* CF.A0 --> Blackfin.Ax */
#endif				/* CONFIG_BFIN_TRUE_IDE */

#if defined(CONFIG_BFIN_CF_IDE)	/* USE CompactFlash Storage Card in the common memory space */
#define CFG_ATA_BASE_ADDR	0x20211800
#define CFG_ATA_IDE0_OFFSET	0x0000
#define CFG_ATA_DATA_OFFSET	0x0000	/* Offset for data I/O */
#define CFG_ATA_REG_OFFSET	0x0000	/* Offset for normal register accesses */
#define CFG_ATA_ALT_OFFSET	0x000E	/* Offset for alternate registers */
#define CFG_ATA_STRIDE		1	/* CF.A0 --> Blackfin.Ax */
#endif				/* CONFIG_BFIN_CF_IDE */

#if defined(CONFIG_BFIN_HDD_IDE)	/* USE TRUE IDE */
#define CFG_ATA_BASE_ADDR	0x20314000
#define CFG_ATA_IDE0_OFFSET	0x0000
#define CFG_ATA_DATA_OFFSET	0x0020	/* Offset for data I/O */
#define CFG_ATA_REG_OFFSET	0x0020	/* Offset for normal register accesses */
#define CFG_ATA_ALT_OFFSET	0x001C	/* Offset for alternate registers */
#define CFG_ATA_STRIDE		2	/* CF.A0 --> Blackfin.A1 */

#undef  CONFIG_SCLK_DIV
#define CONFIG_SCLK_DIV		8
#endif				/* CONFIG_BFIN_HDD_IDE */

#endif				/*CONFIG_BFIN_IDE */


/*
 * Misc Settings
 */
#define CONFIG_MISC_INIT_R
#define CONFIG_RTC_BFIN

/* #define CONFIG_BF537_STAMP_LEDCMD	1 */

#define ADI_CMDS_EXTRA (ADD_IDE_CMD | ADD_NAND_CMD)
#define CONFIG_BFIN_COMMANDS \
	( CFG_BFIN_CMD_BOOTLDR | \
	  CFG_BFIN_CMD_CPLBINFO )

/* Define if want to do post memory test */
#undef CONFIG_POST
#ifdef CONFIG_POST
#define FLASH_START_POST_BLOCK	11	/* Should > = 11 */
#define FLASH_END_POST_BLOCK	71	/* Should < = 71 */
#endif


/*
 * Pull in common ADI header for remaining command/environment setup
 */
#include <configs/bfin_adi_common.h>

#include <asm/blackfin-config-post.h>

#endif
