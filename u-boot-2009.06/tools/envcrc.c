/*
 * (C) Copyright 2001
 * Paolo Scaffardi, AIRVENT SAM s.p.a - RIMINI(ITALY), arsenio@tin.it
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "elf.h"
#include "uswap.h"

#ifndef __ASSEMBLY__
#define	__ASSEMBLY__			/* Dirty trick to get only #defines	*/
#endif
#define	__ASM_STUB_PROCESSOR_H__	/* don't include asm/processor.		*/
#include <config.h>
#undef	__ASSEMBLY__

#if defined(CONFIG_ENV_IS_IN_FLASH)
# ifndef  CONFIG_ENV_ADDR
#  define CONFIG_ENV_ADDR	(CONFIG_SYS_FLASH_BASE + CONFIG_ENV_OFFSET)
# endif
# ifndef  CONFIG_ENV_OFFSET
#  define CONFIG_ENV_OFFSET (CONFIG_ENV_ADDR - CONFIG_SYS_FLASH_BASE)
# endif
# if !defined(CONFIG_ENV_ADDR_REDUND) && defined(CONFIG_ENV_OFFSET_REDUND)
#  define CONFIG_ENV_ADDR_REDUND	(CONFIG_SYS_FLASH_BASE + CONFIG_ENV_OFFSET_REDUND)
# endif
# ifndef  CONFIG_ENV_SIZE
#  define CONFIG_ENV_SIZE	CONFIG_ENV_SECT_SIZE
# endif
# if defined(CONFIG_ENV_ADDR_REDUND) && !defined(CONFIG_ENV_SIZE_REDUND)
#  define CONFIG_ENV_SIZE_REDUND	CONFIG_ENV_SIZE
# endif
# if (CONFIG_ENV_ADDR >= CONFIG_SYS_MONITOR_BASE) && \
     ((CONFIG_ENV_ADDR + CONFIG_ENV_SIZE) <= (CONFIG_SYS_MONITOR_BASE + CONFIG_SYS_MONITOR_LEN))
#  define ENV_IS_EMBEDDED	1
# endif
# if defined(CONFIG_ENV_ADDR_REDUND) || defined(CONFIG_ENV_OFFSET_REDUND)
#  define CONFIG_SYS_REDUNDAND_ENVIRONMENT	1
# endif
#endif	/* CONFIG_ENV_IS_IN_FLASH */

#ifdef CONFIG_SYS_REDUNDAND_ENVIRONMENT
# define ENV_HEADER_SIZE	(sizeof(uint32_t) + 1)
#else
# define ENV_HEADER_SIZE	(sizeof(uint32_t))
#endif

#define ENV_SIZE (CONFIG_ENV_SIZE - ENV_HEADER_SIZE)

unsigned char host_endian(void)
{
	struct {
		union {
			uint16_t sint;
			unsigned char str[2];
		} u;
	} data;
	data.u.sint = 0xBBAA;
	return data.u.str[0] == 0xBB ? ELFDATA2MSB : ELFDATA2LSB;
}
bool swapit;
#define EGET(val) ( \
	!swapit ? (val) : \
		sizeof(val) == 1 ? (val) : \
		sizeof(val) == 2 ? uswap_16(val) : \
		sizeof(val) == 4 ? uswap_32(val) : \
		/*sizeof(val) == 8 ? uswap_64(val) :*/ \
		1 \
)

extern uint32_t crc32 (uint32_t, const unsigned char *, unsigned int);

static bool elfread(FILE *fp, long offset, void *ptr, size_t size, size_t nmemb)
{
	if (fseek(fp, offset, SEEK_SET))
		return false;
	if (fread(ptr, size, nmemb, fp) != nmemb)
		return false;
	return true;
}

