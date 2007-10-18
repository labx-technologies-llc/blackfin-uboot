/* Structures used by the Blackfin on-chip bootrom in misc places.
 * See the "Data Structures" section of the "System Reset and Booting"
 * chapter of BF54x/BF52x HRMs.
 */

#ifndef __BOOTROM_H__
#define __BOOTROM_H__

#if defined(__ADSPBF54x__) || defined(__ADSPBF52x__)
# define BOOTROM_GENERATION 1
#else
# define BOOTROM_GENERATION 0
#endif

#ifndef __ASSEMBLY__

#include <linux/types.h>

typedef struct {
	int32_t dBlockCode;
	void    *pTargetAddress;
	int32_t dByteCount;
	int32_t dArgument;
} ADI_BOOT_HEADER;

typedef struct {
	void    *pSource;
	int32_t dByteCount;
} ADI_BOOT_BUFFER;

typedef struct {
	void    *pSource;
	void    *pDestination;
	int16_t *pControlRegister;
	int16_t *pDmaControlRegister;
	int32_t dControlValue;
	int32_t dByteCount;
	int32_t dFlags;
	int16_t uwDataWidth;
	int16_t uwSrcModifyMult;
	int16_t uwDstModifyMult;
	int16_t uwHwait;
	int16_t uwSsel;
	int16_t uwUserShort;
	int32_t dUserLong;
	int32_t dReserved2;
	void    *pErrorFunction;
	void    *pLoadFunction;
	void    *pCallBackFunction;
	ADI_BOOT_HEADER *pHeader;
	void    *pTempBuffer;
	void    *pTempCurrent;
	int32_t dTempByteCount;
	int32_t dBlockCount;
	int32_t dClock;
	void    *pLogBuffer;
	void    *pLogCurrent;
	int32_t dLogByteCount;
} ADI_BOOT_DATA;

#else
# include "bootrom-asm-offsets.h"
#endif

/* Bit defines for ADI_BOOT_DATA->dFlags */
#define BFLAG_DMACODE_MASK 0x0000000F
#define BFLAG_SAFE         0x00000010
#define BFLAG_AUX          0x00000020
#define BFLAG_FILL         0x00000100
#define BFLAG_QUICKBOOT    0x00000200
#define BFLAG_CALLBACK     0x00000400
#define BFLAG_INIT         0x00000800
#define BFLAG_IGNORE       0x00001000
#define BFLAG_INDIRECT     0x00002000
#define BFLAG_FIRST        0x00004000
#define BFLAG_FINAL        0x00008000
#define BFLAG_HOOK         0x00400000
#define BFLAG_HDRINDIRECT  0x00800000
#define BFLAG_TYPE_MASK    0x00300000
#define BFLAG_TYPE_1       0x00000000
#define BFLAG_TYPE_2       0x00100000
#define BFLAG_TYPE_3       0x00200000
#define BFLAG_TYPE_4       0x00300000
#define BFLAG_FASTREAD     0x00400000
#define BFLAG_NOAUTO       0x01000000
#define BFLAG_PERIPHERAL   0x02000000
#define BFLAG_SLAVE        0x04000000
#define BFLAG_WAKEUP       0x08000000
#define BFLAG_NEXTDXE      0x10000000
#define BFLAG_RETURN       0x20000000
#define BFLAG_RESET        0x40000000
#define BFLAG_NONRESTORE   0x80000000

#endif
