/*
 * drivers/mtd/nand/nand_util.c
 *
 * Copyright (C) 2006 by Weiss-Electronic GmbH.
 * All rights reserved.
 *
 * @author:	Guido Classen <clagix@gmail.com>
 * @descr:	NAND Flash support
 * @references: borrowed heavily from Linux mtd-utils code:
 *		flash_eraseall.c by Arcom Control System Ltd
 *		nandwrite.c by Steven J. Hill (sjhill@realitydiluted.com)
 *			       and Thomas Gleixner (tglx@linutronix.de)
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

#include <common.h>
#include <command.h>
#include <watchdog.h>
#include <malloc.h>
#include <div64.h>


#include <asm/errno.h>
#include <linux/mtd/mtd.h>
#include <nand.h>
#include <jffs2/jffs2.h>

typedef struct erase_info erase_info_t;
typedef struct mtd_info	  mtd_info_t;

/* support only for native endian JFFS2 */
#define cpu_to_je16(x) (x)
#define cpu_to_je32(x) (x)

/*****************************************************************************/
static int nand_block_bad_scrub(struct mtd_info *mtd, loff_t ofs, int getchip)
{
	return 0;
}

/**
 * nand_erase_opts: - erase NAND flash with support for various options
 *		      (jffs2 formating)
 *
 * @param meminfo	NAND device to erase
 * @param opts		options,  @see struct nand_erase_options
 * @return		0 in case of success
 *
 * This code is ported from flash_eraseall.c from Linux mtd utils by
 * Arcom Control System Ltd.
 */
int nand_erase_opts(nand_info_t *meminfo, const nand_erase_options_t *opts)
{
	struct jffs2_unknown_node cleanmarker;
	erase_info_t erase;
	ulong erase_length;
	int bbtest = 1;
	int result;
	int percent_complete = -1;
	int (*nand_block_bad_old)(struct mtd_info *, loff_t, int) = NULL;
	const char *mtd_device = meminfo->name;
	struct mtd_oob_ops oob_opts;
	struct nand_chip *chip = meminfo->priv;

	memset(&erase, 0, sizeof(erase));
	memset(&oob_opts, 0, sizeof(oob_opts));

	erase.mtd = meminfo;
	erase.len  = meminfo->erasesize;
	erase.addr = opts->offset;
	erase_length = opts->length;

	cleanmarker.magic = cpu_to_je16 (JFFS2_MAGIC_BITMASK);
	cleanmarker.nodetype = cpu_to_je16 (JFFS2_NODETYPE_CLEANMARKER);
	cleanmarker.totlen = cpu_to_je32(8);

	/* scrub option allows to erase badblock. To prevent internal
	 * check from erase() method, set block check method to dummy
	 * and disable bad block table while erasing.
	 */
	if (opts->scrub) {
		struct nand_chip *priv_nand = meminfo->priv;

		nand_block_bad_old = priv_nand->block_bad;
		priv_nand->block_bad = nand_block_bad_scrub;
		/* we don't need the bad block table anymore...
		 * after scrub, there are no bad blocks left!
		 */
		if (priv_nand->bbt) {
			kfree(priv_nand->bbt);
		}
		priv_nand->bbt = NULL;
	}

	if (erase_length < meminfo->erasesize) {
		printf("Warning: Erase size 0x%08lx smaller than one "	\
		       "erase block 0x%08x\n",erase_length, meminfo->erasesize);
		printf("         Erasing 0x%08x instead\n", meminfo->erasesize);
		erase_length = meminfo->erasesize;
	}

	for (;
	     erase.addr < opts->offset + erase_length;
	     erase.addr += meminfo->erasesize) {

		WATCHDOG_RESET ();

		if (!opts->scrub && bbtest) {
			int ret = meminfo->block_isbad(meminfo, erase.addr);
			if (ret > 0) {
				if (!opts->quiet)
					printf("\rSkipping bad block at  "
					       "0x%08x                   "
					       "                         \n",
					       erase.addr);
				continue;

			} else if (ret < 0) {
				printf("\n%s: MTD get bad block failed: %d\n",
				       mtd_device,
				       ret);
				return -1;
			}
		}

		result = meminfo->erase(meminfo, &erase);
		if (result != 0) {
			printf("\n%s: MTD Erase failure: %d\n",
			       mtd_device, result);
			continue;
		}

		/* format for JFFS2 ? */
		if (opts->jffs2 && chip->ecc.layout->oobavail >= 8) {
			chip->ops.ooblen = 8;
			chip->ops.datbuf = NULL;
			chip->ops.oobbuf = (uint8_t *)&cleanmarker;
			chip->ops.ooboffs = 0;
			chip->ops.mode = MTD_OOB_AUTO;

			result = meminfo->write_oob(meminfo,
			                            erase.addr,
			                            &chip->ops);
			if (result != 0) {
				printf("\n%s: MTD writeoob failure: %d\n",
				       mtd_device, result);
				continue;
			}
		}

		if (!opts->quiet) {
			unsigned long long n =(unsigned long long)
				(erase.addr + meminfo->erasesize - opts->offset)
				* 100;
			int percent;

			do_div(n, erase_length);
			percent = (int)n;

			/* output progress message only at whole percent
			 * steps to reduce the number of messages printed
			 * on (slow) serial consoles
			 */
			if (percent != percent_complete) {
				percent_complete = percent;

				printf("\rErasing at 0x%x -- %3d%% complete.",
				       erase.addr, percent);

				if (opts->jffs2 && result == 0)
					printf(" Cleanmarker written at 0x%x.",
					       erase.addr);
			}
		}
	}
	if (!opts->quiet)
		printf("\n");

	if (nand_block_bad_old) {
		struct nand_chip *priv_nand = meminfo->priv;

		priv_nand->block_bad = nand_block_bad_old;
		priv_nand->scan_bbt(meminfo);
	}

	return 0;
}

