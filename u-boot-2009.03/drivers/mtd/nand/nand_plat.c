/*
 * Copyright (c) 2006-2009 Analog Devices Inc.
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

#include <nand.h>

#ifdef NAND_PLAT_WRITE_CMD
static void cmd_ctrl(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct nand_chip *this = mtd->priv;

	if (cmd == NAND_CMD_NONE)
		return;

	if (ctrl & NAND_CLE)
		NAND_PLAT_WRITE_CMD(cmd, this);
	else
		NAND_PLAT_WRITE_ADR(cmd, this);

	/* Drain the writebuffer */
	sync();
}
#else
# define cmd_ctrl NULL
#endif

#ifdef NAND_PLAT_DEV_READY
static int dev_ready(struct mtd_info *mtd)
{
	return NAND_PLAT_DEV_READY((struct nand_chip *)mtd->priv);
}
#else
# define dev_ready NULL
#endif

int board_nand_init(struct nand_chip *nand)
{
	NAND_PLAT_INIT();

	nand->cmd_ctrl = cmd_ctrl;
	nand->dev_ready = dev_ready;
	nand->ecc.mode = NAND_ECC_SOFT;

	return 0;
}
