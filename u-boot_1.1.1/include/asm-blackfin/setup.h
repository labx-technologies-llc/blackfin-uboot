/*
 * asm/setup.h -- Definition of the Linux/Frio setup information
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 *
 * Copyright Lineo, Inc 2001 Tony Kou
 *
 *
 *	PROJECT				:	BFIN
 *	VERSION				:	2.0
 *	FILE				:	setup.h
 *	MODIFIED DATE			:	29 jun 2004
 *	AUTHOR				:	BFin Project-ADI
 *	LOCATION			:	LG Soft India,Bangalore
 */

#ifndef _FRIO_SETUP_H
#define _FRIO_SETUP_H

#include <linux/config.h>

/*
 * Linux/Frio Architectures
 */

#define MACH_NISA	1

#ifdef __KERNEL__

#ifndef __ASSEMBLY__
extern unsigned long frio_machtype;
#endif				/* !__ASSEMBLY__ */

#if defined(CONFIG_NISA)
#define MACH_IS_NISA (frio_machtype == MACH_NISA)
#endif

#ifndef MACH_TYPE
#define MACH_TYPE (frio_machtype)
#endif

#endif				/* __KERNEL__ */

/*
 * CPU, FPU and MMU types
 *
 * Note: we don't need now:
 *
 */

#ifndef __ASSEMBLY__
extern unsigned long frio_cputype;
#ifdef CONFIG_VME
extern unsigned long vme_brdtype;
#endif

/*
 *  Miscellaneous
 */

#define NUM_MEMINFO	4
#define CL_SIZE		256

extern int frio_num_memory;	/* # of memory blocks found (and used) */
extern int frio_realnum_memory;	/* real # of memory blocks found */
extern struct mem_info frio_memory[NUM_MEMINFO];	/* memory description */

struct mem_info {
	unsigned long addr;	/* physical address of memory chunk */
	unsigned long size;	/* length of memory chunk (in bytes) */
};
#endif				/* __ASSEMBLY__ */

#endif				/* _FRIO_SETUP_H */
