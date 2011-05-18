/*
 * serial.h - common serial defines for early debug and serial driver.
 *            any functions defined here must be always_inline since
 *            initcode cannot have function calls.
 *
 * Copyright (c) 2004-2011 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef __BFIN_CPU_SERIAL1_H__
#define __BFIN_CPU_SERIAL1_H__

#include <asm/mach-common/bits/uart.h>

#ifndef __ASSEMBLY__

#define MMR_UART(n) _PASTE_UART(n, UART, DLL)
#ifdef UART_DLL
# define UART0_DLL UART_DLL
# if CONFIG_UART_CONSOLE != 0
#  error CONFIG_UART_CONSOLE must be 0 on parts with only one UART
# endif
#endif
#define UART_BASE MMR_UART(CONFIG_UART_CONSOLE)

#define LOB(x) ((x) & 0xFF)
#define HIB(x) (((x) >> 8) & 0xFF)

/*
 * All Blackfin system MMRs are padded to 32bits even if the register
 * itself is only 16bits.  So use a helper macro to streamline this.
 */
#define __BFP(m) u16 m; u16 __pad_##m
struct bfin_mmr_serial {
#if BFIN_UART_HW_VER == 2
	__BFP(dll);
	__BFP(dlh);
	__BFP(gctl);
	__BFP(lcr);
	__BFP(mcr);
	__BFP(lsr);
	__BFP(msr);
	__BFP(scr);
	__BFP(ier_set);
	__BFP(ier_clear);
	__BFP(thr);
	__BFP(rbr);
#else
	union {
		u16 dll;
		u16 thr;
		const u16 rbr;
	};
	const u16 __spad0;
	union {
		u16 dlh;
		u16 ier;
	};
	const u16 __spad1;
	const __BFP(iir);
	__BFP(lcr);
	__BFP(mcr);
	__BFP(lsr);
	__BFP(msr);
	__BFP(scr);
	const u32 __spad2;
	__BFP(gctl);
#endif
};
#undef __BFP

typedef uint16_t uart_lsr_t;
#define _lsr_read(p)     bfin_read(&p->lsr)
#define _lsr_write(p, v) bfin_write(&p->lsr, v)

#if BFIN_UART_HW_VER == 2
# define ACCESS_LATCH()
# define ACCESS_PORT_IER()
#else
# define ACCESS_LATCH()    bfin_write_or(&pUART->lcr, DLAB)
# define ACCESS_PORT_IER() bfin_write_and(&pUART->lcr, ~DLAB)
#endif

