/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * Changes for MATRIX Vision MVsensor (C) Copyright 2001
 * MATRIX Vision GmbH / hg, info@matrix-vision.de
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

#include <common.h>

#undef MVDEBUG
#ifdef MVDEBUG
#define mvdebug debug
#else
#define mvdebug(p) do {} while (0)
#endif

flash_info_t	flash_info[CFG_MAX_FLASH_BANKS]; /* info for FLASH chips	*/


#define FLASH_DATA_MASK 0xffff
#define FLASH_SHIFT 0
typedef unsigned short FLASH_PORT_WIDTH;
typedef volatile unsigned short FLASH_PORT_WIDTHV;

#define FPW	FLASH_PORT_WIDTH
#define FPWV	FLASH_PORT_WIDTHV
#define FLASH_WIDTH	2

/*-----------------------------------------------------------------------
 * Functions
 */
static ulong flash_get_size (vu_long *address, flash_info_t *info);
static int write_word (flash_info_t *info, ulong dest, ulong data);
static void flash_get_offsets (ulong base, flash_info_t *info);

/*-----------------------------------------------------------------------
 */

unsigned long flash_init (void)
{
	unsigned long size_b;
	int i;

	/* Init: no FLASHes known */
	for (i=0; i<CFG_MAX_FLASH_BANKS; ++i) {
		flash_info[i].flash_id = FLASH_UNKNOWN;
	}

	/* Static FLASH Bank configuration here - FIXME XXX */

	size_b = flash_get_size((vu_long *)CFG_FLASH_BASE, &flash_info[0]);

	if (flash_info[0].flash_id == FLASH_UNKNOWN) {
		printf ("## Unknown FLASH on Bank 0 - Size = 0x%08lx = %ld MB\n",
			size_b, size_b<<20);
	}


	flash_get_offsets (CFG_FLASH_BASE, &flash_info[0]);

	/* monitor protection ON by default */
	flash_protect(FLAG_PROTECT_SET,
		      CFG_FLASH_BASE,
		      CFG_FLASH_BASE + 0x20000 -1,
		      &flash_info[0]);


	flash_info[0].size = size_b;

	return (size_b);
}

/*-----------------------------------------------------------------------
 */
static void flash_get_offsets (ulong base, flash_info_t *info)
{
	int i;

	/* set up sector start address table */
	if (info->flash_id & FLASH_BTYPE)
	{	/* bottom boot sector types - these are the useful ones! */
		/* set sector offsets for bottom boot block type */
		if ((info->flash_id & FLASH_TYPEMASK) == FLASH_AM320B)
		{	/* AMDLV320B has 8 x 8k bottom boot sectors */
			for (i = 0; i < 8; i++)							/* +8k		*/
				info->start[i] = base + (i * (0x00002000 << FLASH_SHIFT));
			for (; i < info->sector_count; i++)					/* +64k		*/
				info->start[i] = base + (i * (0x00010000 << FLASH_SHIFT)) - (0x00070000 << FLASH_SHIFT);
		}
		else
		{	/* other types have 4 bottom boot sectors (16,8,8,32) */
			i = 0;
			info->start[i++] = base +  0x00000000;						/* -		*/
			info->start[i++] = base + (0x00004000 << FLASH_SHIFT);				/* +16k		*/
			info->start[i++] = base + (0x00006000 << FLASH_SHIFT);				/* +8k		*/
			info->start[i++] = base + (0x00008000 << FLASH_SHIFT);				/* +8k		*/
			info->start[i++] = base + (0x00010000 << FLASH_SHIFT);				/* +32k		*/
			for (; i < info->sector_count; i++)						/* +64k		*/
				info->start[i] = base + (i * (0x00010000 << FLASH_SHIFT)) - (0x00030000 << FLASH_SHIFT);
		}
	}
	else
	{	/* top boot sector types - not so useful */
		/* set sector offsets for top boot block type */
		if ((info->flash_id & FLASH_TYPEMASK) == FLASH_AM320T)
		{	/* AMDLV320T has 8 x 8k top boot sectors */
			for (i = 0; i < info->sector_count - 8; i++)					/* +64k		*/
				info->start[i] = base + (i * (0x00010000 << FLASH_SHIFT));
			for (; i < info->sector_count; i++)						/* +8k		*/
				info->start[i] = base + (i * (0x00002000 << FLASH_SHIFT));
		}
		else
		{	/* other types have 4 top boot sectors (32,8,8,16) */
			for (i = 0; i < info->sector_count - 4; i++)					/* +64k		*/
				info->start[i] = base + (i * (0x00010000 << FLASH_SHIFT));

			info->start[i++] = base + info->size - (0x00010000 << FLASH_SHIFT);	/* -32k		*/
			info->start[i++] = base + info->size - (0x00008000 << FLASH_SHIFT);	/* -8k		*/
			info->start[i++] = base + info->size - (0x00006000 << FLASH_SHIFT);	/* -8k		*/
			info->start[i]   = base + info->size - (0x00004000 << FLASH_SHIFT);	/* -16k		*/
		}
	}
}

