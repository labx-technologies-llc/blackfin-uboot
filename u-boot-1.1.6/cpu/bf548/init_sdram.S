#define ASSEMBLY

#include <linux/config.h>
#include <config.h>
#include <asm/blackfin.h>
#include <asm/mem_init.h>
.global init_sdram;

#if (CONFIG_CCLK_DIV == 1)
#define CONFIG_CCLK_ACT_DIV	0x0000
#endif
#if (CONFIG_CCLK_DIV == 2)
#define CONFIG_CCLK_ACT_DIV	0x0010
#endif
#if (CONFIG_CCLK_DIV == 4)
#define CONFIG_CCLK_ACT_DIV	0x0020
#endif
#if (CONFIG_CCLK_DIV == 8)
#define CONFIG_CCLK_ACT_DIV	0x0030
#endif
#ifndef CONFIG_CCLK_ACT_DIV
#define CONFIG_CCLK_ACT_DIV	0x0000
#endif

#define PLL_DIV_VAL	(CONFIG_CCLK_ACT_DIV | CONFIG_SCLK_DIV)
#define PLL_CTL_VAL	((CONFIG_VCO_MULT *2) << 8)

#if (BFIN_BOOT_MODE == BFIN_BOOT_UART)
#define BFIN_LDR_BAUD	((((CONFIG_CLKIN_HZ * CONFIG_VCO_MULT) \
	/ CONFIG_SCLK_DIV) / 16) /CONFIG_LDR_LOAD_BAUD)
#define BFIN_LDR_BAUD_HB 	((BFIN_LDR_BAUD >> 8) & 0xFF)
#define BFIN_LDR_BAUD_LB	(BFIN_LDR_BAUD & 0xFF)
#endif

init_sdram:
	[--SP] = ASTAT;
	[--SP] = RETS;
	[--SP] = (R7:0);
	[--SP] = (P5:0);

#if (BFIN_BOOT_MODE == BFIN_BOOT_UART)
	/* force RTS rather than relying on auto RTS */
	P3.h = hi(UART1_MCR);
	P3.l = lo(UART1_MCR);
	R0 = w[P3];
	bitset(R0, 5);	/* set FCPOL */
	w[P3] = R0;
	SSYNC;

	/* Wait for the line to clear up.  We cannot rely on UART
	 * registers as none of them reflect the status of the RSR.
	 * Instead, we'll sleep for ~10 bit times at 9600 baud.
	 * We can precalc things here by assuming boot values for
	 * PLL rather than loading registers and calculating.
	 *	baud    = SCLK / (16 ^ (1 - EDBO) * Divisor)
	 *	EDB0    = 0
	 *	Divisor = (SCLK / baud) / 16
	 *	SCLK    = baud * 16 * Divisor
	 *	SCLK    = (0x14 * CONFIG_CLKIN_HZ) / 5
	 *	CCLK    = (16 * Divisor * 5) * (9600 / 10)
	 * In reality, this will probably be just about 1 second delay,
	 * so assuming 9600 baud is OK (both as a very low and too high
	 * speed as this will buffer things enough).
	 */
#define _NUMBITS (10)                                   /* how many bits to delay */
#define _LOWBAUD (9600)                                 /* low baud rate */
#define _SCLK    ((0x14 * CONFIG_CLKIN_HZ) / 5)         /* SCLK based on PLL */
#define _DIVISOR ((_SCLK / _LOWBAUD) / 16)              /* UART DLL/DLH */
#define _NUMINS  (3)                                    /* how many instructions in loop */
#define _CCLK    (((16 * _DIVISOR * 5) * (_LOWBAUD / _NUMBITS)) / _NUMINS)
	R1.h = HI(_CCLK);
	R1.l = LO(_CCLK);
.Lcheck_again:
	R1 += -1;
	CC = R1 == 0;
	IF !CC jump .Lcheck_again;
