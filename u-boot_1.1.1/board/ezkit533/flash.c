/*
 * (C) Copyright 2001 - Analog Devices, Inc.  All rights reserved.
 *	BF533EzFlash.c
 *	Analog Devices, Inc. - 2001
 *
 *
 *	Change Log
 *	1.00.1
 *	- made changes so that the driver will work with
 *		  the revised GUI
 *	1.00.0
 *	- initial release
 *
 * VisualDSP++ "Flash Programmer" flash driver for use with the
 * ADSP-BF533 EZ-KIT Lite containing the STMicroelectronics PSD4256G
 * flash device.
 *
 *	PROJECT				:	BFIN
 *	VERSION				:	2.0
 *	FILE				:	flash.c
 *	MODIFIED DATE			:	29 jun 2004
 *	AUTHOR				:	BFin Project-ADI
 *	LOCATION			:	LG Soft India,Bangalore
 */

#include "flash-defines.h"

void flash_reset(void)
{
	ResetFlash();
}

unsigned long flash_get_size(ulong baseaddr, flash_info_t * info,
			     int bank_flag)
{
	int id = 0, i = 0;

	id = GetCodes();
	printf("Device ID of the Flash is %x\n", id);
	info->flash_id = id;

	switch (bank_flag) {
	case 0:
		for (i = PriFlashABegin; i < SecFlashABegin; i++)
			info->start[i] = (baseaddr + (i * AFP_SectorSize1));
		info->size = 0x200000;
		info->sector_count = 32;
		break;
	case 1:
		info->start[0] = baseaddr + SecFlashASec1Off;
		info->start[1] = baseaddr + SecFlashASec2Off;
		info->start[2] = baseaddr + SecFlashASec3Off;
		info->start[3] = baseaddr + SecFlashASec4Off;
		info->size = 0x10000;
		info->sector_count = 4;
		break;
	case 2:
		info->start[0] = baseaddr + SecFlashBSec1Off;
		info->start[1] = baseaddr + SecFlashBSec2Off;
		info->start[2] = baseaddr + SecFlashBSec3Off;
		info->start[3] = baseaddr + SecFlashBSec4Off;
		info->size = 0x10000;
		info->sector_count = 4;
		break;
	}
	return (info->size);
}

unsigned long flash_init(void)
{
	unsigned long size_b0, size_b1, size_b2;
	int i;

	size_b0 = size_b1 = size_b2 = 0;
	printf("Flash Memory Start 0x%x\n", CFG_FLASH_BASE);
	printf("Memory Map for the Flash\n");
	printf("0x20000000 - 0x200FFFFF Flash A Primary (1MB)\n");
	printf("0x20100000 - 0x201FFFFF Flash B Primary (1MB)\n");
	printf("0x20200000 - 0x2020FFFF Flash A Secondary (64KB)\n");
	printf("0x20280000 - 0x2028FFFF Flash B Secondary (64KB)\n");
	printf("Please type command flinfo for information on Sectors \n");

	for (i = 0; i < CFG_MAX_FLASH_BANKS; ++i) {
		flash_info[i].flash_id = FLASH_UNKNOWN;
	}

	size_b0 = flash_get_size(CFG_FLASH0_BASE, &flash_info[0], 0);
	size_b1 = flash_get_size(CFG_FLASH0_BASE, &flash_info[1], 1);
	size_b2 = flash_get_size(CFG_FLASH0_BASE, &flash_info[2], 2);

	if (flash_info[0].flash_id == FLASH_UNKNOWN || size_b0 == 0) {
		printf("## Unknown FLASH on Bank 0 - Size = 0x%08lx = %ld MB\n",
		       size_b0, size_b0 >> 20);
	}
	return (size_b0 + size_b1 + size_b2);
}

void flash_print_info(flash_info_t * info)
{
	int i;

	if (info->flash_id == FLASH_UNKNOWN) {
		printf("missing or unknown FLASH type\n");
		return;
	}

	switch (info->flash_id) {
	case FLASH_PSD4256GV:
		printf("ST Microelectronics ");
		break;
	default:
		printf("Unknown Vendor ");
		break;
	}
	for (i = 0; i < info->sector_count; ++i) {
		if ((i % 5) == 0)
			printf("\n   ");
		printf(" %08lX%s",
		       info->start[i],
		       info->protect[i] ? " (RO)" : "     ");
	}
	printf("\n");
	return;
}