/*-----------------------------------------------------------------------
 */
void flash_print_info  (flash_info_t *info)
{
	int i;

	if (info->flash_id == FLASH_UNKNOWN) {
		printf ("missing or unknown FLASH type\n");
		return;
	}

	switch (info->flash_id & FLASH_VENDMASK) {
	case FLASH_MAN_AMD:	printf ("AMD ");		break;
	case FLASH_MAN_FUJ:	printf ("FUJITSU ");	break;
	case FLASH_MAN_STM:	printf ("ST ");			break;
	default:		printf ("Unknown Vendor ");	break;
	}

	switch (info->flash_id & FLASH_TYPEMASK) {
	case FLASH_AM160B:	printf ("AM29LV160B (16 Mbit, bottom boot sect)\n");
				break;
	case FLASH_AM160T:	printf ("AM29LV160T (16 Mbit, top boot sector)\n");
				break;
	case FLASH_AM320B:	printf ("AM29LV320B (32 Mbit, bottom boot sect)\n");
				break;
	case FLASH_AM320T:	printf ("AM29LV320T (32 Mbit, top boot sector)\n");
				break;
	case FLASH_STMW320DB:	printf ("M29W320B (32 Mbit, bottom boot sect)\n");
				break;
	case FLASH_STMW320DT:	printf ("M29W320T (32 Mbit, top boot sector)\n");
				break;
	default:		printf ("Unknown Chip Type\n");
				break;
	}

	printf ("  Size: %ld MB in %d Sectors\n",
		info->size >> 20, info->sector_count);

	printf ("  Sector Start Addresses:");
	for (i=0; i<info->sector_count; ++i) {
		if ((i % 5) == 0)
			printf ("\n   ");
		printf (" %08lX%s",
			info->start[i],
			info->protect[i] ? " (RO)" : "     "
		);
	}
	printf ("\n");
}

/*-----------------------------------------------------------------------
 */


/*-----------------------------------------------------------------------
 */

/*
 * The following code cannot be run from FLASH!
 */

#define	AMD_ID_LV160T_MVS	(AMD_ID_LV160T & FLASH_DATA_MASK)
#define AMD_ID_LV160B_MVS	(AMD_ID_LV160B & FLASH_DATA_MASK)
#define AMD_ID_LV320T_MVS	(AMD_ID_LV320T & FLASH_DATA_MASK)
#define AMD_ID_LV320B_MVS	(AMD_ID_LV320B & FLASH_DATA_MASK)
#define STM_ID_W320DT_MVS	(STM_ID_29W320DT & FLASH_DATA_MASK)
#define STM_ID_W320DB_MVS	(STM_ID_29W320DB & FLASH_DATA_MASK)
#define AMD_MANUFACT_MVS	(AMD_MANUFACT  & FLASH_DATA_MASK)
#define FUJ_MANUFACT_MVS	(FUJ_MANUFACT  & FLASH_DATA_MASK)
#define STM_MANUFACT_MVS	(STM_MANUFACT  & FLASH_DATA_MASK)

