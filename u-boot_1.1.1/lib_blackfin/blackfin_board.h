#ifndef __BOARD_H__
#define __BOARD_H_

extern void asyncbank_init(void);
extern void timer_init(void);
extern void init_IRQ(void);
extern void rtc_init(void);
extern int get_clock(void);
extern void get_sclk(void);

extern ulong uboot_end_data;
extern ulong uboot_end;

ulong monitor_flash_len;

const char version_string[] =
	U_BOOT_VERSION "\n" U_BOOT_BF533_RELEASE " - " __DATE__  "-" __TIME__
	"\nBlackfin support by LG Soft India";
const char moreinfo_string[] =
	"For further information please check this link http://www.blackfin.uclinux.org";

int *g_addr;
static ulong mem_malloc_start;
static ulong mem_malloc_end;
static ulong mem_malloc_brk;
extern unsigned long sclk;
extern char _sram_in_sdram_start[];
extern char _sram_inst_size[];
#ifdef DEBUG
static void display_global_data(void);
#endif

#endif
