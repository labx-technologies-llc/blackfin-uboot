/*
 * U-boot - Configuration file for BF506F EZ-Kit board
 */

#ifndef __CONFIG_BF506F_EZKIT_H__
#define __CONFIG_BF506F_EZKIT_H__

#include <asm/config-pre.h>


/*
 * Processor Settings
 */
#define CONFIG_BFIN_CPU             bf506-0.0
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
#define CONFIG_VCO_MULT			16
/* CCLK_DIV controls the core clock divider				*/
/* Values can be 1, 2, 4, or 8 ONLY					*/
#define CONFIG_CCLK_DIV			1
/* SCLK_DIV controls the system clock divider				*/
/* Values can range from 1-15						*/
#define CONFIG_SCLK_DIV			5


/*
 * Memory Settings
 */
#define CONFIG_MEM_SIZE		0

#define CONFIG_EBIU_SDRRC_VAL	0x0096
#define CONFIG_EBIU_SDGCTL_VAL	(SCTLE | CL_3 | PASR_ALL | TRAS_6 | TRP_3 | TRCD_3 | TWR_2 | PSS)

#define CONFIG_EBIU_AMGCTL_VAL	(AMCKEN | AMBEN_ALL)
#define CONFIG_EBIU_AMBCTL0_VAL	(B1WAT_15 | B1RAT_15 | B1HT_3 | B1RDYPOL | B0WAT_15 | B0RAT_15 | B0HT_3 | B0RDYPOL)
#define CONFIG_EBIU_AMBCTL1_VAL	(B3WAT_15 | B3RAT_15 | B3HT_3 | B3RDYPOL | B2WAT_15 | B2RAT_15 | B2HT_3 | B2RDYPOL)

#define CONFIG_SYS_MONITOR_BASE (L1_DATA_A_SRAM_END)
#define CONFIG_SYS_MONITOR_LEN	(4 * 1024)
#define CONFIG_SYS_MALLOC_LEN	(4 * 1024)


/*
 * Flash Settings
 */
#define CONFIG_FLASH_CFI_DRIVER
#define CONFIG_SYS_FLASH_BASE		0x20000000
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_MAX_FLASH_SECT	71
#define CONFIG_CMD_FLASH


/*
 * SPI Settings
 */
#define CONFIG_BFIN_SPI
#define CONFIG_ENV_SPI_MAX_HZ	30000000
#define CONFIG_SF_DEFAULT_SPEED	30000000
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_STMICRO
#define CONFIG_CMD_SF
#define CONFIG_CMD_SPI


/*
 * Env Storage Settings
 */
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_SIZE 0x400


/*
 * Misc Settings
 */
#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_ICACHE_OFF
#define CONFIG_DCACHE_OFF
#define CONFIG_UART_CONSOLE	0
#define CONFIG_BAUDRATE 115200

#define CONFIG_CMD_MEMORY
#undef CONFIG_GZIP
#undef CONFIG_ZLIB
#undef CONFIG_CMD_BOOTM
#undef CONFIG_BOOTM_RTEMS
#undef CONFIG_BOOTM_LINUX


#endif
