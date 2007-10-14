/*
 * initcode.c - Initialize the processor.  This is usually entails things
 * like external memory, voltage regulators, etc...  Note that this file
 * cannot make any function calls as it may be executed all by itself by
 * the Blackfin's bootrom in LDR format.
 *
 * Copyright (c) 2004-2007 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <config.h>
#include <asm/blackfin.h>
#include <asm/mach-common/bits/ebiu.h>
#include <asm/mach-common/bits/pll.h>
#include <asm/mach-common/bits/uart.h>

#include "bootrom.h"
#include "mem_init.h"
#include "serial.h"

/* XXX: should unify more with serial.c */
#ifdef CONFIG_DEBUG_INITCODE
# define BFIN_DEBUG_INITCODE 1
#else
# define BFIN_DEBUG_INITCODE 0
#endif

__attribute__((always_inline))
static inline void serial_init(void)
{
#ifdef __ADSPBF54x__
# ifdef BFIN_BOOT_UART_USE_RTS
#  define BFIN_UART_USE_RTS 1
# else
#  define BFIN_UART_USE_RTS 0
# endif
	if (BFIN_UART_USE_RTS && BFIN_BOOT_MODE == BFIN_BOOT_UART) {
		size_t i;

		/* force RTS rather than relying on auto RTS */
		bfin_write_UART1_MCR(bfin_read_UART1_MCR() | FCPOL);

		/* Wait for the line to clear up.  We cannot rely on UART
		 * registers as none of them reflect the status of the RSR.
		 * Instead, we'll sleep for ~10 bit times at 9600 baud.
		 * We can precalc things here by assuming boot values for
		 * PLL rather than loading registers and calculating.
		 *	baud    = SCLK / (16 ^ (1 - EDBO) * Divisor)
		 *	EDB0    = 0
		 *	Divisor = (SCLK / baud) / 16
		 *	SCLK    = baud * 16 * Divisor
		 *	SCLK    = (0x14 * CONFIG_CLKIN_HZ) / 5
		 *	CCLK    = (16 * Divisor * 5) * (9600 / 10)
		 * In reality, this will probably be just about 1 second delay,
		 * so assuming 9600 baud is OK (both as a very low and too high
		 * speed as this will buffer things enough).
		 */
#define _NUMBITS (10)                                   /* how many bits to delay */
#define _LOWBAUD (9600)                                 /* low baud rate */
#define _SCLK    ((0x14 * CONFIG_CLKIN_HZ) / 5)         /* SCLK based on PLL */
#define _DIVISOR ((_SCLK / _LOWBAUD) / 16)              /* UART DLL/DLH */
#define _NUMINS  (3)                                    /* how many instructions in loop */
#define _CCLK    (((16 * _DIVISOR * 5) * (_LOWBAUD / _NUMBITS)) / _NUMINS)
		i = _CCLK;
		while (i--)
			asm volatile("" : : : "memory");
	}
#endif

	if (!BFIN_DEBUG_INITCODE)
		return;

	serial_do_portmux();
}

__attribute__((always_inline))
static inline void serial_deinit(void)
{
#ifdef __ADSPBF54x__
	if (BFIN_UART_USE_RTS && BFIN_BOOT_MODE == BFIN_BOOT_UART) {
		/* clear forced RTS rather than relying on auto RTS */
		bfin_write_UART1_MCR(bfin_read_UART1_MCR() & ~FCPOL);
	}
#endif
}

/* We need to reset the baud rate when we have early debug turned on
 * or when we are booting over the UART.
 * XXX: we should fix this to calc the old baud and restore it rather
 *      than hardcoding it via CONFIG_LDR_LOAD_BAUD ...
 */
__attribute__((always_inline))
static inline void serial_reset_baud(void)
{
	if (!BFIN_DEBUG_INITCODE && BFIN_BOOT_MODE != BFIN_BOOT_UART)
		return;

#ifndef CONFIG_LDR_LOAD_BAUD
# define CONFIG_LDR_LOAD_BAUD 115200
#endif
#define BFIN_LDR_SCLK     (CONFIG_CLKIN_HZ * CONFIG_VCO_MULT / CONFIG_SCLK_DIV)
#define BFIN_LDR_BAUD     (BFIN_LDR_SCLK / (16 * CONFIG_LDR_LOAD_BAUD))
#define BFIN_LDR_BAUD_HB  ((BFIN_LDR_BAUD >> 8) & 0xFF)
#define BFIN_LDR_BAUD_LB  (BFIN_LDR_BAUD & 0xFF)

	ACCESS_LATCH();
	*pUART_DLL = BFIN_LDR_BAUD_LB;
	*pUART_DLH = BFIN_LDR_BAUD_HB;
	ACCESS_PORT_IER();
	SSYNC();
}

