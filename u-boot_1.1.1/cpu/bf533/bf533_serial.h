/* bf533_serial.h: Definitions for the BlackFin BF533 DSP serial driver.
 * Copyright (C) 2003	Bas Vermeulen <bas@buyways.nl>
 * 			BuyWays B.V. (www.buyways.nl)
 *
 * Based heavily on:
 * blkfinserial.h: Definitions for the BlackFin DSP serial driver.
 *
 * Copyright (C) 2001	Tony Z. Kou	tonyko@arcturusnetworks.com
 * Copyright (C) 2001   Arcturus Networks Inc. <www.arcturusnetworks.com>
 *
 * Based on code from 68328serial.c which was: 
 * Copyright (C) 1995       David S. Miller    <davem@caip.rutgers.edu>
 * Copyright (C) 1998       Kenneth Albanowski <kjahds@kjahds.com>
 * Copyright (C) 1998, 1999 D. Jeff Dionne     <jeff@uclinux.org>
 * Copyright (C) 1999       Vladimir Gurevich  <vgurevic@cisco.com>
 */

#ifndef _Bf533_SERIAL_H
#define _Bf533_SERIAL_H

#include <linux/config.h>
#include <asm/blackfin.h>

#define SYNC_ALL	__asm__ __volatile__ ("ssync;\n")
#define ACCESS_LATCH	UART_LCR |= UART_LCR_DLAB;
#define ACCESS_PORT_IER	UART_LCR &= (~UART_LCR_DLAB);

void serial_setbrg(void);
static void local_put_char(char ch);
void calc_baud(void);
void serial_setbrg(void);
int serial_init(void);
void serial_putc(const char c);
int serial_tstc(void);
int serial_getc(void);
void serial_puts(const char *s);
static void local_put_char(char ch);

extern int get_clock(void);
extern unsigned long sclk;
int baud_table[5] = {9600, 19200, 38400, 57600, 115200};

struct {
	unsigned char dl_high;
	unsigned char dl_low;
} hw_baud_table[5];

#ifdef CONFIG_STAMP
extern unsigned long pll_div_fact;
#endif

#endif
