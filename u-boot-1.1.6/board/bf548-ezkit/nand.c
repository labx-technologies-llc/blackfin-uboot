/*
 * (C) Copyright 2006 Aubrey.Li, aubrey.li@analog.com
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
#include <asm/io.h>

#if (CONFIG_COMMANDS & CFG_CMD_NAND)

#include <nand.h>

#include <asm/blackfin.h>

/* Bit masks for NFC_CTL */

#define                    WR_DLY  0xf        /* Write Strobe Delay */
#define                    RD_DLY  0xf0       /* Read Strobe Delay */
#define                    NWIDTH  0x100      /* NAND Data Width */
#define                   PG_SIZE  0x200      /* Page Size */

/* Bit masks for NFC_STAT */

#define                     NBUSY  0x1        /* Not Busy */
#define                   WB_FULL  0x2        /* Write Buffer Full */
#define                PG_WR_STAT  0x4        /* Page Write Pending */
#define                PG_RD_STAT  0x8        /* Page Read Pending */
#define                  WB_EMPTY  0x10       /* Write Buffer Empty */

/* Bit masks for NFC_IRQSTAT */

#define                  NBUSYIRQ  0x1        /* Not Busy IRQ */
#define                    WB_OVF  0x2        /* Write Buffer Overflow */
#define                   WB_EDGE  0x4        /* Write Buffer Edge Detect */
#define                    RD_RDY  0x8        /* Read Data Ready */
#define                   WR_DONE  0x10       /* Page Write Done */

static int bf54x_nand_CLE_ALE_flag;
#define CLE_ALE_NONE	0x0
#define BF54X_CLE	0x01
#define BF54X_ALE	0x02

/*
 * hardware specific access to control-lines
 */
static void bf54x_nand_hwcontrol(struct mtd_info *mtd, int cmd)
{
	switch (cmd) {

	case NAND_CTL_SETCLE:
		bf54x_nand_CLE_ALE_flag = BF54X_CLE;
		break;
	case NAND_CTL_CLRCLE:
		bf54x_nand_CLE_ALE_flag = CLE_ALE_NONE;
		break;

	case NAND_CTL_SETALE:
		bf54x_nand_CLE_ALE_flag = BF54X_ALE;
		break;
	case NAND_CTL_CLRALE:
		bf54x_nand_CLE_ALE_flag = CLE_ALE_NONE;
		break;
	case NAND_CTL_SETNCE:
	case NAND_CTL_CLRNCE:
		break;
	}

}

/*
 * bf54x_nand_write_byte
 *
 * Issue command and address cycles to the chip
 */
static void bf54x_nand_write_byte(struct mtd_info *mtd, u_char byte)
{
	while (bfin_read_NFC_STAT() & WB_FULL)
		continue;

	if (bf54x_nand_CLE_ALE_flag == BF54X_CLE)
		bfin_write_NFC_CMD(byte);
	else if (bf54x_nand_CLE_ALE_flag == BF54X_ALE)
		bfin_write_NFC_ADDR(byte);
}

int bf54x_nand_devready(struct mtd_info *mtd)
{
	unsigned short val = bfin_read_NFC_IRQSTAT();

	if ((val & NBUSYIRQ) == NBUSYIRQ)
		return 1;
	else
		return 0;
}

/*----------------------------------------------------------------------------
   * PIO mode for buffer writing and reading
*/
static void bf54x_nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	int i;
	unsigned short val;

	/*
	 * Data reads are requested by first writing to NFC_DATA_RD
	* and then reading back from NFC_READ.
	*/
	for (i = 0; i < len; i++) {
		while (bfin_read_NFC_STAT() & WB_FULL)
			continue;

		/* Contents do not matter */
		bfin_write_NFC_DATA_RD(0x0000);

		while ((bfin_read_NFC_IRQSTAT() & RD_RDY) != RD_RDY)
			continue;

		buf[i] = bfin_read_NFC_READ();

		val = bfin_read_NFC_IRQSTAT();
		val |= RD_RDY;
		bfin_write_NFC_IRQSTAT(val);
	}
}

static uint8_t bf54x_nand_read_byte(struct mtd_info *mtd)
{
	uint8_t val;

	bf54x_nand_read_buf(mtd, &val, 1);

	return val;
}

static void bf54x_nand_write_buf(struct mtd_info *mtd,
	const uint8_t *buf, int len)
{
	int i;

	for (i = 0; i < len; i++) {
		while (bfin_read_NFC_STAT() & WB_FULL)
			continue;

		bfin_write_NFC_DATA_WR(buf[i]);
	}
}

/*
 * Board-specific NAND initialization. The following members of the
 * argument are board-specific (per include/linux/mtd/nand.h):
 * - IO_ADDR_R?: address to read the 8 I/O lines of the flash device
 * - IO_ADDR_W?: address to write the 8 I/O lines of the flash device
 * - hwcontrol: hardwarespecific function for accesing control-lines
 * - dev_ready: hardwarespecific function for  accesing device ready/busy line
 * - enable_hwecc?: function to enable (reset)  hardware ecc generator. Must
 *   only be provided if a hardware ECC is available
 * - eccmode: mode of ecc, see defines
 * - chip_delay: chip dependent delay for transfering data from array to
 *   read regs (tR)
 * - options: various chip options. They can partly be set to inform
 *   nand_scan about special functionality. See the defines for further
 *   explanation
 * Members with a "?" were not set in the merged testing-NAND branch,
 * so they are not set here either.
 */
void board_nand_init(struct nand_chip *nand)
{
	unsigned short val;

	val = bfin_read_NFC_CTL();
	val |= 0x33; /* WR_DLY = 3, RD_DLY = 3*/
	bfin_write_NFC_CTL(val);

	/* clear interrupt status */
	bfin_write_NFC_IRQMASK(0x0);
	val = bfin_read_NFC_IRQSTAT();
	bfin_write_NFC_IRQSTAT(val);

	/* enable GPIO function enable register */
	val = bfin_read_PORTJ_FER();
	val |= 6;
	bfin_write_PORTJ_FER(val);

	nand->hwcontrol = bf54x_nand_hwcontrol;
	nand->read_buf = bf54x_nand_read_buf;
	nand->write_buf = bf54x_nand_write_buf;
	nand->read_byte = bf54x_nand_read_byte;
	nand->write_byte = bf54x_nand_write_byte;

	nand->eccmode = NAND_ECC_SOFT;
	nand->dev_ready = bf54x_nand_devready;
	nand->chip_delay = 0;
}
#endif				/* (CONFIG_COMMANDS & CFG_CMD_NAND) */
