/*
 * U-boot - Configuration file for CM-BF533 board
 */

#ifndef __CONFIG_CM_BF533_H__
#define __CONFIG_CM_BF533_H__

#include <asm/blackfin-config-pre.h>

#define CONFIG_BFIN_CPU             bf533-0.3
#define CONFIG_BFIN_BOOT_MODE       BFIN_BOOT_BYPASS

/*
 * Board settings
 */

#define CFG_LONGHELP		1
#define CONFIG_CMDLINE_EDITING	1
#define CONFIG_AUTO_COMPLETE	1
#define CONFIG_ENV_OVERWRITE	1
#define CONFIG_DEBUG_DUMP	1
#define CONFIG_DEBUG_DUMP_SYMS	1
#define CONFIG_BAUDRATE		115200
#define CONFIG_BOOTDELAY	5

#define CONFIG_DRIVER_SMC91111	1
#define CONFIG_SMC91111_BASE	0x20200300

/*
 * Clock settings
 */

/* CONFIG_CLKIN_HZ is any value in Hz                            */
#define CONFIG_CLKIN_HZ          25000000
/* CONFIG_CLKIN_HALF controls what is passed to PLL 0=CLKIN      */
/*                                                  1=CLKIN/2    */
#define CONFIG_CLKIN_HALF               0
/* CONFIG_PLL_BYPASS controls if the PLL is used 0=don't bypass  */
/*                                               1=bypass PLL    */
#define CONFIG_PLL_BYPASS               0
/* CONFIG_VCO_MULT controls what the multiplier of the PLL is.   */
/* Values can range from 1-64                                    */
#define CONFIG_VCO_MULT			22
/* CONFIG_CCLK_DIV controls what the core clock divider is       */
/* Values can be 1, 2, 4, or 8 ONLY                              */
#define CONFIG_CCLK_DIV			1
/* CONFIG_SCLK_DIV controls what the peripheral clock divider is */
/* Values can range from 1-15                                    */
#define CONFIG_SCLK_DIV			5

#ifdef CONFIG_DRIVER_SMC91111
#define CONFIG_IPADDR		192.168.0.32
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_SERVERIP		192.168.0.33
#define CONFIG_HOSTNAME		CM_BF533
#endif

#define CONFIG_LOADS_ECHO	1

#ifdef CONFIG_DRIVER_SMC91111
# define CONFIG_BFIN_CMD		(CONFIG_CMD_DFL | CFG_CMD_PING | CFG_CMD_DHCP)
#else
# define CONFIG_BFIN_CMD		(CONFIG_CMD_DFL & ~CFG_CMD_NET)
#endif
#define CONFIG_COMMANDS   \
	(CONFIG_BFIN_CMD    | \
	 CFG_CMD_CACHE      | \
	 CFG_CMD_ELF)

#if (CONFIG_COMMANDS & CFG_CMD_NET)
# if (CONFIG_BFIN_BOOT_MODE == BFIN_BOOT_BYPASS)
#  define UBOOT_ENV_FILE "u-boot.bin"
# else
#  define UBOOT_ENV_FILE "u-boot.ldr"
# endif
# if (CONFIG_BFIN_BOOT_MODE == BFIN_BOOT_SPI_MASTER)
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

#define CONFIG_BOOTARGS      "root=/dev/mtdblock0 rw earlyprintk=serial,uart0," MK_STR(CONFIG_BAUDRATE)
#define CONFIG_BOOTCOMMAND 		"run ramboot"

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>


#define CONFIG_MEM_SIZE                 32             /* 128, 64, 32, 16 */

#define	CFG_PROMPT		"CM-BF533> "	/* Monitor Command Prompt */
#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define	CFG_CBSIZE		1024	/* Console I/O Buffer Size */
#else
#define	CFG_CBSIZE		256	/* Console I/O Buffer Size */
#endif
#define	CFG_PBSIZE		(CFG_CBSIZE+sizeof(CFG_PROMPT)+16)	/* Print Buffer Size */
#define	CFG_MAXARGS		16	/* max number of command args */
#define CFG_BARGSIZE		CFG_CBSIZE	/* Boot Argument Buffer Size */
#define CFG_MEMTEST_START	0x00100000	/* memtest works on */
#define CFG_MEMTEST_END		0x01F00000	/* 1 ... 31 MB in DRAM */
#define	CFG_LOAD_ADDR		0x01000000	/* default load address */
#define	CFG_HZ			1000	/* decrementer freq: 10 ms ticks */
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
#define	CFG_SDRAM_BASE		0x00000000
#define CFG_MAX_RAM_SIZE	0x02000000
#define CFG_FLASH_BASE		0x20000000

#define	CFG_MONITOR_LEN		(256 << 10)	/* Reserve 256 kB for Monitor	*/
#define CFG_MONITOR_BASE	(CFG_MAX_RAM_SIZE - CFG_MONITOR_LEN)
#define	CFG_MALLOC_LEN		(128 << 10)	/* Reserve 128 kB for malloc()	*/
#define CFG_MALLOC_BASE		(CFG_MONITOR_BASE - CFG_MALLOC_LEN)
#define CFG_GBL_DATA_SIZE	0x4000
#define CFG_GBL_DATA_ADDR	(CFG_MALLOC_BASE - CFG_GBL_DATA_SIZE)
#define CONFIG_STACKBASE	(CFG_GBL_DATA_ADDR  - 4)

#define	CFG_BOOTMAPSZ		(8 << 20)	/* Initial Memory map for Linux */
#define CFG_FLASH_BASE		0x20000000
#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CFG_MAX_FLASH_SECT	16	/* max number of sectors on one chip */

#define	CFG_ENV_IS_IN_FLASH	1
#define CFG_FLASH0_BASE		0x20000000
#define CFG_ENV_OFFSET		0x20000
#define	CFG_ENV_SECT_SIZE	0x20000	/* Total Size of Environment Sector */
#define CFG_ENV_SIZE		0x10000

#define CFG_LARGE_IMAGE_LEN     0x2000000       /* Large Image Length, set to 32 Meg */

/*
 * Stack sizes
 */
#define CONFIG_STACKSIZE        (128*1024)      /* regular stack */

#define POLL_MODE		1
#define FLASH_TOT_SECT		16
#define FLASH_SIZE		0x200000
#define CFG_FLASH_SIZE		0x200000


#define CONFIG_EBIU_SDRRC_VAL  ((((CONFIG_SCLK_HZ / 1000) * 64) / 8192) - (7 + 2))
#define CONFIG_EBIU_SDGCTL_VAL (SCTLE | PSS | TWR_2 | TRCD_2 | TRP_2 | TRAS_7 | PASR_ALL | CL_3)
#define CONFIG_EBIU_SDBCTL_VAL (EBSZ_32 | EBCAW_9 | EBE)

#define CONFIG_EBIU_AMGCTL_VAL   (AMBEN_ALL)
#define CONFIG_EBIU_AMBCTL0_VAL	 (B1WAT_7 | B1RAT_11 | B1HT_2 | B1ST_3 | B0WAT_7 | B0RAT_11 | B0HT_2 | B0ST_3)
#define CONFIG_EBIU_AMBCTL1_VAL  (B3WAT_7 | B3RAT_11 | B3HT_2 | B3ST_3 | B2WAT_7 | B2RAT_11 | B2HT_2 | B2ST_3)

#include <asm/blackfin-config-post.h>

#endif
