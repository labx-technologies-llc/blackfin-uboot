/*
 * BF533EzFlash.c
 * 
 * Analog Devices, Inc. - 2001
 * 
 * 
 * Change Log
 * 
 * 	1.00.1
 *	- made changes so that the driver will work with
 * 	  the revised GUI
 *
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

void init_EBIU(void)
{
	*(volatile unsigned long *) ambctl0 = 0xbbc3bbc3;
	*(volatile unsigned long *) ambctl1 = 0x99b39983;
	*(volatile unsigned short *) amgctl = 0xf9;
}

void init_Flags(void)
{
	*(volatile unsigned short *) pFIO_DIR = 0x1F;
	*(volatile unsigned short *) pFIO_FLAG_S = 0x1C;
	*(volatile unsigned short *) pFIO_MASKA_D = 0x160;
	*(volatile unsigned short *) pFIO_MASKB_D = 0x80;
	*(volatile unsigned short *) pFIO_POLAR = 0x160;
	*(volatile unsigned short *) pFIO_EDGE = 0x1E0;
	*(volatile unsigned short *) pFIO_INEN = 0x1e0;
	*(volatile unsigned short *) pFIO_FLAG_D = 0x1C;
}

unsigned long flash_get_size(ulong baseaddr, flash_info_t * info)
{
	int man_id = 0, dev_id = 0, i = 0;

	man_id = GetCodes();
	dev_id = man_id & 0xffff;
	man_id = (man_id >> 16) & 0xff;

	printf("Device ID of the Flash is %x\n", dev_id);
	printf("Manufacture ID of the Flash is %x\n", man_id);
	printf("Memory Map of Flash: 0x20000000 - 0x2003ffff\n");

	info->flash_id = dev_id;
	info->sector_count = FLASH_TOT_SECT;
	info->size = FLASH_SIZE;

	info->start[0] = 0x20000000;
	info->start[1] = 0x20004000;
	info->start[2] = 0x20006000;
	info->start[3] = 0x20008000;

	for (i = 4; i < 67; i++)
		info->start[i] = ((baseaddr + AFP_SectorSize1)
				  + ((i - 4) * AFP_SectorSize1));

	return (info->size);
}

unsigned long flash_init(void)
{
	unsigned long size_b0 = 0;
	int i;

	printf("Flash Memory Start 0x%x\n", CFG_FLASH_BASE);
	printf("Please type command flinfo for information on Sectors \n");
	init_Flags();
	for (i = 0; i < CFG_MAX_FLASH_BANKS; ++i) {
		flash_info[i].flash_id = FLASH_UNKNOWN;
	}
	size_b0 = flash_get_size(CFG_FLASH0_BASE, &flash_info[0]);
	if (flash_info[0].flash_id == FLASH_UNKNOWN || size_b0 == 0) {
		printf
		    ("## Unknown FLASH on Bank 0 - Size = 0x%08lx = %ld MB\n",
		     size_b0, size_b0 >> 20);
	}
	asyncbank_init();
	return size_b0;
}

void flash_print_info(flash_info_t * info)
{
	int i;

	init_EBIU();
	init_Flags();

	if (info->flash_id == FLASH_UNKNOWN) {
		printf("missing or unknown FLASH type\n");
		return;
	}
	switch (info->flash_id) {
	case FLASH_DEV_ID:
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
	asyncbank_init();
	return;
}

int flash_erase(flash_info_t * info, int s_first, int s_last)
{
	int i = 0, cnt = 0, j;
	unsigned long Off;

	init_EBIU();
	init_Flags();

	cnt = s_last - s_first + 1;
	udelay(1000);

	if (cnt == FLASH_TOT_SECT) {
		printf("Erasing flash, Please Wait \n");
		EraseFlash();
	} else {
		printf("Sector erasing, Please Wait\n");
		for (i = s_first; i <= s_last; i++) {
			Off = (GetOffset(i) - 0x20000000);
			udelay(1000);
			FLASH_Block_Erase(Off);
			for (j = 0; j < 500000; j++);
			for (j = 0; j < 500000; j++);
			for (j = 0; j < 500000; j++);
			for (j = 0; j < 500000; j++);
		}
	}
	ResetFlash();
	asyncbank_init();
	return 0;
}

unsigned long GetOffset(int sec_num)
{
	if (sec_num == 0)
		return 0x20000000;
	else if (sec_num == 1)
		return 0x20004000;
	else if (sec_num == 2)
		return 0x20006000;
	else if (sec_num == 3)
		return 0x20008000;
	else
		return ((0x20000000 + AFP_SectorSize1) +
			((sec_num - 4) * AFP_SectorSize1));
}


int write_buff(flash_info_t * info, uchar * src, ulong addr, ulong cnt)
{
	if ((addr + cnt) > 0x20400000) {
		printf("Outside available Flash \n");
		return -1;
	}

	init_EBIU();
	init_Flags();
	WriteData(addr, cnt, 1, (int *) src);
	asyncbank_init();
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

	for (i = 0; (i < lCount / 4) && (i < BUFFER_SIZE); i++) {
		for (iShift = 0, j = 0; (j < iNumWords);
		     j++, ulOffset += (lStride * 2)) {
			FLASH_Write(ulOffset, pnData[i] >> iShift);
			iShift += 16;
		}
	}
	iShift = 16;
	if (nLeftover > 0) {
		FLASH_Write(ulOffset, pnData[i]);
		if (nLeftover == 3) {
			ulOffset += 2;
			FLASH_Write(ulOffset, (pnData[i] >> iShift));
		}
	}
	ResetFlash();
	return 0;
}

bool FLASH_Write(long addr, int data)
{
	int t;

	LED6_On();
	/* This C code works fine, but very slow */
	FLASH_Base[0x555] = 0xAA;
	FLASH_Base[0x2AA] = 0x55;
	FLASH_Base[0x555] = 0xA0;
	*(volatile unsigned short *) (0x20000000 + addr) = data;
	asm("ssync;");
	for (t = 0; t < 5000; t++) {
	}
	LED6_Off();
	return TRUE;
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

	init_EBIU();
	init_Flags();

	for (i = 0; (i < lCount / 4) && (i < BUFFER_SIZE); i++) {
		for (iShift = 0, j = 0; j < iNumWords; j += 2) {
			FLASH_Read(ulOffset, &nLow);
			ulOffset += (lStride * 2);
			FLASH_Read(ulOffset, &nHi);
			ulOffset += (lStride * 2);
			pnData[i] = (nHi << 16) | nLow;
		}
	}
	if (nLeftover > 0) {
		FLASH_Read(ulOffset, &pnData[i]);
	}
	asyncbank_init();
	return TRUE;
}

