/*
 *
 *	PROJECT				:	BFIN
 *	VERSION				:	2.0
 *	FILE				:	linkage.h
 *	MODIFIED DATE			:	29 jun 2004
 *	AUTHOR				:	BFin Project-ADI
 *	LOCATION			:	LG Soft India,Bangalore
 */

#ifndef _LINUX_LINKAGE_H
#define _LINUX_LINKAGE_H

#include <linux/config.h>

#ifdef __cplusplus
#define CPP_ASMLINKAGE		extern "C"
#else
#define CPP_ASMLINKAGE
#endif

#define asmlinkage CPP_ASMLINKAGE

#define SYMBOL_NAME_STR(X)	#X
#define SYMBOL_NAME(X)		X
#ifdef __STDC__
#define SYMBOL_NAME_LABEL(X)	X##:
#else
#define SYMBOL_NAME_LABEL(X)	X:
#endif

#define __ALIGN .align		4
#define __ALIGN_STR		".align 4"

#ifdef __ASSEMBLY__

#define ALIGN			__ALIGN
#define ALIGN_STR		__ALIGN_STR

#define ENTRY(name) \
	.globl SYMBOL_NAME(name); \
	ALIGN; \
	SYMBOL_NAME_LABEL(name)
#endif

#endif
