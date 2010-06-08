/*
 * U-boot - main board file
 *
 * Copyright (c) 2010 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <common.h>
#include <config.h>
#include <command.h>
#include <spi.h>
#include <asm/blackfin.h>
#include <asm/mach-common/bits/otp.h>
#include <asm/mach-common/bits/spi.h>

DECLARE_GLOBAL_DATA_PTR;

int checkboard(void)
{
	printf("Board: ADI BF527 AD7160-EVAL board\n");
	printf("       Support: http://blackfin.uclinux.org/\n");
	return 0;
}

int misc_init_r(void)
{
	return 0;
}