/* Avoid using elf.h since not all systems have it */
unsigned char environment[CONFIG_ENV_SIZE];
bool read_env_from_elf(const char *elf_file)
{
	const char env_symbol[] = "default_environment";
	char buf[256];
	FILE *fp;
	bool ret = false;
	int i;

	Elf32_Ehdr ehdr;
	Elf32_Shdr shdr, strtab, symtab;
	Elf32_Sym sym;

	fp = fopen(elf_file, "r");
	if (!fp)
		return false;

	/* Make sure this is a valid ELF */
	if (!elfread(fp, 0, &ehdr, sizeof(ehdr), 1))
		goto done;
	if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG)) {
		errno = EINVAL;
		goto done;
	}
	if (ehdr.e_ident[EI_CLASS] != ELFCLASS32) {
		errno = EINVAL;
		goto done;
	}
	swapit = host_endian() == ehdr.e_ident[EI_DATA] ? false : true;

	/* Find the string & symbol table */
	memset(&strtab, 0, sizeof(strtab)); /* shut gcc the hell up */
	memset(&symtab, 0, sizeof(symtab)); /* shut gcc the hell up */
	strtab.sh_type = SHT_NULL;
	symtab.sh_type = SHT_NULL;
	for (i = 0; i < EGET(ehdr.e_shnum); ++i) {
		long off = EGET(ehdr.e_shoff) + i * EGET(ehdr.e_shentsize);
		if (!elfread(fp, off, &shdr, sizeof(shdr), 1))
			goto done;
		if (EGET(shdr.sh_type) == SHT_STRTAB)
			strtab = shdr;
		else if (EGET(shdr.sh_type) == SHT_SYMTAB)
			symtab = shdr;
	}
	if (strtab.sh_type == SHT_NULL || symtab.sh_type == SHT_NULL) {
		errno = EINVAL;
		goto done;
	}

	/* Find the environment symbol */
	for (i = 0; i < EGET(symtab.sh_size) / EGET(symtab.sh_entsize); ++i) {
		char *tbuf;
		long off = EGET(symtab.sh_offset) + i * sizeof(sym);
		if (!elfread(fp, off, &sym, sizeof(sym), 1))
			goto done;
		off = EGET(strtab.sh_offset) + EGET(sym.st_name);
		tbuf = buf;
		if (!elfread(fp, off, tbuf, 1, sizeof(env_symbol)))
			goto done;
		/* handle ABI prefixed symbols automatically */
		if (tbuf[0] == '_') {
			tbuf[sizeof(env_symbol)] = '\0';
			++tbuf;
		}
		if (!strcmp(tbuf, env_symbol)) {
			off = EGET(ehdr.e_shoff) + EGET(sym.st_shndx) * EGET(ehdr.e_shentsize);
			if (!elfread(fp, off, &shdr, sizeof(shdr), 1))
				goto done;
			off = EGET(shdr.sh_offset) + EGET(sym.st_value);
			if (!elfread(fp, off, environment + ENV_HEADER_SIZE, 1, EGET(sym.st_size)))
				goto done;
			ret = true;
			break;
		}
	}

 done:
	fclose(fp);
	return ret;
}

int main (int argc, char **argv)
{
#if defined(ENV_IS_EMBEDDED) || defined(ENV_IS_EMBEDDED_CUSTOM)
	unsigned char pad = 0x00;
	uint32_t crc;
	unsigned char *envptr = environment,
		*dataptr = envptr + ENV_HEADER_SIZE;
	unsigned int datasize = ENV_SIZE;
	const char *env_file;

	if (argc < 2) {
		puts("Usage: envcrc <environment object> [--binary [le]]");
		return 1;
	}
	env_file = argv[1];

	if (argv[2] && !strncmp(argv[2], "--binary", 8)) {
		int ipad = 0xff;
		if (argv[2][8] == '=')
			sscanf(argv[2] + 9, "%i", &ipad);
		pad = ipad;
	}
	memset(dataptr, pad, datasize);

	if (!read_env_from_elf(env_file)) {
		fprintf(stderr, "unable to read environment from %s: %s\n",
			env_file, strerror(errno));
		return 1;
	}

	crc = crc32 (0, dataptr, datasize);

	/* Check if verbose mode is activated passing a parameter to the program */
	if (argc > 2) {
		if (!strncmp(argv[2], "--binary", 8)) {
			int le = (argc > 3 ? !strcmp(argv[3], "le") : 1);
			size_t i, start, end, step;
			if (le) {
				start = 0;
				end = ENV_HEADER_SIZE;
				step = 1;
			} else {
				start = ENV_HEADER_SIZE - 1;
				end = -1;
				step = -1;
			}
			for (i = start; i != end; i += step)
				printf("%c", (crc & (0xFF << (i * 8))) >> (i * 8));
			fwrite(dataptr, 1, datasize, stdout);
		} else {
			printf("CRC32 from offset %08X to %08X of environment = %08X\n",
				(unsigned int) (dataptr - envptr),
				(unsigned int) (dataptr - envptr) + datasize,
				crc);
		}
	} else {
		printf ("0x%08X\n", crc);
	}
#else
	printf ("0\n");
#endif
	return EXIT_SUCCESS;
}
