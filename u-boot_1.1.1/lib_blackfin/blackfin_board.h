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
const char moreinfo_string[] =
	"For further information please check this link http://www.blackfin.uclinux.org";
const char version_string[] =
	U_BOOT_VERSION " (gcc version: 3.3.3)\n" U_BOOT_BF533_RELEASE
	" released on " __DATE__ " - " __TIME__
	"\nBlackfin support by LG Soft India";
int *g_addr;
static ulong mem_malloc_start;
static ulong mem_malloc_end;
static ulong mem_malloc_brk;
extern unsigned long sclk;

#endif