/* XXX U-BOOT XXX */

#define MAX_PAGE_SIZE	2048
#define MAX_OOB_SIZE	64

/*
 * buffer array used for writing data
 */
static unsigned char data_buf[MAX_PAGE_SIZE];
static unsigned char oob_buf[MAX_OOB_SIZE];

/* OOB layouts to pass into the kernel as default */
static struct nand_ecclayout none_ecclayout = {
	.eccbytes = 0,
};

static struct nand_ecclayout jffs2_ecclayout = {
	.eccbytes = 6,
	.eccpos = {0, 1, 2, 3, 6, 7},
	.oobfree = { {8, 8} },
	.oobavail = 8,
};

static struct nand_ecclayout yaffs_ecclayout = {
	.eccbytes = 6,
	.eccpos = { 8, 9, 10, 13, 14, 15},
	.oobfree = { {0, 4}, {6, 2}, {11, 2} },
	.oobavail = 8,
};

/* ecclayout on chips that page_size = 2K, byte 0,1 is bad block marker */
static struct nand_ecclayout yaffs2_ecclayout = {
	.eccbytes = 24,
	.eccpos = {
		40, 41, 42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 52, 53, 54, 55,
		56, 57, 58, 59, 60, 61, 62, 63},
	.oobfree = { {2, 38} },
	.oobavail = 38,
};

static struct nand_ecclayout autoplace_ecclayout = {
	.oobavail = 38,
};

/* XXX U-BOOT XXX */
#if 0
/******************************************************************************
 * Support for locking / unlocking operations of some NAND devices
 *****************************************************************************/

#define NAND_CMD_LOCK		0x2a
#define NAND_CMD_LOCK_TIGHT	0x2c
#define NAND_CMD_UNLOCK1	0x23
#define NAND_CMD_UNLOCK2	0x24
#define NAND_CMD_LOCK_STATUS	0x7a

/**
 * nand_lock: Set all pages of NAND flash chip to the LOCK or LOCK-TIGHT
 *	      state
 *
 * @param meminfo	nand mtd instance
 * @param tight		bring device in lock tight mode
 *
 * @return		0 on success, -1 in case of error
 *
 * The lock / lock-tight command only applies to the whole chip. To get some
 * parts of the chip lock and others unlocked use the following sequence:
 *
 * - Lock all pages of the chip using nand_lock(mtd, 0) (or the lockpre pin)
 * - Call nand_unlock() once for each consecutive area to be unlocked
 * - If desired: Bring the chip to the lock-tight state using nand_lock(mtd, 1)
 *
 *   If the device is in lock-tight state software can't change the
 *   current active lock/unlock state of all pages. nand_lock() / nand_unlock()
 *   calls will fail. It is only posible to leave lock-tight state by
 *   an hardware signal (low pulse on _WP pin) or by power down.
 */
