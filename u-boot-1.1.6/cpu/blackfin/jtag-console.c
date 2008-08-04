/*
 * jtag-console.c - console driver over Blackfin JTAG
 *
 * Copyright (c) 2008 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <common.h>
#include <asm/blackfin.h>

#ifdef CONFIG_JTAG_CONSOLE

/* JTAG has no "baud" concept */
void serial_set_baud(uint32_t baud) {}
void serial_setbrg(void) {}
int serial_init(void)
{
	return 0;
}

/* Transmit a buffer.  The format is:
 * [32bit length][actual data]
 */
static void bfin_jtag_send(const char *c, uint32_t len)
{
	uint32_t i;

	if (len == 0)
		return;

	/* First send the length */
	while (bfin_read_DBGSTAT() & 0x1)
		continue;
	__asm__ __volatile__("emudat = %0;" : : "d"(len));

	/* Then send the data */
	for (i = 0; i < len; i += 4) {
		uint32_t emudat;

		while (bfin_read_DBGSTAT() & 0x1)
			continue;

		emudat = (c[i] << 0) | (c[i+1] << 8) | (c[i+2] << 16) | (c[i+3] << 24);
		__asm__ __volatile__("emudat = %0;" : : "d"(emudat));
	}
}

void serial_putc(const char c)
{
	bfin_jtag_send(&c, 1);
}

void serial_puts(const char *s)
{
	bfin_jtag_send(s, strlen(s));
}

int serial_tstc(void)
{
	return (bfin_read_DBGSTAT() & 0x2);
}

/* Receive a buffer.  The format is:
 * [32bit length][actual data]
 */
static size_t inbound_len;
static int leftovers_len;
static uint32_t leftovers;
int serial_getc(void)
{
	int ret;
	uint32_t emudat;

	/* see if any data is left over */
	if (leftovers_len) {
		--leftovers_len;
		ret = leftovers & 0xff;
		leftovers >>= 8;
		return ret;
	}

	/* wait for new data ! */
	while (!serial_tstc())
		continue;
	__asm__("%0 = emudat;" : "=d"(emudat));

	if (inbound_len == 0) {
		/* grab the length */
		inbound_len = emudat;
	} else {
		/* store the bytes */
		leftovers_len = min(4, inbound_len);
		inbound_len -= leftovers_len;
		leftovers = emudat;
	}

	return serial_getc();
}

#endif