int flash_erase(flash_info_t * info, int s_first, int s_last)
{
	int i = 0, j, cnt = 0;

	ResetFlash();
	cnt = s_last - s_first + 1;

	if (cnt == FLASH_TOT_SECT) {
		printf("Erasing flash, Please Wait \n");
		EraseFlash();
	} else {
		printf("Erasing Flash locations, Please Wait\n");
		for (i = s_first; i <= s_last; i++) {
			if (info->start[i] >= 0x20100000) {
				for (j = 0; j < 500000; j++);
				for (j = 0; j < 500000; j++);
			}
			EraseBlockFlash(i, info->start[i]);
		}
	}
	return 0;
}

int write_buff(flash_info_t * info, uchar * src, ulong addr, ulong cnt)
{
	WriteData(addr, cnt, 1, (int *) src);
	return 0;
}


bool WriteData(long lStart, long lCount, long lStride, int *pnData)
{
	long i = 0;
	int j = 0;
	unsigned long ulOffset = lStart - CFG_FLASH_BASE;
	int iShift = 0;
	int iNumWords = 2;
	int nLeftover = lCount % 4;
	int nSector = 0;

	for (i = 0; (i < lCount / 4) && (i < BUFFER_SIZE); i++) {
		for (iShift = 0, j = 0; (j < iNumWords);
		     j++, ulOffset += (lStride * 2)) {
			if ((ulOffset >= INVALIDLOCNSTART)
			    && (ulOffset < INVALIDLOCNEND)) {
				printf("Invalid locations, Try writing to another location \n");
				return -1;	/* Invalid Location */
			}
			GetSectorNumber(ulOffset, &nSector);
			UnlockFlash(ulOffset);
			WriteFlash(ulOffset, (pnData[i] >> iShift));
			PollToggleBit(ulOffset);
			iShift += 16;
		}
	}
	if (nLeftover > 0) {
		if ((ulOffset >= INVALIDLOCNSTART)
		    && (ulOffset < INVALIDLOCNEND))
			return -1;
		GetSectorNumber(ulOffset, &nSector);
		UnlockFlash(ulOffset);
		WriteFlash(ulOffset, pnData[i]);
		PollToggleBit(ulOffset);
	}
	ResetFlash();
	return 0;
}

bool ReadData(long ulStart, long lCount, long lStride, int *pnData)
{
	long i = 0;
	int j = 0;
	long ulOffset = ulStart;
	int iShift = 0;
	int iNumWords = 2;
	int nLeftover = lCount % 4;
	int nHi, nLow;
	int nSector = 0;

	for (i = 0; (i < lCount / 4) && (i < BUFFER_SIZE); i++) {
		for (iShift = 0, j = 0; j < iNumWords; j += 2) {
			if ((ulOffset >= INVALIDLOCNSTART)
			    && (ulOffset < INVALIDLOCNEND))
				return -1;

			GetSectorNumber(ulOffset, &nSector);
			ReadFlash(ulOffset, &nLow);
			ulOffset += (lStride * 2);
			ReadFlash(ulOffset, &nHi);
			ulOffset += (lStride * 2);
			pnData[i] = (nHi << 16) | nLow;
		}
	}
	if (nLeftover > 0) {
		if ((ulOffset >= INVALIDLOCNSTART)
		    && (ulOffset < INVALIDLOCNEND))
			return -1;

		GetSectorNumber(ulOffset, &nSector);
		ReadFlash(ulOffset, &pnData[i]);
	}
	return TRUE;
}

bool WriteFlash(long nOffset, int nValue)
{
	long addr;

	addr = (CFG_FLASH_BASE + nOffset);
	asm("ssync;");
	*(unsigned volatile short *) addr = nValue;
	asm("ssync;");
	printf("");	/* FIXME */
	return TRUE;
}

bool ReadFlash(long nOffset, int *pnValue)
{
	int nValue = 0x0;
	long addr = (CFG_FLASH_BASE + nOffset);

	if (nOffset != 0x2)
		ResetFlash();	/* FIXME */
	asm("ssync;");
	nValue = *(volatile unsigned short *) addr;
	asm("ssync;");
	*pnValue = nValue;
	return TRUE;
}