__attribute__((always_inline))
static inline void serial_putc(char c)
{
	if (!BFIN_DEBUG_INITCODE)
		return;

	if (c == '\n')
		*pUART_THR = '\r';

	*pUART_THR = c;
	while (!(*pUART_LSR & TEMT))
		SSYNC();
}


/* Max SCLK can be 133MHz ... dividing that by 4 gives
 * us a freq of 33MHz for SPI which should generally be
 * slow enough for the slow reads the bootrom uses.
 */
#ifndef CONFIG_SPI_BAUD_INITBLOCK
# define CONFIG_SPI_BAUD_INITBLOCK 4
#endif

/* PLL_DIV defines */
#ifndef CONFIG_PLL_DIV_VAL
# if (CONFIG_CCLK_DIV == 1)
#  define CONFIG_CCLK_ACT_DIV CCLK_DIV1
# elif (CONFIG_CCLK_DIV == 2)
#  define CONFIG_CCLK_ACT_DIV CCLK_DIV2
# elif (CONFIG_CCLK_DIV == 4)
#  define CONFIG_CCLK_ACT_DIV CCLK_DIV4
# elif (CONFIG_CCLK_DIV == 8)
#  define CONFIG_CCLK_ACT_DIV CCLK_DIV8
# else
#  define CONFIG_CCLK_ACT_DIV CONFIG_CCLK_DIV_not_defined_properly
# endif
# define CONFIG_PLL_DIV_VAL (CONFIG_CCLK_ACT_DIV | CONFIG_SCLK_DIV)
#endif

#ifndef CONFIG_PLL_LOCKCNT_VAL
# define CONFIG_PLL_LOCKCNT_VAL 0x0300
#endif

#ifndef CONFIG_PLL_CTL_VAL
# define CONFIG_PLL_CTL_VAL ((CONFIG_VCO_MULT * 2) << 8)
#endif

/* Make sure our voltage value is sane so we don't blow up! */
#ifndef CONFIG_VR_CTL_VAL
# define BFIN_CCLK ((CONFIG_CLKIN_HZ * CONFIG_VCO_MULT) / CONFIG_CCLK_DIV)
# if defined(__ADSPBF533__) || defined(__ADSPBF532__) || defined(__ADSPBF531__)
#  define CCLK_VLEV_120	400000000
#  define CCLK_VLEV_125	533000000
# elif defined(__ADSPBF537__) || defined(__ADSPBF536__) || defined(__ADSPBF534__)
#  define CCLK_VLEV_120	401000000
#  define CCLK_VLEV_125	401000000
# elif defined(__ADSPBF561__)
#  define CCLK_VLEV_120	300000000
#  define CCLK_VLEV_125	501000000
# endif
# if BFIN_CCLK < CCLK_VLEV_120
#  define CONFIG_VR_CTL_VLEV VLEV_120
# elif BFIN_CCLK < CCLK_VLEV_125
#  define CONFIG_VR_CTL_VLEV VLEV_125
# else
#  define CONFIG_VR_CTL_VLEV VLEV_130
# endif
# if defined(__ADSPBF52x__)	/* TBD; use default */
#  undef CONFIG_VR_CTL_VLEV
#  define CONFIG_VR_CTL_VLEV VLEV_110
# elif defined(__ADSPBF54x__)	/* TBD; use default */
#  undef CONFIG_VR_CTL_VLEV
#  define CONFIG_VR_CTL_VLEV VLEV_120
# endif

# ifdef CONFIG_BFIN_MAC
#  define CONFIG_VR_CTL_CLKBUF CLKBUFOE
# else
#  define CONFIG_VR_CTL_CLKBUF 0
# endif

# if defined(__ADSPBF52x__)
#  define CONFIG_VR_CTL_FREQ FREQ_1000
# else
#  define CONFIG_VR_CTL_FREQ (GAIN_20 | FREQ_1000)
# endif

# define CONFIG_VR_CTL_VAL (CONFIG_VR_CTL_CLKBUF | CONFIG_VR_CTL_VLEV | CONFIG_VR_CTL_FREQ)
#endif

