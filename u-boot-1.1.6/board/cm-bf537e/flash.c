/*
 * U-boot - flash.c Flash driver for MT28F320J3FS-11
 *

 * Copyright (c) 2005 blackfin.uclinux.org
 * This file is based on BF533EzFlash.c originally written by Analog Devices, Inc.
 *
 * (C) Copyright 2000-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * based on EZ-KIT flash driver
 * Edited by Thomas Tamandl, thomas.tamandl@bluetechnix.at
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
#include <command.h>

#include <asm/blackfin.h>
#include "flash-defines.h"

static int write_flash(long nOffset, unsigned short nValue);
static int erase_block_flash(int nBlock, unsigned long address);

#define GPIO_4 (1<<4)

#define SWITCH_BANK do { if (memIndex >= 0x20200000) {\
			  *pPORTFIO_SET = GPIO_4; \
			  memIndex -= 0x200000; \
			  crossed = 1;\
			} else { \
			  *pPORTFIO_CLEAR = GPIO_4; \
			} \
			asm("ssync;"); } while (0)

#define SWITCH_BACK do {if (crossed) {\
			*pPORTFIO_CLEAR = GPIO_4; \
			memIndex += 0x200000; \
			crossed = 0;\
			asm("ssync;");\
			} } while (0)

unsigned long flash_init(void)
{
	int i = 0;

	if (CFG_MAX_FLASH_BANKS > 1)
		printf("Only FLASH bank 0 will be used!");
	flash_info[0].flash_id = FLASH_UNKNOWN;
	if (get_codes() == MT_MANUFACT_CM_BF533)
		flash_info[0].flash_id = MT_MANUFACT_CM_BF533;
	printf("Device ID of the Flash is %x\n", flash_info[0].flash_id);
	for (i = 0; i < CFG_MAX_FLASH_SECT; i++)
		flash_info[0].start[i] =
		    (CFG_FLASH_BASE + (i * FLASH_SECTOR_SIZE));
	flash_info[0].size = CFG_FLASH_SIZE;
	flash_info[0].sector_count = FLASH_TOT_SECT;
	if (flash_info[0].flash_id == FLASH_UNKNOWN
	    && CFG_FLASH_SIZE == 0x200000) {
		printf("## Unknown FLASH on Bank 0\n");
	} else {
		printf("Flash Memory Start 0x%x\n", CFG_FLASH_BASE);
		printf("Memory Map for the Flash\n");
		printf("0x20000000 - 0x203FFFFF Single Flash Chip (4MB)\n");
		printf("Using PF4 as a 2M bank switch\n");
		printf
		    ("Please type command flinfo for information on Sectors \n");
		*pPORTF_FER &= ~GPIO_4;
		*pPORTFIO_DIR |= GPIO_4;
		*pPORTFIO_CLEAR = GPIO_4;
		asm("ssync;");
	}
	return (CFG_FLASH_SIZE);
}

void flash_print_info(flash_info_t *info)
{
	int i;

	if (info->flash_id == FLASH_UNKNOWN) {
		printf("missing or unknown FLASH type\n");
		return;
	}

	switch (info->flash_id) {
	case MT_MANUFACT_CM_BF533:
		printf("MT_MANUFACT_on_CM-BF537");
		break;
	default:
		printf("Unknown Vendor ");
		break;
	}
	for (i = 0; i < info->sector_count; ++i) {
		if ((i % 5) == 0)
			printf("\n   ");
		printf(" %08lX%s",
		       info->start[i], info->protect[i] ? " (RO)" : "     ");
	}
	printf("\n");
	return;
}

int flash_erase(flash_info_t *info, int s_first, int s_last)
{
	int prot, sect, i;

	prot = 0;
	for (sect = s_first; sect <= s_last; ++sect) {
		if (info->protect[sect])
			prot++;
	}
	if (prot)
		printf("- Warning: %d protected sectors will not be erased!\n",
		       prot);
	else
		printf("\n");
	printf("Erasing Flash locations, Please Wait\n");
	for (i = s_first; i <= s_last; i++) {
		if (info->protect[i] == 0) {	/* not protected */
			if (erase_block_flash(i, info->start[i]) < 0) {
				printf("Error Sector erasing \n");
				return ERR_INVAL;
			} else {
				printf("Sector %i erased.\n", i);
			}
		}
	}
	return ERR_OK;
}