bool PollToggleBit(long lOffset)
{
	bool bError = FALSE;

	asm("POLL_TOGGLE_BIT:");
	asm("p2.l = 0x0000;");
	asm("p2.h = 0x2000;");
	asm("r2 = r0;");
	asm("r1 = p2;");
	asm("r1 = r1 + r2;");
	asm("p2 = r1;");
	asm("r1 = w[p2](z);");
	asm("SSYNC;");
	asm("r2 = w[p2](z);");
	asm("SSYNC;");
	asm("r1 = r1 ^ r2;");
	asm("cc = bittst(r1, 6);");
	asm("if !cc jump DONE_TOGGLE_BIT;");
	asm("cc = bittst(r2, 5);");
	asm("if !cc jump POLL_TOGGLE_BIT;");
	asm("r1 = w[p2](z);");
	asm("SSYNC;");
	asm("r2 = w[p2](z);");
	asm("SSYNC;");
	asm("r1 = r1 ^ r2;");
	asm("cc = bittst(r1, 6);");
	asm("if !cc jump DONE_TOGGLE_BIT;");
	
	ResetFlash();
	
	asm("DONE_TOGGLE_BIT:");
	bError = TRUE;

	return !bError;
}

bool ResetFlash()
{
	WriteFlash(WRITESEQ1, 0xf0);
	return TRUE;
}

bool EraseFlash()
{
/* Hardcoding to be removed later. Currently wrong code is getting generated */
	int ErrorCode = 0;

	WriteFlash(0x0AAA, 0xaa);
	WriteFlash(0x0554, 0x55);
	WriteFlash(0x0AAA, 0x80);
	WriteFlash(0x0AAA, 0xaa);
	WriteFlash(0x0554, 0x55);
	WriteFlash(0x0AAA, 0x10);

	PollToggleBit(0x0000);

	WriteFlash(0x200AAA, 0xaa);
	WriteFlash(0x200554, 0x55);
	WriteFlash(0x200AAA, 0x80);
	WriteFlash(0x200AAA, 0xaa);
	WriteFlash(0x200554, 0x55);
	WriteFlash(0x200AAA, 0x10);

	PollToggleBit(0x200000);

	WriteFlash(0x100AAA, 0xaa);
	WriteFlash(0x100554, 0x55);
	WriteFlash(0x100AAA, 0x80);
	WriteFlash(0x100AAA, 0xaa);
	WriteFlash(0x100554, 0x55);
	WriteFlash(0x100AAA, 0x10);

	PollToggleBit(0x100000);

	WriteFlash(0x280AAA, 0xaa);
	WriteFlash(0x280554, 0x55);
	WriteFlash(0x280AAA, 0x80);
	WriteFlash(0x280AAA, 0xaa);
	WriteFlash(0x280554, 0x55);
	WriteFlash(0x280AAA, 0x10);

	ErrorCode = PollToggleBit(0x280000);

	return TRUE;
}

bool EraseBlockFlash(int nBlock, unsigned long address)
{

	long ulSectorOff = 0x0;
	int ErrorCode = 0;

	if ((nBlock < 0) || (nBlock > AFP_NumSectors))
		return FALSE;

	ulSectorOff = (address - 0x20000000);

	WriteFlash((0x0AAA | ulSectorOff), 0xaa);
	WriteFlash((0x0554 | ulSectorOff), 0x55);
	WriteFlash((0x0AAA | ulSectorOff), 0x80);
	WriteFlash((0x0AAA | ulSectorOff), 0xaa);
	WriteFlash((0x0554 | ulSectorOff), 0x55);

	WriteFlash(ulSectorOff, 0x30);

	ErrorCode = PollToggleBit(ulSectorOff);

	return TRUE;
}

bool UnlockFlash(long ulOffset)
{
	unsigned long ulOffsetAddr = ulOffset;
	ulOffsetAddr &= 0xFFFF0000;

	WriteFlash((WRITESEQ1 | ulOffsetAddr), UNLOCKDATA1);
	WriteFlash((WRITESEQ2 | ulOffsetAddr), UNLOCKDATA2);
	WriteFlash((WRITESEQ3 | ulOffsetAddr), UNLOCKDATA3);

	return 1;
}

