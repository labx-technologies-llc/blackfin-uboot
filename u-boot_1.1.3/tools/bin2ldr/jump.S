#include "bin2ldr.h"
	p0.l = (APP_ENTRY & 0xFFFF);
	p0.h = (APP_ENTRY >> 16);
	jump (p0);	 
