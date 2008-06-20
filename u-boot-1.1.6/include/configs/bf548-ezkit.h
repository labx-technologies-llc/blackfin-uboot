/*
 * U-boot - Configuration file for BF548 STAMP board
 */

#ifndef __CONFIG_BF548_EZKIT_H__
#define __CONFIG_BF548_EZKIT_H__

#include <asm/blackfin-config-pre.h>


/*
 * Processor Settings
 */
#define CONFIG_BFIN_CPU             bf548-0.0
#define CONFIG_BFIN_BOOT_MODE       BFIN_BOOT_PARA


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
#define CONFIG_VCO_MULT			21
/* CCLK_DIV controls the core clock divider				*/
/* Values can be 1, 2, 4, or 8 ONLY					*/
#define CONFIG_CCLK_DIV			1
/* SCLK_DIV controls the system clock divider				*/
/* Values can range from 1-15						*/
#define CONFIG_SCLK_DIV			4


/*
 * Memory Settings
 */
#define CONFIG_MEM_ADD_WDTH	10
#define CONFIG_MEM_SIZE		64

#define CONFIG_EBIU_DDRCTL0_VAL	0x218A83FE
#define CONFIG_EBIU_DDRCTL1_VAL	0x20022222
#define CONFIG_EBIU_DDRCTL2_VAL	0x00000021

/* Default EZ-Kit bank mapping:
 *	Async Bank 0 - 32MB Burst Flash
 *	Async Bank 1 - Ethernet
 *	Async Bank 2 - Nothing
 *	Async Bank 3 - Nothing
 */
#define CONFIG_EBIU_AMGCTL_VAL	0xFF
#define CONFIG_EBIU_AMBCTL0_VAL	0x7BB07BB0
#define CONFIG_EBIU_AMBCTL1_VAL	0xFFC27BB0
#define CONFIG_EBIU_FCTL_VAL	(BCLK_4)
#define CONFIG_EBIU_MODE_VAL	(B0MODE_FLASH)

#define CFG_MONITOR_LEN		(384 * 1024)	/* Reserve 384 kB for monitor */
#define CFG_MALLOC_LEN		(640 * 1024)	/* Reserve 640 kB for malloc() (video/spi are big) */
#define CFG_GBL_DATA_SIZE	0x4000


/*
 * Network Settings
 */
#define ADI_CMDS_NETWORK	1
#define CONFIG_DRIVER_SMSC9118	1
#define CONFIG_SMSC9118_BASE	0x24000000
#define CONFIG_HOSTNAME		bf548-ezkit
/* Uncomment next line to use fixed MAC address */
/* #define CONFIG_ETHADDR	02:80:ad:20:31:e8 */


/*
 * Flash Settings
 */
#define CFG_FLASH_CFI		/* The flash is CFI compatible */
#define CFG_FLASH_CFI_DRIVER	/* Use common CFI driver */
#define CFG_FLASH_BASE		0x20000000
#define CFG_FLASH_PROTECTION
#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CFG_MAX_FLASH_SECT	259	/* max number of sectors on one chip */
#if (CONFIG_BFIN_BOOT_MODE == BFIN_BOOT_SPI_MASTER)
#define CFG_ENV_IS_IN_EEPROM           1
#define CFG_ENV_OFFSET                 0x4000
#define CFG_ENV_SIZE                   0x2000
#elif (CONFIG_BFIN_BOOT_MODE == BFIN_BOOT_NAND)
#define CFG_ENV_IS_IN_NAND             1
#define CFG_ENV_OFFSET                 0x1000
#define CFG_ENV_SIZE                   0x1000
#else
#define	CFG_ENV_IS_IN_FLASH	1
#define CFG_ENV_ADDR		0x20002000
#define CFG_ENV_OFFSET		(CFG_ENV_ADDR - CFG_FLASH_BASE)
#define CFG_ENV_SIZE		0x2000
#define	CFG_ENV_SECT_SIZE	(128 * 1024)	/* Total Size of Environment Sector */
#endif
#define ENV_IS_EMBEDDED_CUSTOM

/* CONFIG_SPI_BAUD controls the SPI peripheral clock divider		*/
/* Values can range from 2-65535					*/
/* SCK Frequency = SCLK / (2 * CONFIG_SPI_BAUD)				*/
#define CONFIG_SPI
#define CONFIG_SPI_BAUD			2
#define CONFIG_SPI_BAUD_INITBLOCK	4


/*
 * NAND Settings
 */
#define CFG_BFIN_NFC_CTL_VAL	0x0033
#define CFG_NAND_BASE		0 /* not actually used */
#define CFG_MAX_NAND_DEVICE	1
#define NAND_MAX_CHIPS		1
#define ADI_CMDS_EXTRA		CFG_CMD_NAND


/*
 * I2C Settings
 */
#define CONFIG_HARD_I2C			1	/* I2C TWI */
#define CFG_I2C_SPEED			50000
#define CFG_I2C_SLAVE			0


/*
 * Misc Settings
 */
#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_MISC_INIT_R
#define CONFIG_RTC_BFIN
#define CONFIG_UART_CONSOLE	1

#define CONFIG_BFIN_COMMANDS \
	( CFG_BFIN_CMD_BOOTLDR | \
	  CFG_BFIN_CMD_CPLBINFO | \
	  CFG_BFIN_CMD_OTP )

#ifndef __ADSPBF542__
/* Don't waste time transferring a logo over the UART */
# if (CONFIG_BFIN_BOOT_MODE != BFIN_BOOT_UART)
#  define CONFIG_VIDEO
# endif
# define CONFIG_DEB_DMA_URGENT
#endif

/* Define if want to do post memory test */
#undef CONFIG_POST
#ifdef CONFIG_POST
#define FLASH_START_POST_BLOCK 11       /* Should > = 11 */
#define FLASH_END_POST_BLOCK   71       /* Should < = 71 */
#endif


/*
 * Pull in common ADI header for remaining command/environment setup
 */
#include <configs/bfin_adi_common.h>

#include <asm/blackfin-config-post.h>

#endif
