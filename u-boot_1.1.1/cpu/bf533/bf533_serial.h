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
 *
 *	PROJECT				:	BFIN
 *	VERSION				:	2.0
 *	FILE				:	bf533_serial.h
 *	MODIFIED DATE			:	29 jun 2004
 *	AUTHOR				:	BFin Project-ADI
 *	LOCATION			:	LG Soft India,Bangalore
 */

#ifndef _Bf533_SERIAL_H
#define _Bf533_SERIAL_H

#include <linux/config.h>
#include <asm/blackfin.h>

struct serial_struct {
	int type;
	int line;
	int hub2;
	int irq;
	int flags;
	int xmit_fifo_size;
	int custom_divisor;
	int baud_base;
	unsigned short close_delay;
	char reserved_char[2];
	int hub6;
	unsigned short closing_wait;
	unsigned short closing_wait2;
	int reserved[4];
};

struct bf533_serial {
	int magic;
	int hub2;
	int irq;
	int flags;		/* defined in tty.h */
	char soft_carrier;	/* Use soft carrier on this channel */
	char break_abort;	/* Is serial console in, so process brk/abrt */
	char is_cons;		/* Is this our console. */
	unsigned char clk_divisor;
	int baud;
	int baud_base;
	int type;		/* UART type */
	struct tty_struct *tty;
	int read_status_mask;
	int ignore_status_mask;
	int timeout;
	int xmit_fifo_size;
	int custom_divisor;
	int x_char;		/* xon/xoff character */
	int close_delay;
	unsigned short closing_wait;
	unsigned short closing_wait2;
	unsigned long event;
	unsigned long last_active;
	int line;
	int count;		/* # of fd on device */
	int blocked_open;	/* # of blocked opens */
	long session;		/* Session of opening process */
	long pgrp;		/* pgrp of opening process */
	unsigned char *xmit_buf;
	int xmit_head;
	int xmit_tail;
	int xmit_cnt;
};

#define SYNC_ALL			__asm__ __volatile__ ("ssync;\n")
#define ACCESS_LATCH			UART_LCR |= UART_LCR_DLAB;
#define ACCESS_PORT_IER			UART_LCR &= (~UART_LCR_DLAB);
#define	CONSOLE_BAUD_RATE		57600
#define	DEFAULT_CBAUD			B57600
#define _INLINE_ 			inline
#define NR_PORTS 			(sizeof(bf533_soft) / sizeof(struct bf533_serial))
#define BAUD_TABLE_SIZE 		(sizeof(baud_table)/sizeof(baud_table[0]))

extern int request_irq(unsigned int irq,
		       void (*handler) (int, void *, struct pt_regs *),
		       unsigned long flags, const char *devname,
		       void *dev_id);
void serial_setbrg(void);
void serial_isr(int irq, void *dev_id, struct pt_regs *regs);
static _INLINE_ void receive_chars(struct bf533_serial *info,
				   struct pt_regs *regs,
				   unsigned short rx);
static _INLINE_ void transmit_chars(struct bf533_serial *info);
static void local_put_char(char ch);

static struct bf533_serial bf533_soft = { 0, 0, IRQ_UART, 0 };
static int baud_table[] = {
	0, 114, 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 0
};

#ifdef CONFIG_EZKIT533
struct {
	unsigned short dl_high, dl_low;
} hw_baud_table[] = {
	{
	0xff, 0xff},		/* approximately 0 */
	{
	0xff, 0xff},		/* 114 */
	{
	0x61, 0xa8},		/* 300 */
	{
	0x18, 0x6a},		/* 1200 */
	{
	0xc, 0x35},		/* 2400 */
	{
	0x6, 0x1a},		/* 4800 */
	{
	0x1, 0x5f},		/* 9600 */
	{
	0x0, 0xaf},		/* 19200 */
	{
	0x0, 0x57},		/* 38400 */
	{
	0x0, 0x3a},		/* 57600 */
	{
	0x0, 0x1D},		/* 115200 */
/*
 * rate = SCLK / (16 * DL) - SCLK = 120MHz
 * DL = (dl_high:dl_low) 
 */
};
#endif

#ifdef CONFIG_STAMP
struct {
	unsigned short dl_high, dl_low;
} hw_baud_table[] = {
	{
	0xff, 0xff},		/* approximately 0 */
	{
	0xff, 0xff},		/* 114 */
	{
	0x61, 0xa8},		/* 300 */
	{
	0x18, 0x6a},		/* 1200 */
	{
	0xc, 0x35},		/* 2400 */
	{
	0x6, 0x1a},		/* 4800 */
	{
	0x3, 0x5B},		/* 9600 */
	{
	0x1, 0xad},		/* 19200 */
	{
	0x0, 0xd6},		/* 38400 */
	{
	0x0, 0x8F},		/* 57600 */
	{
	0x0, 0x47},		/* 115200 */
/*
 * rate = SCLK / (16 * DL) - SCLK = 120MHz 
 * DL = (dl_high:dl_low)
 */
};
#endif

#endif
