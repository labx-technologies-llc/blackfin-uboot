/*
 * U-boot - Configuration file for BF533 STAMP board
 */

#ifndef __CONFIG_BF533_STAMP_H__
#define __CONFIG_BF533_STAMP_H__

#include <asm/blackfin-config-pre.h>


/*
 * Processor Settings
 */
#define CONFIG_BFIN_CPU             bf533-0.3
#define CONFIG_BFIN_BOOT_MODE       BFIN_BOOT_BYPASS


/*
 * Clock Settings
 *	CCLK = (CLKIN * VCO_MULT) / CCLK_DIV
 *	SCLK = (CLKIN * VCO_MULT) / SCLK_DIV
 */
/* CONFIG_CLKIN_HZ is any value in Hz					*/
#define CONFIG_CLKIN_HZ			11059200
/* CLKIN_HALF controls the DF bit in PLL_CTL      0 = CLKIN		*/
/*                                                1 = CLKIN / 2		*/
#define CONFIG_CLKIN_HALF		0
/* PLL_BYPASS controls the BYPASS bit in PLL_CTL  0 = do not bypass	*/
/*                                                1 = bypass PLL	*/
#define CONFIG_PLL_BYPASS		0
/* VCO_MULT controls the MSEL (multiplier) bits in PLL_CTL		*/
/* Values can range from 0-63 (where 0 means 64)			*/
#define CONFIG_VCO_MULT			36
/* CCLK_DIV controls the core clock divider				*/
/* Values can be 1, 2, 4, or 8 ONLY					*/
#define CONFIG_CCLK_DIV			1
/* SCLK_DIV controls the system clock divider				*/
/* Values can range from 1-15						*/
#define CONFIG_SCLK_DIV			5


/*
 * Memory Settings
 */
#define CONFIG_MEM_ADD_WDTH	11
#define CONFIG_MEM_SIZE		128

#define CONFIG_EBIU_SDRRC_VAL	0x268
#define CONFIG_EBIU_SDGCTL_VAL	0x911109
#define CONFIG_EBIU_SDBCTL_VAL	(EBSZ_128 | EBCAW_11 | EBE)

#define CONFIG_EBIU_AMGCTL_VAL	0xFF
#define CONFIG_EBIU_AMBCTL0_VAL	0xBBC3BBC3
#define CONFIG_EBIU_AMBCTL1_VAL	0x99B39983

#define CFG_MONITOR_LEN		(256 * 1024)	/* Reserve 256 kB for monitor */
#define CFG_MALLOC_LEN		(384 * 1024)	/* Reserve 384 kB for malloc() (video/spi are big) */
#define CFG_GBL_DATA_SIZE	0x4000		/* Reserve 16k for Global Data */


/*
 * Network Settings
 */
#define ADI_CMDS_NETWORK	1
#define CONFIG_DRIVER_SMC91111	1
#define CONFIG_SMC91111_BASE	0x20300300
#define SMC91111_EEPROM_INIT()	{ *pFIO_DIR = 0x01; *pFIO_FLAG_S = 0x01; SSYNC(); }
#define CONFIG_HOSTNAME		bf533-stamp
/* To remove hardcoding and enable MAC storage in EEPROM  */
/* #define CONFIG_ETHADDR	02:80:ad:20:31:b8 */


/*
 * Flash Settings
 */
#define CFG_FLASH_CFI		/* The flash is CFI compatible  */
#define CFG_FLASH_CFI_DRIVER	/* Use common CFI driver	*/
#define	CFG_FLASH_CFI_AMD_RESET

#define CFG_FLASH_BASE		0x20000000
#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CFG_MAX_FLASH_SECT	67	/* max number of sectors on one chip */

#if (CONFIG_BFIN_BOOT_MODE == BFIN_BOOT_SPI_MASTER)
#define CFG_ENV_IS_IN_EEPROM	1
#define CFG_ENV_OFFSET		0x4000
#else
#define CFG_ENV_IS_IN_FLASH	1
#define CFG_ENV_ADDR		0x20004000
#define	CFG_ENV_OFFSET		(CFG_ENV_ADDR - CFG_FLASH_BASE)
#endif
#define	CFG_ENV_SIZE		0x2000
#define CFG_ENV_SECT_SIZE 	0x2000	/* Total Size of Environment Sector */
#if (CONFIG_BFIN_BOOT_MODE == BFIN_BOOT_BYPASS)
#define	ENV_IS_EMBEDDED
#else
#define	ENV_IS_EMBEDDED_CUSTOM
#endif

/* CONFIG_SPI_BAUD controls the SPI peripheral clock divider		*/
/* Values can range from 2-65535					*/
/* SCK Frequency = SCLK / (2 * CONFIG_SPI_BAUD)				*/
#define CONFIG_SPI
#define CONFIG_SPI_BAUD			2
#define CONFIG_SPI_BAUD_INITBLOCK	4


/*
 * I2C Settings
 * By default PF2 is used as SDA and PF3 as SCL on the Stamp board
 */
#define CONFIG_SOFT_I2C
#define PF_SCL			PF3
#define PF_SDA			PF2
#ifdef CONFIG_SOFT_I2C
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
#define CFG_I2C_SLAVE		0
#endif


/*
 * Compact Flash / IDE / ATA Settings
 */

/* Enabled below option for CF support */
/* #define CONFIG_STAMP_CF */
#if defined(CONFIG_STAMP_CF) && (CONFIG_COMMANDS & CFG_CMD_IDE)
#define CONFIG_MISC_INIT_R
#define CONFIG_DOS_PARTITION	1
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

#undef CONFIG_EBIU_AMBCTL1_VAL
#define CONFIG_EBIU_AMBCTL1_VAL	0x99B3ffc2
#endif


/*
 * Misc Settings
 */
#define CONFIG_RTC_BFIN

#define CONFIG_BFIN_COMMANDS \
	( CFG_BFIN_CMD_CPLBINFO )

/* FLASH/ETHERNET uses the same async bank */
#define SHARED_RESOURCES 	1


/*
 * Pull in common ADI header for remaining command/environment setup
 */
#include <configs/bfin_adi_common.h>

#include <asm/blackfin-config-post.h>

#endif
