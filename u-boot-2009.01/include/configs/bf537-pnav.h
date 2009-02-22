/*
 * U-boot - Configuration file for BF537 PNAV board
 */

#ifndef __CONFIG_BF537_PNAV_H__
#define __CONFIG_BF537_PNAV_H__

#include <asm/blackfin-config-pre.h>

#define CONFIG_BFIN_CPU             bf537-0.2
#define CONFIG_BFIN_BOOT_MODE       BFIN_BOOT_SPI_MASTER

#define CONFIG_SYS_LONGHELP		1
#define CONFIG_CMDLINE_EDITING	1
#define CONFIG_ENV_OVERWRITE	1
#define CONFIG_BAUDRATE		115200

#define CONFIG_MISC_INIT_R

#ifndef __ADSPBF534__
#define CONFIG_BFIN_MAC
#define CONFIG_RMII
#define CONFIG_NET_MULTI	1
#endif

#define CONFIG_DCACHE_OFF
#define CONFIG_ICACHE_OFF

/* Set default serial console for bf537 */
#define CONFIG_UART_CONSOLE	0

#define CONFIG_RTC_BFIN		1
#define CONFIG_BOOT_RETRY_TIME	-1	/* Enable this if bootretry required, currently its disabled */

/* CONFIG_CLKIN_HZ is any value in Hz                            */
#define CONFIG_CLKIN_HZ          24576000
/* CONFIG_CLKIN_HALF controls what is passed to PLL 0=CLKIN      */
/*                                                  1=CLKIN/2    */
#define CONFIG_CLKIN_HALF               0
/* CONFIG_PLL_BYPASS controls if the PLL is used 0=don't bypass  */
/*                                               1=bypass PLL    */
#define CONFIG_PLL_BYPASS               0
/* CONFIG_VCO_MULT controls what the multiplier of the PLL is.   */
/* Values can range from 1-64                                    */
#define CONFIG_VCO_MULT			20
/* CONFIG_CCLK_DIV controls what the core clock divider is       */
/* Values can be 1, 2, 4, or 8 ONLY                              */
#define CONFIG_CCLK_DIV			1
/* CONFIG_SCLK_DIV controls what the peripheral clock divider is */
/* Values can range from 1-15                                    */
#define CONFIG_SCLK_DIV			4
/* CONFIG_SPI_BAUD controls the SPI peripheral clock divider     */
/* Values can range from 2-65535                                 */
/* SCK Frequency = SCLK / (2 * CONFIG_SPI_BAUD)                  */
#define CONFIG_SPI_BAUD                 2

#define CONFIG_LOADS_ECHO	1

#define CONFIG_SYS_AUTOLOAD                    "no"    /*rarpb, bootp or dhcp commands will perform only a */
                                                /* configuration lookup from the BOOTP/DHCP server, */
                                                /* but not try to load any image using TFTP         */

/*
 * Network Settings
 */
/* network support */
#ifdef CONFIG_BFIN_MAC
#define CONFIG_IPADDR           192.168.0.15
#define CONFIG_NETMASK          255.255.255.0
#define CONFIG_GATEWAYIP        192.168.0.1
#define CONFIG_SERVERIP         192.168.0.2
#define CONFIG_HOSTNAME         bf537-pnav
#endif

#define CONFIG_ROOTPATH		/romfs
/* Uncomment next line to use fixed MAC address */
#define CONFIG_ETHADDR          02:80:ad:24:21:18
/* This is the routine that copies the MAC in Flash to the 'ethaddr' setting */

#if (CONFIG_BFIN_BOOT_MODE == BFIN_BOOT_UART)
# define CONFIG_BOOTDELAY	-1
#else
# define CONFIG_BOOTDELAY	5
#endif

#define CONFIG_SYS_LONGHELP			1
#define CONFIG_BOOT_RETRY_TIME		-1	/* Enable this if bootretry required, currently its disabled */
#define CONFIG_BOOTCOMMAND 		"run ramboot"

#define CONFIG_BF537_NAND 		/* Add nand flash support */