#endif

	/*************************************************
	* System interrupt wakeup registers (SIC_IWRx)   *
	* Required for PLL_CTL and VR_CTL configuration  *
	* Processor idle and wake up after PLL_LOCKCNT   *
	* expired.reset = 0xFFFF FFFF                    *
	* disable all other interrupt sources in the     *
	* meantime to disable unwanted wake up events    *
	* Note: These vales are set in BootRom, here we  *
	* should not modify these values. We can modify  *
	* these values when u-boot is executed from DDR  *
	* Notice: code is now run in BootRom environment *
	**************************************************/
	P5.h = hi(SIC_IWR0);
	P5.l = lo(SIC_IWR0);
	R7 = [P5];
	bitset(R7, 0);
	[P5] = R7;

	/* PLL Lock counter register */
	P5.h = hi(PLL_LOCKCNT);
	P5.l = lo(PLL_LOCKCNT);
	R7.l = 0x0200;
	w[P5] = R7.l;

	/***********************************************
	* VR_CTL: reset value 0x40DB                   *
	* Change from 1.2V core volt to 1.25V volt     *
	* Notice: The same value should not be written *
	*         twice to VR_CTL register             *
	************************************************/
	P5.h = hi(VR_CTL);
	P5.l = lo(VR_CTL);
	R6 = w[P5] (z);
	R7.l = 0x40EB;
	R7.h = 0x0;
	CC = R6 == R7;
	if CC jump skip_vr_ctl;
 	CLI R6;
	w[P5] = R7.l;
	IDLE;
	STI R6;
skip_vr_ctl: nop;

	/***********************************
	* PLL divider register             *
	* Can be configured on the fly     *
	* First configure divider          *
	* then configure MSEL in PLL_CTL   *
	************************************/
	P5.h = hi(PLL_DIV);
	P5.l = lo(PLL_DIV);
	R7.l = PLL_DIV_VAL;
	w[P5] = R7.l;

	/***************************************
	* PLL control register                 *
	* NOTICE: the same value should not be *
	*    written twice to PLL_CTL register *
	****************************************/
	P5.h = hi(PLL_CTL);
	P5.l = lo(PLL_CTL);
	R6 = w[P5](z);
	R7 = PLL_CTL_VAL (z);
	CC = R6 == R7;
	if CC jump skip_pll_ctl;
	CLI R6;
	w[P5] = R7.l;
	IDLE;
	STI R6;
skip_pll_ctl: nop;

#if (BFIN_BOOT_MODE == BFIN_BOOT_UART)
	P5.l = lo(UART1_DLL);
	P5.h = hi(UART1_DLL);
	R7 = BFIN_LDR_BAUD_LB (z);
	w[P5] = R7.l;
	SSYNC;

	P5.l = lo(UART1_DLH);
	P5.h = hi(UART1_DLH);
	R7 = BFIN_LDR_BAUD_HB (z);
	w[P5] = R7.l;
	SSYNC;
#endif

#if (BFIN_BOOT_MODE == BFIN_BOOT_SPI_MASTER)
	p0.h = hi(SPI0_BAUD);
	p0.l = lo(SPI0_BAUD);
	r0.l = CONFIG_SPI_BAUD_INITBLOCK;
	w[p0] = r0.l;
	SSYNC;