int nand_lock(nand_info_t *meminfo, int tight)
{
	int ret = 0;
	int status;
	struct nand_chip *this = meminfo->priv;

	/* select the NAND device */
	this->select_chip(meminfo, 0);

	this->cmdfunc(meminfo,
		      (tight ? NAND_CMD_LOCK_TIGHT : NAND_CMD_LOCK),
		      -1, -1);

	/* call wait ready function */
	status = this->waitfunc(meminfo, this, FL_WRITING);

	/* see if device thinks it succeeded */
	if (status & 0x01) {
		ret = -1;
	}

	/* de-select the NAND device */
	this->select_chip(meminfo, -1);
	return ret;
}

/**
 * nand_get_lock_status: - query current lock state from one page of NAND
 *			   flash
 *
 * @param meminfo	nand mtd instance
 * @param offset	page address to query (muss be page aligned!)
 *
 * @return		-1 in case of error
 *			>0 lock status:
 *			  bitfield with the following combinations:
 *			  NAND_LOCK_STATUS_TIGHT: page in tight state
 *			  NAND_LOCK_STATUS_LOCK:  page locked
 *			  NAND_LOCK_STATUS_UNLOCK: page unlocked
 *
 */
int nand_get_lock_status(nand_info_t *meminfo, ulong offset)
{
	int ret = 0;
	int chipnr;
	int page;
	struct nand_chip *this = meminfo->priv;

	/* select the NAND device */
	chipnr = (int)(offset >> this->chip_shift);
	this->select_chip(meminfo, chipnr);


	if ((offset & (meminfo->writesize - 1)) != 0) {
		printf ("nand_get_lock_status: "
			"Start address must be beginning of "
			"nand page!\n");
		ret = -1;
		goto out;
	}

	/* check the Lock Status */
	page = (int)(offset >> this->page_shift);
	this->cmdfunc(meminfo, NAND_CMD_LOCK_STATUS, -1, page & this->pagemask);

	ret = this->read_byte(meminfo) & (NAND_LOCK_STATUS_TIGHT
					  | NAND_LOCK_STATUS_LOCK
					  | NAND_LOCK_STATUS_UNLOCK);

 out:
	/* de-select the NAND device */
	this->select_chip(meminfo, -1);
	return ret;
}

/**
 * nand_unlock: - Unlock area of NAND pages
 *		  only one consecutive area can be unlocked at one time!
 *
 * @param meminfo	nand mtd instance
 * @param start		start byte address
 * @param length	number of bytes to unlock (must be a multiple of
 *			page size nand->writesize)
 *
 * @return		0 on success, -1 in case of error
 */
int nand_unlock(nand_info_t *meminfo, ulong start, ulong length)
{
	int ret = 0;
	int chipnr;
	int status;
	int page;
	struct nand_chip *this = meminfo->priv;
	printf ("nand_unlock: start: %08x, length: %d!\n",
		(int)start, (int)length);

	/* select the NAND device */
	chipnr = (int)(start >> this->chip_shift);
	this->select_chip(meminfo, chipnr);

	/* check the WP bit */
	this->cmdfunc(meminfo, NAND_CMD_STATUS, -1, -1);
	if ((this->read_byte(meminfo) & 0x80) == 0) {
		printf ("nand_unlock: Device is write protected!\n");
		ret = -1;
		goto out;
	}

	if ((start & (meminfo->writesize - 1)) != 0) {
		printf ("nand_unlock: Start address must be beginning of "
			"nand page!\n");
		ret = -1;
		goto out;
	}

	if (length == 0 || (length & (meminfo->writesize - 1)) != 0) {
		printf ("nand_unlock: Length must be a multiple of nand page "
			"size!\n");
		ret = -1;
		goto out;
	}

	/* submit address of first page to unlock */
	page = (int)(start >> this->page_shift);
	this->cmdfunc(meminfo, NAND_CMD_UNLOCK1, -1, page & this->pagemask);

	/* submit ADDRESS of LAST page to unlock */
	page += (int)(length >> this->page_shift) - 1;
	this->cmdfunc(meminfo, NAND_CMD_UNLOCK2, -1, page & this->pagemask);

	/* call wait ready function */
	status = this->waitfunc(meminfo, this, FL_WRITING);
	/* see if device thinks it succeeded */
	if (status & 0x01) {
		/* there was an error */
		ret = -1;
		goto out;
	}

 out:
	/* de-select the NAND device */
	this->select_chip(meminfo, -1);
	return ret;
}
#endif

