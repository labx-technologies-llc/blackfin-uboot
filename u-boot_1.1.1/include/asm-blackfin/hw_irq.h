/*
 * linux/arch/$(ARCH)/platform/$(PLATFORM)/hw_irq.h
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 *
 * BlackFin (ADI) assembler restricted values by Ted Ma <mated@sympatico.ca>
 * Copyright (c) 2002 Arcturus Networks Inc. (www.arcturusnetworks.com)
 * Copyright (c) 2002 Lineo, Inc <mattw@lineo.com>
 *
 *	PROJECT				:	BFIN
 *	VERSION				:	2.0
 *	FILE				:	hw_irq.h
 *	MODIFIED DATE			:	29 jun 2004
 *	AUTHOR				:	BFin Project-ADI
 *	LOCATION			:	LG Soft India,Bangalore
 */

#include <linux/config.h>
#ifdef CONFIG_EZKIT533
#include <asm/bf533_irq.h>
#endif
#ifdef CONFIG_STAMP
#include <asm/bf533_irq.h>
#endif
