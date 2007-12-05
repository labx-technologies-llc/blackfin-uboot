/*
 * smsc9118_eeprom.c - EEPROM interface to SMSC9118 parts.
 *
 * Copyright 2004-2007 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 *
 * Based on smc91111_eeprom.c which:
 * Heavily borrowed from the following peoples GPL'ed software:
 *  - Wolfgang Denk, DENX Software Engineering, wd@denx.de
 *       Das U-boot
 *  - Ladislav Michl ladis@linux-mips.org
 *       A rejected patch on the U-Boot mailing list
 */

#include <common.h>
#include <exports.h>

#ifdef CONFIG_DRIVER_SMSC9118

#include "../drivers/smsc9118.h"

void dump_reg(void);
void dump_eeprom(void);
int write_eeprom_reg(u8, u8);
void copy_from_eeprom(void);
void print_MAC(void);
u8 read_eeprom_reg(u8);
void print_macaddr(void);
static void write_stuff(char *line);
static ulong GetMacReg(int Reg);
static int SetMacReg(int Reg, ulong Value);

/**
 *	smsc_ctrlc - detect press of CTRL+C (common ctrlc() isnt exported!?)
 */
static int smsc_ctrlc(void)
{
	return (tstc() && getc() == 0x03);
}

/**
 *	usage - dump usage information
 */
static void usage(void)
{
	puts(
		"LAN9x18 MAC/EEPROM Commands:\n"
		" P : Print the MAC addresses\n"
		" D : Dump the LAN9x18 EEPROM contents\n"
		" M : Dump the LAN9x18 MAC contents\n"
		" C : Copy the MAC address from the EEPROM to the LAN9x18\n"
		" W : Write a register in the EEPROM or in the MAC\n"
		" Q : Quit\n"
		"\n"
		"Some commands take arguments:\n"
		" W <E|M> <register> <value>\n"
		"    E: EEPROM   M: MAC\n"
	);
}

/**
 *	smsc9118_init - get the MAC/EEPROM up and ready for use
 */
static int smsc9118_init(void)
{
	/* See if there is anything there */
	if (((*ID_REV & ID_REV_ID_MASK) == ID_REV_CHIP_118) ||
	    ((*ID_REV & ID_REV_ID_MASK) == ID_REV_CHIP_218))
	{
		printf("LAN9x18 (0x%08x) detected at 0x%08x\n", *ID_REV, CONFIG_SMSC9118_BASE);
	} else {
		printf("Failed to detect LAN9118 (ID_REV = 0x%08x) at 0x%08x\n", *ID_REV, CONFIG_SMSC9118_BASE);
		return 1;
	}

	/* Reset the MAC */
	*HW_CFG = HW_CFG_SRST;
	DELAY(10);

	/* Make sure we set EEDIO/EECLK to the EEPROM */
	if (*GPIO_CFG & GPIO_CFG_EEPR_EN_MSK) {
		while (*E2P_CMD & E2P_CMD_EPC_BUSY)
			if (smsc_ctrlc()) {
				printf("%s: timeout (E2P_CMD = 0x%08x)\n", __func__, *E2P_CMD);
				return 1;
			}
		*GPIO_CFG = (*GPIO_CFG & ~GPIO_CFG_EEPR_EN_MSK);
	}

	return 0;
}

/**
 *	getline - consume a line of input and handle some escape sequences
 */
static char *getline(void)
{
	static char buffer[100];
	char c;
	size_t i;

	i = 0;
	while (1) {
		buffer[i] = '\0';
		while (!tstc())
			continue;

		c = getc();
		/* Convert to uppercase */
		if (c >= 'a' && c <= 'z')
			c -= ('a' - 'A');

		switch (c) {
		case '\r':	/* Enter/Return key */
		case '\n':
			puts("\n");
			return buffer;

		case 0x03:	/* ^C - break */
			return NULL;

		case 0x5F:
		case 0x08:	/* ^H  - backspace */
		case 0x7F:	/* DEL - backspace */
			if (i) {
				puts("\b \b");
				i--;
			}
			break;

		default:
			/* Ignore control characters */
			if (c < 0x20)
				break;
			/* Queue up all other characters */
			buffer[i++] = c;
			printf("%c", c);
			break;
		}
	}
}

/**
 *	skip_space - find first non-whitespace in given pointer
 */
static char *skip_space(char *buf)
{
	while (buf[0] == ' ' || buf[0] == '\t')
		++buf;
	return buf;
}

/**
 *	smsc9118_eeprom - our application's main() function
 */