#define AUTOSELECT_ADDR1	0x0555
#define AUTOSELECT_ADDR2	0x02AA
#define AUTOSELECT_ADDR3	AUTOSELECT_ADDR1

#define AUTOSELECT_DATA1	(0x00AA00AA & FLASH_DATA_MASK)
#define AUTOSELECT_DATA2	(0x00550055 & FLASH_DATA_MASK)
#define AUTOSELECT_DATA3	(0x00900090 & FLASH_DATA_MASK)

#define RESET_BANK_DATA		(0x00F000F0 & FLASH_DATA_MASK)

static ulong flash_get_size (vu_long *address, flash_info_t *info)
{
	short i;
	ushort value;
	vu_short *addr = (vu_short *)address;
	ulong base = (ulong)address;

	/* Write auto select command: read Manufacturer ID */
	addr[AUTOSELECT_ADDR1] = AUTOSELECT_DATA1;
	addr[AUTOSELECT_ADDR2] = AUTOSELECT_DATA2;
	addr[AUTOSELECT_ADDR3] = AUTOSELECT_DATA3;

	asm("ssync;");

	value = addr[0];			/* manufacturer ID	*/
	switch (value) {
	case AMD_MANUFACT_MVS:
		info->flash_id = FLASH_MAN_AMD;
		break;
	case FUJ_MANUFACT_MVS:
		info->flash_id = FLASH_MAN_FUJ;
		break;
	case STM_MANUFACT_MVS:
		info->flash_id = FLASH_MAN_STM;
		break;
	default:
		info->flash_id = FLASH_UNKNOWN;
		info->sector_count = 0;
		info->size = 0;
		return (0);			/* no or unknown flash	*/
	}

	value = addr[1];			/* device ID		*/
	switch (value) {
	case AMD_ID_LV160T_MVS:
		info->flash_id += FLASH_AM160T;
		info->sector_count = 37;
		info->size = (0x00200000 << FLASH_SHIFT);
		break;				/* => 2 or 4 MB		*/

	case AMD_ID_LV160B_MVS:
		info->flash_id += FLASH_AM160B;
		info->sector_count = 37;
		info->size = (0x00200000 << FLASH_SHIFT);
		break;				/* => 2 or 4 MB		*/

	case AMD_ID_LV320T_MVS:
		info->flash_id += FLASH_AM320T;
		info->sector_count = 71;
		info->size = (0x00400000 << FLASH_SHIFT);
		break;				/* => 4 or 8 MB		*/

	case AMD_ID_LV320B_MVS:
		info->flash_id += FLASH_AM320B;
		info->sector_count = 71;
		info->size = (0x00400000 << FLASH_SHIFT);
		break;				/* => 4 or 8MB		*/

	case STM_ID_W320DT_MVS:
		info->flash_id += FLASH_STMW320DT;
		info->sector_count = 67;
		info->size = (0x00400000 << FLASH_SHIFT);
		break;				/* => 4 or 8 MB		*/

	case STM_ID_W320DB_MVS:
		info->flash_id += FLASH_STMW320DB;
		info->sector_count = 67;
		info->size = (0x00400000 << FLASH_SHIFT);
		break;				/* => 4 or 8MB		*/

	default:
		info->flash_id = FLASH_UNKNOWN;
		return (0);			/* => no or unknown flash */

	}

	/* set up sector start address table */
	flash_get_offsets (base, info);

	/* check for protected sectors */
	for (i = 0; i < info->sector_count; i++) {
		/* read sector protection at sector address, (A7 .. A0) = 0x02 */
		/* D0 = 1 if protected */
		addr = (vu_short *)(info->start[i]);
		info->protect[i] = addr[2] & 1;
	}

	/*
	 * Prevent writes to uninitialized FLASH.
	 */
	if (info->flash_id != FLASH_UNKNOWN) {
		addr = (vu_short *)info->start[0];
		*addr = RESET_BANK_DATA;	/* reset bank */
	}

	return (info->size);
}


