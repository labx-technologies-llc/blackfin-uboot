/*
 * Driver for Blackfin On-Chip MAC device
 *
 * Copyright (c) 2005-2008 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

/*#define DEBUG*/

#include <common.h>
#include <malloc.h>
#include <spi.h>

#include <asm/blackfin.h>
#include <asm/mach-common/bits/spi.h>

struct bfin_spi_slave {
	struct spi_slave slave;
	void *mmr_base;
	u16 ctl, baud, flg;
};

#define MAKE_SPI_FUNC(mmr, off) \
static inline void write_##mmr(struct bfin_spi_slave *bss, u16 val) { bfin_write16(bss->mmr_base + off, val); } \
static inline u16 read_##mmr(struct bfin_spi_slave *bss) { return bfin_read16(bss->mmr_base + off); }
MAKE_SPI_FUNC(SPI_CTL,  0x00)
MAKE_SPI_FUNC(SPI_FLG,  0x04)
MAKE_SPI_FUNC(SPI_STAT, 0x08)
MAKE_SPI_FUNC(SPI_TDBR, 0x0c)
MAKE_SPI_FUNC(SPI_RDBR, 0x10)
MAKE_SPI_FUNC(SPI_BAUD, 0x14)

#define to_bfin_spi_slave(s) container_of(s, struct bfin_spi_slave, slave)

__attribute__((weak))
int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return (cs >= 1 && cs <= 7);
}

__attribute__((weak))
void spi_cs_activate(struct spi_slave *slave)
{
	struct bfin_spi_slave *bss = to_bfin_spi_slave(slave);
	write_SPI_FLG(bss,
		(read_SPI_FLG(bss) &
		~((!bss->flg << 8) << slave->cs)) |
		(1 << slave->cs));
	debug("%s: SPI_FLG:%x\n", __func__, read_SPI_FLG(bss));
}

__attribute__((weak))
void spi_cs_deactivate(struct spi_slave *slave)
{
	struct bfin_spi_slave *bss = to_bfin_spi_slave(slave);
	write_SPI_FLG(bss, read_SPI_FLG(bss) & ~(1 << slave->cs));
	debug("%s: SPI_FLG:%x\n", __func__, read_SPI_FLG(bss));
}

void spi_init()
{
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
		unsigned int max_hz, unsigned int mode)
{
	struct bfin_spi_slave *bss;
	u32 mmr_base;
	u32 baud;

	if (!spi_cs_is_valid(bus, cs))
		return NULL;

	switch (bus) {
#ifdef SPI_CTL
# define SPI0_CTL SPI_CTL
#endif
		case 0: mmr_base = SPI0_CTL; break;
#ifdef SPI1_CTL
		case 1: mmr_base = SPI1_CTL; break;
#endif
#ifdef SPI2_CTL
		case 2: mmr_base = SPI2_CTL; break;
#endif
		default: return NULL;
	}

	baud = get_sclk() / (2 * max_hz);
	if (baud < 2)
		return NULL; /* too slow */
	if (baud > (u16)-1)
		return NULL; /* too fast */

	bss = malloc(sizeof(*bss));
	if (!bss)
		return NULL;

	bss->slave.bus = bus;
	bss->slave.cs = cs;
	bss->mmr_base = (void *)mmr_base;
	bss->ctl = SPE | MSTR | TDBR_CORE;
	if (mode & SPI_CPHA) bss->ctl |= CPHA;
	if (mode & SPI_CPOL) bss->ctl |= CPOL;
	if (mode & SPI_LSB_FIRST) bss->ctl |= LSBF;
	bss->baud = baud;
	bss->flg = mode & SPI_CS_HIGH ? 1 : 0;

	debug("%s: bus:%i cs:%i mmr:%x ctl:%x baud:%i flg:%i\n", __func__,
		bus, cs, mmr_base, bss->ctl, baud, bss->flg);

	return &bss->slave;
}

void spi_free_slave(struct spi_slave *slave)
{
	struct bfin_spi_slave *bss = to_bfin_spi_slave(slave);
	free(bss);
}