int smsc9118_eeprom(int argc, char *argv[])
{
	/* Print the ABI version */
	app_startup(argv);
	if (XF_VERSION != get_version()) {
		printf("Expects ABI version %d\n", XF_VERSION);
		printf("Actual U-Boot ABI version %d\n", get_version());
		printf("Can't run\n\n");
		return 1;
	}

	/* Initialize the MAC/EEPROM somewhat */
	puts("\n");
	if (smsc9118_init())
		return 1;

	/* Dump helpful usage information */
	puts("\n");
	usage();
	puts("\n");

	while (1) {
		char *line;

		/* Send the prompt and wait for a line */
		puts("LAN9x18> ");
		line = getline();

		/* Got a ctrl+c */
		if (!line)
			return 0;

		/* Eat leading space */
		line = skip_space(line);

		/* Empty line, try again */
		if (!line[0])
			continue;

		/* Only accept 1 letter commands */
		if (line[0] && line[1] && line[1] != ' ' && line[1] != '\t')
			goto unknown_cmd;

		/* Now parse the command */
		switch (line[0]) {
		case 'W': write_stuff(line);  break;
		case 'D': dump_eeprom();      break;
		case 'M': dump_reg();         break;
		case 'C': copy_from_eeprom(); break;
		case 'P': print_macaddr();    break;
		unknown_cmd:
		default:  puts("ERROR: Unknown command!\n\n");
		case '?':
		case 'H': usage();            break;
		case 'Q': return 0;
		}
	}
}

/**
 *	write_stuff - handle writing of MAC registers / eeprom
 */
static void write_stuff(char *line)
{
	char dest;
	char *endp;
	u8 reg;
	u32 value;

	/* Skip over the "W " part of the command */
	line = skip_space(line + 1);

	/* Figure out destination */
	switch (line[0]) {
	case 'E':
	case 'M':
		dest = line[0];
		break;
	default:
	invalid_usage:
		printf("ERROR: Invalid write usage\n");
		usage();
		return;
	}

	/* Get the register to write */
	line = skip_space(line + 1);
	reg = simple_strtoul(line, &endp, 16);
	if (line == endp)
		goto invalid_usage;

	/* Get the value to write */
	line = skip_space(endp);
	value = simple_strtoul(line, &endp, 16);
	if (line == endp)
		goto invalid_usage;

	/* Check for trailing cruft */
	line = skip_space(endp);
	if (line[0])
		goto invalid_usage;

	/* Finally, execute the command */
	if (dest == 'E') {
		printf("Writing EEPROM register %02x with %02x\n", reg, value);
		write_eeprom_reg(value, reg);
	} else {
		printf("Writing MAC register %02x with %08x\n", reg, value);
		*(u32 *)(CONFIG_SMSC9118_BASE + reg) = value;
	}
}

/**
 *	copy_from_eeprom - copy MAC address in eeprom to address registers
 */
void copy_from_eeprom(void)
{
	ulong addrl =
		read_eeprom_reg(0x01) |
		read_eeprom_reg(0x02) << 8 |
		read_eeprom_reg(0x03) << 16 |
		read_eeprom_reg(0x04) << 24;
	ulong addrh =
		read_eeprom_reg(0x05) |
		read_eeprom_reg(0x06) << 8;
	if (SetMacReg(MAC_ADDRL, addrl) && SetMacReg(MAC_ADDRH, addrh))
		puts("EEPROM contents copied to MAC\n");
	else
		puts("Failed to set MAC from EEPROM\n");
}

/**
 *	print_macaddr - print MAC address registers and MAC address in eeprom
 */
void print_macaddr(void)
{
	puts("Current MAC Address in LAN9x18:  ");
	ulong addrl = GetMacReg(MAC_ADDRL);
	ulong addrh = GetMacReg(MAC_ADDRH);
	printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
		(u8)(addrl), (u8)(addrl >> 8), (u8)(addrl >> 16),
		(u8)(addrl >> 24), (u8)(addrh), (u8)(addrh >> 8));

	puts("Current MAC Address in EEPROM:   ");
	int i;
	for (i = 1; i < 6; ++i)
		printf("%02x:", read_eeprom_reg(i));
	printf("%02x\n", read_eeprom_reg(i));
}

/**
 *	dump_eeprom - dump the whole content of the EEPROM
 */
void dump_eeprom(void)
{
	int i;
	puts("EEPROM:\n");
	for (i = 0; i < 7; ++i)
		printf("%02x: 0x%02x\n", i, read_eeprom_reg(i));
}

