/**********************************************************************************************************

                        PROJECT                 :       BFIN
                        VERISON                 :       2.0
                        FILE                    :       flash-defines.h
                        MODIFIED DATE           :       29 jun 2004
                        AUTHOR                  :       BFin Project-ADI
                        LOCATION                :       LG Soft India,Bangalore

***********************************************************************************************************/

#ifndef _FLASH-DEFINES_H_
#define _FLASH-DEFINES_H_

#include <common.h>

#undef VID_DEBUG
#ifdef VID_DEBUG
	#define vidprintf printf
#else
	#define vidprintf
#endif


#define V_ULONG(a)	(*(volatile unsigned long *)( a ))
#define V_BYTE(a)	(*(volatile unsigned char *)( a ))

#define bool unsigned char


#define TRUE			0x1
#define FALSE			0x0
#define BUFFER_SIZE		0x80000

#define NO_COMMAND		0
#define GET_CODES		1
#define RESET			2
#define WRITE			3
#define FILL			4
#define ERASE_ALL		5
#define ERASE_SECT		6
#define READ			7
#define GET_SECTNUM		8

#define FLASH_START_L 		0x0000
#define FLASH_START_H 		0x2000

#define FLASH_TOT_SECT		40
#define FLASH_SIZE 		0x220000
#define FLASH_MAN_ST 		2
#define FLASH_DEV_ID		233
#define CFG_FLASH0_BASE		0x20000000

asm ("#define FLASH_START_L 0x0000");
asm ("#define FLASH_START_H 0x2000");
flash_info_t flash_info[CFG_MAX_FLASH_BANKS];

int GetCodes(void);
bool PollToggleBit(long lOffset);
bool ResetFlash(void);
bool EraseFlash(void);
bool EraseBlock( int nBlock );
bool UnlockFlash(long lOffset);
bool WriteData( long lStart, long lCount, long lStride, int *pnData );
bool FillData( long lStart, long lCount, long lStride, int *pnData );
bool ReadData( long lStart, long lCount, long lStride, int *pnData );
bool ReadFlash( long nOffset, int *pnValue );
bool WriteFlash( long nOffset, int nValue );
bool GetSectorNumber( long lOffset, int *pnSector );
int GetSectorProtectionStatus(flash_info_t * info, int nSector );
int GetOffset(int nBlock);

int AFP_NumSectors = 40;
long AFP_SectorSize1 = 0x10000;
int AFP_SectorSize2 = 0x4000;


#define WRITESEQ1	0x0AAA
#define WRITESEQ2	0x0554
#define WRITESEQ3	0x0AAA
#define WRITESEQ4	0x0AAA
#define WRITESEQ5	0x0554
#define WRITESEQ6	0x0AAA

#define WRITEDATA1	0xaa
#define WRITEDATA2	0x55
#define WRITEDATA3	0x80
#define WRITEDATA4	0xaa
#define WRITEDATA5	0x55
#define WRITEDATA6	0x10

#define PriFlashABegin	 	0
#define SecFlashABegin		32
#define SecFlashBBegin		36

#define PriFlashAOff		0x0
#define PriFlashBOff		0x100000
#define SecFlashAOff		0x200000
#define SecFlashBOff		0x280000

#define INVALIDLOCNSTART	0x20270000
#define INVALIDLOCNEND		0x20280000

#define BlockEraseVal		0x30

#define UNLOCKDATA1		0xaa
#define UNLOCKDATA2		0x55
#define UNLOCKDATA3		0xa0

#define GETCODEDATA1		0xaa
#define GETCODEDATA2		0x55
#define GETCODEDATA3		0x90

#define SecFlashASec1Off		0x200000
#define SecFlashASec2Off		0x204000
#define SecFlashASec3Off		0x206000
#define SecFlashASec4Off		0x208000
#define SecFlashAEndOff			0x210000

#define SecFlashBSec1Off		0x280000
#define SecFlashBSec2Off		0x284000
#define SecFlashBSec3Off		0x286000
#define SecFlashBSec4Off		0x288000
#define SecFlashBEndOff			0x290000

#endif
