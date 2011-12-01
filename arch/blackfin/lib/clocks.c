/*
 * clocks.c - figure out sclk/cclk/vco and such
 *
 * Copyright (c) 2005-2008 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <common.h>
#include <asm/blackfin.h>

#ifdef PLL_CTL
# include <asm/mach-common/bits/pll.h>
# define pll_is_bypassed() (bfin_read_PLL_STAT() & DF)
#else
# include <asm/mach-common/bits/cgu.h>
# define pll_is_bypassed() (bfin_read_CGU_STAT() & PLLBP)
# define bfin_read_PLL_CTL() bfin_read_CGU_CTL()
# define bfin_read_PLL_DIV() bfin_read_CGU_DIV()
#endif

/* Get the voltage input multiplier */
u_long get_vco(void)
{
#ifndef CONFIG_BFIN_GET_VCO
	static u_long cached_vco_pll_ctl, cached_vco;

	u_long msel, pll_ctl;

	pll_ctl = bfin_read_PLL_CTL();
	if (pll_ctl == cached_vco_pll_ctl)
		return cached_vco;
	else
		cached_vco_pll_ctl = pll_ctl;

	msel = (pll_ctl & MSEL) >> MSEL_P;
	if (0 == msel)
		msel = (MSEL >> MSEL_P) + 1;

	cached_vco = CONFIG_CLKIN_HZ;
	cached_vco >>= (pll_ctl & DF);
	cached_vco *= msel;
	return cached_vco;
#else
	return CONFIG_BFIN_GET_VCO;
#endif
}

/* Get the Core clock */
u_long get_cclk(void)
{
#ifndef CONFIG_BFIN_GET_CCLK
	static u_long cached_cclk_pll_div, cached_cclk;
	u_long div, csel, ssel;

	if (pll_is_bypassed())
		return CONFIG_CLKIN_HZ;

	div = bfin_read_PLL_DIV();
	if (div == cached_cclk_pll_div)
		return cached_cclk;
	else
		cached_cclk_pll_div = div;

	csel = (div & CSEL) >> CSEL_P;
#ifndef CGU_DIV
	ssel = (div & SSEL) >> SSEL_P;
	if (ssel && ssel < (1 << csel))	/* SCLK > CCLK */
		cached_cclk = get_vco() / ssel;
	else
#endif
		cached_cclk = get_vco() >> csel;
	return cached_cclk;
#else
	return CONFIG_BFIN_GET_CCLK;
#endif
}

/* Get the System clock */
#ifdef CGU_DIV

static u_long cached_sclk_pll_div, cached_sclk, cached_sclk0, cached_sclk1, cached_dclk;
static u_long _get_sclk(u_long *cache)
{
	u_long div, ssel;

	if (pll_is_bypassed())
		return CONFIG_CLKIN_HZ;

	div = bfin_read_PLL_DIV();
	if (div == cached_sclk_pll_div)
		return *cache;
	else
		cached_sclk_pll_div = div;

	ssel = (div & SYSSEL) >> SYSSEL_P;
	cached_sclk = get_vco() / ssel;

	ssel = (div & S0SEL) >> S0SEL_P;
	cached_sclk0 = cached_sclk / ssel;

	ssel = (div & S1SEL) >> S1SEL_P;
	cached_sclk1 = cached_sclk / ssel;

	return *cache;
}

u_long get_sclk(void)
{
#ifndef CONFIG_BFIN_GET_SCLK
	return _get_sclk(&cached_sclk);
#else
	return CONFIG_BFIN_GET_SCLK;
#endif
}

u_long get_sclk0(void)
{
#ifndef CONFIG_BFIN_GET_SCLK0
	return _get_sclk(&cached_sclk0);
#else
	return CONFIG_BFIN_GET_SCLK0;
#endif
}

u_long get_sclk1(void)
{
#ifndef CONFIG_BFIN_GET_SCLK1
	return _get_sclk(&cached_sclk1);
#else
	return CONFIG_BFIN_GET_SCLK1;
#endif
}

u_long get_dclk(void)
{
#ifndef CONFIG_BFIN_GET_DCLK
	return _get_sclk(&cached_dclk);
#else
	return CONFIG_BFIN_GET_DCLK;
#endif
}
#else

u_long get_sclk(void)
{
#ifndef CONFIG_BFIN_GET_SCLK
	static u_long cached_sclk_pll_div, cached_sclk;
	u_long div, ssel;

	if (pll_is_bypassed())
		return CONFIG_CLKIN_HZ;

	div = bfin_read_PLL_DIV();
	if (div == cached_sclk_pll_div)
		return cached_sclk;
	else
		cached_sclk_pll_div = div;

	ssel = (div & SSEL) >> SSEL_P;
	cached_sclk = get_vco() / ssel;

	return cached_sclk;
#else
	return CONFIG_BFIN_GET_SCLK;
#endif
}

#endif
