/*
 * U-boot - Configuration file for CSP Minotaur board
 *
 * Thu Oct 25 15:30:44 CEST 2007 <hackfin@section5.ch>
 *    Minotaur config, brushed up for official uClinux dist.
 *    Parallel flash support disabled, SPI flash boot command
 *    added ('run flashboot').
 *
 * Flash image map:
 *
 * 0x00000000      u-boot bootstrap
 * 0x00010000      environment
 * 0x00020000      u-boot code
 * 0x00030000      uImage.initramfs
 *
 */

#ifndef __CONFIG_BF537_SRV1_H__
#define __CONFIG_BF537_SRV1_H__

#include <asm/blackfin-config-pre.h>

#define CONFIG_BFIN_CPU             bf537-0.2
#define CONFIG_BFIN_BOOT_MODE       BFIN_BOOT_SPI_MASTER

#define CFG_LONGHELP		1
#define CONFIG_CMDLINE_EDITING	1
#define CONFIG_ENV_OVERWRITE	1
#define CONFIG_BAUDRATE		57600

/* We don't have a parallel flash chip there */
#define CFG_NO_FLASH

#ifndef __ADSPBF534__
#define CONFIG_BFIN_MAC
#define CONFIG_NETCONSOLE	1
#define CONFIG_NET_MULTI	1
#endif

/* Set default serial console for bf537 */
#define CONFIG_UART_CONSOLE	0
/* define CONFIG_BF537_SRV1_LEDCMD to enable LED command*/
/*#define CONFIG_BF537_SRV1_LEDCMD	1*/

#define CONFIG_PANIC_HANG 1

#define CONFIG_RTC_BFIN		1
#define CONFIG_BOOT_RETRY_TIME	-1	/* Enable this if bootretry required, currently its disabled */

/* CONFIG_CLKIN_HZ is any value in Hz				*/
#define CONFIG_CLKIN_HZ          22118400
/* CONFIG_CLKIN_HALF controls what is passed to PLL 0=CLKIN	*/
/*						    1=CLKIN/2	*/
#define CONFIG_CLKIN_HALF	0
/* CONFIG_PLL_BYPASS controls if the PLL is used 0=don't bypass */
/*						    1=bypass PLL*/
#define CONFIG_PLL_BYPASS	0
/* CONFIG_VCO_MULT controls what the multiplier of the PLL is.	*/
/* Values can range from 1-64					*/
#define CONFIG_VCO_MULT			20
/* CONFIG_CCLK_DIV controls what the core clock divider is	*/
/* Values can be 1, 2, 4, or 8 ONLY				*/
#define CONFIG_CCLK_DIV			1
/* CONFIG_SCLK_DIV controls what the peripheral clock divider is*/
/* Values can range from 1-15					*/
#define CONFIG_SCLK_DIV			5
/* CONFIG_SPI_BAUD controls the SPI peripheral clock divider	*/
/* Values can range from 2-65535				*/
/* SCK Frequency = SCLK / (2 * CONFIG_SPI_BAUD)			*/
#define CONFIG_SPI_BAUD			2
#define CONFIG_SPI_BAUD_INITBLOCK	4

#define CONFIG_LOADS_ECHO		1

/*
 * rarpb, bootp or dhcp commands will perform only a
 * configuration lookup from the BOOTP/DHCP server
 * but not try to load any image using TFTP
 */
#define CFG_AUTOLOAD			"no"

/*
 * Network Settings
 */
/* network support */
#ifdef CONFIG_BFIN_MAC
#define CONFIG_IPADDR		192.168.0.15
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_GATEWAYIP	192.168.0.1
#define CONFIG_SERVERIP		192.168.0.2
#define CONFIG_HOSTNAME		bf537-srv1
#endif

#define CONFIG_ROOTPATH		/romfs
/* Use a fixed MAC address for booting up. Firstboot linux
 * must fetch a valid MAC from the production server. */
#define CONFIG_ETHADDR	02:80:ad:20:31:42
/* This is the routine that copies the MAC in Flash to the 'ethaddr' setting */

#if (CONFIG_BFIN_BOOT_MODE == BFIN_BOOT_UART)
# define CONFIG_BOOTDELAY	-1
#else
# define CONFIG_BOOTDELAY	5
#endif

#define CONFIG_BOOT_RETRY_TIME	-1	/* Enable this if bootretry required, currently its disabled */
#define CONFIG_BOOTCOMMAND 	"run flashboot"

#include <config_cmd_default.h>

#ifdef CONFIG_BFIN_MAC
# define CONFIG_CMD_DHCP
# define CONFIG_CMD_PING
#else
# undef CONFIG_CMD_NET
#endif

#define CONFIG_CMD_BOOTLDR
#define CONFIG_CMD_CACHE
#define CONFIG_CMD_DATE
#define CONFIG_CMD_EEPROM
#define CONFIG_CMD_ELF
#undef CONFIG_CMD_FLASH
#define CONFIG_CMD_I2C
#undef CONFIG_CMD_IMLS