/*-----------------------------------------------------------------------
 */

#define ERASE_ADDR1 0x0555
#define ERASE_ADDR2 0x02AA
#define ERASE_ADDR3 ERASE_ADDR1
#define ERASE_ADDR4 ERASE_ADDR1
#define ERASE_ADDR5 ERASE_ADDR2

#define ERASE_DATA1 (0x00AA00AA & FLASH_DATA_MASK)
#define ERASE_DATA2 (0x00550055 & FLASH_DATA_MASK)
#define ERASE_DATA3 (0x00800080 & FLASH_DATA_MASK)
#define ERASE_DATA4 ERASE_DATA1
#define ERASE_DATA5 ERASE_DATA2

#define ERASE_SECTOR_DATA (0x00300030 & FLASH_DATA_MASK)
#define ERASE_CHIP_DATA (0x00100010 & FLASH_DATA_MASK)
#define ERASE_CONFIRM_DATA (0x00800080 & FLASH_DATA_MASK)

int	flash_erase (flash_info_t *info, int s_first, int s_last)
{
	vu_short *addr = (vu_short *)(info->start[0]);
	int flag, prot, sect;
	ulong start, now, last;

	if ((s_first < 0) || (s_first > s_last)) {
		if (info->flash_id == FLASH_UNKNOWN) {
			printf ("- missing\n");
		} else {
			printf ("- no sectors to erase\n");
		}
		return 1;
	}

	if ((info->flash_id == FLASH_UNKNOWN) ||
	    (info->flash_id > FLASH_AMD_COMP)) {
		printf ("Can't erase unknown flash type %08lx - aborted\n",
			info->flash_id);
		return 1;
	}

	prot = 0;
	for (sect=s_first; sect<=s_last; ++sect) {
		if (info->protect[sect]) {
			prot++;
		}
	}

	if (prot) {
		printf ("- Warning: %d protected sectors will not be erased!\n",
			prot);
	} else {
		printf ("\n");
	}

//	l_sect = -1;

	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts();


	/* Start erase on unprotected sectors */
	for (sect = s_first; sect<=s_last; sect++)
	{
		printf("Erasing sector: %2d ... ", sect);
		addr[ERASE_ADDR1] = ERASE_DATA1;
		addr[ERASE_ADDR2] = ERASE_DATA2;
		addr[ERASE_ADDR3] = ERASE_DATA3;
		addr[ERASE_ADDR4] = ERASE_DATA4;
		addr[ERASE_ADDR5] = ERASE_DATA5;

		asm("ssync;");

		if (info->protect[sect] == 0)
		{	/* not protected */
			addr = (vu_short *)(info->start[sect]);
			addr[0] = ERASE_SECTOR_DATA;
//			l_sect = sect;
			asm("ssync;");
		}

		/* re-enable interrupts if necessary */
		if (flag)
			enable_interrupts();

		/* wait at least 80us - let's wait 1 ms */
		udelay (1000);

		/*
		 * We wait for the last triggered sector
		 */
//		if (l_sect < 0)
//			goto DONE;

		start = get_timer (0);
		last  = start;
		addr = (vu_short *)(info->start[sect]);
		while ((addr[0] & ERASE_CONFIRM_DATA) != ERASE_CONFIRM_DATA) {
			if ((now = get_timer(start)) > CFG_FLASH_ERASE_TOUT) {
				printf ("Timeout\n");
				return 1;
			}
			/* show that we're waiting */
			if ((now - last) > 100) {	/* every second */
				putc ('.');
				last = now;
			}
		}
		asm("ssync;");

		/* reset to read mode */
		addr = (vu_short *)info->start[0];
		addr[0] = RESET_BANK_DATA;	/* reset bank */
		asm("ssync;");
		printf("OK\n");
	}
	printf (" done\n");
	return 0;
}


