/*
 * U-boot - cplb_status.c
 *
 * Copyright (c) 2007 Analog Devices Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * Licensed under the GPL-2 or later.
 */

#include <common.h>
#include <command.h>
#include <asm/blackfin.h>
#include <asm/cplb.h>

extern const unsigned int icplb_table[page_descriptor_table_size][2];
extern const unsigned int dcplb_table[page_descriptor_table_size][2];

/*
 * Translate the PAGE_SIZE bits into a human string
 */
static const char *cplb_page_size(uint32_t data)
{
	static const char page_size_string_table[][4] = { "1K", "4K", "1M", "4M" };
	return page_size_string_table[(data & PAGE_SIZE_MASK) >> PAGE_SIZE_SHIFT];
}

/*
 * format a single cplb entry for us humans to read
 *
 * note that we only dump the first 20 bits of the data register since the
 *  rest are reserved -> noise
 */
static void show_table_entry(size_t n, uint32_t addr, uint32_t data)
{
	printf(" %2i 0x%p  0x%05X   %s     %c      %c\n",
		n, addr, data,
		cplb_page_size(data),
		(data & CPLB_VALID ? 'Y' : 'N'),
		(data & CPLB_LOCK ? 'Y' : 'N'));
}

/*
 * show a software cplb table
 *
 */
static void show_soft_table(const unsigned int table[page_descriptor_table_size][2])
{
	size_t i;
	printf("      Address     Data   Size  Valid  Locked\n");
	for (i = 0; i < page_descriptor_table_size; ++i)
		show_table_entry(i+1, table[i][0], table[i][1]);
}

/*
 * show a hardware cplb table
 */
static void show_hard_table(uint32_t *addr, uint32_t *data)
{
	size_t i;
	printf("      Address     Data   Size  Valid  Locked\n");
	for (i = 1; i <= 16; ++i) {
		show_table_entry(i, *addr, *data);
		++addr;
		++data;
	}
}

/*
 * display current instruction and data cplb tables
 */
int do_cplbinfo(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	printf("Instruction CPLB table (%s):\n", "hardware");
	show_hard_table((uint32_t*)ICPLB_ADDR0, (uint32_t*)ICPLB_DATA0);
	printf("Instruction CPLB table (%s):\n", "software");
	show_soft_table(icplb_table);

	printf("Data CPLB table (%s):\n", "hardware");
	show_hard_table((uint32_t*)DCPLB_ADDR0, (uint32_t*)DCPLB_DATA0);
	printf("Data CPLB table (%s):\n", "software");
	show_soft_table(dcplb_table);

	return 0;
}

U_BOOT_CMD(cplbinfo, 2, 0, do_cplbinfo,
	"cplbinfo- display current CPLB tables\n",
	"\n"
	"    - display current CPLB tables\n");
