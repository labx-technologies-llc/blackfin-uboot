/*
 * Copyright (C) 2008 Analog Device Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>

#include <part.h>
#include <mmc.h>

#include <asm/io.h>
#include <asm/errno.h>
#include <asm/byteorder.h>
#include <asm/blackfin.h>
#include <asm/mach-common/bits/sdh.h>

#include "bfin_sdh.h"

#ifdef DEBUG
#define pr_debug(fmt, args...) printf(fmt, ##args)
#else
#define pr_debug(...) do { } while (0)
#endif

/*SD_CLK frequency must be less than 400k in identification mode*/
#define CFG_MMC_CLK_ID		200000
/*SD_CLK for normal working*/
#define CFG_MMC_CLK_OP		25000000
/*support 3.2-3.3V and 3.3-3.4V*/
#define CFG_MMC_OP_COND		0x00300000
#define MMC_DEFAULT_RCA		1

static unsigned int mmc_rca;
static int mmc_card_is_sd;
static block_dev_desc_t mmc_blkdev;

block_dev_desc_t *mmc_get_dev(int dev)
{
	return &mmc_blkdev;
}

static void mci_set_clk(unsigned long clk)
{
	unsigned long sys_clk;
	unsigned long clk_div;
	u16 clk_ctl = 0;

	/*setting SD_CLK*/
	sys_clk = get_sclk();
	if (sys_clk % (2 * clk) == 0)
			clk_div = sys_clk / (2 * clk) - 1;
		else
			clk_div = sys_clk / (2 * clk);

	if (clk_div > 0xff)
			clk_div = 0xFF;
		clk_ctl |= clk_div & 0xFF;
	bfin_write_SDH_CLK_CTL(clk_ctl);
}

static int
mmc_cmd(unsigned long cmd, unsigned long arg,
	void *resp, unsigned long flags)
{
	unsigned int sdh_cmd;
	unsigned int status;
	int ret = 0;
	sdh_cmd = 0;
	unsigned long *response = resp;
	sdh_cmd |= cmd;

	if (flags & MMC_RSP_PRESENT)
		sdh_cmd |= CMD_RSP;

	if (flags & MMC_RSP_136)
		sdh_cmd |= CMD_L_RSP;

	bfin_write_SDH_ARGUMENT(arg);
	bfin_write_SDH_COMMAND(sdh_cmd | CMD_E);
	bfin_write_SDH_CLK_CTL(bfin_read_SDH_CLK_CTL() | CLK_E);
	/*wait for a while*/
	do {
		udelay(40);
		status = bfin_read_SDH_STATUS();
	} while (!(status & (CMD_SENT | CMD_RESP_END | CMD_TIME_OUT |
		CMD_CRC_FAIL)));

	if (flags & MMC_RSP_PRESENT) {
		response[0] = bfin_read_SDH_RESPONSE0();
		if (flags & MMC_RSP_136) {
			response[1] = bfin_read_SDH_RESPONSE1();
			response[2] = bfin_read_SDH_RESPONSE2();
			response[3] = bfin_read_SDH_RESPONSE3();
		}
	}

	if (status & CMD_TIME_OUT)
		ret |= -ETIMEDOUT;
	else if (status & CMD_CRC_FAIL && flags & MMC_RSP_CRC)
		ret |= -EILSEQ;
	bfin_write_SDH_STATUS_CLR(CMD_SENT_STAT | CMD_RESP_END_STAT |
				CMD_TIMEOUT_STAT | CMD_CRC_FAIL_STAT);
	bfin_write_SDH_CLK_CTL(bfin_read_SDH_CLK_CTL() & ~CLK_E);
	return ret;
}

static int mmc_acmd(unsigned long cmd, unsigned long arg,
		    void *resp, unsigned long flags)
{
	unsigned long aresp[4];
	int ret = 0;

	ret = mmc_cmd(MMC_CMD_APP_CMD, 0, aresp,
		      MMC_RSP_PRESENT);
	if (ret)
		return ret;

	if ((aresp[0] & (ILLEGAL_COMMAND | APP_CMD)) != APP_CMD)
		return -ENODEV;
	ret = mmc_cmd(cmd, arg, resp, flags);
	return ret;
}

