#ifndef __CONFIG_DSPSTAMP_H__
#define __CONFIG_DSPSTAMP_H__

#define	CONFIG_DSPSTAMP
#define CFG_LONGHELP		1
#define CONFIG_BAUDRATE		57600
#define CONFIG_BOOTDELAY	5

#if 1
#define CONFIG_DRIVER_SMC91111	1
#define CONFIG_SMC91111_BASE	0x20300300
#if 0
#define	CONFIG_MII
#define CFG_DISCOVER_PHY
#endif
#endif

#define CONFIG_RTC_BF533	1
#define CONFIG_BOOT_RETRY_TIME	-1	/* Enable this if bootretry required, currently its disabled */

/* CONFIG_CLKIN_HZ is any value in Hz                            */
#define CONFIG_CLKIN_HZ			27000000
/* CONFIG_CLKIN_HALF controls what is passed to PLL 0=CLKIN      */
/*                                                  1=CLKIN/2    */
#define CONFIG_CLKIN_HALF		0
/* CONFIG_PLL_BYPASS controls if the PLL is used 0=don't bypass  */
/*                                               1=bypass PLL    */
#define CONFIG_PLL_BYPASS       	0
/* CONFIG_VCO_MULT controls what the multiplier of the PLL is.   */
/* Values can range from 1-64                                    */
#define CONFIG_VCO_MULT			22
/* CONFIG_CCLK_DIV controls what the core clock divider is       */
/* Values can be 1, 2, 4, or 8 ONLY                              */
#define CONFIG_CCLK_DIV			1
/* CONFIG_SCLK_DIV controls what the peripheral clock divider is */
/* Values can range from 1-15                                    */
#define CONFIG_SCLK_DIV			7


#if ( CONFIG_CLKIN_HALF == 0 )
#define CONFIG_VCO_HZ			( CONFIG_CLKIN_HZ * CONFIG_VCO_MULT )
#else
#define CONFIG_VCO_HZ			(( CONFIG_CLKIN_HZ * CONFIG_VCO_MULT ) / 2 )
#endif

#if (CONFIG_PLL_BYPASS == 0)
#define CONFIG_CCLK_HZ			( CONFIG_VCO_HZ / CONFIG_CCLK_DIV )
#define CONFIG_SCLK_HZ			( CONFIG_VCO_HZ / CONFIG_SCLK_DIV )
#else
#define CONFIG_CCLK_HZ			CONFIG_CLKIN_HZ
#define CONFIG_SCLK_HZ			CONFIG_CLKIN_HZ
#endif

#define CONFIG_MEM_SIZE			32             /* 128, 64, 32, 16 */
#define CONFIG_MEM_ADD_WDTH     	9             /* 8, 9, 10, 11    */
#define CONFIG_MEM_MT48LC16M16A2TG_75	1

#define CONFIG_LOADS_ECHO		1


#define CONFIG_COMMANDS			(CFG_CMD_LOADB	| \
					 CFG_CMD_LOADS	| \
					 CFG_CMD_ELF	| \
					 CFG_CMD_DATE)

/* This must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

/*
 * Miscellaneous configurable options
 */
#define	CFG_PROMPT		"stamp>"	/* Monitor Command Prompt */
#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define	CFG_CBSIZE		1024	/* Console I/O Buffer Size */
#else
#define	CFG_CBSIZE		256	/* Console I/O Buffer Size */
#endif
#define	CFG_PBSIZE		(CFG_CBSIZE+sizeof(CFG_PROMPT)+16)	/* Print Buffer Size */
#define	CFG_MAXARGS		16	/* max number of command args */
#define CFG_BARGSIZE		CFG_CBSIZE	/* Boot Argument Buffer Size */

#define CFG_MEMTEST_START	0x00100000	/* memtest works on */
#define CFG_MEMTEST_END		0x01F7FFFF	/* 4 ... 12 MB in DRAM */
#define	CFG_LOAD_ADDR		0x00100000	/* default load address */
#define	CFG_HZ			1000		/* 1ms time tick */ 
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
#define	CFG_SDRAM_BASE		0x00000000
#define CFG_MAX_RAM_SIZE	0x02000000

#define	CFG_MONITOR_LEN		(256 << 10)	/* Reserve 256 kB for Monitor	*/
#define CFG_MONITOR_BASE	(CFG_MAX_RAM_SIZE - CFG_MONITOR_LEN)
#define	CFG_MALLOC_LEN		(128 << 10)	/* Reserve 128 kB for malloc()	*/
#define CFG_MALLOC_BASE		(CFG_MONITOR_BASE - CFG_MALLOC_LEN)
#define CFG_GBL_DATA_SIZE	0x4000
#define CFG_GBL_DATA_ADDR	(CFG_MALLOC_BASE - CFG_GBL_DATA_SIZE)
#define CONFIG_STACKBASE	(CFG_GBL_DATA_ADDR  - 4)

/*
 * Stack sizes
 */
#define CONFIG_STACKSIZE        (128*1024)      /* regular stack */

// #define CFG_ENV_IS_IN_FLASH	1
// #define	CFG_MAX_FLASH_SECT	0
#define	CFG_NO_FLASH
#define	CFG_ENV_SIZE		1024
#define	CFG_ENV_IS_NOWHERE
#define CONFIG_VDSP		1

/* FLASH/ETHERNET uses the same address range */
// #define SHARED_RESOURCES 	1

#define __ADSPLPBLACKFIN__	1
#define __ADSPBF533__		1

/* 0xFF, 0xBBC3BBc3, 0x99B39983 */
/*#define AMGCTLVAL             (AMBEN_P0 | AMBEN_P1 | AMBEN_P2 | AMCKEN)
#define AMBCTL0VAL              (B1WAT_11 | B1RAT_11 | B1HT_3 | B1ST_4 | B1TT_4 | B1RDYPOL |    \
                                B1RDYEN | B0WAT_11 | B0RAT_11 | B0HT_3 | B0ST_4 | B0TT_4 | B0RDYPOL | B0RDYEN)
#define AMBCTL1VAL              (B3WAT_9 | B3RAT_9 | B3HT_2 | B3ST_3 | B3TT_4 | B3RDYPOL |      \
                                B3RDYEN | B2WAT_9 | B2RAT_9 | B2HT_2 | B2ST_4 | B2TT_4 | B2RDYPOL | B2RDYEN)
*/
#define AMGCTLVAL               0xFF
#define AMBCTL0VAL              0xBBC3BBC3
#define AMBCTL1VAL              0x99B39983
#define CF_AMBCTL1VAL           0x99B3ffc2

#ifdef CONFIG_VDSP
#define ET_EXEC_VDSP		0x8
#define SHT_STRTAB_VDSP		0x1
#define ELFSHDRSIZE_VDSP	0x2C
#define VDSP_ENTRY_ADDR		0xFFA00000
#endif


#endif
