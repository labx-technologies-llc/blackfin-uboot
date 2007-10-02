/* Structures used by the Blackfin on-chip bootrom in misc places. */

#ifndef __BOOTROM_H__
#define __BOOTROM_H__

#ifndef __ASSEMBLY__

typedef struct {
	long   nBlockCode;
	void  *pTargetAddress;
	long   nByteCount;
	long   nArgument;
} header_struct;

typedef struct {
	void  *pSource;
	long   nByteCount;
} buffer_struct;

typedef struct {
	void  *pSource;
	void  *pDestination;
	short *pControlRegister;
	short *pDmaControlRegister;
	long   nControlValue;
	long   nByteCount;
	long   nFlags;
	short  nDataWidth;
	short  nSrcModifyMult;
	short  nDstModifyMult;
	short  nHwait;
	short  nSsel;
	short  nUserShort;
	long   nUserLong;
	long   nReserved2;
	void  *pErrorFunction;
	void  *pLoadFunction;
	void  *pCallBackFunction;
	header_struct *pHeader;
	void  *pTempBuffer;
	long   nTempByteCount;
	long   nBlockCount;
	long   nClock;
} boot_struct;

#else
# include "bootrom-asm-offsets.h"
#endif

#endif
