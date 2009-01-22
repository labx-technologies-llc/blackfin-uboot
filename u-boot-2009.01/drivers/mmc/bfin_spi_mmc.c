/**************************************************************
*
* Copyright (C) 2007, Rubico AB (www.rubico.se). All Rights Reserve.
*
* Developed as a part the CDT project C4 (www.cdt.ltu.se).
*
* Robert Selberg, <robert@rubico.se>
* Hans Eklund, <hans@rubico.se>
*
**************************************************************
*
* This program is free software; you can distribute it and/or modify it
* under the terms of the GNU General Public License (Version 2) as
* published by the Free Software Foundation.
*
* This program is distributed in the hope it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
*
**************************************************************/
#include <config.h>
#include <common.h>
#include <mmc.h>
#include <asm/errno.h>
#include <part.h>
#include "bfin_spi_channel.h"
#include "bfin_spimmc_mode.h"

#define SPI_INIT_BAUD 0x0100
#define SPI_FULL_BAUD 0x0003
#define POLL_TIMEOUT 10000000

#define MMC_BLOCK_SIZE 512
#define MMC_MAGIC_OFFSET (0x80000 + CONFIG_SYS_FLASH_BASE)

/******************************************************************************\
| Definitions                                                                  |
\******************************************************************************/
static int spi_wait_write(unsigned char *buffer, unsigned int count, void *dummy);
static int spi_wait_read(unsigned char *buffer, unsigned int count, void *dummy);
static int init_spi(void);
static void spi_assert(void);
static void spi_deassert(void);
/******************************************************************************\
| Globals                                                                      |
\******************************************************************************/
static block_dev_desc_t mmc_block_dev_desc;
static struct mmc_spi_dev msdev;
static short cs_sel_pf_pin = -1;
/******************************************************************************\
| Functions                                                                    |
\******************************************************************************/
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

int mmc_init(int verbose)
{
	int retval;
	init_spi();
	msdev.read = &spi_wait_read;
	msdev.write = &spi_wait_write;
	msdev.assert = &spi_assert;
	msdev.deassert = &spi_deassert;
	retval = mmc_spi_init_card(&msdev);
	if (retval != 0) {
		printf("mmc_init failed. retval=0x%X\n", retval);
		return -1;
	}
	printf("OK! MMC/SD card found.\n");
	*(unsigned short *)(SPI0_REGBASE + SPI_BAUD_OFF) = SPI_FULL_BAUD;
	mmc_block_dev_desc.if_type = IF_TYPE_MMC;
	mmc_block_dev_desc.part_type = PART_TYPE_DOS;
	mmc_block_dev_desc.dev = 0;
	mmc_block_dev_desc.blksz = MMC_BLOCK_SIZE;
	mmc_block_dev_desc.block_read = mmc_block_read;
	sprintf(mmc_block_dev_desc.vendor, "Rubico AB <www.rubico.se>");
	init_part(&mmc_block_dev_desc);
	return 0;
}

/* Pull chip select here, instead of inside spi_wait_read/write to follow
  MMC/SD spec. better. However, it does not seem to be nessecary. What may
  be nessesary though is to not use the SPI bus to other devices until
  a full ommand sequence(such as a mmc_read_block) is completed. The empty
  functions is needed since mmc_spi_mode.c makes call to them.
*/
static void spi_assert(void)
{
	*pPORTFIO_CLEAR = 1<<cs_sel_pf_pin;
	return;
}

static void spi_deassert(void)
{
	*pPORTFIO_SET = 1<<cs_sel_pf_pin;
	return;
}

unsigned int bfin_pio_spi_transfer(unsigned char *in, unsigned char out)
{
	unsigned int tc = POLL_TIMEOUT;
	unsigned short SPI;
	volatile unsigned short tmp = 0;
	volatile unsigned int spi_rx_buffer = (SPI0_REGBASE + SPI_RXBUFF_OFF);
	/* Transmit data */
	*(unsigned short *)(SPI0_REGBASE + SPI_TXBUFF_OFF) = out;

	/* Wait until d is transmitted */
	SPI = *(unsigned short *)(SPI0_REGBASE + SPI_STAT_OFF);
	while ((SPI & 0x0008) && tc--)
		SPI = *(unsigned short *)(SPI0_REGBASE + SPI_STAT_OFF);
	/* Wait until recv buffer is ready */
	tc = POLL_TIMEOUT;
	while (!(SPI & 0x20) && tc--)
		SPI = *(unsigned short *)(SPI0_REGBASE + SPI_STAT_OFF);
	if (!tc)
		return 0;
	/* Read out recv buffer */
	tmp = *(unsigned short *)(spi_rx_buffer);
	*in = (unsigned char)tmp;

	return 1;
}