int write_buff(flash_info_t *info, unsigned char *src, ulong addr, ulong cnt)
{
	long i = 0;
	unsigned long ulOffset = addr - CFG_FLASH_BASE;
	int nLeftover = cnt % 4;
	int iSectors = 0;
	int iFirst_sector = 0;
	int iProgress = cnt / (4 * 20);

	iFirst_sector = (int)(ulOffset / FLASH_SECTOR_SIZE);
	for (iSectors = 1; (iSectors * FLASH_SECTOR_SIZE) < cnt; iSectors++) ;

	printf("Bytes for programming: %li\n", cnt);
	printf("First sector: %d\n", iFirst_sector);
	printf("Sectors needed:%d\n", iSectors);

	if (iFirst_sector + iSectors > FLASH_TOT_SECT) {
		printf("Not enough free flash-sectors!\n");
		return ERR_INVAL;
	}

	for (i = iFirst_sector; i < (iFirst_sector + iSectors); i++) {
		if (check_sector(i) == ERR_NOT_ERASED)
			printf("Sector %d not erased \n", i);
		/* return ERR_NOT_ERASED; */
	}

	printf("[                     ]\n[");

	for (i = 0; (i < cnt / 4); i++) {
		if ((i % iProgress) == 0)
			printf(".");

		if (write_flash(ulOffset, (((int *)src)[i])) < 0) {
			printf("Error programming the flash \n");
			return ERR_TIMOUT;
		}
		ulOffset += 2;
		if (write_flash(ulOffset, (((int *)src)[i] >> 16)) < 0) {
			printf("Error programming the flash \n");
			return ERR_TIMOUT;
		}
		ulOffset += 2;
	}
	if (nLeftover > 0) {
		if (write_flash(ulOffset, ((int *)src)[i]) < 0) {
			printf("Error programming flash \n");
			return ERR_TIMOUT;
		}
	}
	printf("]\n");
	return ERR_OK;
}

/* assumes all bytes to be within a single bank */
static int read_flash_chunk(char *addr, int size, char *dest)
{
	unsigned long memIndex;
	int crossed = 0;

	memIndex = (unsigned long)addr;
	SWITCH_BANK;
	memcpy((void *)dest, (void *)memIndex, size);
	SWITCH_BACK;
	return size;
}


/* handles bank switching (we hope) */

int read_flash(char *addr, int size, char *dest)
{
	int chunk;
	int done;
	chunk = size;
	done = 0;
	if (addr < (char *)0x20200000) {
		if ((addr + size) > (char *)0x20200000) {
			chunk = (int)((char *)0x20200000 - addr);
			done = read_flash_chunk(addr, chunk, dest);
			addr += chunk;
			dest += chunk;
			chunk = size - chunk;
		}
	}
	done += read_flash_chunk(addr, chunk, dest);
	return done;
}

static int check_sector(unsigned short usSector)
{
	long i = 0;
	int ret;
	unsigned short iData;
	unsigned long memIndex;
	int crossed = 0;

	printf("Checking sector %d", usSector);
	memIndex = (unsigned long)(CFG_FLASH_BASE +
				   (usSector * FLASH_SECTOR_SIZE));
	printf("\nmemIndex 1  %x crossed %d\n", memIndex, crossed);

	SWITCH_BANK;
	printf("\nmemIndex 2  %x crossed %d\n", memIndex, crossed);

	ret = ERR_OK;

	for (i = 0; i < FLASH_SECTOR_SIZE; i += 2) {

		iData = *(volatile unsigned short *)memIndex;
		if (iData != 0xFFFF) {
			ret = ERR_NOT_ERASED;
			break;
		}
		memIndex += 2;
	}

	SWITCH_BACK;

	if (ret == ERR_OK) {
		printf(" ... sector empty\n");
	} else {
		printf(" ... not empty!\n");
	}
	return ret;
}

