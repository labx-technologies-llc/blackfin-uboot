/*
 * File: arch-common/anomaly.h
 * Bugs: Enter bugs at http://blackfin.uclinux.org/
 *
 * Copyright (C) 2004-2007 Analog Devices Inc.
 * Licensed under the GPL-2 or later.
 */

#ifndef _ARCH_COMMON_ANOMALY_H_
#define _ARCH_COMMON_ANOMALY_H_

/* a little bit of glue so we can use kernel headers
 * without any modification.
 */
#ifdef __ADSPBF533__
# define CONFIG_BF533
#endif
#ifdef __ADSPBF536__
# define CONFIG_BF536
#endif
#ifdef __ADSPBF537__
# define CONFIG_BF537
#endif
#include <asm/arch/anomaly.h>
#undef CONFIG_BF533
#undef CONFIG_BF536
#undef CONFIG_BF537

#endif
