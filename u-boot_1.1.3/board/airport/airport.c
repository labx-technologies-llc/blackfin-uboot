/*
 * Copyright (C) 2004 Klingler
 * Richard Klingler <richard@uclinux.net>
 *
 * Support for the Apple Airport Snow
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <mpc8xx.h>
#include <miiphy.h>

#if 0
#define DEBUG
#endif

/* ------------------------------------------------------------------------- */

#define	_NOT_USED_	0xFFFFFFFF

/*
 * SDRAM are 2 ISSI IS42S16400A-7T chips
 * Minimal CPU frequency is 40MHz.
 */
#if 1
const uint sdram_table[] =
{
/*
* Single Read. (Offset 0 in UPMA RAM)
*/
0x1f07fc24, 0xe0aefc04, 0x10adfc04, 0xe0bbbc00,
0x10f77c44, 0xf3fffc07, 0xffffffff, 0xffffffff,

/*
* Burst Read. (Offset 8 in UPMA RAM)
*/
0x1f07fc24, 0xe0aefc04, 0x10adfc04, 0xf0affc00,
0xf0affc00, 0xf0affc00, 0xf0affc00, 0x10a77c44,
0xf7bffc47, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,

/*
* Single Write. (Offset 18 in UPMA RAM)
*/
0x1f27fc24, 0xe0aebc04, 0x00b93c00, 0x13f77c47,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,

/*
* Burst Write. (Offset 20 in UPMA RAM)
*/
0x1f07fc24, 0xeeaebc00, 0x10ad7c00, 0xf0affc00,
0xf0affc00, 0xe0abbc00, 0x1fb77c47, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,

/*
* Refresh  (Offset 30 in UPMA RAM)
*/
0x1ff77c04, 0x1ff5fc84, 0xfffffc04, 0xfffffc04,
0xfffffc04, 0xFFFFFC84, 0xfffffc07,

                                    0xeffbbc04, 
0x1ff77c05, 0x0ffffc34, 0x0f057c34, 0xfffffc35,

/*
* Exception. (Offset 3c in UPMA RAM)
*/
0x7FFFFC27, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
};
#else
const uint sdram_table[] =
{
/*
* Single Read. (Offset 0 in UPMA RAM)
*/
0x1F07FC04, 0xEEAEFC04, 0x11ADFC04, 0xEFBBBC00,
0x1FF77C47, 0xf3fffc07, _NOT_USED_, _NOT_USED_,

/*
* Burst Read. (Offset 8 in UPMA RAM)
*/
0x1F07FC04, 0xEEAEFC04, 0x10ADFC04, 0xF0AFFC00,
0xF0AFFC00, 0xF1AFFC00, 0xEFBBBC00, 0x1FF77C47,
_NOT_USED_, _NOT_USED_, _NOT_USED_, _NOT_USED_,
_NOT_USED_, _NOT_USED_, _NOT_USED_, _NOT_USED_,

/*
* Single Write. (Offset 18 in UPMA RAM)
*/
0x1F27FC04, 0xEEAEBC00, 0x01B93C04, 0x1FF77C47,
_NOT_USED_, _NOT_USED_, _NOT_USED_, _NOT_USED_,

/*
* Burst Write. (Offset 20 in UPMA RAM)
*/
0x1F07FC04, 0xEEAEBC00, 0x10AD7C00, 0xF0AFFC00,
0xF0AFFC00, 0xE1BBBC04, 0x1FF77C47, _NOT_USED_,
_NOT_USED_, _NOT_USED_, _NOT_USED_, _NOT_USED_,
_NOT_USED_, _NOT_USED_, _NOT_USED_, _NOT_USED_,

/*
* Refresh  (Offset 30 in UPMA RAM)
*/
0x1FF5FC84, 0xFFFFFC04, 0xFFFFFC04, 0xFFFFFC04,
0xFFFFFC84, 0xFFFFFC07, _NOT_USED_, _NOT_USED_, 
_NOT_USED_, _NOT_USED_, _NOT_USED_, _NOT_USED_,

/*
* Exception. (Offset 3c in UPMA RAM)
*/
0x7FFFFC07, _NOT_USED_, _NOT_USED_, _NOT_USED_
};
#endif