static void spi_portmux(struct spi_slave *slave)
{
#if defined(__ADSPBF51x__)
#define SET_MUX(port, mux, func) port##mux = ((port##mux & ~PORT_x_MUX_##mux##_MASK) | PORT_x_MUX_##mux##_FUNC_##func)
	u16 fmux = bfin_read_PORTF_MUX();
	u16 ffer = bfin_read_PORTF_FER();
	u16 gmux = bfin_read_PORTG_MUX();
	u16 gfer = bfin_read_PORTG_FER();
	u16 hmux = bfin_read_PORTH_MUX();
	u16 hfer = bfin_read_PORTH_FER();
	switch (slave->bus) {
	case 0:
		/* set SCK/MISO/MOSI */
		SET_MUX(g, 7, 1);
		gfer |= PG12 | PG13 | PG14;
		switch (slave->cs) {
			case 1: SET_MUX(f, 2, 1); ffer |= PF7;  break;
			case 2: /* see G above */ gfer |= PG15; break;
			case 3: SET_MUX(h, 1, 3); ffer |= PH4;  break;
			case 4: /* no muxing */                 break;
			case 5: SET_MUX(g, 1, 3); hfer |= PG3;  break;
			case 6: /* no muxing */                 break;
			case 7: /* no muxing */                 break;
		}
	case 1:
		/* set SCK/MISO/MOSI */
		SET_MUX(h, 0, 2);
		hfer |= PH1 | PH2 | PH3;
		switch (slave->cs) {
			case 1: SET_MUX(h, 2, 3); hfer |= PH6;  break;
			case 2: SET_MUX(f, 0, 3); ffer |= PF0;  break;
			case 3: SET_MUX(g, 0, 3); gfer |= PG0;  break;
			case 4: SET_MUX(f, 3, 3); ffer |= PF8;  break;
			case 5: SET_MUX(g, 6, 3); hfer |= PG11; break;
			case 6: /* no muxing */                 break;
			case 7: /* no muxing */                 break;
		}
	}
	bfin_write_PORTF_MUX(fmux);
	bfin_write_PORTF_FER(ffer);
	bfin_write_PORTG_MUX(gmux);
	bfin_write_PORTG_FER(gfer);
	bfin_write_PORTH_MUX(hmux);
	bfin_write_PORTH_FER(hfer);
#elif defined(__ADSPBF52x__)
#define SET_MUX(port, mux, func) port##mux = ((port##mux & ~PORT_x_MUX_##mux##_MASK) | PORT_x_MUX_##mux##_FUNC_##func)
	u16 fmux = bfin_read_PORTF_MUX();
	u16 ffer = bfin_read_PORTF_FER();
	u16 gmux = bfin_read_PORTG_MUX();
	u16 gfer = bfin_read_PORTG_FER();
	u16 hmux = bfin_read_PORTH_MUX();
	u16 hfer = bfin_read_PORTH_FER();
	/* set SCK/MISO/MOSI */
	SET_MUX(g, 0, 3);
	gfer |= PG2 | PG3 | PG4;
	switch (slave->cs) {
		case 1: /* see G above */ gfer |= PG1; break;
		case 2: SET_MUX(f, 4, 3); ffer |= PF12; break;
		case 3: SET_MUX(f, 4, 3); ffer |= PF13; break;
		case 4: SET_MUX(h, 1, 1); hfer |= PH8;  break;
		case 5: SET_MUX(h, 2, 1); hfer |= PH9;  break;
		case 6: SET_MUX(f, 1, 3); ffer |= PF9;  break;
		case 7: SET_MUX(f, 2, 3); ffer |= PF10; break;
	}
	bfin_write_PORTF_MUX(fmux);
	bfin_write_PORTF_FER(ffer);
	bfin_write_PORTG_MUX(gmux);
	bfin_write_PORTG_FER(gfer);
	bfin_write_PORTH_MUX(hmux);
	bfin_write_PORTH_FER(hfer);
#elif defined(__ADSPBF534__) || defined(__ADSPBF536__) || defined(__ADSPBF537__)
	u16 mux = bfin_read_PORT_MUX();
	u16 ffer = bfin_read_PORTF_FER();
	u16 jfer = bfin_read_PORTJ_FER();
	/* set SCK/MISO/MOSI */
	ffer |= PF11 | PF12 | PF13;
	switch (slave->cs) {
		case 1: ffer |= PF10; break;
		case 2: mux |= PJSE; jfer |= PJ11; break;
		case 3: mux |= PJSE; jfer |= PJ10; break;
		case 4: mux |= PFS4E; ffer |= PF6; break;
		case 5: mux |= PFS5E; ffer |= PF5; break;
		case 6: mux |= PFS6E; ffer |= PF4; break;
		case 7: mux |= PJCE_SPI; jfer |= PJ5; break;
	}
	bfin_write_PORT_MUX(mux);
	bfin_write_PORTF_FER(ffer);
	bfin_write_PORTJ_FER(jfer);
#elif defined(__ADSPBF54x__)
#define DO_MUX(port, pin) \
	mux = ((mux & ~PORT_x_MUX_##pin##_MASK) | PORT_x_MUX_##pin##_FUNC_1); \
	fer |= P##port##pin;
	u32 mux;
	u16 fer;
	switch (slave->bus) {
	case 0:
		mux = bfin_read_PORTE_MUX();
		fer = bfin_read_PORTE_FER();
		/* set SCK/MISO/MOSI */
		DO_MUX(E, 0);
		DO_MUX(E, 1);
		DO_MUX(E, 2);
		switch (slave->cs) {
			case 1: DO_MUX(E, 4); break;
			case 2: DO_MUX(E, 5); break;
			case 3: DO_MUX(E, 6); break;
		}
		break;
		bfin_write_PORTE_MUX(mux);
		bfin_write_PORTE_FER(fer);
	case 1:
		mux = bfin_read_PORTG_MUX();
		fer = bfin_read_PORTG_FER();
		/* set SCK/MISO/MOSI */
		DO_MUX(G, 8);
		DO_MUX(G, 9);
		DO_MUX(G, 10);
		switch (slave->cs) {
			case 1: DO_MUX(G, 5); break;
			case 2: DO_MUX(G, 6); break;
			case 3: DO_MUX(G, 7); break;
		}
		bfin_write_PORTG_MUX(mux);
		bfin_write_PORTG_FER(fer);
		break;
	case 2:
		mux = bfin_read_PORTB_MUX();
		fer = bfin_read_PORTB_FER();
		/* set SCK/MISO/MOSI */
		DO_MUX(B, 12);
		DO_MUX(B, 13);
		DO_MUX(B, 14);
		switch (slave->cs) {
			case 1: DO_MUX(B, 9); break;
			case 2: DO_MUX(B, 10); break;
			case 3: DO_MUX(B, 11); break;
		}
		bfin_write_PORTB_MUX(mux);
		bfin_write_PORTB_FER(fer);
		break;
	}
#endif
}

int spi_claim_bus(struct spi_slave *slave)
{
	struct bfin_spi_slave *bss = to_bfin_spi_slave(slave);

	debug("%s: bus:%i cs:%i\n", __func__, slave->bus, slave->cs);

	spi_portmux(slave);
	write_SPI_CTL(bss, bss->ctl);
	write_SPI_BAUD(bss, bss->baud);
	SSYNC();

	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	struct bfin_spi_slave *bss = to_bfin_spi_slave(slave);
	debug("%s: bus:%i cs:%i\n", __func__, slave->bus, slave->cs);
	write_SPI_CTL(bss, 0);
	SSYNC();
}

int spi_xfer(struct spi_slave *slave, unsigned int bitlen, const void *dout,
		void *din, unsigned long flags)
{
	struct bfin_spi_slave *bss = to_bfin_spi_slave(slave);
	const u8 *tx = dout;
	u8 *rx = din;
	uint bytes = bitlen / 8;
	int ret = 0;

	debug("%s: bus:%i cs:%i bitlen:%i bytes:%i flags:%lx\n", __func__,
		slave->bus, slave->cs, bitlen, bytes, flags);

	if (bitlen == 0)
		goto done;

	/* we can only do 8 bit transfers */
	if (bitlen % 8) {
		flags |= SPI_XFER_END;
		goto done;
	}

	if (flags & SPI_XFER_BEGIN)
		spi_cs_activate(slave);

	/* todo: take advantage of hardware fifos and setup RX dma */
	while (bytes--) {
		debug("%s: tx:%x ", __func__, *tx);
		write_SPI_TDBR(bss, *tx++);
		SSYNC();
		while ((read_SPI_STAT(bss) & TXS))
			if (ctrlc()) {
				ret = -1;
				goto done;
			}
		while (!(read_SPI_STAT(bss) & SPIF))
			if (ctrlc()) {
				ret = -1;
				goto done;
			}
		while (!(read_SPI_STAT(bss) & RXS))
			if (ctrlc()) {
				ret = -1;
				goto done;
			}
		*rx++ = read_SPI_RDBR(bss);
		debug("rx:%x\n", rx[-1]);
	}

 done:
	if (flags & SPI_XFER_END)
		spi_cs_deactivate(slave);

	return ret;
}
