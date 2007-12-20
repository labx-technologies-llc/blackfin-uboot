/*
 * U-boot - cmd_cache_dump.c
 *
 * Copyright (c) 2007 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <config.h>
#include <common.h>
#include <command.h>

#if CONFIG_BFIN_COMMANDS & CFG_BFIN_CMD_CACHE_DUMP

#include <asm/blackfin.h>

static int check_limit(const char *type, size_t start_limit, size_t end_limit, size_t start, size_t end)
{
	if (start >= start_limit && start <= end_limit && \
	    end <= end_limit && end >= start_limit && \
	    start <= end)
		return 0;

	printf("%s limit violation: %i <= (user:%i) <= (user:%i) <= %i\n",
		type, start_limit, start, end, end_limit);
	return 1;
}

int do_icache_dump(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int cache_status = icache_status();

	if (cache_status)
		icache_disable();

	uint32_t cmd_base, tag, cache_upper, cache_lower;

	size_t way, way_start = 0, way_end = 3;
	size_t sbnk, sbnk_start = 0, sbnk_end = 3;
	size_t set, set_start = 0, set_end = 31;
	size_t dw;

	if (argc > 1) {
		way_start = way_end = simple_strtoul(argv[1], NULL, 10);
		if (argc > 2) {
			sbnk_start = sbnk_end = simple_strtoul(argv[2], NULL, 10);
			if (argc > 3)
				set_start = set_end = simple_strtoul(argv[3], NULL, 10);
		}
	}

	if (check_limit("way", 0, 3, way_start, way_end) || \
	    check_limit("subbank", 0, 3, sbnk_start, sbnk_end) || \
	    check_limit("set", 0, 31, set_start, set_end))
		return 1;

	puts("Way:Subbank:Set: [valid-tag lower upper] {invalid-tag lower upper}...\n");

	for (way = way_start; way <= way_end; ++way) {
		for (sbnk = sbnk_start; sbnk <= sbnk_end; ++sbnk) {
			for (set = set_start; set <= set_end; ++set) {
				printf("%i:%i:%2i: ", way, sbnk, set);
				for (dw = 0; dw < 4; ++dw) {
					if (ctrlc())
						return 1;

					cmd_base = \
						(way  << 26) | \
						(sbnk << 16) | \
						(set  <<  5) | \
						(dw   <<  3);

					/* first read the tag */
					bfin_write_ITEST_COMMAND(cmd_base | 0x0);
					SSYNC();
					tag = bfin_read_ITEST_DATA0();
					printf("%c%08x ", (tag & 0x1 ? ' ' : '{'), tag);

					/* grab the data at this loc */
					bfin_write_ITEST_COMMAND(cmd_base | 0x4);
					SSYNC();
					cache_lower = bfin_read_ITEST_DATA0();
					cache_upper = bfin_read_ITEST_DATA1();
					printf("%08x %08x%c ", cache_lower, cache_upper, (tag & 0x1 ? ' ' : '}'));
				}
				puts("\n");
			}
		}
	}

	if (cache_status)
		icache_enable();

	return 0;
}

U_BOOT_CMD(icache_dump, 4, 0, do_icache_dump,
	"icache_dump - dump current instruction cache\n",
	"[way] [subbank] [set]");

#endif