/**
 * get_len_incl_bad
 *
 * Check if length including bad blocks fits into device.
 *
 * @param nand NAND device
 * @param offset offset in flash
 * @param length image length
 * @return image length including bad blocks
 */
static size_t get_len_incl_bad (nand_info_t *nand, size_t offset,
				const size_t length)
{
	size_t len_incl_bad = 0;
	size_t len_excl_bad = 0;
	size_t block_len;

	while (len_excl_bad < length) {
		block_len = nand->erasesize - (offset & (nand->erasesize - 1));

		if (!nand_block_isbad (nand, offset & ~(nand->erasesize - 1)))
			len_excl_bad += block_len;

		len_incl_bad += block_len;
		offset       += block_len;

		if ((offset + len_incl_bad) >= nand->size)
			break;
	}

	return len_incl_bad;
}

/**
 * nand_write_skip_bad:
 *
 * Write image to NAND flash.
 * Blocks that are marked bad are skipped and then is written to the next
 * block instead as long as the image is short enough to fit even after
 * skipping the bad blocks.
 *
 * @param nand  	NAND device
 * @param offset	offset in flash
 * @param length	buffer length
 * @param buf           buffer to read from
 * @return		0 in case of success
 */
int nand_write_skip_bad(nand_info_t *nand, size_t offset, size_t *length,
			u_char *buffer)
{
	int rval;
	size_t left_to_write = *length;
	size_t len_incl_bad;
	u_char *p_buffer = buffer;

	/* Reject writes, which are not page aligned */
	if ((offset & (nand->writesize - 1)) != 0 ||
	    (*length & (nand->writesize - 1)) != 0) {
		printf ("Attempt to write non page aligned data\n");
		return -EINVAL;
	}

	len_incl_bad = get_len_incl_bad (nand, offset, *length);

	if ((offset + len_incl_bad) >= nand->size) {
		printf ("Attempt to write outside the flash area\n");
		return -EINVAL;
	}

	if (len_incl_bad == *length) {
		rval = nand_write (nand, offset, length, buffer);
		if (rval != 0)
			printf ("NAND write to offset %zx failed %d\n",
				offset, rval);

		return rval;
	}

	while (left_to_write > 0) {
		size_t block_offset = offset & (nand->erasesize - 1);
		size_t write_size;

		if (nand_block_isbad (nand, offset & ~(nand->erasesize - 1))) {
			printf ("Skip bad block 0x%08zx\n",
				offset & ~(nand->erasesize - 1));
			offset += nand->erasesize - block_offset;
			continue;
		}

		if (left_to_write < (nand->erasesize - block_offset))
			write_size = left_to_write;
		else
			write_size = nand->erasesize - block_offset;

		rval = nand_write (nand, offset, &write_size, p_buffer);
		if (rval != 0) {
			printf ("NAND write to offset %zx failed %d\n",
				offset, rval);
			*length -= left_to_write;
			return rval;
		}

		left_to_write -= write_size;
		offset        += write_size;
		p_buffer      += write_size;
	}

	return 0;
}

/**
 * nand_write_opts: - write image to NAND flash with support for various options
 *
 * @param meminfo	NAND device to erase
 * @param opts		write options (@see nand_write_options)
 * @return		0 in case of success
 *
 * This code is ported from nandwrite.c from Linux mtd utils by
 * Steven J. Hill and Thomas Gleixner.
 */