static unsigned long
mmc_bread(int dev, unsigned long start, lbaint_t blkcnt,
	  void *buffer)
{
	int ret, i = 0;
	unsigned long resp[4];
	unsigned long card_status, data;
	unsigned long wordcount;
	u32 *p = buffer;
	u32 status;
	unsigned int length;
	unsigned int blk_size;
	unsigned int data_ctl = 0;

	if (blkcnt == 0)
		return 0;
	pr_debug("mmc_bread: dev %d, start %lx, blkcnt %lx\n",
		 dev, start, blkcnt);
	length = mmc_blkdev.blksz * blkcnt;
	bfin_write_SDH_DATA_LGTH(length);
	switch (mmc_blkdev.blksz) {
	case 2048:
		blk_size = 11;
		break;
	case 1024:
		blk_size = 10;
		break;
	case 512:
		blk_size = 9;
		break;
	case 256:
		blk_size = 8;
		break;
	case 128:
		blk_size = 7;
		break;
	case 64:
		blk_size = 6;
		break;
	case 32:
		blk_size = 5;
		break;
	case 16:
		blk_size = 4;
		break;
	default:
		blk_size = 3;
		break;
	}
	data_ctl |= blk_size << 4;
	data_ctl |= DTX_DIR;
	bfin_write_SDH_DATA_CTL(data_ctl);
	/* FIXME later */
	bfin_write_SDH_DATA_TIMER(0xFFFFFFFF);
	/* Put the device into Transfer state */
	ret = mmc_cmd(MMC_CMD_SELECT_CARD, mmc_rca << 16, resp, MMC_RSP_R1);
	if (ret)
		goto out;
	/* Set block length */
	ret = mmc_cmd(MMC_CMD_SET_BLOCKLEN, mmc_blkdev.blksz, resp, MMC_RSP_R1);
	if (ret)
		goto out;
	for (i = 0; i < blkcnt; i++, start++) {
		ret = mmc_cmd(MMC_CMD_READ_SINGLE_BLOCK,
			      start * mmc_blkdev.blksz, resp,
			      MMC_RSP_R1);
		if (ret)
			goto out;
bfin_write_SDH_DATA_CTL(bfin_read_SDH_DATA_CTL() | DTX_E);
bfin_write_SDH_CLK_CTL(bfin_read_SDH_CLK_CTL() | CLK_E);
		wordcount = 0;
		do {
			/*
			do {
				status = bfin_read_SDH_STATUS();
			} while (!(status & (RX_FIFO_STAT | DAT_TIME_OUT | DAT_CRC_FAIL | RX_OVERRUN)));
			if (status & (DAT_TIME_OUT | DAT_CRC_FAIL | RX_OVERRUN)) {
				bfin_write_SDH_STATUS_CLR(DAT_TIMEOUT_STAT | \
					DAT_CRC_FAIL_STAT | RX_OVERRUN);
				goto read_error;
			} else if (status & RX_FIFO_STAT) {
				data = bfin_read_SDH_FIFO();
				*p++ = data;
				wordcount++;
			}*/
			data = bfin_read_SDH_FIFO();
				*p++ = data;
				wordcount++;
		} while (wordcount < (mmc_blkdev.blksz / 4));
		bfin_write_SDH_STATUS_CLR(DAT_END_STAT | DAT_TIMEOUT_STAT | \
			DAT_CRC_FAIL_STAT | DAT_BLK_END_STAT | RX_OVERRUN_STAT);
	}
out:
	/* Put the device back into Standby state */
	mmc_cmd(MMC_CMD_SELECT_CARD, 0, resp, 0);
	bfin_write_SDH_CLK_CTL(bfin_read_SDH_CLK_CTL() & ~CLK_E);
	return i;

read_error:
	mmc_cmd(MMC_CMD_SEND_STATUS, mmc_rca << 16, &card_status, MMC_RSP_R1);
	printf("mmc: bread failed, status = %08x, card status = %08lx\n",
	       status, card_status);
	goto out;
}

