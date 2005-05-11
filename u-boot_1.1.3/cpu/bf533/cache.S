
#include <asm-blackfin/linkage.h>

.text
.align 2
ENTRY(blackfin_icache_flush_range)
	R2 = -32;
	R2 = R0 & R2;
	P0 = R2;
	P1 = R1;
	CSYNC;
1:
	IFLUSH[P0++];
	CC = P0 < P1(iu);
	IF CC JUMP 1b(bp);
	IFLUSH[P0];
	SSYNC;
	RTS;

ENTRY(blackfin_dcache_flush_range)
	R2 = -32;
        R2 = R0 & R2;
        P0 = R2;
        P1 = R1;
        CSYNC;
1:
        FLUSH[P0++];
        CC = P0 < P1(iu);
        IF CC JUMP 1b(bp);
        FLUSH[P0];
        SSYNC;
        RTS;
