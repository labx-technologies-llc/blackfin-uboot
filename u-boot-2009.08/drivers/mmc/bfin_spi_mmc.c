/*
 * Copyright (C) 2007, Rubico AB (www.rubico.se). All Rights Reserve.
 *
 * Developed as a part the CDT project C4 (www.cdt.ltu.se).
 *
 * Robert Selberg, <robert@rubico.se>
 * Hans Eklund, <hans@rubico.se>
 *
 * Licensed under the GPL-2.
 */

#include <common.h>
#include <malloc.h>
#include <spi.h>
#include <mmc.h>
#include "bfin_spimmc_mode.h"

#ifndef CONFIG_SPI_MMC_DEFAULT_CS
# define CONFIG_SPI_MMC_DEFAULT_CS 1
#endif
#ifndef CONFIG_SPI_MMC_DEFAULT_SPEED
# define CONFIG_SPI_MMC_DEFAULT_SPEED 30000000
#endif
#ifndef CONFIG_SPI_MMC_DEFAULT_MODE
# define CONFIG_SPI_MMC_DEFAULT_MODE SPI_MODE_3
#endif

#define MMC_BLOCK_SIZE 512

static block_dev_desc_t mmc_block_dev_desc;
static struct mmc_spi_dev msdev;

block_dev_desc_t *mmc_get_dev(int dev)
{
    debug("mmc_get_dev\n");
    return (block_dev_desc_t *)&mmc_block_dev_desc;
}

static int r;
unsigned long mmc_block_read(int dev, unsigned long blk_start, lbaint_t blkcnt, void *dst2)
{
	int i;
	unsigned char *dst = dst2;

	for (i = 0; i < blkcnt; i++, blk_start++, dst += MMC_BLOCK_SIZE) {
		r += MMC_BLOCK_SIZE;
		if (mmc_spi_read_mmc_block(&msdev, dst, blk_start * MMC_BLOCK_SIZE) != RVAL_OK)
			printf("error in mmc_block_read\n");;
	}
	debug("mmc_block_read: %d bytes\n", r);
	return blkcnt;
}

static struct spi_slave *slave;

static void spi_assert(void)
{
	spi_cs_activate(slave);
}

static void spi_deassert(void)
{
	spi_cs_deactivate(slave);
}

static int spi_wait_write(unsigned char *buffer, unsigned int count, void *dummy)
{
	spi_xfer(slave, count * 8, buffer, NULL, 0);
	return count;
}

static int spi_wait_read(unsigned char *buffer, unsigned int count, void *dummy)
{
	spi_xfer(slave, count * 8, NULL, buffer, 0);
	return count;
}

static int spi_mmc_init(void)
{
	if (slave) {
		spi_release_bus(slave);
		spi_free_slave(slave);
	}

	slave = spi_setup_slave(0, CONFIG_SPI_MMC_DEFAULT_CS,
		CONFIG_SPI_MMC_DEFAULT_SPEED, CONFIG_SPI_MMC_DEFAULT_MODE);
	if (!slave)
		return -1;
	spi_claim_bus(slave);

	return 0;
}

int mmc_legacy_init(int verbose)
{
	int ret;

	spi_mmc_init();
	msdev.read = &spi_wait_read;
	msdev.write = &spi_wait_write;
	msdev.assert = &spi_assert;
	msdev.deassert = &spi_deassert;
	ret = mmc_spi_init_card(&msdev);
	if (ret)
		return ret;
	mmc_block_dev_desc.if_type = IF_TYPE_MMC;
	mmc_block_dev_desc.part_type = PART_TYPE_DOS;
	mmc_block_dev_desc.dev = 0;
	mmc_block_dev_desc.blksz = MMC_BLOCK_SIZE;
	mmc_block_dev_desc.block_read = mmc_block_read;
	sprintf(mmc_block_dev_desc.vendor, "Rubico AB <www.rubico.se>");
	init_part(&mmc_block_dev_desc);
	return 0;
}