static unsigned long
mmc_bwrite(int dev, unsigned long start, lbaint_t blkcnt,
	  const void *buffer)
{
	int ret, i = 0;
	unsigned long resp[4];
	unsigned long card_status;
	unsigned long wordcount;
	const u32 *p = buffer;
	u32 status;
	unsigned int length;
	unsigned int blk_size;
	unsigned int data_ctl = 0;

	if (blkcnt == 0)
		return 0;

	pr_debug("mmc_bread: dev %d, start %lx, blkcnt %lx\n",
		 dev, start, blkcnt);
	length = mmc_blkdev.blksz * blkcnt;
	bfin_write_SDH_DATA_LGTH(length);
	switch (mmc_blkdev.blksz) {
	case 2048:
		blk_size = 11;
		break;
	case 1024:
		blk_size = 10;
		break;
	case 512:
		blk_size = 9;
		break;
	case 256:
		blk_size = 8;
		break;
	case 128:
		blk_size = 7;
		break;
	case 64:
		blk_size = 6;
		break;
	case 32:
		blk_size = 5;
		break;
	case 16:
		blk_size = 4;
		break;
	default:
		blk_size = 3;
		break;
	}
	data_ctl |= blk_size << 4;
	data_ctl &= ~DTX_DIR;
	bfin_write_SDH_DATA_CTL(data_ctl);
	/* FIXME later */
	bfin_write_SDH_DATA_TIMER(0xFFFFFFFF);
	bfin_write_SDH_DATA_CTL(bfin_read_SDH_DATA_CTL() | DTX_E);
	/* Put the device into Transfer state */
	ret = mmc_cmd(MMC_CMD_SELECT_CARD, mmc_rca << 16, resp, MMC_RSP_R1);
	if (ret)
		goto out;

	/* Set block length */
	ret = mmc_cmd(MMC_CMD_SET_BLOCKLEN, mmc_blkdev.blksz, resp, MMC_RSP_R1);
	if (ret)
		goto out;

	for (i = 0; i < blkcnt; i++, start++) {
		ret = mmc_cmd(MMC_CMD_WRITE_BLOCK,
			      start * mmc_blkdev.blksz, resp,
			      MMC_RSP_R1);
		if (ret)
			goto out;

		wordcount = 0;
		do {
			bfin_write_SDH_FIFO(*p++);
			wordcount++;
		} while (wordcount < (mmc_blkdev.blksz / 4));

		status = bfin_read_SDH_STATUS();
		if (status & (DAT_TIME_OUT | DAT_CRC_FAIL | TX_UNDERRUN)) {
			bfin_write_SDH_STATUS_CLR(DAT_TIMEOUT_STAT | \
				DAT_CRC_FAIL_STAT | TX_UNDERRUN);
			goto write_error;
		}
		bfin_write_SDH_STATUS_CLR(DAT_END_STAT | DAT_TIMEOUT_STAT | \
			DAT_CRC_FAIL_STAT | DAT_BLK_END_STAT | TX_UNDERRUN);
	}
out:
	/* Put the device back into Standby state */
	mmc_cmd(MMC_CMD_SELECT_CARD, 0, resp, 0);
	bfin_write_SDH_CLK_CTL(bfin_read_SDH_CLK_CTL() & ~CLK_E);
	return i;

write_error:
	mmc_cmd(MMC_CMD_SEND_STATUS, mmc_rca << 16, &card_status, MMC_RSP_R1);
	printf("mmc: bwrite failed, status = %08x, card status = %08lx\n",
	       status, card_status);
	goto out;
}

static void mmc_parse_cid(struct mmc_cid *cid, unsigned long *resp)
{
	cid->mid = resp[0] >> 24;
	cid->oid = (resp[0] >> 8) & 0xffff;
	cid->pnm[0] = resp[0];
	cid->pnm[1] = resp[1] >> 24;
	cid->pnm[2] = resp[1] >> 16;
	cid->pnm[3] = resp[1] >> 8;
	cid->pnm[4] = resp[1];
	cid->pnm[5] = resp[2] >> 24;
	cid->pnm[6] = 0;
	cid->prv = resp[2] >> 16;
	cid->psn = (resp[2] << 16) | (resp[3] >> 16);
	cid->mdt = resp[3] >> 8;
}