int nand_write_opts(nand_info_t *meminfo, const nand_write_options_t *opts)
{
	int imglen = 0;
	int pagelen;
	int baderaseblock;
	int blockstart = -1;
	loff_t offs;
	int readlen;
	int ecclayout_changed = 0;
	int percent_complete = -1;
	struct nand_ecclayout *old_ecclayout;
	struct nand_chip *chip = meminfo->priv;
	ulong mtdoffset = opts->offset;
	ulong erasesize_blockalign;
	u_char *buffer = opts->buffer;
	size_t written;
	int result;

	if (opts->pad && opts->writeoob) {
		printf("Can't pad when oob data is present.\n");
		return -1;
	}

	/* set erasesize to specified number of blocks - to match
	 * jffs2 (virtual) block size */
	if (opts->blockalign == 0)
		erasesize_blockalign = meminfo->erasesize;
	else
		erasesize_blockalign = meminfo->erasesize * opts->blockalign;

	/* make sure device page sizes are valid */
	if (!(meminfo->oobsize == 16 && meminfo->writesize == 512)
	    && !(meminfo->oobsize == 8 && meminfo->writesize == 256)
	    && !(meminfo->oobsize == 64 && meminfo->writesize == 2048)) {
		printf("Unknown flash (not normal NAND)\n");
		return -1;
	}

	/* read the current ecclayout */
	old_ecclayout =  meminfo->ecclayout;

	/* write without ecc? */
	if (opts->noecc) {
		meminfo->ecclayout = &none_ecclayout;
		ecclayout_changed = 1;
	}

	/* autoplace ECC? */
	if (opts->autoplace && (chip->ecc.mode != MTD_NANDECC_AUTOPLACE)) {

		meminfo->ecclayout = &autoplace_ecclayout,
		ecclayout_changed = 1;
	}

	/* force OOB layout for jffs2 or yaffs? */
	if (opts->forcejffs2 || opts->forceyaffs) {
		struct nand_ecclayout *oobsel =
			opts->forcejffs2 ? &jffs2_ecclayout : &yaffs2_ecclayout;

		if (meminfo->oobsize == 8) {
			if (opts->forceyaffs) {
				printf("YAFSS cannot operate on "
				       "256 Byte page size\n");
				goto restoreoob;
			}
		}

		meminfo->ecclayout = oobsel;
		ecclayout_changed = 1;
	}

	/* get image length */
	imglen = opts->length;
	pagelen = meminfo->writesize
		+ ((opts->writeoob != 0) ? meminfo->oobsize : 0);

	/* check, if file is pagealigned */
	if ((!opts->pad) && ((imglen % pagelen) != 0)) {
		printf("Input block length is not page aligned\n");
		goto restoreoob;
	}

	/* check, if length fits into device */
	if (((imglen / pagelen) * meminfo->writesize)
	     > (meminfo->size - opts->offset)) {
		printf("Image %d bytes, NAND page %d bytes, "
		       "OOB area %u bytes, device size %u bytes\n",
		       imglen, pagelen, meminfo->writesize, meminfo->size);
		printf("Input block does not fit into device\n");
		goto restoreoob;
	}

	if (!opts->quiet)
		printf("\n");

	/* get data from input and write to the device */
	while (imglen && (mtdoffset < meminfo->size)) {

		WATCHDOG_RESET();

		/*
		 * new eraseblock, check for bad block(s). Stay in the
		 * loop to be sure if the offset changes because of
		 * a bad block, that the next block that will be
		 * written to is also checked. Thus avoiding errors if
		 * the block(s) after the skipped block(s) is also bad
		 * (number of blocks depending on the blockalign
		 */
		while (blockstart != (mtdoffset & (~erasesize_blockalign+1))) {
			blockstart = mtdoffset & (~erasesize_blockalign+1);
			offs = blockstart;
			baderaseblock = 0;

			/* check all the blocks in an erase block for
			 * bad blocks */
			do {
				int ret = meminfo->block_isbad(meminfo, offs);

				if (ret < 0) {
					printf("Bad block check failed\n");
					goto restoreoob;
				}
				if (ret == 1) {
					baderaseblock = 1;
					if (!opts->quiet)
						printf("\rBad block at 0x%lx "
						       "in erase block from "
						       "0x%x will be skipped\n",
						       (long) offs,
						       blockstart);
				}

				if (baderaseblock) {
					mtdoffset = blockstart
						+ erasesize_blockalign;
				}
				offs +=	 erasesize_blockalign
					/ opts->blockalign;
			} while (offs < blockstart + erasesize_blockalign);
		}

		readlen = meminfo->writesize;
		if (opts->pad && (imglen < readlen)) {
			readlen = imglen;
			memset(data_buf + readlen, 0xff,
			       meminfo->writesize - readlen);
		}

		/* read page data from input memory buffer */
		memcpy(data_buf, buffer, readlen);
		buffer += readlen;

		if (opts->writeoob) {
			/* read OOB data from input memory block, exit
			 * on failure */
			memcpy(oob_buf, buffer, meminfo->oobsize);
			buffer += meminfo->oobsize;

			struct mtd_oob_ops ops = {
				.mode = MTD_OOB_AUTO,
				.len = meminfo->writesize,
				.retlen = 0,
				.ooblen = meminfo->ecclayout->oobavail,
				.oobretlen = 0,
				.ooboffs = 0,
				.datbuf = NULL,
				.oobbuf = oob_buf,
			};
			result = meminfo->write_oob(meminfo, mtdoffset, &ops);

			if (result != 0) {
				printf("\nMTD writeoob failure: %d\n",
				       result);
				goto restoreoob;
			}
			imglen -= meminfo->oobsize;
		}

		/* write out the page data */
		result = meminfo->write(meminfo,
					mtdoffset,
					meminfo->writesize,
					&written,
					(unsigned char *) &data_buf);

		if (result != 0) {
			printf("writing NAND page at offset 0x%lx failed\n",
			       mtdoffset);
			goto restoreoob;
		}
		imglen -= readlen;

		if (!opts->quiet) {
			int percent = (int)
				((unsigned long long)
				 (opts->length-imglen) * 100
				 / opts->length);
			/* output progress message only at whole percent
			 * steps to reduce the number of messages printed
			 * on (slow) serial consoles
			 */
			if (percent != percent_complete) {
				printf("\rWriting data at 0x%x "
				       "-- %3d%% complete.",
				       mtdoffset, percent);
				percent_complete = percent;
			}
		}

		mtdoffset += meminfo->writesize;
	}

	if (!opts->quiet)
		printf("\n");

restoreoob:
	if (ecclayout_changed)
		meminfo->ecclayout = old_ecclayout;

	if (imglen > 0) {
		printf("Data did not fit into device, due to bad blocks\n");
		return -1;
	}

	/* return happy */
	return 0;
}