static int write_flash(long nOffset, unsigned short nValue)
{

	unsigned long memIndex;
	unsigned long nTimeout;
	unsigned short status;
	int crossed = 0;

	memIndex = (unsigned long)(CFG_FLASH_BASE + nOffset);
	SWITCH_BANK;

	*(volatile unsigned short *)memIndex = 0x0050;	/* Reset status register */
	asm("ssync;");

	*(volatile unsigned short *)memIndex = 0x0040;
	asm("ssync;");

	*(volatile unsigned short *)memIndex = (unsigned short)nValue;
	asm("ssync;");

	nTimeout = 0;
	do {
		status = *(volatile unsigned short *)memIndex;
		nTimeout++;
		asm("ssync;");
	}
	while (((status & 0x0080) == 0x00) && (nTimeout < FLASH_TIMEOUT));
	if (nTimeout >= FLASH_TIMEOUT)
		status = 0xff00;

	nTimeout = FLASH_TIMEOUT;
	do {
		*(volatile unsigned short *)memIndex = 0xff;	/* Back to "read array mode". */
		asm("ssync;");
		nTimeout--;
	} while ((*(volatile unsigned short *)memIndex
		  != (unsigned short)nValue) && (nTimeout > 0));
	if (!nTimeout)
		status = 0xee00;

	SWITCH_BACK;

	if (status == 0x0080)
		return (ERR_OK);
	else
		return (ERR_TIMOUT);
}

static int erase_block_flash(int nBlock, unsigned long address)
{
	unsigned long memIndex;
	unsigned short status;
	unsigned long nTimeout;
	int crossed = 0;

	if ((nBlock < 0) || (nBlock >= CFG_MAX_FLASH_SECT)) {
		printf("Invalid sector number\n");
		return -1;
	}

	memIndex = (unsigned long)(address);	/* + CFG_FLASH_BASE); */

	SWITCH_BANK;

	*(volatile unsigned short *)memIndex = 0x0050;
	asm("ssync;");

	*(volatile unsigned short *)memIndex = 0x0020;
	asm("ssync;");

	*(volatile unsigned short *)memIndex = 0x00d0;
	asm("ssync;");

	nTimeout = 0;
	do {
		status = *(volatile unsigned short *)memIndex;
		asm("ssync;");
		nTimeout++;
	}
	while (((status & 0x0080) == 0x00) && (nTimeout < FLASH_TIMEOUT));
	if (nTimeout >= FLASH_TIMEOUT)
		status = 0xff00;

	*(volatile unsigned short *)memIndex = 0xff;	/* Back to "Read Array Mode" */
	asm("ssync;");

	SWITCH_BACK;

	if (status == 0xFF00)
		return -1;
	else
		return ERR_OK;
}

int get_codes()
{
	int dev_id = 0;
	long int manufacturer_id;
	volatile unsigned short *nFlashAddr;

	nFlashAddr = (unsigned short *)CFG_FLASH_BASE;

	*nFlashAddr = 0x0090;	/* Read identifier command. */
	asm("ssync;");

	manufacturer_id = *nFlashAddr;
	asm("ssync;");
	nFlashAddr++;
	dev_id = *nFlashAddr;
	asm("ssync;");

	*nFlashAddr = 0x00ff;	/* Return to read array mode. */
	asm("ssync;");

	dev_id = ((manufacturer_id << 16) | dev_id);
	return dev_id;
}

int do_pf4(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ushort data;
	ulong dflg;

	if (argc > 1) {
		dflg = simple_strtoul(argv[1], NULL, 16);
		if (dflg > 0) {
			*pPORTFIO_SET = GPIO_4;
		} else {
			*pPORTFIO_CLEAR = GPIO_4;
		}
	} else {
		data = *pPORTFIO;
		printf(" PF4 command argc = %d data %04x\n", argc, data);
	}
	return 0;
}

U_BOOT_CMD(pf4, CFG_MAXARGS, 1, do_pf4,
	   "pf4- set/clear pf4 mem bank switch\n",
	   "\n    - set (1) uses 2nd bank clear (0) uses 1st bank\n"
	   "pf4  1 ...\n" "pf4  0 ...\n");
