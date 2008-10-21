/*
 * U-boot - main board file
 *
 * Copyright (c) 2008 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <common.h>
#include <config.h>
#include <command.h>
#include <net.h>
#include <netdev.h>
#include <spi.h>
#include <asm/blackfin.h>
#include <asm/io.h>
#include <asm/mach-common/bits/otp.h>

DECLARE_GLOBAL_DATA_PTR;

int checkboard(void)
{
	printf("Board: ADI BF518F EZ-Board board\n");
	printf("       Support: http://blackfin.uclinux.org/\n");
	return 0;
}

phys_size_t initdram(int board_type)
{
	gd->bd->bi_memstart = CFG_SDRAM_BASE;
	gd->bd->bi_memsize = CFG_MAX_RAM_SIZE;
	return CFG_MAX_RAM_SIZE;
}

#if defined(CONFIG_BFIN_MAC)
void board_get_enetaddr(uchar *mac_addr)
{
#if 0
	/* the MAC is stored in OTP memory page 0xDF */
	uint32_t ret;
	uint64_t otp_mac;

	ret = bfrom_OtpRead(0xDF, OTP_LOWER_HALF, &otp_mac);
	if (!(ret & OTP_MASTER_ERROR)) {
		uchar *otp_mac_p = (uchar *)&otp_mac;

		for (ret = 0; ret < 6; ++ret)
			mac_addr[ret] = otp_mac_p[5 - ret];

		if (is_valid_ether_addr(mac_addr))
			return;
	}
#endif

	puts("Warning: Generating 'random' MAC address\n");

	/* make something up */
	const char s[] = __DATE__;
	size_t i;
	u32 cycles;
	for (i = 0; i < 6; ++i) {
		asm("%0 = CYCLES;" : "=r" (cycles));
		mac_addr[i] = cycles ^ s[i];
	}
	mac_addr[0] = (mac_addr[0] | 0x02) & ~0x01; /* make it local unicast */
}

int board_eth_init(bd_t *bis)
{
	int ret = -1;
	struct spi_slave *slave = spi_setup_slave(0, 1, 5000000, SPI_MODE_3);
	if (slave) {
		if (!spi_claim_bus(slave)) {
			unsigned char dout[3] = { 2, 1, 1, };
			unsigned char din[3];
			ret = spi_xfer(slave, sizeof(dout) * 8, dout, din, SPI_XFER_BEGIN | SPI_XFER_END);
			if (!ret)
				bfin_EMAC_initialize(bis);
			spi_release_bus(slave);
		}
		spi_free_slave(slave);
	}
	return ret;
}
#endif
