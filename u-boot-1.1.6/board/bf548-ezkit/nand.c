/*
 * Driver for Blackfin on-chip NAND controller.
 *
 * Enter bugs at http://blackfin.uclinux.org/
 *
 * Copyright (c) 2007-2008 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

/* TODO:
 * - move bit defines into mach-common/bits/nand.h
 * - try and replace all IRQSTAT usage with STAT polling
 * - have software ecc mode use same algo as hw ecc ?
 */

#include <common.h>
#include <asm/io.h>

#if (CONFIG_COMMANDS & CFG_CMD_NAND)

#ifdef DEBUG
# define pr_stamp() printf("%s:%s:%i: here i am\n", __FILE__, __func__, __LINE__)
#else
# define pr_stamp()
#endif

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

#define NAND_IS_512() (CFG_BFIN_NFC_CTL_VAL & 0x200)

static int CLE_ALE_flag;
#define CLE_ALE_NONE	0x0
#define CLE	0x01
#define ALE	0x02

/*
 * hardware specific access to control-lines
 */
static void bfin_nfc_hwcontrol(struct mtd_info *mtd, int cmd)
{
	pr_stamp();

	switch (cmd) {
	case NAND_CTL_SETCLE:
		CLE_ALE_flag = CLE;
		break;
	case NAND_CTL_CLRCLE:
		CLE_ALE_flag = CLE_ALE_NONE;
		break;

	case NAND_CTL_SETALE:
		CLE_ALE_flag = ALE;
		break;
	case NAND_CTL_CLRALE:
		CLE_ALE_flag = CLE_ALE_NONE;
		break;
	case NAND_CTL_SETNCE:
	case NAND_CTL_CLRNCE:
		break;
	}
}

/*
 * bfin_nfc_write_byte
 *
 * Issue command and address cycles to the chip
 */
static void bfin_nfc_write_byte(struct mtd_info *mtd, u_char byte)
{
	pr_stamp();

	while (bfin_read_NFC_STAT() & WB_FULL)
		if (ctrlc())
			return;

	if (CLE_ALE_flag == CLE)
		bfin_write_NFC_CMD(byte);
	else if (CLE_ALE_flag == ALE)
		bfin_write_NFC_ADDR(byte);
}

int bfin_nfc_devready(struct mtd_info *mtd)
{
	pr_stamp();
	return (bfin_read_NFC_STAT() & NBUSY ? 1 : 0);
}

/*
 * PIO mode for buffer writing and reading
 */
static void bfin_nfc_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	pr_stamp();

	int i;

	/*
	 * Data reads are requested by first writing to NFC_DATA_RD
	* and then reading back from NFC_READ.
	*/
	for (i = 0; i < len; ++i) {
		while (bfin_read_NFC_STAT() & WB_FULL)
			if (ctrlc())
				return;

		/* Contents do not matter */
		bfin_write_NFC_DATA_RD(0x0000);

		while (!(bfin_read_NFC_IRQSTAT() & RD_RDY))
			if (ctrlc())
				return;

		buf[i] = bfin_read_NFC_READ();

		bfin_write_NFC_IRQSTAT(RD_RDY);
	}
}

static uint8_t bfin_nfc_read_byte(struct mtd_info *mtd)
{
	pr_stamp();

	uint8_t val;
	bfin_nfc_read_buf(mtd, &val, 1);
	return val;
}

static void bfin_nfc_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	pr_stamp();

	int i;

	for (i = 0; i < len; ++i) {
		while (bfin_read_NFC_STAT() & WB_FULL)
			if (ctrlc())
				return;

		bfin_write_NFC_DATA_WR(buf[i]);
	}
}

/*
 * ECC functions
 * These allow the bfin to use the controller's ECC
 * generator block to ECC the data as it passes through
 */

/*
 * ECC error correction function
 */