#include <config_cmd_default.h>
#define CONFIG_CMD_CACHE
#define CONFIG_CMD_EEPROM
#undef CONFIG_CMD_IMI
#define CONFIG_CMD_JFFS2
#undef CONFIG_CMD_LOADB
#undef CONFIG_CMD_LOADS
#define CONFIG_CMD_NAND
#undef CONFIG_CMD_NFS
#ifndef CONFIG_BFIN_MAC
#undef CONFIG_CMD_NET
#endif

#define CONFIG_BOOTARGS "root=/dev/mtdblock1 rw rootfstype=yaffs"
#define CONFIG_LOADADDR	0x1000000

#if (CONFIG_BFIN_BOOT_MODE == BFIN_BOOT_BYPASS)
# define BOOT_ENV_SETTINGS \
	"update=tftpboot $(loadaddr) u-boot.bin;" \
		"protect off 0x20000000 0x2003FFFF;" \
		"erase 0x20000000 0x2003FFFF;cp.b 0x1000000 0x20000000 $(filesize)\0"
#elif (CONFIG_BFIN_BOOT_MODE == BFIN_BOOT_SPI_MASTER)
# define BOOT_ENV_SETTINGS \
	"update=tftpboot $(loadaddr) u-boot.ldr;" \
		"eeprom write $(loadaddr) 0x0 $(filesize);\0"
#else
# define BOOT_ENV_SETTINGS
#endif
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
	"flashboot=bootm 0x20100000\0" \
	"nandboot=nand read $(loadaddr) 0x20000 0x100000;bootm $(loadaddr)" \
	BOOT_ENV_SETTINGS

#define CONFIG_SYS_PROMPT "bfin> "

#ifdef CONFIG_CMD_KGDB
#define CONFIG_SYS_CBSIZE		1024	/* Console I/O Buffer Size */
#else
#define CONFIG_SYS_CBSIZE		256	/* Console I/O Buffer Size */
#endif
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)	/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS		16	/* max number of command args */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size */
#define CONFIG_SYS_MEMTEST_START	0x00100000	/* memtest works on */
#define CONFIG_SYS_MEMTEST_END		0x03F00000	/* 1 ... 63 MB in DRAM */
#define CONFIG_SYS_LOAD_ADDR		0x01000000	/* default load address */
#define CONFIG_SYS_HZ			1000	/* decrementer freq: 10 ms ticks */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
#define CONFIG_SYS_SDRAM_BASE		0x00000000
#define CONFIG_MEM_SIZE                 64             /* 128, 64, 32, 16 */
#define CONFIG_SYS_MAX_RAM_SIZE	0x04000000

#define CONFIG_SYS_FLASH_BASE		0x20000000
#define CONFIG_SYS_FLASH_CFI		/* The flash is CFI compatible */
#define CONFIG_FLASH_CFI_DRIVER	/* Use common CFI driver */

#define CONFIG_SYS_MONITOR_LEN		(256 << 10)	/* Reserve 256 kB for Monitor	*/
#define CONFIG_SYS_MONITOR_BASE	(CONFIG_SYS_MAX_RAM_SIZE - CONFIG_SYS_MONITOR_LEN)
#define CONFIG_SYS_MALLOC_LEN		(128 << 10)	/* Reserve 128 kB for malloc()	*/
#define CONFIG_SYS_MALLOC_BASE		(CONFIG_SYS_MONITOR_BASE - CONFIG_SYS_MALLOC_LEN)
#define CONFIG_SYS_GBL_DATA_ADDR	(CONFIG_SYS_MALLOC_BASE - CONFIG_SYS_GBL_DATA_SIZE)
#define CONFIG_STACKBASE	(CONFIG_SYS_GBL_DATA_ADDR  - 4)

#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CONFIG_SYS_MAX_FLASH_SECT	71	/* max number of sectors on one chip */

