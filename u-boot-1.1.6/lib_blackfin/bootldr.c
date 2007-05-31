/*
 * U-boot - bootldr.c
 *
 * Copyright (c) 2005-2007 Analog Devices Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * Licensed under the GPL-2 or later.
 */

#include <config.h>
#include <common.h>
#include <command.h>
#include <asm/blackfin.h>

#ifdef CONFIG_BFIN_BOOTLDR

/*
 * the bootldr command loads an address, checks to see if there
 *   is a Boot stream that the on-chip BOOTROM can understand,
 *   and loads it via the BOOTROM Callback. It is possible
 *   to also add booting from SPI, or TWI, but this function does
 *   not currently support that.
 */

int do_bootldr(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	uint32_t addr, entry;
	uint32_t *data;

	/* Get the address */
	if (argc < 2)
		addr = load_addr;
	else
		addr = simple_strtoul(argv[1], NULL, 16);

	/* Check if it is a LDR file */
	data = (uint32_t *)addr;
	if (*data == 0xFF800060 || *data == 0xFF800040 || *data == 0xFF800020) {
		/* We want to boot from FLASH or SDRAM */
		entry = _BOOTROM_BOOT_DXE_FLASH;
		printf("## Booting ldr image at 0x%08lx ...\n", addr);

		icache_disable();
		dcache_disable();

		__asm__(
			"R7 = %0;\n"
			"P0 = %1;\n"
			"JUMP (P0);\n":
			: "d"(addr), "a"(entry)
			: "R7", "P0");

	} else
		printf("## No ldr image at address 0x%08lx\n", addr);

	return 0;
}

U_BOOT_CMD(bootldr, 2, 0, do_bootldr,
	"bootldr - boot ldr image from memory\n",
	"[addr]\n"
	"    - boot ldr image stored in memory\n");

#endif