static int bfin_nfc_correct_data_256(struct mtd_info *mtd, u_char *dat,
					u_char *read_ecc, u_char *calc_ecc)
{
	u32 syndrome[5];
	u32 calced, stored;
	unsigned short failing_bit, failing_byte;
	u_char data;

	pr_stamp();

	calced = calc_ecc[0] | (calc_ecc[1] << 8) | (calc_ecc[2] << 16);
	stored = read_ecc[0] | (read_ecc[1] << 8) | (read_ecc[2] << 16);

	syndrome[0] = (calced ^ stored);

	/*
	 * syndrome 0: all zero
	 * No error in data
	 * No action
	 */
	if (!syndrome[0] || !calced || !stored)
		return 0;

	/*
	 * sysdrome 0: only one bit is one
	 * ECC data was incorrect
	 * No action
	 */
	if (hweight32(syndrome[0]) == 1)
		return 1;

	syndrome[1] = (calced & 0x7FF) ^ (stored & 0x7FF);
	syndrome[2] = (calced & 0x7FF) ^ ((calced >> 11) & 0x7FF);
	syndrome[3] = (stored & 0x7FF) ^ ((stored >> 11) & 0x7FF);
	syndrome[4] = syndrome[2] ^ syndrome[3];

	/*
	 * sysdrome 0: exactly 11 bits are one, each parity
	 * and parity' pair is 1 & 0 or 0 & 1.
	 * 1-bit correctable error
	 * Correct the error
	 */
	if (hweight32(syndrome[0]) == 11 && syndrome[4] == 0x7FF) {
		failing_bit = syndrome[1] & 0x7;
		failing_byte = syndrome[1] >> 0x3;
		data = *(dat + failing_byte);
		data = data ^ (0x1 << failing_bit);
		*(dat + failing_byte) = data;

		return 0;
	}

	/*
	 * sysdrome 0: random data
	 * More than 1-bit error, non-correctable error
	 * Discard data, mark bad block
	 */

	return 1;
}

static int bfin_nfc_correct_data(struct mtd_info *mtd, u_char *dat,
					u_char *read_ecc, u_char *calc_ecc)
{
	int ret;

	pr_stamp();

	ret = bfin_nfc_correct_data_256(mtd, dat, read_ecc, calc_ecc);

	/* If page size is 512, correct second 256 bytes */
	if (NAND_IS_512()) {
		dat += 256;
		read_ecc += 8;
		calc_ecc += 8;
		ret |= bfin_nfc_correct_data_256(mtd, dat, read_ecc, calc_ecc);
	}

	return ret;
}

static void reset_ecc(void)
{
	bfin_write_NFC_RST(0x1);
	while (bfin_read_NFC_RST() & 1)
		continue;
}

static void bfin_nfc_enable_hwecc(struct mtd_info *mtd, int mode)
{
	reset_ecc();
}

static int bfin_nfc_calculate_ecc(struct mtd_info *mtd,
		const u_char *dat, u_char *ecc_code)
{
	u16 ecc0, ecc1;
	u32 code[2];
	u8 *p;

	pr_stamp();

	/* first 4 bytes ECC code for 256 page size */
	ecc0 = bfin_read_NFC_ECC0();
	ecc1 = bfin_read_NFC_ECC1();

	code[0] = (ecc0 & 0x7FF) | ((ecc1 & 0x7FF) << 11);

	/* first 3 bytes in ecc_code for 256 page size */
	p = (u8 *) code;
	memcpy(ecc_code, p, 3);

	/* second 4 bytes ECC code for 512 page size */
	if (NAND_IS_512()) {
		ecc0 = bfin_read_NFC_ECC2();
		ecc1 = bfin_read_NFC_ECC3();
		code[1] = (ecc0 & 0x7FF) | ((ecc1 & 0x7FF) << 11);

		/* second 3 bytes in ecc_code for second 256
		 * bytes of 512 page size
		 */
		p = (u8 *) (code + 1);
		memcpy((ecc_code + 3), p, 3);
	}

	reset_ecc();

	return 0;
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
	pr_stamp();

	/* set width/ecc/timings/etc... */
	bfin_write_NFC_CTL(CFG_BFIN_NFC_CTL_VAL);

	/* clear interrupt status */
	bfin_write_NFC_IRQMASK(0x0);
	bfin_write_NFC_IRQSTAT(0xffff);

	/* enable GPIO function enable register */
#ifdef __ADSPBF54x__
	bfin_write_PORTJ_FER(bfin_read_PORTJ_FER() | 6);
#elif defined(__ADSPBF52x__)
	bfin_write_PORTH_FER(bfin_read_PORTH_FER() | 0xFCFF);
	bfin_write_PORTH_MUX(0);
#else
# error no support for this variant
#endif

	nand->hwcontrol = bfin_nfc_hwcontrol;
	nand->read_buf = bfin_nfc_read_buf;
	nand->write_buf = bfin_nfc_write_buf;
	nand->read_byte = bfin_nfc_read_byte;
	nand->write_byte = bfin_nfc_write_byte;

#ifdef CFG_BFIN_NFC_NO_HW_ECC
# define ECC_HW 0
#else
# define ECC_HW 1
#endif
	if (ECC_HW) {
		nand->eccmode = (NAND_IS_512() ? NAND_ECC_HW6_512 : NAND_ECC_HW3_256);
		nand->calculate_ecc = bfin_nfc_calculate_ecc;
		nand->correct_data = bfin_nfc_correct_data;
		nand->enable_hwecc = bfin_nfc_enable_hwecc;
	} else
		nand->eccmode = NAND_ECC_SOFT;
	nand->dev_ready = bfin_nfc_devready;
	nand->chip_delay = 0;
}

#endif