#define CONFIG_BOOTARGS "root=/dev/mtdblock0 rw"
#define CONFIG_LOADADDR	0x1000000

#define BOOT_ENV_SETTINGS \
	"update=tftpboot $(loadaddr) u-boot.ldr;" \
		"eeprom write $(loadaddr) 0x0 $(filesize);\0" \
	"flashboot= eeprom read 0x1000000 0x30000 0x320000;" \
			"bootm 0x1000000\0"
#ifdef CONFIG_BFIN_MAC
# define NETWORK_ENV_SETTINGS \
	"nfsargs=setenv bootargs root=/dev/nfs rw " \
		"nfsroot=$(serverip):$(rootpath)\0" \
	"addip=setenv bootargs $(bootargs) " \
		"ip=$(ipaddr):$(serverip):$(gatewayip):$(netmask)" \
		":$(hostname):eth0:off\0" \
	"ramboot=tftpboot $(loadaddr) linux;" \
		"run ramargs;run addip;bootelf\0" \
	"nfsboot=tftpboot $(loadaddr) linux;" \
		"run nfsargs;run addip;bootelf\0"
#else
# define NETWORK_ENV_SETTINGS
#endif
#define CONFIG_EXTRA_ENV_SETTINGS \
	NETWORK_ENV_SETTINGS \
	"ramargs=setenv bootargs root=/dev/mtdblock0 rw\0" \
	BOOT_ENV_SETTINGS

#define CFG_PROMPT "srv1> "

#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define CFG_CBSIZE		1024	/* Console I/O Buffer Size */
#else
#define CFG_CBSIZE		256	/* Console I/O Buffer Size */
#endif
#define CONFIG_MEM_SIZE			32	/* 128, 64, 32, 16 */
#define CFG_MAX_RAM_SIZE       	(CONFIG_MEM_SIZE * 1024*1024)
#define CFG_PBSIZE		(CFG_CBSIZE+sizeof(CFG_PROMPT)+16)	/* Print Buffer Size */
#define CFG_MAXARGS		16	/* max number of command args */
#define CFG_BARGSIZE		CFG_CBSIZE	/* Boot Argument Buffer Size */
#define CFG_MEMTEST_START	0x0	/* memtest works on */
#define CFG_MEMTEST_END		( (CONFIG_MEM_SIZE - 1) * 1024*1024)	/* 1 ... 63 MB in DRAM */
#define CFG_LOAD_ADDR		CONFIG_LOADADDR	/* default load address */
#define CFG_HZ			1000	/* decrementer freq: 10 ms ticks */
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
#define CFG_SDRAM_BASE		0x00000000

#define CFG_BOOTM_LEN		0x4000000	/* Large Image Length, set to 64 Meg */

#define CFG_FLASH_BASE		0x20000000

#define CFG_MONITOR_LEN		(256 << 10)	/* Reserve 256 kB for Monitor	*/
#define CFG_MONITOR_BASE	(CFG_MAX_RAM_SIZE - CFG_MONITOR_LEN)
#define CFG_MALLOC_LEN		(384 << 10)	/* Reserve 128 kB for malloc()	*/
#define CFG_MALLOC_BASE		(CFG_MONITOR_BASE - CFG_MALLOC_LEN)
#define CFG_GBL_DATA_ADDR	(CFG_MALLOC_BASE - CFG_GBL_DATA_SIZE)
#define CONFIG_STACKBASE	(CFG_GBL_DATA_ADDR  - 4)


#define CONFIG_ENV_IS_IN_EEPROM	1
#define CONFIG_ENV_OFFSET	0x10000
#define CONFIG_ENV_SIZE		0x10000
#define CONFIG_ENV_SECT_SIZE	0x10000	/* Total Size of Environment Sector */
#define ENV_IS_EMBEDDED_CUSTOM

/*
 * Serial Flash Infomation
 */
#define CONFIG_SPI
#define CFG_I2C_FRAM

/*
 * I2C settings
 */
#define CONFIG_BFIN_TWI_I2C	1
#define CONFIG_HARD_I2C		1
#define CFG_I2C_SPEED		50000
#define CFG_I2C_SLAVE		0

#define CONFIG_EBIU_SDRRC_VAL  0x2ac
#define CONFIG_EBIU_SDGCTL_VAL 0x91110d
#define CONFIG_EBIU_SDBCTL_VAL 0x13

#define CONFIG_EBIU_AMGCTL_VAL		0xFF
#define CONFIG_EBIU_AMBCTL0_VAL		0x7BB07BB0
#define CONFIG_EBIU_AMBCTL1_VAL		0xFFC27BB0

#include <asm/blackfin-config-post.h>

#endif