__attribute__((saveall))
void initcode(boot_struct *bootstruct)
{
	serial_init();

	serial_putc('S');

	/* Blackfin bootroms use the SPI slow read opcode instead of the SPI
	 * fast read, so we need to slow down the SPI clock a lot more during
	 * boot.  Once we switch over to u-boot's SPI flash driver, we'll
	 * increase the speed appropriately.
	 */
	if (BFIN_BOOT_MODE == BFIN_BOOT_SPI_MASTER)
#ifdef SPI0_BAUD
		bfin_write_SPI0_BAUD(CONFIG_SPI_BAUD_INITBLOCK);
#else
		bfin_write_SPI_BAUD(CONFIG_SPI_BAUD_INITBLOCK);
#endif

	serial_putc('B');

	/* Disable all peripheral wakeups except for the PLL event. */
#ifdef SIC_IWR0
	bfin_write_SIC_IWR0(1);
	bfin_write_SIC_IWR1(0);
# ifdef SIC_IWR2
	bfin_write_SIC_IWR2(0);
# endif
#elif defined(SICA_IWR0)
	bfin_write_SICA_IWR0(1);
	bfin_write_SICA_IWR1(0);
#else
	bfin_write_SIC_IWR(1);
#endif

	serial_putc('L');

	bfin_write_PLL_LOCKCNT(CONFIG_PLL_LOCKCNT_VAL);

	serial_putc('A');

	/* Only reprogram when needed to avoid triggering unnecessary
	 * PLL relock sequences.
	 */
	if (bfin_read_VR_CTL() != CONFIG_VR_CTL_VAL) {
		serial_putc('!');
		bfin_write_VR_CTL(CONFIG_VR_CTL_VAL);
		asm("idle;");
	}

	serial_putc('C');

	bfin_write_PLL_DIV(CONFIG_PLL_DIV_VAL);

	serial_putc('K');

	/* Only reprogram when needed to avoid triggering unnecessary
	 * PLL relock sequences.
	 */
	if (bfin_read_PLL_CTL() != CONFIG_PLL_CTL_VAL) {
		serial_putc('!');
		bfin_write_PLL_CTL(CONFIG_PLL_CTL_VAL);
		asm("idle;");
	}

	/* Since we've changed the SCLK above, we may need to update
	 * the UART divisors (UART baud rates are based on SCLK).
	 */
	serial_reset_baud();

	serial_putc('F');

	/* Program the async banks controller. */
	bfin_write_EBIU_AMBCTL0(CONFIG_EBIU_AMBCTL0_VAL);
	bfin_write_EBIU_AMBCTL1(CONFIG_EBIU_AMBCTL1_VAL);
	bfin_write_EBIU_AMGCTL(CONFIG_EBIU_AMGCTL_VAL);

	serial_putc('I');

	/* Program the external memory controller. */
#ifdef EBIU_RSTCTL
# ifndef CONFIG_EBIU_RSTCTL_VAL
#  define CONFIG_EBIU_RSTCTL_VAL 0 /* only MDDRENABLE is useful */
# endif
	bfin_write_EBIU_RSTCTL(bfin_read_EBIU_RSTCTL() | 0x1 /*DDRSRESET*/ | CONFIG_EBIU_RSTCTL_VAL);
	bfin_write_EBIU_DDRCTL0(CONFIG_EBIU_DDRCTL0_VAL);
	bfin_write_EBIU_DDRCTL1(CONFIG_EBIU_DDRCTL1_VAL);
	bfin_write_EBIU_DDRCTL2(CONFIG_EBIU_DDRCTL2_VAL);
# ifdef CONFIG_EBIU_DDRCTL3_VAL
	/* default is disable, so don't need to force this */
	bfin_write_EBIU_DDRCTL3(CONFIG_EBIU_DDRCTL3_VAL);
# endif
#else
	bfin_write_EBIU_SDRRC(CONFIG_EBIU_SDRRC_VAL);
	bfin_write_EBIU_SDBCTL(CONFIG_EBIU_SDBCTL_VAL);
	bfin_write_EBIU_SDGCTL(CONFIG_EBIU_SDGCTL_VAL);
#endif

	serial_putc('N');

	/* Restore all peripheral wakeups. */
#ifdef SIC_IWR0
	bfin_write_SIC_IWR0(-1);
	bfin_write_SIC_IWR1(-1);
# ifdef SIC_IWR2
	bfin_write_SIC_IWR2(-1);
# endif
#elif defined(SICA_IWR0)
	bfin_write_SICA_IWR0(-1);
	bfin_write_SICA_IWR1(-1);
#else
	bfin_write_SIC_IWR(-1);
#endif

	serial_putc('>');

	serial_deinit();
}
