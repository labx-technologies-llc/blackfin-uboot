/*
 * blackfin-config-pre.h - common defines for Blackfin boards in config.h
 *
 * Copyright (c) 2007 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef __ASM_BLACKFIN_CONFIG_PRE_H__
#define __ASM_BLACKFIN_CONFIG_PRE_H__

/* Misc helper functions */
#define XMK_STR(x) #x
#define MK_STR(x) XMK_STR(x)

/* Configurable Blackfin-specific monitor commands */
#define CFG_BFIN_CMD_BOOTLDR  0x01
#define CFG_BFIN_CMD_CPLBINFO 0x02

/* CPU defines -- your config needs to select this via BFIN_CPU */
#define ADSP_BF531		531
#define ADSP_BF532		532
#define ADSP_BF533		533
#define ADSP_BF534		534
#define ADSP_BF536		536
#define ADSP_BF537		537
#define ADSP_BF542		542
#define ADSP_BF544		544
#define ADSP_BF548		548
#define ADSP_BF549		549
#define ADSP_BF561		561

/* Family defines -- automatically selected by blackfin-config-post.h */
#define ADSP_BF53X		53
#define ADSP_BF54X		54
#define ADSP_BF56X		56

/* Bootmode defines -- your config needs to select this via BFIN_BOOT_MODE
 * Depending on your cpu, some of these may not be valid, check your HRM.
 * The actual values here are meaningless as long as they're unique.
 */
#define BFIN_BOOT_BYPASS      1
#define BFIN_BOOT_PARA        2
#define BFIN_BOOT_SPI_MASTER  3
#define BFIN_BOOT_SPI_SLAVE   4
#define BFIN_BOOT_TWI_MASTER  5
#define BFIN_BOOT_TWI_SLAVE   6
#define BFIN_BOOT_UART        7
#define BFIN_BOOT_IDLE        8
#define BFIN_BOOT_FIFO        9
#define BFIN_BOOT_MEM         10
#define BFIN_BOOT_16HOST_DMA  11
#define BFIN_BOOT_8HOST_DMA   12

#endif
