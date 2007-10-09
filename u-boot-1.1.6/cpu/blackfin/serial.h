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

#ifndef pUART_LSR
# if (CONFIG_UART_CONSOLE == 1)
#  define pUART_DLH  pUART1_DLH
#  define pUART_DLL  pUART1_DLL
#  define pUART_GCTL pUART1_GCTL
#  define pUART_LCR  pUART1_LCR
#  define pUART_LSR  pUART1_LSR
#  define pUART_RBR  pUART1_RBR
#  define pUART_THR  pUART1_THR
# elif (CONFIG_UART_CONSOLE == 0)
#  define pUART_DLH  pUART0_DLH
#  define pUART_DLL  pUART0_DLL
#  define pUART_GCTL pUART0_GCTL
#  define pUART_LCR  pUART0_LCR
#  define pUART_LSR  pUART0_LSR
#  define pUART_RBR  pUART0_RBR
#  define pUART_THR  pUART0_THR
# endif
#endif

#ifdef __ADSPBF54x__
# define ACCESS_LATCH()
# define ACCESS_PORT_IER()
#else
# define ACCESS_LATCH()    (*pUART_LCR |= DLAB)
# define ACCESS_PORT_IER() (*pUART_LCR &= ~DLAB)
#endif

__attribute__((always_inline))
static void serial_do_portmux(void)
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

#endif
