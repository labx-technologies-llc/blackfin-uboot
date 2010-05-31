/*
 * U-boot - main board file
 *
 * Copyright (c) 2010 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <common.h>
#include <config.h>
#include <command.h>
#include <spi.h>
#include <asm/blackfin.h>
#include <asm/mach-common/bits/otp.h>
#include <asm/mach-common/bits/spi.h>

DECLARE_GLOBAL_DATA_PTR;

int checkboard(void)
{
	printf("Board: ADI BF527 AD7160-EVAL board\n");
	printf("       Support: http://blackfin.uclinux.org/\n");
	return 0;
}

/*
 * Temporary hack to avoid modifying the bfin_spi driver.
 */

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

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	if (cs == 0) {
		bfin_write_PORTH_FER(bfin_read_PORTH_FER() & ~PH3);
		bfin_write_PORTHIO_SET(PH3);
		bfin_write_PORTHIO_DIR(bfin_read_PORTHIO_DIR() | PH3);
	}
	return (cs >= 0 && cs <= 7);
}

void spi_cs_activate(struct spi_slave *slave)
{
	struct bfin_spi_slave *bss = to_bfin_spi_slave(slave);
	if (slave->cs == 0) {
		bfin_write_PORTHIO_CLEAR(PH3);
		return;
	}
	write_SPI_FLG(bss,
		(read_SPI_FLG(bss) &
		~((!bss->flg << 8) << slave->cs)) |
		(1 << slave->cs));
	SSYNC();
	debug("%s: SPI_FLG:%x\n", __func__, read_SPI_FLG(bss));
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	struct bfin_spi_slave *bss = to_bfin_spi_slave(slave);
	u16 flg;

	if (slave->cs == 0) {
		bfin_write_PORTHIO_SET(PH3);
		return;
	}

	/* make sure we force the cs to deassert rather than let the
	 * pin float back up.  otherwise, exact timings may not be
	 * met some of the time leading to random behavior (ugh).
	 */
	flg = read_SPI_FLG(bss) | ((!bss->flg << 8) << slave->cs);
	write_SPI_FLG(bss, flg);
	SSYNC();
	debug("%s: SPI_FLG:%x\n", __func__, read_SPI_FLG(bss));

	flg &= ~(1 << slave->cs);
	write_SPI_FLG(bss, flg);
	SSYNC();
	debug("%s: SPI_FLG:%x\n", __func__, read_SPI_FLG(bss));
}

int misc_init_r(void)
{
	return 0;
}