#endif

	/**********************************************************
	*  PORTG configure for DDR test                           *
	*  if DDR in successfully initialized, LED1 is lighted on *
	*  or LED2 is lighted on                                  *
	*  PORTG is configured as GPIO to drive LED               *
	***********************************************************/
	p0.l = lo(PORTG_FER);
	p0.h = hi(PORTG_FER);
	r0.l = 0x0000;
	w[p0]= r0;
	ssync;

	p0.l = lo(PORTG_MUX);
	p0.h = hi(PORTG_MUX);
	r0.l = 0x0000;
	w[p0]= r0;
	ssync;

	p0.l = lo(PORTG_DIR_SET);
	p0.h = hi(PORTG_DIR_SET);
	r0.l = 0x0FC0;
	w[p0]= r0;
	ssync;

	/*********************************************************/
	/* DDR SDRAM part# Micron MT46V32M16P-6T:F (8 Meg x 16 x 4 banks) 512Mbits */
	/* 8k rows, 1k columns*/
	/* populated DDR SDRAM addresses -> 0x00000000 - 0x03FFFFFF */
	/* SCLK = 83.33 MHz */
	/* CAS Latency = 2 */
	/* All other settings are default */
	/* C Initialization from pre-boot included in comments */
	/*********************************************************/


	/* release the DDR controller from reset as per spec
	/* *pEBIU_RST_CTL |= 0x0001; */
	/* ssync(); */
	p0.l = lo(EBIU_RSTCTL);
	p0.h = hi(EBIU_RSTCTL);
	r0.l = 0x0001;
	r1.l = w[p0];
	r0 = r0 | r1;
	w[p0] = r0;
	ssync;

	/* Write DDR configuration values */
	//*pEBIU_MEM_CTL0  =  OTP_Get_Field(page_16_lower, \
			OTP_EBIU_CTRL0, 32) & 0x3FFFFFFFlu;
	p0.l = lo(EBIU_DDRCTL0);
	p0.h = hi(EBIU_DDRCTL0);
	//r0.l = 0x840E; //for 133 MHz SCLK
	//r0.l = 0x830C; //for 100 MHz SCLK
	r0.l = 0x8287; //for 83 MHz SCLK
	//r0.h = 0x218A; //normal operation
	r0.h =  0x23FE; //increased tRAS & tRP to max
	[p0] = r0;

	/* Write DDR configuration values */
	//*pEBIU_MEM_CTL1  = (OTP_Get_Field(page_16_lower, \
		OTP_EBIU_CTRL1, 32) & 0xF00FC3FFlu) | 0x00002000lu;\
		// fiels [13:12] must be 0b10
	p0.l = lo(EBIU_DDRCTL1);
	p0.h = hi(EBIU_DDRCTL1);
	//r0.l = 0x2323;
	//r0.h = 0x2002;
	r0.l = 0x2223; //Increased tRCD to 3, minumum setting is 2
	r0.h = 0x1002;
	[p0] = r0;
	ssync;

	/* DDR ram test */
	/* test 64K times, one cell per 1k bytes */
	r0.l = 0x0000;
	r0.h = 0x0001;
	p1 = 0x400;
	p0 = 0x0;
ddr_test:
	r1.l = 0xa5a5;
	r1.h = 0xa5a5;
	[p0] = r1;
	r2 = [p0];
	cc=r2==r1;
	if !cc jump test_failed;
	r1.l = 0x5a5a;
	r1.h = 0x5a5a;
	[p0] = r1;
	r2 = [p0];
	cc=r2==r1;
	if !cc jump test_failed;
	p0 = p0 + p1;
	r0 += -1;
	cc = r0 ==0;
	if !cc jump ddr_test;
test_ok:
	p0.l = lo(PORTG_SET);
	p0.h = hi(PORTG_SET);
	r0.l = 0x0040; /* Test Ok, light on LED1 */
	w[p0]= r0;
	ssync
	jump ddr_init_end;

test_failed:
	p0.l = lo(PORTG_SET);
	p0.h = hi(PORTG_SET);
	r0.l = 0x0080; /* Test failed, light on LED2 */
	w[p0]= r0;
	ssync
ddr_init_end:

#if (BFIN_BOOT_MODE == BFIN_BOOT_UART)
	/* clear forced RTS rather than relying on auto RTS */
	P3.h = hi(UART1_MCR)
	P3.l = lo(UART1_MCR)
	R0 = w[P3];
	bitclr(R0, 5);
	w[p3] = R0;
#endif

	(P5:0) = [SP++];
	(R7:0) = [SP++];
	RETS   = [SP++];
	ASTAT  = [SP++];
	RTS;

