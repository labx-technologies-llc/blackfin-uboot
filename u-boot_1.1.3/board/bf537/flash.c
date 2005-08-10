/*
 * U-boot - flash.c Flash driver for PSD4256GV
 *
 * Copyright (c) 2005 blackfin.uclinux.org
 * This file is based on BF533EzFlash.c originally written by Analog Devices, Inc.
 *
 * (C) Copyright 2000-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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

#include <malloc.h>
#include "flash-defines.h"

void flash_reset(void)
{
	reset_flash();
}

unsigned long flash_get_size(ulong baseaddr, flash_info_t * info,
			     int bank_flag)
{
	int id = 0, i = 0;
	static int FlagDev = 1;

	id = get_codes();
	if(FlagDev)	{
		FlagDev = 0;
	}
	info->flash_id = id;
	switch (bank_flag) {
	case 0:
		for (i = PriFlashABegin; i < SecFlashABegin; i++)
			info->start[i] = (baseaddr + (i * AFP_SectorSize1));
		for (i = SecFlashABegin; i < NUM_SECTORS; i++)
			info->start[i] = (baseaddr + SecFlashAOff + ((i-SecFlashABegin) * AFP_SectorSize2));
		info->size = 0x400000;
		info->sector_count = NUM_SECTORS;
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
	unsigned long size_b;
	int i;

	size_b = 0;
	for (i = 0; i < CFG_MAX_FLASH_BANKS; ++i) {
		flash_info[i].flash_id = FLASH_UNKNOWN;
	}

	size_b = flash_get_size(CFG_FLASH_BASE, &flash_info[0], 0);

	if (flash_info[0].flash_id == FLASH_UNKNOWN || size_b == 0) {
		printf("## Unknown FLASH on Bank 0 - Size = 0x%08lx = %ld MB\n",
			size_b, size_b >> 20);
	}

	(void)flash_protect(FLAG_PROTECT_SET,CFG_FLASH_BASE,(flash_info[0].start[2] - 1),&flash_info[0]);

	return (size_b);
}

void flash_print_info(flash_info_t * info)
{
	int i;

	if (info->flash_id == FLASH_UNKNOWN) {
		printf("missing or unknown FLASH type\n");
		return;
	}
	
	switch (info->flash_id) {
	case (STM_ID_29W320DB & 0xFFFF):
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
	int cnt = 0,i;
	int prot,sect;

	prot = 0;
        for (sect = s_first; sect <= s_last; ++sect) {
                if (info->protect[sect])
                        prot++;
        }
	if (prot)
                printf ("- Warning: %d protected sectors will not be erased!\n", prot);
        else
                printf ("\n");	

	cnt = s_last - s_first + 1;

	if (cnt == FLASH_TOT_SECT) {
		printf("Erasing flash, Please Wait \n");
		if(erase_flash() < 0) {
			printf("Erasing flash failed \n");
			return FLASH_FAIL;
		}
	} else {
		printf("Erasing Flash locations, Please Wait\n");
		for (i = s_first; i <= s_last; i++) {
			if (info->protect[i] == 0) {	/* not protected */
				if(erase_block_flash(i) < 0) {
					printf("Error Sector erasing \n");
					return FLASH_FAIL;
				}
			}
		}
	}
	return FLASH_SUCCESS;
}

int write_buff(flash_info_t * info, uchar * src, ulong addr, ulong cnt)
{
	int d;
	int *temp;
	if(addr%2){
		if(cnt % 2){
			read_flash(addr-1 - CFG_FLASH_BASE,&d);
			d = (int)((d&0x00FF)|(*src++ <<8));	
			write_data(addr-1,2,1,&d);
			temp = (int *)malloc( (cnt-1/2));
			memcpy(temp,src,cnt-1);
			write_data(addr+1,cnt-1,1,temp);
			}
		else{
			read_flash(addr-1 - CFG_FLASH_BASE,&d);
			d = (int)((d&0x00FF)|(*src++ <<8));
			write_data(addr-1,2,1,&d);
			temp = (int *)malloc( (cnt-2/2));
			memcpy(temp,src,cnt-2);
			write_data(addr+1,cnt-2,1,temp);
			read_flash(addr+cnt-1 - CFG_FLASH_BASE,&d);
			d = (int)((d&0xFF00)|*(src+cnt-1));
			write_data(addr+cnt-1,2,1,&d);
			} 
		  }
	else
		write_data(addr, cnt, 1, (int *) src);
	return FLASH_SUCCESS;
}