static void sd_parse_cid(struct mmc_cid *cid, unsigned long *resp)
{
	cid->mid = resp[0] >> 24;
	cid->oid = (resp[0] >> 8) & 0xffff;
	cid->pnm[0] = resp[0];
	cid->pnm[1] = resp[1] >> 24;
	cid->pnm[2] = resp[1] >> 16;
	cid->pnm[3] = resp[1] >> 8;
	cid->pnm[4] = resp[1];
	cid->pnm[5] = 0;
	cid->pnm[6] = 0;
	cid->prv = resp[2] >> 24;
	cid->psn = (resp[2] << 8) | (resp[3] >> 24);
	cid->mdt = (resp[3] >> 8) & 0x0fff;
}

static void mmc_dump_cid(const struct mmc_cid *cid)
{
	printf("Manufacturer ID:       %02X\n", cid->mid);
	printf("OEM/Application ID:    %04X\n", cid->oid);
	printf("Product name:          %s\n", cid->pnm);
	printf("Product Revision:      %u.%u\n",
	       cid->prv >> 4, cid->prv & 0x0f);
	printf("Product Serial Number: %lu\n", cid->psn);
	printf("Manufacturing Date:    %02u/%02u\n",
	       cid->mdt >> 4, cid->mdt & 0x0f);
}

static void mmc_dump_csd(const struct mmc_csd *csd)
{
	unsigned long *csd_raw = (unsigned long *)csd;
	printf("CSD data: %08lx %08lx %08lx %08lx\n",
	       csd_raw[0], csd_raw[1], csd_raw[2], csd_raw[3]);
	printf("CSD structure version:   1.%u\n", csd->csd_structure);
	printf("MMC System Spec version: %u\n", csd->spec_vers);
	printf("Card command classes:    %03x\n", csd->ccc);
	printf("Read block length:       %u\n", 1 << csd->read_bl_len);
	if (csd->read_bl_partial)
		puts("Supports partial reads\n");
	else
		puts("Does not support partial reads\n");
	printf("Write block length:      %u\n", 1 << csd->write_bl_len);
	if (csd->write_bl_partial)
		puts("Supports partial writes\n");
	else
		puts("Does not support partial writes\n");
	if (csd->wp_grp_enable)
		printf("Supports group WP:      %u\n", csd->wp_grp_size + 1);
	else
		puts("Does not support group WP\n");
	printf("Card capacity:		%u bytes\n",
	       (csd->c_size + 1) * (1 << (csd->c_size_mult + 2)) *
	       (1 << csd->read_bl_len));
	printf("File format:            %u/%u\n",
	       csd->file_format_grp, csd->file_format);
	puts("Write protection:        ");
	if (csd->perm_write_protect)
		puts(" permanent");
	if (csd->tmp_write_protect)
		puts(" temporary");
	putc('\n');
}

static int mmc_idle_cards(void)
{
	int ret = 0;

	/* Reset and initialize all cards */
	ret = mmc_cmd(MMC_CMD_GO_IDLE_STATE, 0, NULL, 0);
	return ret;
}

static int sd_init_card(struct mmc_cid *cid, int verbose)
{
	unsigned long resp[4];
	int i, ret = 0;

	mmc_idle_cards();
	for (i = 0; i < 1000; i++) {
		ret = mmc_acmd(SD_CMD_APP_SEND_OP_COND, CFG_MMC_OP_COND,
			       resp, MMC_RSP_R3);
		if (ret || (resp[0] & 0x80000000))
			break;
		ret = -ETIMEDOUT;
	}

	if (ret)
		return ret;
	ret = mmc_cmd(MMC_CMD_ALL_SEND_CID, 0, resp, MMC_RSP_R2);
	if (ret)
		return ret;
	sd_parse_cid(cid, resp);
	if (verbose)
		mmc_dump_cid(cid);

	/* Get RCA of the card that responded */
	ret = mmc_cmd(SD_CMD_SEND_RELATIVE_ADDR, 0, resp, MMC_RSP_R6);
	if (ret)
		return ret;

	mmc_rca = resp[0] >> 16;
	if (verbose)
		printf("SD Card detected (RCA %u)\n", mmc_rca);
	mmc_card_is_sd = 1;
	return 0;
}

