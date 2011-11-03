/*
 * U-boot - Configuration file for BF609 EZ-Kit board
 */

#ifndef __CONFIG_BF609_EZKIT_H__
#define __CONFIG_BF609_EZKIT_H__

#include <asm/config-pre.h>


/*
 * Processor Settings
 */
#define __ADSPBF60x__ 1
#define __ADSPBF609__ 1
#undef __ADSPBF592__
#undef __ADSPBF59x__
#define CONFIG_BFIN_CPU             bf609-0.0
#define CONFIG_BFIN_BOOT_MODE       BFIN_BOOT_PARA


/*
 * Clock Settings
 *	CCLK = (CLKIN * VCO_MULT) / CCLK_DIV
 *	SCLK = (CLKIN * VCO_MULT) / SYSCLK_DIV
 *	SCLK0 = SCLK / SCLK0_DIV
 *	SCLK1 = SCLK / SCLK1_DIV
 */
/* CONFIG_CLKIN_HZ is any value in Hz					*/
#define CONFIG_CLKIN_HZ			25000000
/* CLKIN_HALF controls the DF bit in PLL_CTL      0 = CLKIN		*/
/*                                                1 = CLKIN / 2		*/
#define CONFIG_CLKIN_HALF		0

/* VCO_MULT controls the MSEL (multiplier) bits in PLL_CTL		*/
/* Values can range from 0-127 (where 0 means 128)			*/
#define CONFIG_VCO_MULT			15

/* CCLK_DIV controls the core clock divider				*/
/* Values can range from 0-31 (where 0 means 32)			*/
#define CONFIG_CCLK_DIV			1
/* SCLK_DIV controls the system clock divider				*/
/* Values can range from 0-31 (where 0 means 32)			*/
#define CONFIG_SYSCLK_DIV		3
/* Values can range from 0-7 (where 0 means 8)				*/
#define CONFIG_SCLK0_DIV		1
#define CONFIG_SCLK1_DIV		2
/* DCLK_DIV controls the DDR clock divider				*/
/* Values can range from 0-31 (where 0 means 32)			*/
#define CONFIG_DCLK_DIV			3
/* OCLK_DIV controls the output clock divider				*/
/* Values can range from 0-127 (where 0 means 128)			*/
#define CONFIG_OCLK_DIV			16

#define CONFIG_BFIN_GET_VCO CONFIG_CLKIN_HZ
#define CONFIG_PLL_CLK  (get_vco()*CONFIG_VCO_MULT)

#define CONFIG_BFIN_GET_CCLK (CONFIG_PLL_CLK/CONFIG_CCLK_DIV)
#define CONFIG_CCLK_HZ CONFIG_BFIN_GET_CCLK

#define CONFIG_BFIN_GET_SCLK (CONFIG_PLL_CLK/CONFIG_SYSCLK_DIV)
#define CONFIG_BFIN_GET_SCLK0 (get_sclk()/CONFIG_SCLK0_DIV)
/* #define CONFIG_BFIN_GET_SCLK1 (get_cclk() / 4) */

/*
 * Memory Settings
 */
#define CONFIG_MEM_SIZE		128

#define CONFIG_SMC_GCTL_VAL	0x00000010
#define CONFIG_SMC_B1CTL_VAL	0x01002001
#define CONFIG_SMC_B1TIM_VAL	0x08070977
#define CONFIG_SMC_B1ETIM_VAL	0x00092231

#define CONFIG_SYS_MONITOR_LEN	(256 * 1024)
#define CONFIG_SYS_MALLOC_LEN	(256 * 1024)

#if 1
#define CONFIG_ICACHE_OFF
#define CONFIG_DCACHE_OFF
#endif

/*
 * Network Settings
 */
#define ADI_CMDS_NETWORK	1
#define CONFIG_NETCONSOLE	1
#define CONFIG_NET_MULTI	1
#define CONFIG_HOSTNAME		bf609-ezkit
#define CONFIG_DESIGNWARE_ETH
#define CONFIG_DW_PORTS		1
#define CONFIG_DW_AUTONEG
#define CONFIG_DW_ALTDESCRIPTOR
#define CONFIG_CMD_NET
#define CONFIG_CMD_MII
#define CONFIG_MII
#define CONFIG_ETHADDR	02:80:ad:20:31:e8

/* i2c Settings */
#define CONFIG_BFIN_TWI_I2C	1
#define CONFIG_HARD_I2C		1

/*
 * Flash Settings
 */
#undef CONFIG_CMD_IMLS
#undef CONFIG_CMD_JFFS2
#define CONFIG_SYS_FLASH_CFI_WIDTH     2
#define CONFIG_FLASH_CFI_DRIVER
#define CONFIG_SYS_FLASH_BASE          0xb0000000
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_SYS_FLASH_PROTECTION
#define CONFIG_SYS_MAX_FLASH_BANKS     1
#define CONFIG_SYS_MAX_FLASH_SECT      259

/*
 * Env Storage Settings
 */
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_SIZE 0x600
#undef CONFIG_CMD_EXPORTENV
#undef CONFIG_CMD_IMPORTENV


/*
 * Misc Settings
 */
#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_UART_CONSOLE	0
#define CONFIG_BAUDRATE 57600

#define CONFIG_DEBUG_DUMP	1
#define CONFIG_KALLSYMS		1

#define CONFIG_CMD_MEMORY

#if 0
#define CONFIG_UART_MEM 1024
#undef CONFIG_UART_CONSOLE
#undef CONFIG_JTAG_CONSOLE
#undef CONFIG_UART_CONSOLE_IS_JTAG
#endif

/*
 * Pull in common ADI header for remaining command/environment setup
 */
#include <configs/bfin_adi_common.h>
#endif