/* ------------------------------------------------------------------------- */


/*
 * Check Board Identity:
 *
 */

int checkboard (void)
{
	puts ("Board: Apple Airport Snow\n       adapted by Richard Klingler <richard@uclinux.net>\n");
	return (0);
}

/* ------------------------------------------------------------------------- */

long int initdram (int board_type)
{
	volatile immap_t *immap = (immap_t *) CFG_IMMR;
	volatile memctl8xx_t *memctl = &immap->im_memctl;
	

	upmconfig (UPMA, (uint *) sdram_table,
			   sizeof (sdram_table) / sizeof (uint));

#if 0
	memctl->memc_mptpr = CFG_MPTPR_2BK_4K;

	memctl->memc_mar = 0x00000088;

	memctl->memc_or1 = CFG_OR1_PRELIM;
	memctl->memc_br1 = CFG_BR1_PRELIM;

	memctl->memc_mamr = CFG_MAMR_8COL & (~(MAMR_PTAE));
	memctl->memc_mbmr = 0x00000000;
#else
	memctl->memc_mptpr = 0x0200;

	memctl->memc_mar = 0x00000088;

	memctl->memc_mamr = 0x18822112;
	memctl->memc_mbmr = 0x00000000;

	memctl->memc_or1 = CFG_OR1_PRELIM;
	memctl->memc_br1 = CFG_BR1_PRELIM;
#endif
	udelay (200);

	/* perform SDRAM initializsation sequence */

#if 0
	memctl->memc_mcr = 0x80808105;	/* SDRAM bank 0 */
	udelay (1);
	memctl->memc_mcr = 0x80808830;	/* SDRAM bank 0 - execute twice */
	udelay (1);
	memctl->memc_mcr = 0x80808106;	/* SDRAM bank 0 - execute twice */
	udelay (1);
#else
	memctl->memc_mcr = 0x80002137;	/* SDRAM bank 0 */
	udelay (1);
	memctl->memc_mcr = 0x80002130;	/* SDRAM bank 0 - execute twice */
	udelay (1);
	memctl->memc_mcr = 0x80002130;	/* SDRAM bank 0 */
	udelay (1);
	memctl->memc_mcr = 0x80002139;	/* SDRAM bank 0 - execute twice */
	udelay (1);
#endif

	udelay (1000);

	return (0x00800000);
}


/*
 * Miscellaneous intialization
 */
int misc_init_r (void)
{
#if 1
	volatile immap_t *immap = (immap_t *) CFG_IMMR;
//	volatile memctl8xx_t *memctl = &immap->im_memctl;

	/* BCM5221 for 10/100MB is using Port A for Reset */
	immap->im_ioport.iop_padir |= 0x2000;
	immap->im_ioport.iop_padat |= 0x2000;
	
	/* Configure Port D for MII */
	immap->im_ioport.iop_pdpar = 0x1fff;
	immap->im_ioport.iop_pddir = 0x1fff;

	mii_init();

#endif
	return 0;
}



/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

void reset_phy (void)
{
//	miiphy_reset(0x0);	/* reset PHY */
				/* Auto negotiation on and restart */
	miiphy_write(0x0, PHY_BMCR, PHY_BMCR_AUTON | PHY_BMCR_RST_NEG);
}


/* ------------------------------------------------------------------------- */

#ifdef CONFIG_IDE_LED
void ide_led (uchar led, uchar status)
{
	volatile immap_t *immap = (immap_t *) CFG_IMMR;

	/* We have one led for both pcmcia slots */
	if (status) {				/* led on */
		immap->im_ioport.iop_padat |= 0x0001;
	} else {
		immap->im_ioport.iop_padat &= ~0x0001;
	}
}
#endif /* CONFIG_IDE_LED */
/* ------------------------------------------------------------------------- */

