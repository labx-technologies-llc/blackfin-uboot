#ifndef __BOARD_H__
#define __BOARD_H_

extern void asyncbank_init(void);
extern void timer_init(void);
extern void init_IRQ(void);
extern void rtc_init(void);

extern ulong uboot_end_data;
extern ulong uboot_end;

ulong monitor_flash_len;

#define U_BOOT_BF533_SUPPORT  "Blackfin support by LG Soft India"

#define VERSION_STRING_SIZE  150 /* including 40 bytes buffer to change any string */ 
#define VERSION_STRING_FORMAT "%s (gcc version : %d.%d.%d)\n%s - %s-%s\n%s"
#define VERSION_STRING		U_BOOT_VERSION, \
				__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__, \
				U_BOOT_BF533_RELEASE, \
				__DATE__, __TIME__ , \
				U_BOOT_BF533_SUPPORT

char version_string[VERSION_STRING_SIZE];
const char moreinfo_string[] =
	"For further information please check this link http://www.blackfin.uclinux.org";

int *g_addr;
static ulong mem_malloc_start;
static ulong mem_malloc_end;
static ulong mem_malloc_brk;
extern char _sram_in_sdram_start[];
extern char _sram_inst_size[];
#ifdef DEBUG
static void display_global_data(void);
#endif

/* definitions used to check the SMC card availability */
#define SMC_BASE_ADDRESS CONFIG_SMC91111_BASE
#define UPPER_BYTE_MASK	0xFF00
#define SMC_IDENT	0x3300

#endif