int write_data(long lStart, long lCount, long lStride, int *pnData)
{
	long i = 0;
	int j = 0;
	unsigned long ulOffset = lStart - CFG_FLASH_BASE;
	int d;
	int iShift = 0;
	int iNumWords = 2;
	int nLeftover = lCount % 4;
	int nSector = 0;

	for (i = 0; (i < lCount / 4) && (i < BUFFER_SIZE); i++) {
		for (iShift = 0, j = 0; (j < iNumWords);
			j++, ulOffset += (lStride * 2)) {
			
			/* unlock the flash, do the write, increase shift, and wait for completion */
			get_sector_number(ulOffset, &nSector);
			read_flash(ulOffset,&d);
			if(d != 0xffff) {
				printf("Flash not erased at offset 0x%x Please erase to reprogram \n",ulOffset);
				return FLASH_FAIL;
			}
			unlock_flash(ulOffset);
			write_flash(ulOffset, (pnData[i] >>iShift));
			if(poll_toggle_bit(ulOffset) < 0){
				printf("Error programming the flash \n");
				return FLASH_FAIL;
			}
			iShift += 16;
		}
	}
	
	if (nLeftover > 0) {
		get_sector_number(ulOffset, &nSector);
		read_flash(ulOffset,&d);
		if(d != 0xffff) {
			printf("Flash already programmed. Please erase to reprogram \n");
			printf("uloffset = 0x%x \t d = 0x%x\n",ulOffset,d);
			return FLASH_FAIL;
		}
		unlock_flash(ulOffset);
		write_flash(ulOffset, pnData[i] );
		if(poll_toggle_bit(ulOffset) < 0) {
			printf("Error programming the flash \n");
			return FLASH_FAIL;
		}
	}
	return FLASH_SUCCESS;
}

int read_data(long ulStart, long lCount, long lStride, int *pnData)
{
	long i = 0;						// loop counter
	int j = 0;						// inner loop counter
	unsigned long ulOffset = ulStart;			// current offset to write
	int iShift = 0;					// shift value by iShift bits
	int iNumWords = 2;				// number of words in a long
	int nLeftover = lCount % 4;		// how much if any do we have leftover to write
	int nHi,nLow;
	
	// read the buffer up to BUFFER_SIZE items
	for (i = 0; (i < lCount/4) && (i < BUFFER_SIZE); i++)
	{
		for ( iShift = 0, j = 0; j < iNumWords ; j+=2 )
		{
			// Read flash
			read_flash( ulOffset, &nLow );
			ulOffset += (lStride*2);
			read_flash( ulOffset, &nHi );
			ulOffset += (lStride*2);
			pnData[i] = (nHi << 16) | nLow;
		}
	}
	// because of the way our ldr file is built, we will always have
	// 2 bytes leftover if there is leftover, because the flash is 16 bit
	// that will mean that there is only one read left to do.
	if( nLeftover > 0 )
	{
		read_flash( ulOffset, &pnData[i] );
	}
	return FLASH_SUCCESS;
}

int write_flash(long nOffset, int nValue)
{
	long addr;

	addr = (CFG_FLASH_BASE + nOffset);
	*(unsigned volatile short *) addr = nValue;
	return FLASH_SUCCESS;
}

int read_flash(long nOffset, int *pnValue)
{
	unsigned short *pFlashAddr = (unsigned short *)(CFG_FLASH_BASE + nOffset);

	*pnValue = *pFlashAddr;

	return TRUE;
}

