/* Changed by Lineo Inc. May 2001
 *
 * Project		: BFin
 * Version		: 2.0
 * File			: string.h
 * Modified date	: 29 jun 2004
 * Author		: BFin Project-ADI
 * Location		: LG Soft India,Bangalore
 */

#ifndef _BLACKFINNOMMU_STRING_H_
#define _BLACKFINNOMMU_STRING_H_

#ifdef __KERNEL__		/* only set these up for kernel code */

/*
 * Still not sure whether the optimised inline asm 
 * are small.
 */

#include <asm/setup.h>
#include <asm/page.h>
#include <asm/bf533.h>

#define __HAVE_ARCH_STRCPY
static inline char *strcpy(char *dest, const char *src)
{
	char *xdest = dest;
	char temp = 0;

	__asm__ __volatile__
	    ("1:\t%2 = B [%1++] (Z);\n\t"
	     "B [%0++] = %2;\n\t"
	     "CC = %2;\n\t"
	     "if cc jump 1b (bp);\n":"=a"(dest), "=a"(src), "=d"(temp)
	     :"0"(dest), "1"(src), "2"(temp):"memory");
	return xdest;
}

#define __HAVE_ARCH_STRNCPY
static inline char *strncpy(char *dest, const char *src, size_t n)
{
	char *xdest = dest;
	char temp = 0;

	if (n == 0)
		return xdest;

	__asm__ __volatile__
	    ("1:\t%3 = B [%1++] (Z);\n\t"
	     "B [%0++] = %3;\n\t"
	     "CC = %3;\n\t"
	     "if ! cc jump 2f;\n\t"
	     "%2 += -1;\n\t"
	     "CC = %2 == 0;\n\t"
	     "if ! cc jump 1b (bp);\n"
	     "2:\n":"=a"(dest), "=a"(src), "=da"(n), "=d"(temp)
	     :"0"(dest), "1"(src), "2"(n), "3"(temp)
	     :"memory");

	return xdest;
}

#define __HAVE_ARCH_STRCMP
static inline int strcmp(const char *cs, const char *ct)
{
	char __res1, __res2;

	__asm__("1:\t%2 = B[%0++] (Z);\n\t"	/* get *cs */
		"%3 = B[%1++] (Z);\n\t"	/* get *ct */
		"CC = %2 == %3;\n\t"	/* compare a byte */
		"if ! cc jump 2f;\n\t"	/* not equal, break out */
		"CC = %2;\n\t"	/* at end of cs? */
		"if cc jump 1b (bp);\n\t"	/* no, keep going */
		"jump.s 3f;\n"	/* strings are equal */
		"2:\t%2 = %2 - %3;\n"	/* *cs - *ct */
      "3:\n":	"=a"(cs), "=a"(ct), "=d"(__res1),
		"=d"(__res2)
      :	"0"(cs), "1"(ct));

	return __res1;
}

#define __HAVE_ARCH_STRNCMP
static inline int strncmp(const char *cs, const char *ct, size_t count)
{
	char __res1, __res2;

	if (!count)
		return 0;
	__asm__("1:\t%3 = B[%0++] (Z);\n\t"	/* get *cs */
		"%4 = B[%1++] (Z);\n\t"	/* get *ct */
		"CC = %3 == %4;\n\t"	/* compare a byte */
		"if ! cc jump 3f;\n\t"	/* not equal, break out */
		"CC = %3;\n\t"	/* at end of cs? */
		"if ! cc jump 4f;\n\t"	/* yes, all done */
		"%2 += -1;\n\t"	/* no, adjust count */
		"CC = %2 == 0;\n\t" "if ! cc jump 1b;\n"	/* more to do, keep going */
		"2:\t%3 = 0;\n\t"	/* strings are equal */
		"jump.s    4f;\n" "3:\t%3 = %3 - %4;\n"	/* *cs - *ct */
      "4:":	"=a"(cs), "=a"(ct), "=da"(count), "=d"(__res1),
		"=d"(__res2)
      :	"0"(cs), "1"(ct), "2"(count));

	return __res1;
}

/**
 * memcpy - Copy one area of memory to another
 * @dest: Where to copy to
 * @src: Where to copy from
 * @count: The size of the area.
 *
 * You should not use this function to access IO space, use memcpy_toio()
 * or memcpy_fromio() instead.
 */
#define __HAVE_ARCH_MEMCPY
extern inline void * memcpy(void * dest,const void *src,size_t count)
{
        char *tmp = (char *) dest, *s = (char *) src;

	if ((tmp >= (char*)L1_ISRAM) && (tmp < (char*)L1_ISRAM_END))
	{
		/* Copy sram functions from sdram to sram */

		/* Setup destination start address */
		*(volatile unsigned long *)MDMA_D0_START_ADDR = (unsigned long)dest;
		/* Setup destination xcount */
		*(volatile unsigned short *)MDMA_D0_X_COUNT = count / 2 ;
		/* Setup destination xmodify */
		*(volatile unsigned short *)MDMA_D0_X_MODIFY = 2;

		/* Setup Source start address */
		*(volatile unsigned long *)MDMA_S0_START_ADDR = (unsigned long)src;
		/* Setup Source xcount */
		*(volatile unsigned short *)MDMA_S0_X_COUNT = count /2 ;
		/* Setup Source xmodify */
		*(volatile unsigned short *)MDMA_S0_X_MODIFY = 2;

		/* Set word size to 16, set to read, enable interrupt for wakeup */
		/* Enable source DMA */
		*(volatile unsigned short *)MDMA_S0_CONFIG = (WDSIZE16 | DMAEN);
		asm("ssync;");
		/* Enable Destination DMA */
		*(volatile unsigned short *)MDMA_D0_CONFIG = (DI_EN | WDSIZE16 | WNR | DMAEN);
	
		/* Go into idle and wait for wakeup that indicates DMA is done */
		asm("idle;");
		/* Clear interrupt */
		*(volatile unsigned short *)MDMA_D0_IRQ_STATUS = 0x1;
	}
	else
	{
        	while (count--)
                	*tmp++ = *s++;

	}

        return dest;
}

extern void *memset(void *s, int c, size_t count);
extern int memcmp(const void *, const void *, __kernel_size_t);

#else				/* KERNEL */

/*
 * let user libraries deal with these,
 * IMHO the kernel has no place defining these functions for user apps
 */

#define __HAVE_ARCH_STRCPY	1
#define __HAVE_ARCH_STRNCPY	1
#define __HAVE_ARCH_STRCAT	1
#define __HAVE_ARCH_STRNCAT	1
#define __HAVE_ARCH_STRCMP	1
#define __HAVE_ARCH_STRNCMP	1
#define __HAVE_ARCH_STRNICMP	1
#define __HAVE_ARCH_STRCHR	1
#define __HAVE_ARCH_STRRCHR	1
#define __HAVE_ARCH_STRSTR	1
#define __HAVE_ARCH_STRLEN	1
#define __HAVE_ARCH_STRNLEN	1
#define __HAVE_ARCH_MEMSET	1
#define __HAVE_ARCH_MEMCPY	1
#define __HAVE_ARCH_MEMMOVE	1
#define __HAVE_ARCH_MEMSCAN	1
#define __HAVE_ARCH_MEMCMP	1
#define __HAVE_ARCH_MEMCHR	1
#define __HAVE_ARCH_STRTOK	1

#endif				/* KERNEL */

#endif				/* _BLACKFIN_STRING_H_ */
