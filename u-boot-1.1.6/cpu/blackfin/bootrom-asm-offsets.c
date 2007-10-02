/* A little trick taken from the kernel asm-offsets.h where we convert
 * the C structures automatically into a bunch of defines for use in
 * the assembly files.
 */

#include <linux/stddef.h>
#include "bootrom.h"

#define _DEFINE(sym, val) asm volatile("\n->" #sym " %0 " #val : : "i" (val))
#define DEFINE(s, m) _DEFINE(offset_##s##_##m, offsetof(s, m))

int main(int argc, char *argv[])
{
	DEFINE(header_struct, nBlockCode);
	DEFINE(header_struct, pTargetAddress);
	DEFINE(header_struct, nByteCount);
	DEFINE(header_struct, nArgument);

	DEFINE(buffer_struct, pSource);
	DEFINE(buffer_struct, nByteCount);

	DEFINE(boot_struct, pSource);
	DEFINE(boot_struct, pDestination);
	DEFINE(boot_struct, pControlRegister);
	DEFINE(boot_struct, pDmaControlRegister);
	DEFINE(boot_struct, nControlValue);
	DEFINE(boot_struct, nByteCount);
	DEFINE(boot_struct, nFlags);
	DEFINE(boot_struct, nDataWidth);
	DEFINE(boot_struct, nSrcModifyMult);
	DEFINE(boot_struct, nDstModifyMult);
	DEFINE(boot_struct, nHwait);
	DEFINE(boot_struct, nSsel);
	DEFINE(boot_struct, nUserShort);
	DEFINE(boot_struct, nUserLong);
	DEFINE(boot_struct, nReserved2);
	DEFINE(boot_struct, pErrorFunction);
	DEFINE(boot_struct, pLoadFunction);
	DEFINE(boot_struct, pCallBackFunction);
	DEFINE(boot_struct, pHeader);
	DEFINE(boot_struct, pTempBuffer);
	DEFINE(boot_struct, nTempByteCount);
	DEFINE(boot_struct, nBlockCount);
	DEFINE(boot_struct, nClock);

	return 0; /* silence warnings */
}
