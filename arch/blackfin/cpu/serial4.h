/*
 * serial.h - common serial defines for early debug and serial driver.
 *            any functions defined here must be always_inline since
 *            initcode cannot have function calls.
 *
 * Copyright (c) 2004-2011 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef __BFIN_CPU_SERIAL4_H__
#define __BFIN_CPU_SERIAL4_H__

#include <asm/mach-common/bits/uart4.h>

#ifndef __ASSEMBLY__

#define MMR_UART(n) _PASTE_UART(n, UART, REVID)
#define UART_BASE MMR_UART(CONFIG_UART_CONSOLE)

struct bfin_mmr_serial {
	u32 revid;
	u32 control;
	u32 status;
	u32 scr;
	u32 clock;
	u32 emask;
	u32 emaskst;
	u32 emaskcl;
	u32 rbr;
	u32 thr;
	u32 taip;
	u32 tsr;
	u32 rsr;
	u32 txdiv_cnt;
	u32 rxdiv_cnt;
};

typedef uint32_t uart_lsr_t;
#define _lsr_read(p)     bfin_read(&p->status)
#define _lsr_write(p, v) bfin_write(&p->status, v)

__attribute__((always_inline))
static inline void serial_early_do_portmux(void)
{
#if defined(__ADSPBF60x__)
# define DO_MUX(port, tx, rx, func) do \
{\
	bfin_write_PORT##port##_MUX((bfin_read_PORT##port##_MUX() & ~(PORT_x_MUX_##tx##_MASK | PORT_x_MUX_##rx##_MASK)) | PORT_x_MUX_##tx##_FUNC_##func | PORT_x_MUX_##rx##_FUNC_##func); \
	bfin_write_PORT##port##_FER_SET(P##port##tx | P##port##rx);\
} while (0);
	switch (CONFIG_UART_CONSOLE) {
	case 0:
		DO_MUX(D, 7, 8, 2);
		break; /* Port D; PD7 and PD8; function 2 */
	case 1:
		DO_MUX(G, 15, 14, 1);
		break; /* Port G; PG15 and PH14; function 1 */
	}
	SSYNC();
#else
# if (P_UART(RX) & P_DEFINED) || (P_UART(TX) & P_DEFINED)
#  error "missing portmux logic for UART"
# endif
#endif
}

__attribute__((always_inline))
static inline uint32_t uart_sclk(void)
{
#if defined(BFIN_IN_INITCODE) || defined(CONFIG_DEBUG_EARLY_SERIAL)
	/* We cannot use get_sclk() early on as it uses caches in external memory */
	return (CONFIG_CLKIN_HZ * CONFIG_VCO_MULT / CONFIG_SYSCLK_DIV / CONFIG_SCLK0_DIV);
#else
	return get_sclk0();
#endif
}

__attribute__((always_inline))
static inline int uart_init(uint32_t uart_base)
{
	/* always enable UART to 8-bit mode */
	bfin_write(&pUART->control, UEN | UMOD_UART | WLS_8);

	SSYNC();

	return 0;
}

__attribute__((always_inline))
static inline int serial_early_init(uint32_t uart_base)
{
	/* handle portmux crap on different Blackfins */
	serial_do_portmux();

	return uart_init(uart_base);
}

__attribute__((always_inline))
static inline int serial_early_uninit(uint32_t uart_base)
{
	/* disable the UART by clearing UEN */
	bfin_write(&pUART->control, 0);

	return 0;
}

__attribute__((always_inline))
static inline int serial_early_enabled(uint32_t uart_base)
{
	return bfin_read(&pUART->control) & UEN;
}

__attribute__((always_inline))
static inline void serial_early_set_baud(uint32_t uart_base, uint32_t baud)
{
	uint32_t divisor = uart_sclk() / (baud * 16);

	/* Program the divisor to get the baud rate we want */
	bfin_write(&pUART->clock, divisor);
	SSYNC();
}

__attribute__((always_inline))
static inline void serial_early_put_div(uint32_t divisor)
{
	uint32_t uart_base = UART_BASE;
	bfin_write(&pUART->clock, divisor);
}

__attribute__((always_inline))
static inline uint32_t serial_early_get_div(void)
{
	uint32_t uart_base = UART_BASE;
	return bfin_read(&pUART->clock);
}

#endif

#endif
