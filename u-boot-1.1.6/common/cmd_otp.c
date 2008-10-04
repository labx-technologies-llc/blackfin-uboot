/*
 * cmd_otp.c - interface to Blackfin on-chip One-Time-Programmable memory
 *
 * Copyright (c) 2007 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

/* There are 512 128-bit "pages" (0x000 through 0x1FF).
 * The pages are accessable as 64-bit "halfpages" (an upper and lower half).
 * The pages are not part of the memory map.  There is an OTP controller which
 * handles scanning in/out of bits.  While access is done through OTP MMRs,
 * the bootrom provides C-callable helper functions to handle the interaction.
 */

#include <config.h>
#include <common.h>
#include <command.h>

#if (CONFIG_BFIN_COMMANDS & CFG_BFIN_CMD_OTP)

#include <asm/blackfin.h>
#include <asm/mach-common/bits/otp.h>

static const char *otp_strerror(uint32_t err)
{
	switch (err) {
	case 0:                   return "no error";
	case OTP_WRITE_ERROR:     return "OTP fuse write error";
	case OTP_READ_ERROR:      return "OTP fuse read error";
	case OTP_ACC_VIO_ERROR:   return "invalid OTP address";
	case OTP_DATA_MULT_ERROR: return "multiple bad bits detected";
	case OTP_ECC_MULT_ERROR:  return "error in ECC bits";
	case OTP_PREV_WR_ERROR:   return "space already written";
	case OTP_DATA_SB_WARN:    return "single bad bit in half page";
	case OTP_ECC_SB_WARN:     return "single bad bit in ECC";
	default:                  return "unknown error";
	}
}

#define lowup(x) ((x) % 2 ? "upper" : "lower")

static uint32_t otp_default_timing;

int do_otp(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	uint32_t ret, base_flags = 0;
	bool prompt_user = true;

	if (argc < 4)
		goto usage;

	if (otp_default_timing == 0)
		otp_default_timing = bfin_read_OTP_TIMING();

	uint32_t otp_timing;
	uint32_t (*otp_func)(uint32_t page, uint32_t flags, uint64_t *page_content);
	if (!strcmp(argv[1], "read")) {
		otp_func = bfrom_OtpRead;
		base_flags = 0;
		otp_timing = otp_default_timing;
		prompt_user = false;
	} else if (!strcmp(argv[1], "write")) {
		otp_func = bfrom_OtpWrite;
		base_flags = OTP_CHECK_FOR_PREV_WRITE;

		uint32_t otp_tp1, otp_tp2, otp_tp3;
		/* OTP_TP1 = 1000 / sclk_period (in nanoseconds)
		 * OTP_TP1 = 1000 / (1 / get_sclk() * 10^9)
		 * OTP_TP1 = (1000 * get_sclk()) / 10^9
		 * OTP_TP1 = get_sclk() / 10^6
		 */
		otp_tp1 = get_sclk() / 1000000;
		/* OTP_TP2 = 400 / (2 * sclk_period)
		 * OTP_TP2 = 400 / (2 * 1 / get_sclk() * 10^9)
		 * OTP_TP2 = (400 * get_sclk()) / (2 * 10^9)
		 * OTP_TP2 = (2 * get_sclk()) / 10^7
		 */
		otp_tp2 = (2 * get_sclk() / 10000000) << 8;
		/* OTP_TP2 = magic constant */
		otp_tp3 = (0x1401) << 15;
		otp_timing = otp_tp1 | otp_tp2 | otp_tp3;

		if (!strcmp(argv[2], "--force")) {
			prompt_user = false;
			argv[2] = argv[1];
			argv++;
			--argc;
		}

		if (!strcmp(argv[2], "--nolock")) {
			argv[2] = argv[1];
			argv++;
			--argc;
		} else
			base_flags |= OTP_LOCK;
	} else {
 usage:
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	uint64_t *addr = (uint64_t *)simple_strtoul(argv[2], NULL, 16);
	uint32_t page = simple_strtoul(argv[3], NULL, 16);
	uint32_t flags;
	size_t i, count;
	ulong half;

	if (argc > 4)
		count = simple_strtoul(argv[4], NULL, 16);
	else
		count = 2;

	if (argc > 5) {
		half = simple_strtoul(argv[5], NULL, 16);
		if (half != 0 && half != 1) {
			puts("Error: 'half' can only be '0' or '1'\n");
			goto usage;
		}
	} else
		half = 0;

	/* do to the nature of OTP, make sure users are sure */
	if (prompt_user) {
		printf(
			"Writing one time programmable memory\n"
			"Make sure your operating voltages and temperature are within spec\n"
			"   source address:  0x%p\n"
			"   OTP destination: %s page 0x%03X - %s page 0x%03X\n"
			"   number to write: %ld halfpages\n"
			" type \"YES\" (no quotes) to confirm: ",
			addr,
			lowup(half), page,
			lowup(half + count - 1), page + (half + count - 1) / 2,
			half + count
		);

		i = 0;
		while (1) {
			if (tstc()) {
				const char exp_ans[] = "YES\r";
				char c;
				putc(c = getc());
				if (exp_ans[i++] != c) {
					printf(" Aborting\n");
					return 1;
				} else if (!exp_ans[i]) {
					puts("\n");
					break;
				}
			}
		}
	}

	/* Enable writing as need be */
	if (otp_func == bfrom_OtpWrite) {
		ret = bfrom_OtpCommand(OTP_INIT, otp_timing);
		if (ret) {
			printf("Error: otp init returned %u for timing 0x%08x\n", ret, otp_timing);
			return 1;
		}
	}

	printf("OTP memory %s: addr 0x%08lx  page 0x%03X  count %ld ... ",
		argv[1], addr, page, count);

	ret = 0;
	for (i = half; i < count + half; ++i) {
		flags = base_flags | (i % 2 ? OTP_UPPER_HALF : OTP_LOWER_HALF);
		ret = otp_func(page, flags, addr);
		if (ret & 0x1)
			break;
		else if (ret)
			puts("W");
		else
			puts(".");
		++addr;
		if (i % 2)
			++page;
	}
	if (ret & 0x1)
		printf("\nERROR at page 0x%03X (%s-halfpage): 0x%03X: %s\n",
			page, lowup(i), ret, otp_strerror(ret));
	else
		puts(" done\n");

	/* Disable writing to prevent random behavior */
	if (otp_func == bfrom_OtpWrite) {
		bfrom_OtpCommand(OTP_CLOSE, 0);
		bfrom_OtpCommand(OTP_INIT, otp_default_timing);
		bfrom_OtpCommand(OTP_CLOSE, 0);
	}

	return ret;
}

U_BOOT_CMD(otp, 8, 0, do_otp,
	"otp     - One-Time-Programmable sub-system\n",
	"read <addr> <page> [count] [half]\n"
	"otp write [--force] [--nolock] <addr> <page> [count] [half]\n"
	"    - read/write 'count' half-pages starting at page 'page' (offset 'half')\n");

#endif