int GetCodes()
{
	int dev_id = 0;

	WriteFlash(WRITESEQ1, GETCODEDATA1);
	WriteFlash(WRITESEQ2, GETCODEDATA2);
	WriteFlash(WRITESEQ3, GETCODEDATA3);

	ReadFlash(0x0002, &dev_id);
	dev_id &= 0x00FF;

	ResetFlash();

	return dev_id;
}

bool GetSectorNumber(long ulOffset, int *pnSector)
{
	int nSector = 0;

	if (ulOffset >= SecFlashAOff) {
		if ((ulOffset < SecFlashASec1Off)
		    && (ulOffset < SecFlashASec2Off)) {
			nSector = 32;
		} else if ((ulOffset >= SecFlashASec2Off)
			   && (ulOffset < SecFlashASec3Off)) {
			nSector = 33;
		} else if ((ulOffset >= SecFlashASec3Off)
			   && (ulOffset < SecFlashASec4Off)) {
			nSector = 34;
		} else if ((ulOffset >= SecFlashASec4Off)
			   && (ulOffset < SecFlashAEndOff)) {
			nSector = 35;
		}
	} else if (ulOffset >= SecFlashBOff) {
		if ((ulOffset < SecFlashBSec1Off)
		    && (ulOffset < SecFlashBSec2Off)) {
			nSector = 36;
		}
		if ((ulOffset < SecFlashBSec2Off)
		    && (ulOffset < SecFlashBSec3Off)) {
			nSector = 37;
		}
		if ((ulOffset < SecFlashBSec3Off)
		    && (ulOffset < SecFlashBSec4Off)) {
			nSector = 38;
		}
		if ((ulOffset < SecFlashBSec4Off)
		    && (ulOffset < SecFlashBEndOff)) {
			nSector = 39;
		}
	} else if ((ulOffset >= PriFlashAOff) && (ulOffset < SecFlashAOff)) {
		nSector = ulOffset & 0xffff0000;
		nSector = ulOffset >> 16;
		nSector = nSector & 0x000ff;
	}

	if ((nSector >= 0) && (nSector < AFP_NumSectors)) {
		*pnSector = nSector;
		return 1;
	} else
		return -1;
}


int GetSectorProtectionStatus(flash_info_t * info, int nSector)
{
	int protect_sts = 0;

	/* send the auto select command to the flash */
	WriteFlash((info->start[nSector] + 0xaaa), 0xaa);
	WriteFlash((info->start[nSector] + 0x554), 0x55);
	WriteFlash((info->start[nSector] + 0xaaa), 0x90);

	/* now we can read the codes */
	ReadFlash((info->start[nSector] + 4), &protect_sts);
	protect_sts &= 0x00FF;

	/* we need to issue another command to get the part out
	 * of auto select mode so issue a reset which just puts
	 * the device back in read mode
	 */
	ResetFlash();

	return protect_sts;
}

int GetOffset(int nBlock)
{
	int ulSectorOff;

	if ((nBlock < 0) || (nBlock > AFP_NumSectors))
		return FALSE;

	if ((nBlock >= 0) && (nBlock < SecFlashABegin)) {
		ulSectorOff = (nBlock * AFP_SectorSize1);
	} else if (nBlock == SecFlashABegin) {
		ulSectorOff = SecFlashASec1Off;
	} else if (nBlock == (SecFlashABegin + 1)) {
		ulSectorOff = SecFlashASec2Off;
	} else if (nBlock == (SecFlashABegin + 2)) {
		ulSectorOff = SecFlashASec3Off;
	} else if (nBlock == (SecFlashABegin + 3)) {
		ulSectorOff = SecFlashASec4Off;
	} else if (nBlock == SecFlashBBegin) {
		ulSectorOff = SecFlashBSec1Off;
	} else if (nBlock == (SecFlashBBegin + 1)) {
		ulSectorOff = SecFlashBSec2Off;
	} else if (nBlock == (SecFlashBBegin + 2)) {
		ulSectorOff = SecFlashBSec3Off;
	} else if (nBlock == (SecFlashBBegin + 3)) {
		ulSectorOff = SecFlashBSec4Off;
	}
	return ulSectorOff;
}