int poll_toggle_bit(long lOffset)
{
	unsigned int u1,u2;
	unsigned long timeout = 0xFFFFFFFF;
	volatile unsigned long *FB = (volatile unsigned long *)(0x20000000 + lOffset);
	while(1) {
		if(timeout < 0)
			break;
		u1 = *(volatile unsigned short *)FB;
		u2 = *(volatile unsigned short *)FB;
		if((u1 & 0x0040) == (u2 & 0x0040))
			return FLASH_SUCCESS;
		if((u2 & 0x0020) == 0x0000)
			continue;
		u1 = *(volatile unsigned short *)FB;
		if((u2 & 0x0040) == (u1 & 0x0040))
			return FLASH_SUCCESS;
		else {
			reset_flash();
			return FLASH_FAIL;
		}
		timeout--;
	}
	printf("Time out occured \n");
	if(timeout <0)	return FLASH_FAIL;
}

void reset_flash(void)
{
	write_flash(WRITESEQ1, RESET_VAL);
	/* Wait for 10 micro seconds */
	udelay(10);
}

int erase_flash(void)
{
	write_flash(WRITESEQ1, WRITEDATA1);
	write_flash(WRITESEQ2, WRITEDATA2);
	write_flash(WRITESEQ3, WRITEDATA3);
	write_flash(WRITESEQ4, WRITEDATA4);
	write_flash(WRITESEQ5, WRITEDATA5);
	write_flash(WRITESEQ6, WRITEDATA6);

	if(poll_toggle_bit(0x0000) < 0)
		return FLASH_FAIL;

	return FLASH_SUCCESS;
}

int erase_block_flash(int nBlock)
{
	long ulSectorOff = 0x0;

	if ((nBlock < 0) || (nBlock > AFP_NumSectors))
		return FALSE;
		
	// figure out the offset of the block in flash
	if(  (nBlock >=0) && (nBlock <SecFlashABegin))
		ulSectorOff = nBlock * AFP_SectorSize1;

	else if( (nBlock >= SecFlashABegin) && (nBlock < NUM_SECTORS) )
		ulSectorOff = SecFlashAOff + (nBlock-SecFlashABegin)* AFP_SectorSize2;			
	// no such sector
	else
		return FLASH_FAIL;
	
	write_flash((WRITESEQ1 | ulSectorOff), WRITEDATA1);
	write_flash((WRITESEQ2 | ulSectorOff), WRITEDATA2);
	write_flash((WRITESEQ3 | ulSectorOff), WRITEDATA3);
	write_flash((WRITESEQ4 | ulSectorOff), WRITEDATA4);
	write_flash((WRITESEQ5 | ulSectorOff), WRITEDATA5);

	write_flash(ulSectorOff, BlockEraseVal);

	if(poll_toggle_bit(ulSectorOff) < 0)
		return FLASH_FAIL;
	printf(".");

	return FLASH_SUCCESS;
}

void unlock_flash(long ulOffset)
{
	unsigned long ulOffsetAddr = ulOffset;
	ulOffsetAddr &= 0xFFFF0000;

	write_flash((WRITESEQ1 | ulOffsetAddr), UNLOCKDATA1);
	write_flash((WRITESEQ2 | ulOffsetAddr), UNLOCKDATA2);
	write_flash((WRITESEQ3 | ulOffsetAddr), UNLOCKDATA3);
}

int get_codes()
{
	int dev_id = 0;

	write_flash(WRITESEQ1, GETCODEDATA1);
	write_flash(WRITESEQ2, GETCODEDATA2);
	write_flash(WRITESEQ3, GETCODEDATA3);
	
	

	read_flash(0x0402, &dev_id);
	dev_id &= 0x0000FFFF;

	reset_flash();

	return dev_id;
}

void get_sector_number(long ulOffset, int *pnSector)
{
	int nSector = 0;
	long lMainEnd = 0x400000;
	long lBootEnd = 0x10000;

	// sector numbers for the FLASH A boot sectors
	if(ulOffset < lBootEnd)
	{
		nSector = (int)ulOffset / AFP_SectorSize1 ;
	}
	// sector numbers for the FLASH B boot sectors
	else if( ( ulOffset >= lBootEnd ) && ( ulOffset < lMainEnd ) )
	{
		nSector = (( ulOffset / (AFP_SectorSize2) ) + 7 );
	}

	// if it is a valid sector, set it
	if ( (nSector >= 0) && (nSector < AFP_NumSectors) )
		*pnSector = nSector;

}