#if (CONFIG_BFIN_BOOT_MODE == BFIN_BOOT_BYPASS) || (CONFIG_BFIN_BOOT_MODE == BFIN_BOOT_UART)
#define ENV_IS_EMBEDDED
#define CONFIG_ENV_IS_IN_FLASH	1
#define CONFIG_ENV_ADDR		0x20004000
#define CONFIG_ENV_OFFSET	0x4000
#elif (CONFIG_BFIN_BOOT_MODE == BFIN_BOOT_SPI_MASTER)
#define ENV_IS_EMBEDDED_CUSTOM
#define CONFIG_ENV_IS_IN_EEPROM	1
#define CONFIG_ENV_OFFSET	0x4000
#endif
#define CONFIG_ENV_SIZE		0x1000
#define CONFIG_ENV_SECT_SIZE	0x2000	/* Total Size of Environment Sector */

/* JFFS Partition offset set  */
#define CONFIG_SYS_JFFS2_FIRST_BANK 0
#define CONFIG_SYS_JFFS2_NUM_BANKS  1
/* 512k reserved for u-boot */
#define CONFIG_SYS_JFFS2_FIRST_SECTOR                 15


#define FLASH_TOT_SECT		71


/*
 * Serial Flash Infomation
 */
	/* 	  NAME	    Sector Size	   Sectors
	 *	 m25p05     32 * 1024        2
	 *	 m25p10     32 * 1024        4
	 *	 m25p20     64 * 1024        4
	 *	 m25p40     64 * 1024        8
	 *	 m25p80     64 * 1024        16
	 *	 m25p16     64 * 1024        32
	 *	 m25p32     64 * 1024        64
	 *	 m25p64     64 * 1024        128
	 *	 m25p128   256 * 1024        64
	 */

#define CONFIG_SPI
#define CONFIG_SYS_I2C_FRAM

/*
 * Board NAND Infomation
 */

#define CONFIG_SYS_NAND_ADDR          0x20100000
#define CONFIG_SYS_NAND_BASE          CONFIG_SYS_NAND_ADDR
#define CONFIG_SYS_MAX_NAND_DEVICE    1
#define SECTORSIZE             512
#define ADDR_COLUMN             1
#define ADDR_PAGE               2
#define ADDR_COLUMN_PAGE        3
#define NAND_ChipID_UNKNOWN    0x00
#define NAND_MAX_FLOORS        1
#define NAND_MAX_CHIPS         1
#define BFIN_NAND_READY		PF12

#define CONFIG_NAND_GPIO_PORT H

#define NAND_WAIT_READY(nand)  			\
	do { 					\
		int timeout = 0; 		\
		while(!(*pPORTHIO & BFIN_NAND_READY)) 	\
			if (timeout++ > 100000)	\
				break;		\
	} while (0)

#define BFIN_NAND_CLE          (1<<2)                  /* A2 -> Command Enable */
#define BFIN_NAND_ALE          (1<<1)                  /* A1 -> Address Enable */

#define WRITE_NAND_COMMAND(d, adr) do{ *(volatile __u8 *)((unsigned long)adr | BFIN_NAND_CLE) = (__u8)(d); } while(0)
#define WRITE_NAND_ADDRESS(d, adr) do{ *(volatile __u8 *)((unsigned long)adr | BFIN_NAND_ALE) = (__u8)(d); } while(0)
#define WRITE_NAND(d, adr) do{ *(volatile __u8 *)((unsigned long)adr) = (__u8)d; } while(0)
#define READ_NAND(adr) ((volatile unsigned char)(*(volatile __u8 *)(unsigned long)adr))

/*
 * Initialize PSD4256 registers for using I2C
 */

#define CONFIG_SYS_BOOTM_LEN			0x4000000       /* Large Image Length, set to 64 Meg */

/*
 * I2C settings
 */
#define CONFIG_BFIN_TWI_I2C	1
#define CONFIG_HARD_I2C		1
#define CONFIG_SYS_I2C_SPEED		50000
#define CONFIG_SYS_I2C_SLAVE		0

#define CONFIG_EBIU_SDRRC_VAL  0x3b7
#define CONFIG_EBIU_SDGCTL_VAL 0x9111cd
#define CONFIG_EBIU_SDBCTL_VAL 0x25

#define CONFIG_EBIU_AMGCTL_VAL               0xFF
#define CONFIG_EBIU_AMBCTL0_VAL              0x7BB033B0
#define CONFIG_EBIU_AMBCTL1_VAL              0xFFC27BB0

#include <asm/blackfin-config-post.h>

#endif