/**
 * nand_read_skip_bad:
 *
 * Read image from NAND flash.
 * Blocks that are marked bad are skipped and the next block is readen
 * instead as long as the image is short enough to fit even after skipping the
 * bad blocks.
 *
 * @param nand NAND device
 * @param offset offset in flash
 * @param length buffer length, on return holds remaining bytes to read
 * @param buffer buffer to write to
 * @return 0 in case of success
 */
int nand_read_skip_bad(nand_info_t *nand, size_t offset, size_t *length,
		       u_char *buffer)
{
	int rval;
	size_t left_to_read = *length;
	size_t len_incl_bad;
	u_char *p_buffer = buffer;

	len_incl_bad = get_len_incl_bad (nand, offset, *length);

	if ((offset + len_incl_bad) >= nand->size) {
		printf ("Attempt to read outside the flash area\n");
		return -EINVAL;
	}

	if (len_incl_bad == *length) {
		rval = nand_read (nand, offset, length, buffer);
		if (rval != 0)
			printf ("NAND read from offset %zx failed %d\n",
				offset, rval);

		return rval;
	}

	while (left_to_read > 0) {
		size_t block_offset = offset & (nand->erasesize - 1);
		size_t read_length;

		if (nand_block_isbad (nand, offset & ~(nand->erasesize - 1))) {
			printf ("Skipping bad block 0x%08zx\n",
				offset & ~(nand->erasesize - 1));
			offset += nand->erasesize - block_offset;
			continue;
		}

		if (left_to_read < (nand->erasesize - block_offset))
			read_length = left_to_read;
		else
			read_length = nand->erasesize - block_offset;

		rval = nand_read (nand, offset, &read_length, p_buffer);
		if (rval != 0) {
			printf ("NAND read from offset %zx failed %d\n",
				offset, rval);
			*length -= left_to_read;
			return rval;
		}

		left_to_read -= read_length;
		offset       += read_length;
		p_buffer     += read_length;
	}

	return 0;
}