__attribute__((always_inline))
static inline void serial_early_do_portmux(void)
{
#if defined(__ADSPBF50x__)
# define DO_MUX(port, mux_tx, mux_rx, tx, rx) \
	bfin_write_PORT##port##_MUX((bfin_read_PORT##port##_MUX() & ~(PORT_x_MUX_##mux_tx##_MASK | PORT_x_MUX_##mux_rx##_MASK)) | PORT_x_MUX_##mux_tx##_FUNC_1 | PORT_x_MUX_##mux_rx##_FUNC_1); \
	bfin_write_PORT##port##_FER(bfin_read_PORT##port##_FER() | P##port##tx | P##port##rx);
	switch (CONFIG_UART_CONSOLE) {
	case 0: DO_MUX(G, 7, 7, 12, 13); break;	/* Port G; mux 7; PG12 and PG13 */
	case 1: DO_MUX(F, 3, 3, 6, 7);   break;	/* Port F; mux 3; PF6 and PF7 */
	}
	SSYNC();
#elif defined(__ADSPBF51x__)
# define DO_MUX(port, mux_tx, mux_rx, tx, rx) \
	bfin_write_PORT##port##_MUX((bfin_read_PORT##port##_MUX() & ~(PORT_x_MUX_##mux_tx##_MASK | PORT_x_MUX_##mux_rx##_MASK)) | PORT_x_MUX_##mux_tx##_FUNC_2 | PORT_x_MUX_##mux_rx##_FUNC_2); \
	bfin_write_PORT##port##_FER(bfin_read_PORT##port##_FER() | P##port##tx | P##port##rx);
	switch (CONFIG_UART_CONSOLE) {
	case 0: DO_MUX(G, 5, 5, 9, 10);  break;	/* Port G; mux 5; PG9 and PG10 */
	case 1: DO_MUX(F, 2, 3, 14, 15); break;	/* Port H; mux 2/3; PH14 and PH15 */
	}
	SSYNC();
#elif defined(__ADSPBF52x__)
# define DO_MUX(port, mux, tx, rx) \
	bfin_write_PORT##port##_MUX((bfin_read_PORT##port##_MUX() & ~PORT_x_MUX_##mux##_MASK) | PORT_x_MUX_##mux##_FUNC_3); \
	bfin_write_PORT##port##_FER(bfin_read_PORT##port##_FER() | P##port##tx | P##port##rx);
	switch (CONFIG_UART_CONSOLE) {
	case 0: DO_MUX(G, 2, 7, 8);   break;	/* Port G; mux 2; PG2 and PG8 */
	case 1: DO_MUX(F, 5, 14, 15); break;	/* Port F; mux 5; PF14 and PF15 */
	}
	SSYNC();
#elif defined(__ADSPBF537__) || defined(__ADSPBF536__) || defined(__ADSPBF534__)
	const uint16_t func[] = { PFDE, PFTE, };
	bfin_write_PORT_MUX(bfin_read_PORT_MUX() & ~func[CONFIG_UART_CONSOLE]);
	bfin_write_PORTF_FER(bfin_read_PORTF_FER() |
	                     (1 << P_IDENT(P_UART(RX))) |
	                     (1 << P_IDENT(P_UART(TX))));
	SSYNC();
#elif defined(__ADSPBF54x__)
# define DO_MUX(port, tx, rx) \
	bfin_write_PORT##port##_MUX((bfin_read_PORT##port##_MUX() & ~(PORT_x_MUX_##tx##_MASK | PORT_x_MUX_##rx##_MASK)) | PORT_x_MUX_##tx##_FUNC_1 | PORT_x_MUX_##rx##_FUNC_1); \
	bfin_write_PORT##port##_FER(bfin_read_PORT##port##_FER() | P##port##tx | P##port##rx);
	switch (CONFIG_UART_CONSOLE) {
	case 0: DO_MUX(E, 7, 8); break;	/* Port E; PE7 and PE8 */
	case 1: DO_MUX(H, 0, 1); break;	/* Port H; PH0 and PH1 */
	case 2: DO_MUX(B, 4, 5); break;	/* Port B; PB4 and PB5 */
	case 3: DO_MUX(B, 6, 7); break;	/* Port B; PB6 and PB7 */
	}
	SSYNC();
#elif defined(__ADSPBF561__)
	/* UART pins could be GPIO, but they aren't pin muxed.  */
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
	return (CONFIG_CLKIN_HZ * CONFIG_VCO_MULT / CONFIG_SCLK_DIV);
#else
	return get_sclk();
#endif
}

__attribute__((always_inline))
static inline int uart_init(uint32_t uart_base)
{
	/* always enable UART -- avoids anomalies 05000309 and 05000350 */
	bfin_write(&pUART->gctl, UCEN);

	/* Set LCR to Word Lengh 8-bit word select */
	bfin_write(&pUART->lcr, WLS_8);

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
	/* disable the UART by clearing UCEN */
	bfin_write(&pUART->gctl, 0);

	return 0;
}

__attribute__((always_inline))
static inline int serial_early_enabled(uint32_t uart_base)
{
	return bfin_read(&pUART->gctl) & UCEN;
}

__attribute__((always_inline))
static inline void serial_early_set_baud(uint32_t uart_base, uint32_t baud)
{
	/* Translate from baud into divisor in terms of SCLK.  The
	 * weird multiplication is to make sure we over sample just
	 * a little rather than under sample the incoming signals.
	 */
	uint16_t divisor = (uart_sclk() + (baud * 8)) / (baud * 16) - ANOMALY_05000230;

	/* Set DLAB in LCR to Access DLL and DLH */
	ACCESS_LATCH();
	SSYNC();

	/* Program the divisor to get the baud rate we want */
	bfin_write(&pUART->dll, LOB(divisor));
	bfin_write(&pUART->dlh, HIB(divisor));
	SSYNC();

	/* Clear DLAB in LCR to Access THR RBR IER */
	ACCESS_PORT_IER();
	SSYNC();
}

__attribute__((always_inline))
static inline uint16_t serial_early_get_div(void)
{
	uint32_t uart_base = UART_BASE;

	/* Set DLAB in LCR to Access DLL and DLH */
	ACCESS_LATCH();
	SSYNC();

	uint8_t dll = bfin_read(&pUART->dll);
	uint8_t dlh = bfin_read(&pUART->dlh);
	uint16_t divisor = (dlh << 8) | dll;

	/* Clear DLAB in LCR to Access THR RBR IER */
	ACCESS_PORT_IER();
	SSYNC();

	return divisor;
}

#endif

#endif