/*-----------------------------------------------------------------------
 * Copy memory to flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
int write_buff (flash_info_t *info, uchar *src, ulong addr, ulong cnt)
{
	ulong cp, wp, data;
	int l;
	int i, rc;

	wp = (addr & ~(FLASH_WIDTH-1));	/* get lower word aligned address */

	/*
	 * handle unaligned start bytes
	 */
	if ((l = addr - wp) != 0) {
		data = 0;
		for (i=0, cp=wp; i<l; ++i, ++cp) {
			data = (data >> 8) | (*(uchar *)cp);
		}
		for (; i<FLASH_WIDTH && cnt>0; ++i) {
			data = (data << 8) | *src++;
			--cnt;
			++cp;
		}
		for (; cnt==0 && i<FLASH_WIDTH; ++i, ++cp) {
			data = (data << 8) | (*(uchar *)cp);
		}

		if ((rc = write_word(info, wp, data)) != 0) {
			return (rc);
		}
		wp += FLASH_WIDTH;
	}

	/*
	 * handle word aligned part
	 */
	while (cnt >= FLASH_WIDTH) {
		data = 0;
		for (i=0; i<FLASH_WIDTH; ++i) {
			data = (data << 8) | *src++;
		}
		if ((rc = write_word(info, wp, data)) != 0) {
			return (rc);
		}
		wp  += FLASH_WIDTH;
		cnt -= FLASH_WIDTH;
	}
	if (cnt == 0) {
		return ERR_OK;
	}

	/*
	 * handle unaligned tail bytes
	 */
	data = 0;
	for (i=0, cp=wp; i<FLASH_WIDTH && cnt>0; ++i, ++cp) {
		data = (data << 8) | *src++;
		--cnt;
	}
	for (; i<FLASH_WIDTH; ++i, ++cp) {
		data = (data << 8) | (*(uchar *)cp);
	}

	return write_word(info, wp, data);
}

#define WRITE_ADDR1 0x0555
#define WRITE_ADDR2 0x02AA
#define WRITE_ADDR3 WRITE_ADDR1

#define WRITE_DATA1 (0x00AA00AA & FLASH_DATA_MASK)
#define WRITE_DATA2 (0x00550055 & FLASH_DATA_MASK)
#define WRITE_DATA3 (0x00A000A0 & FLASH_DATA_MASK)

#define WRITE_CONFIRM_DATA ERASE_CONFIRM_DATA

/*-----------------------------------------------------------------------
 * Write a halfword to Flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
static int write_halfword (flash_info_t *info, ulong dest, ushort data)
{
	vu_short *addr = (vu_short *)(info->start[0]);
	ulong start;
	int flag;

	mvdebug (("+write_halfword (to 0x%08lx)\n", dest));
	/* Check if Flash is (sufficiently) erased */
	if ((*((vu_short *)dest) & data) != data)
	{
		return (2);
	}
	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts();

	addr[WRITE_ADDR1] = WRITE_DATA1;
	addr[WRITE_ADDR2] = WRITE_DATA2;
	addr[WRITE_ADDR3] = WRITE_DATA3;

	*((vu_short *)dest) = data;

	asm("ssync;");

	/* re-enable interrupts if necessary */
	if (flag)
		enable_interrupts();

	/* data polling for D7 */
	start = get_timer (0);
	addr = (vu_short *)dest;
	while ((*addr & WRITE_CONFIRM_DATA) != (data & WRITE_CONFIRM_DATA)) {
		if (get_timer(start) > CFG_FLASH_WRITE_TOUT) {
			return (1);
		}
	}
 	mvdebug (("-write_halfword\n"));
	return (0);
}


/*-----------------------------------------------------------------------
 * Write a word to Flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
static int write_word (flash_info_t *info, ulong dest, ulong data)
{
	int result = 0;

	if (write_halfword (info, dest, ((data & 0xff00) >> 8) | 0xff00) == 0)
	{
//		dest += 2;
		data = ((data << 8)  & 0xff00) | ((data >> 8) & 0x00ff);
		result = write_halfword (info, dest, data);
	}
	return result;
}

/*-----------------------------------------------------------------------
 */
