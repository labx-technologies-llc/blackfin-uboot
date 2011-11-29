/*
 * U-boot - main board file
 *
 * Copyright (c) 2008-2011 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef __SOFT_SWITCH_H__
#define __SOFT_SWITCH_H__

#define SW_CAN_EN		0
#define SW_CAN_STS		1
#define SW_CAN0_ERR_EN		2
#define SW_CAN0RX_EN		3
#define SW_CNT0UD_EN		4
#define SW_CNT0DG_EN		5
#define SW_CNT0ZM_EN		6
#define SW_RMII_CLK_EN		7

#define SW_SD_CD_EN		26
#define SW_SD_WP_EN		27


int setup_soft_switch(int index, int level);
#endif