bool FLASH_Read(long addr, int *pnValue)
{
	int nValue = 0x0, t;
	long Off = (CFG_FLASH_BASE + addr);

	FLASH_Base[0x0] = 0xF0;
	for (t = 0; t < 500; t++) {
	}

	nValue = *(volatile unsigned short *) Off;
	*pnValue = nValue;
	return TRUE;
}

bool ResetFlash()
{
	int j;

	for (j = 0; j < 500000; j++);

	FLASH_Base[0x555] = 0xaa;
	FLASH_Base[0x2aa] = 0x55;
	FLASH_Base[0x0] = 0xF0;

	for (j = 0; j < 50000; j++);

	return TRUE;
}

bool EraseFlash()
{
	short t;

	udelay(1000);
	FLASH_Base[0x555] = 0xAA;
	FLASH_Base[0x2AA] = 0x55;
	FLASH_Base[0x555] = 0x80;
	FLASH_Base[0x555] = 0xAA;
	FLASH_Base[0x2AA] = 0x55;
	FLASH_Base[0x555] = 0x10;
	for (t = 0; t < 1000; t++);
	while ((FLASH_Base[0] & 3) == 0);
	udelay(1000);
	return TRUE;
}

int GetCodes()
{
	unsigned long dev_id = 0;
	unsigned long man_id = 0;

	FLASH_Base[0x555] = 0xAA;
	FLASH_Base[0x2AA] = 0x55;
	FLASH_Base[0x555] = 0x90;
	asm("ssync;");
	man_id = FLASH_Base[0x0];
	dev_id = *(unsigned short *) FB;
	asm("ssync;");

	FLASH_Base[0x0] = 0xF0;
	return ((man_id << 16) | dev_id);
}

void FLASH_Block_Erase(unsigned long Block_Address)
{
	int t;

	udelay(1000);
	FLASH_Base[0x555] = 0xAA;
	FLASH_Base[0x2AA] = 0x55;
	FLASH_Base[0x555] = 0x80;
	FLASH_Base[0x555] = 0xAA;
	FLASH_Base[0x2AA] = 0x55;
	*(volatile unsigned short *) (0x20000000 + Block_Address) = 0x30;

	for (t = 0; t < 1000; t++);
}