static int spi_wait_write(unsigned char *buffer, unsigned int count, void *dummy)
{
	unsigned int i;
	unsigned char in;
	unsigned int bytes_written = 0;
	for (i = 0; i < count; i++)
		bytes_written += bfin_pio_spi_transfer(&in, buffer[i]);

	return bytes_written;
}

static int spi_wait_read(unsigned char *buffer, unsigned int count, void *dummy)
{
	unsigned int i;
	unsigned int bytes_read = 0;
	for (i = 0; i < count; i++) {
		/* send 0xFF means send nothing */
		bytes_read += bfin_pio_spi_transfer(&buffer[i], 0xFF);
	}

	return bytes_read;
}

static int init_spi(void)
{
	char *spienv, *dummy;
	int mmc_spi_ssel;

	spienv = getenv("mmc_spi_ssel");
	if (spienv != NULL) {
		mmc_spi_ssel = (int) simple_strtoul(spienv, &dummy, 10);
		if (mmc_spi_ssel > 7 ||  mmc_spi_ssel < 1)
			mmc_spi_ssel = 4;
		printf("Using SPISSEL %d\n", mmc_spi_ssel);
	} else {
		mmc_spi_ssel = 4;
		printf("Using rubico default SPISSEL %d\n", mmc_spi_ssel);
		printf("To change, set environment variable mmc_spi_ssel\n");
		printf("to the desired SSEL and run mmcinit again\n");
		printf("Currently SSEL 2,3 and 5 doesnt work!\n");
	}

	switch (mmc_spi_ssel) {
	case 1:
		cs_sel_pf_pin = 10;
	break;
	case 2:
		cs_sel_pf_pin = -1;
	case 3:
		cs_sel_pf_pin = -1;
		*pPORT_MUX &= ~PJSE;
	break;
	case 4:
		cs_sel_pf_pin = 6;
		*pPORT_MUX &= ~PFS4E;
	break;
	case 5:
		cs_sel_pf_pin = -1;
		*pPORT_MUX &= ~PFS5E;
	break;
	case 6:
		cs_sel_pf_pin = 4;
		*pPORT_MUX &= ~PFS6E;
	break;
	case 7:
		cs_sel_pf_pin = -1;
		*pPORT_MUX &= ~PJCE_SPI;
	break;
	}
	if (cs_sel_pf_pin == -1) {
		printf("Invalid SSEL selected SSEL%d is not implemented!\n", cs_sel_pf_pin);
		return -1;
	}
	/* put MISO,MOSI,CLK,PF6,PF4 in peripheral mode */
	*pPORTF_FER |= 0x3800;

	/* set selected spissel PF pins in GPIO mode and output */
	*pPORTF_FER = *pPORTF_FER & ~(1<<cs_sel_pf_pin);
	*pPORTFIO_DIR |= (1 << cs_sel_pf_pin);
	SSYNC();

	*(unsigned short *)(SPI0_REGBASE + SPI_BAUD_OFF) = SPI_INIT_BAUD;
	*(unsigned short *)(SPI0_REGBASE + SPI_FLAG_OFF) = 0xFF00;
	*(unsigned short *)(SPI0_REGBASE + SPI_CTRL_OFF) = 0x5009;
	SSYNC();
	/* clear status reg */
	*(unsigned short *)(SPI0_REGBASE + SPI_STAT_OFF) = 0xFFFF;
	return 0;
}

int mmc_read(ulong src, uchar *dst, int size)
{
	return -ENOSYS;
}

int mmc_write(uchar *src, ulong dst, int size)
{
	return -ENOSYS;
}

int mmc2info(ulong addr)
{
	return 0;
}
