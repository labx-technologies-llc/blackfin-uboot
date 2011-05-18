/*
 * U-boot - main board file
 *
 * Copyright (c) 2008-2011 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <common.h>
#include <netdev.h>
#include <asm/blackfin.h>
#include <asm/portmux.h>

int checkboard(void)
{
	printf("Board: ADI BF609 EZ-Kit board\n");
	printf("       Support: http://blackfin.uclinux.org/\n");
	return 0;
}

int board_early_init_f(void)
{
	static const unsigned short pins[] = {
		P_A4, P_A5, P_A6, P_A7, P_A8, P_A9, P_A10, P_A11, P_A12, P_A13,
		P_A14, P_A15, P_A16, P_A17, P_A18, P_A19, P_A20, P_A21, P_A22,
		P_A23, P_A24, P_A25, P_AMS1, P_AMS2, P_AMS3, 0,
	};
	peripheral_request_list(pins, "smc");

	return 0;
}

#ifdef CONFIG_DESIGNWARE_ETH
int board_eth_init(bd_t *bis)
{
	int ret = 0;

	if (CONFIG_DW_PORTS & 1) {
		static const unsigned short pins[] = P_RMII0;
		if (!peripheral_request_list(pins, "emac0"))
			ret += designware_initialize(0, EMAC0_MACCFG, 0);
	}

	if (CONFIG_DW_PORTS & 2) {
		static const unsigned short pins[] = P_RMII1;
		if (!peripheral_request_list(pins, "emac1"))
			ret += designware_initialize(1, EMAC1_MACCFG, 0);
	}

	return ret;
}
#endif
