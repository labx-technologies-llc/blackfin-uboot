#define __ASSEMBLY__
#include <asm/blackfin.h>
.global __start
__start:
	sp.l = LO(L1_SRAM_SCRATCH_END - 20);
	sp.h = HI(L1_SRAM_SCRATCH_END - 20);
	call _initcode;
1:
	emuexcpt;
	jump 1b;
.size __start, .-__start