static int mmc_init_card(struct mmc_cid *cid, int verbose)
{
	unsigned long resp[4];
	int i, ret = 0;
	mmc_idle_cards();

	for (i = 0; i < 1000; i++) {
		ret = mmc_cmd(MMC_CMD_SEND_OP_COND, CFG_MMC_OP_COND, resp,
			      MMC_RSP_R3);
		if (ret || (resp[0] & 0x80000000))
			break;
		ret = -ETIMEDOUT;
	}

	if (ret)
		return ret;
	/* Get CID of all cards. FIXME: Support more than one card */
	ret = mmc_cmd(MMC_CMD_ALL_SEND_CID, 0, resp, MMC_RSP_R2);
	if (ret)
		return ret;
	mmc_parse_cid(cid, resp);
	if (verbose)
		mmc_dump_cid(cid);

	/* Set Relative Address of the card that responded */
	ret = mmc_cmd(MMC_CMD_SET_RELATIVE_ADDR, mmc_rca << 16, resp,
		      MMC_RSP_R1);
	return ret;
}

int mmc_init(int verbose)
{
	struct mmc_cid cid;
	struct mmc_csd csd;
	u16 pwr_ctl = 0;
	u16 data_ctl = 0;
	int ret;
	unsigned int max_blksz;

	/* Initialize sdh controller */
	bfin_write_PORTC_FER(bfin_read_PORTC_FER() | 0x3F00);
	bfin_write_PORTC_MUX(bfin_read_PORTC_MUX() & ~0xFFF0000);
	bfin_write_SDH_CFG(bfin_read_SDH_CFG() | CLKS_EN);
	/* Disable card detect pin */
	bfin_write_SDH_CFG((bfin_read_SDH_CFG() & 0x1F) | 0x60);
	/*block length always is 512*/
	data_ctl |= 9 << 4;
	bfin_write_SDH_DATA_CTL(data_ctl);
	mci_set_clk(CFG_MMC_CLK_ID);
	/*setting power control*/
	pwr_ctl |= ROD_CTL;
	pwr_ctl |= PWR_ON;
	bfin_write_SDH_PWR_CTL(pwr_ctl);
	mmc_card_is_sd = 0;
	ret = sd_init_card(&cid, verbose);
	if (ret) {
		mmc_rca = MMC_DEFAULT_RCA;
		ret = mmc_init_card(&cid, verbose);
	}
	if (ret)
		return ret;
	/* Get CSD from the card */
	ret = mmc_cmd(MMC_CMD_SEND_CSD, mmc_rca << 16, &csd, MMC_RSP_R2);
	if (ret)
		return ret;
	if (verbose)
		mmc_dump_csd(&csd);
	/* Initialize the blockdev structure */
	mmc_blkdev.if_type = IF_TYPE_SD;
	mmc_blkdev.part_type = PART_TYPE_DOS;
	mmc_blkdev.block_read = mmc_bread;
	mmc_blkdev.block_write = mmc_bwrite;
	sprintf((char *)mmc_blkdev.vendor,
		"Man %02x%04x Snr %08lx",
		cid.mid, cid.oid, cid.psn);
	strncpy((char *)mmc_blkdev.product, cid.pnm,
		sizeof(mmc_blkdev.product));
	sprintf((char *)mmc_blkdev.revision, "%x %x",
		cid.prv >> 4, cid.prv & 0x0f);

	/*
	 * If we can't use 512 byte blocks, refuse to deal with the
	 * card. Tons of code elsewhere seems to depend on this.
	 */
/*
	max_blksz = 1 << csd.read_bl_len;
	if (max_blksz < 512 || (max_blksz > 512 && !csd.read_bl_partial)) {
		printf("Card does not support 512 byte reads, aborting.\n");
		return -ENODEV;
	}
*/
	mmc_blkdev.blksz = 1 << csd.read_bl_len;
	mmc_blkdev.lba = (csd.c_size + 1) * (1 << (csd.c_size_mult + 2));
	mci_set_clk(CFG_MMC_CLK_OP);
	init_part(&mmc_blkdev);
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
