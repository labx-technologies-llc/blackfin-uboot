/**********************************************************************************************************

                        PROJECT                 :       BFIN
                        VERISON                 :       2.0
                        FILE                    :       flash-defines.h
                        MODIFIED DATE           :       29 jun 2004
                        AUTHOR                  :       BFin Project-ADI
                        LOCATION                :       LG Soft India,Bangalore

***********************************************************************************************************/

#ifndef __FLASHDEFINES_H__
#define __FLASHDEFINES_H__

#include <common.h>

#define V_ULONG(a)	(*(volatile unsigned long *)( a ))
#define V_BYTE(a)	(*(volatile unsigned char *)( a ))

#define bool unsigned char

#define TRUE			0x1
#define FALSE			0x0
#define BUFFER_SIZE		0x100000

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

#define FLASH_TOT_SECT		67
#define FLASH_SIZE 		0x400000
#define FLASH_MAN_ST 		2
#define FLASH_DEV_ID		0x22CB
#define CFG_FLASH0_BASE		0x20000000

asm ("#define FLASH_START_L 0x0000");
asm ("#define FLASH_START_H 0x2000");

flash_info_t flash_info[CFG_MAX_FLASH_BANKS];
int GetCodes(void);
bool ResetFlash(void);
bool EraseFlash(void);
bool WriteData( long lStart, long lCount,long lStride, int *pnData );
bool FLASH_Write(long addr, int data);
bool FLASH_Read( long addr,int *pnValue);
bool ReadData( long ulStart, long lCount, long lStride, int *pnData );
bool GetSectorNumber( long lOffset, int *pnSector );
int GetSectorProtectionStatus(flash_info_t * info, int nSector );
unsigned long GetOffset(int sec_num);
void FLASH_Block_Erase(unsigned long Block_Address);

void init_EBIU(void);
void init_Flags(void);

int AFP_SectorSize1 = 0x10000;
int AFP_SectorSize2 = 0x4000;


volatile unsigned long *ambctl0 = (volatile unsigned long *)0xffc00a04;
volatile unsigned long *ambctl1 = (volatile unsigned long *)0xffc00a08;
volatile unsigned long *amgctl = (volatile unsigned long *)0xffc00a00;

volatile unsigned long *pFIO_DIR = (volatile unsigned long *)0xffc00730;
volatile unsigned long *pFIO_FLAG_S = (volatile unsigned long *)0xffc00708;
volatile unsigned long *pFIO_FLAG_C = (volatile unsigned long *)0xffc00704;
volatile unsigned long *pFIO_MASKA_D = (volatile unsigned long *)0xffc00710;
volatile unsigned long *pFIO_MASKB_D = (volatile unsigned long *)0xffc00720;
volatile unsigned long *pFIO_POLAR = (volatile unsigned long *)0xffc00734;
volatile unsigned long *pFIO_EDGE = (volatile unsigned long *)0xffc00738; 
volatile unsigned long *pFIO_INEN = (volatile unsigned long *)0xffc00740;
volatile unsigned long *pFIO_FLAG_D = (volatile unsigned long *)0xffc00700;

inline void LED6_On(void)  {*(volatile unsigned short *)pFIO_FLAG_C	= 0x0004;}
inline void LED6_Off(void) {*(volatile unsigned short *)pFIO_FLAG_S	= 0x0004;}

volatile unsigned short *FLASH_Base = (unsigned int) 0x20000000;
unsigned volatile long *FB = 0x20000002;


#endif
