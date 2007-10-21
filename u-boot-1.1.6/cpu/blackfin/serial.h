/*
 * serial.h - common serial defines for early debug and serial driver.
 *            any functions defined here must be always_inline since
 *            initcode cannot have function calls.
 *
 * Copyright (c) 2004-2007 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef __BFIN_CPU_SERIAL_H__
#define __BFIN_CPU_SERIAL_H__

#include <asm/blackfin.h>
#include <asm/mach-common/bits/uart.h>

#ifdef CONFIG_DEBUG_EARLY_SERIAL
# define BFIN_DEBUG_EARLY_SERIAL 1
#else
# define BFIN_DEBUG_EARLY_SERIAL 0
#endif

#define LOB(x) ((x) & 0xFF)
#define HIB(x) (((x) >> 8) & 0xFF)

#ifndef UART_LSR
# if (CONFIG_UART_CONSOLE == 1)
#  define pUART_DLH  pUART1_DLH
#  define pUART_DLL  pUART1_DLL
#  define pUART_GCTL pUART1_GCTL
#  define pUART_LCR  pUART1_LCR
#  define pUART_LSR  pUART1_LSR
#  define pUART_RBR  pUART1_RBR
#  define pUART_THR  pUART1_THR
#  define  UART_THR   UART1_THR
#  define  UART_LSR   UART1_LSR
# elif (CONFIG_UART_CONSOLE == 0)
#  define pUART_DLH  pUART0_DLH
#  define pUART_DLL  pUART0_DLL
#  define pUART_GCTL pUART0_GCTL
#  define pUART_LCR  pUART0_LCR
#  define pUART_LSR  pUART0_LSR
#  define pUART_RBR  pUART0_RBR
#  define pUART_THR  pUART0_THR
#  define  UART_THR   UART0_THR
#  define  UART_LSR   UART0_LSR
# endif
#endif

#ifndef __ASSEMBLY__

/* We cannot use get_sclk() in initcode as it is defined elsewhere. */
#ifdef BFIN_IN_INITCODE
# define get_sclk() (CONFIG_CLKIN_HZ * CONFIG_VCO_MULT / CONFIG_SCLK_DIV)
#endif

#ifdef __ADSPBF54x__
# define ACCESS_LATCH()
# define ACCESS_PORT_IER()
#else
# define ACCESS_LATCH()    (*pUART_LCR |= DLAB)
# define ACCESS_PORT_IER() (*pUART_LCR &= ~DLAB)
#endif

__attribute__((always_inline))
static inline void serial_do_portmux(void)
{
#ifdef __ADSPBF52x__
	if (CONFIG_UART_CONSOLE == 0) {
		bfin_write_PORTG_MUX((bfin_read_PORTG_MUX() & ~PORT_x_MUX_2_MASK) | PORT_x_MUX_2_FUNC_3);
		bfin_write_PORTG_FER(bfin_read_PORTG_FER() | PG7 | PG8);
	} else if (CONFIG_UART_CONSOLE == 1) {
		bfin_write_PORTF_MUX((bfin_read_PORTF_MUX() & ~PORT_x_MUX_5_MASK) | PORT_x_MUX_5_FUNC_3);
		bfin_write_PORTF_FER(bfin_read_PORTF_FER() | PF14 | PF15);
	}
	SSYNC();
#elif defined(__ADSPBF537__) || defined(__ADSPBF536__) || defined(__ADSPBF534__)
	if (CONFIG_UART_CONSOLE == 0) {
		bfin_write_PORT_MUX(bfin_read_PORT_MUX() & ~PFDE);
		bfin_write_PORTF_FER(bfin_read_PORTF_FER() | PF0 | PF1);
	} else if (CONFIG_UART_CONSOLE == 1) {
		bfin_write_PORT_MUX(bfin_read_PORT_MUX() & ~PFTE);
		bfin_write_PORTF_FER(bfin_read_PORTF_FER() | PF2 | PF3);
	}
	SSYNC();
#endif
}

__attribute__((always_inline))
static inline void serial_early_init(void)
{
	/* handle portmux crap on different Blackfins */
	serial_do_portmux();

	/* Enable UART */
	*pUART_GCTL = UCEN;

	/* Set LCR to Word Lengh 8-bit word select */
	*pUART_LCR = WLS_8;

	SSYNC();
}

__attribute__((always_inline))
static inline uint32_t serial_early_get_baud(void)
{
	/* If the UART isnt enabled, then we are booting an LDR
	 * from a non-UART source (so like flash) which means
	 * the baud rate here is meaningless.
	 */
	if ((*pUART_GCTL & UCEN) != UCEN)
		return 0;

#if (0)	/* See comment for serial_reset_baud() in initcode.c */
	/* Set DLAB in LCR to Access DLL and DLH */
	ACCESS_LATCH();
	SSYNC();

	uint8_t dll = *pUART_DLL;
	uint8_t dlh = *pUART_DLH;
	uint16_t divisor = (dlh << 8) | dll;
	uint32_t baud = get_sclk() / (divisor * 16);

	/* Clear DLAB in LCR to Access THR RBR IER */
	ACCESS_PORT_IER();
	SSYNC();

	return baud;
#else
	return CONFIG_BAUDRATE;
#endif
}

__attribute__((always_inline))
static inline void serial_early_set_baud(uint32_t baud)
{
	/* Translate from baud into divisor in terms of SCLK.
	 * The +1 is to make sure we over sample just a little
	 * rather than under sample the incoming signals.
	 */
	uint16_t divisor = (get_sclk() / (baud * 16)) + 1;

	/* Set DLAB in LCR to Access DLL and DLH */
	ACCESS_LATCH();
	SSYNC();

	/* Program the divisor to get the baud rate we want */
	*pUART_DLL = LOB(divisor);
	*pUART_DLH = HIB(divisor);
	SSYNC();

	/* Clear DLAB in LCR to Access THR RBR IER */
	ACCESS_PORT_IER();
	SSYNC();
}

#ifndef BFIN_IN_INITCODE
__attribute__((always_inline))
static inline void serial_early_puts(const char *s)
{
	if (BFIN_DEBUG_EARLY_SERIAL) {
		serial_puts("Early: ");
		serial_puts(s);
	}
}
#endif

#else

.macro serial_early_init
#ifdef CONFIG_DEBUG_EARLY_SERIAL
	call _serial_initialize;
#endif
.endm

.macro serial_early_set_baud
#ifdef CONFIG_DEBUG_EARLY_SERIAL
	R0.L = LO(CONFIG_BAUDRATE);
	R0.H = HI(CONFIG_BAUDRATE);
	call _serial_set_baud;
#endif
.endm

.macro _serial_early_putc byte:req morebytes:vararg
#ifdef CONFIG_DEBUG_EARLY_SERIAL
	R0 = \byte;
	call _serial_putc;
.ifnb \morebytes
	_serial_early_putc \morebytes
.else
.if (\byte != '\n')
	_serial_early_putc '\n'
.endif
.endif
#endif
.endm

.macro serial_early_putc byte:req morebytes:vararg
#ifdef CONFIG_DEBUG_EARLY_SERIAL
	_serial_early_putc 'E', 'a', 'r', 'l', 'y', ':', ' ', \byte, \morebytes
#endif
.endm

#endif

#endif