/**
 *	do_eeprom_cmd - handle eeprom communication
 */
int do_eeprom_cmd(int cmd, u8 reg)
{
	if (*E2P_CMD & E2P_CMD_EPC_BUSY) {
		printf("%s: busy at start (E2P_CMD = 0x%08x)\n", __func__, *E2P_CMD);
		return -1;
	}

	*E2P_CMD = (E2P_CMD_EPC_BUSY | cmd | reg);

	while (*E2P_CMD & E2P_CMD_EPC_BUSY)
		if (smsc_ctrlc()) {
			printf("%s: timeout (E2P_CMD = 0x%08x)\n", __func__, *E2P_CMD);
			return -1;
		}

	return 0;
}

/**
 *	read_eeprom_reg - read specified register in EEPROM
 */
u8 read_eeprom_reg(u8 reg)
{
	int ret = do_eeprom_cmd(E2P_CMD_EPC_CMD_READ, reg);
	return (ret ? : *E2P_DATA);
}

/**
 *	write_eeprom_reg - write specified value into specified register in EEPROM
 */
int write_eeprom_reg(u8 value, u8 reg)
{
	int ret;

	/* enable erasing/writing */
	ret = do_eeprom_cmd(E2P_CMD_EPC_CMD_EWEN, reg);
	if (ret)
		goto done;

	/* erase the eeprom reg */
	ret = do_eeprom_cmd(E2P_CMD_EPC_CMD_ERASE, reg);
	if (ret)
		goto done;

	/* write the eeprom reg */
	*E2P_DATA = value;
	ret = do_eeprom_cmd(E2P_CMD_EPC_CMD_WRITE, reg);
	if (ret)
		goto done;

	/* disable erasing/writing */
	ret = do_eeprom_cmd(E2P_CMD_EPC_CMD_EWDS, reg);

 done:
	return ret;
}

/**
 *	dump_reg - dump the MAC registers
 *
 * Registers 0x00 - 0x50 are FIFOs.  The 0x50+ are the control registers
 * and they're all 32bits long.  0xB8+ are reserved, so don't bother.
 */
void dump_reg(void)
{
	u8 i, j = 0;
	for (i = 0x50; i < 0xB8; i += sizeof(u32))
		printf("%02x: 0x%08x %c", i, *(u32 *)(CONFIG_SMSC9118_BASE + i), (j++ % 2 ? '\n' : ' '));
}



/*
 *
 * Following functions are copied right from smsc9118.c driver
 *
 */

static int MacBusy(int ReqTO)
{
	  int timeout = ReqTO;
	  int RetVal = FALSE;	  /* No timeout */

	  while (timeout--) {
			if (!(*MAC_CSR_CMD & MAC_CSR_CMD_CSR_BUSY)) {
				  goto done;
			}
	  }
	  RetVal = TRUE;		  /* Timeout */
done:
	  return (RetVal);
}

static ulong
GetMacReg(int Reg)
{
	  ulong RegVal = 0xffffffff;

	  if (*MAC_CSR_CMD & MAC_CSR_CMD_CSR_BUSY) {
			LAN9118_WARN("GetMacReg: previous command not complete\n");
			goto done;
	  }

	  *MAC_CSR_CMD = MAC_RD_CMD(Reg);
	  DELAY(1);

	  if (MacBusy(MAC_TIMEOUT) == TRUE) {
			LAN9118_WARN("GetMacReg: timeout waiting for response "
				  "from MAC\n");
			goto done;
	  }

	  RegVal = *MAC_CSR_DATA;
done:
	  return (RegVal);
}

static int
SetMacReg(int Reg, ulong Value)
{
	  int RetVal = FALSE;

	  if (*MAC_CSR_CMD & MAC_CSR_CMD_CSR_BUSY) {
			LAN9118_WARN("SetMacReg: previous command not complete\n");
			goto done;
	  }

	  *MAC_CSR_DATA = Value;
	  DELAY(1);
	  *MAC_CSR_CMD = MAC_WR_CMD(Reg);
	  DELAY(1);

	  if (MacBusy(MAC_TIMEOUT) == TRUE) {
			LAN9118_WARN("SetMacReg: timeout waiting for response "
				  "from MAC\n");
				  goto done;
	  }

	  RetVal = TRUE;
done:
	  return (RetVal);
}

#else
int smsc9118_eeprom(int argc, char *argv[])
{
	puts("Not supported for this board\n");
	return 1;
}
#endif
