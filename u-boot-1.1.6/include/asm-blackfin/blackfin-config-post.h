/*
 * blackfin-config-post.h - setup common defines for Blackfin boards based on config.h
 *
 * Copyright (c) 2007 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef __ASM_BLACKFIN_CONFIG_POST_H__
#define __ASM_BLACKFIN_CONFIG_POST_H__

/* Check to make sure everything fits in external RAM */
#if ((CFG_MONITOR_BASE + CFG_MONITOR_LEN) > CFG_MAX_RAM_SIZE)
# error Memory Map does not fit into configuration
#endif

/* Sanity check BFIN_CPU */
#ifndef BFIN_CPU
# error BFIN_CPU: your board config needs to define this
#endif

/* Set default CONFIG_VCO_HZ if need be */
#if !defined(CONFIG_VCO_HZ)
# if (CONFIG_CLKIN_HALF == 0)
#  define CONFIG_VCO_HZ (CONFIG_CLKIN_HZ * CONFIG_VCO_MULT)
# else
#  define CONFIG_VCO_HZ ((CONFIG_CLKIN_HZ * CONFIG_VCO_MULT) / 2)
# endif
#endif

/* Set default CONFIG_CCLK_HZ if need be */
#if !defined(CONFIG_CCLK_HZ)
# if (CONFIG_PLL_BYPASS == 0)
#  define CONFIG_CCLK_HZ (CONFIG_VCO_HZ / CONFIG_CCLK_DIV)
# else
#  define CONFIG_CCLK_HZ CONFIG_CLKIN_HZ
# endif
#endif

/* Set default CONFIG_SCLK_HZ if need be */
#if !defined(CONFIG_SCLK_HZ)
# if (CONFIG_PLL_BYPASS == 0)
#  define CONFIG_SCLK_HZ (CONFIG_VCO_HZ / CONFIG_SCLK_DIV)
# else
#  define CONFIG_SCLK_HZ CONFIG_CLKIN_HZ
# endif
#endif

#endif
