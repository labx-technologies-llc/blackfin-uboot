#define ASSEMBLY

#include <linux/config.h>
#include <config.h>
#include <asm/blackfin.h>
#include <asm/mem_init.h>
.global init_sdram;

#if (BFIN_BOOT_MODE != BF537_UART_BOOT)
#if (CONFIG_CCLK_DIV == 1)
#define CONFIG_CCLK_ACT_DIV   CCLK_DIV1
#endif
#if (CONFIG_CCLK_DIV == 2)
#define CONFIG_CCLK_ACT_DIV   CCLK_DIV2
#endif
#if (CONFIG_CCLK_DIV == 4)
#define CONFIG_CCLK_ACT_DIV   CCLK_DIV4
#endif
#if (CONFIG_CCLK_DIV == 8)
#define CONFIG_CCLK_ACT_DIV   CCLK_DIV8
#endif
#ifndef CONFIG_CCLK_ACT_DIV
#define CONFIG_CCLK_ACT_DIV   CONFIG_CCLK_DIV_not_defined_properly
#endif
#endif

init_sdram:
	[--SP] = ASTAT;
	[--SP] = RETS;
	[--SP] = (R7:0);
	[--SP] = (P5:0);

#if (BFIN_BOOT_MODE == BFIN_SPI_MASTER_BOOT)
        p0.h = hi(SIC_IWR0);
        p0.l = lo(SIC_IWR0);
        r0.l = 0x1;
        w[p0] = r0.l;
        SSYNC;

	p0.h = hi(SPI_BAUD);
	p0.l = lo(SPI_BAUD);
	r0.l = CONFIG_SPI_BAUD_INITBLOCK;
	w[p0] = r0.l;
    	SSYNC;
#endif

#if 0 //mh
#if (BFIN_BOOT_MODE != BFIN_UART_BOOT)

#ifdef CONFIG_BF537
	/* Enable PHY CLK buffer output */
	p0.h = hi(VR_CTL);
	p0.l = lo(VR_CTL);
	r0.l = w[p0];
	bitset(r0, 14);
	w[p0] = r0.l;
	ssync;
#endif
	/*
         * PLL_LOCKCNT - how many SCLK Cycles to delay while PLL becomes stable
         */
	p0.h = hi(PLL_LOCKCNT);
	p0.l = lo(PLL_LOCKCNT);
	r0 = 0x300(Z);
	w[p0] = r0.l;
	ssync;

	/*
         * Put SDRAM in self-refresh, incase anything is running
         */
        P2.H = hi(EBIU_SDGCTL);
        P2.L = lo(EBIU_SDGCTL);
        R0 = [P2];
        BITSET (R0, 24);
        [P2] = R0;
        SSYNC;

        /*
         *  Set PLL_CTL with the value that we calculate in R0
         *   - [14:09] = MSEL[5:0] : CLKIN / VCO multiplication factors
         *   - [8]     = BYPASS    : BYPASS the PLL, run CLKIN into CCLK/SCLK
         *   - [7]     = output delay (add 200ps of delay to mem signals)
         *   - [6]     = input delay (add 200ps of input delay to mem signals)
         *   - [5]     = PDWN      : 1=All Clocks off
         *   - [3]     = STOPCK    : 1=Core Clock off
         *   - [1]     = PLL_OFF   : 1=Disable Power to PLL
         *   - [0]     = DF        : 1=Pass CLKIN/2 to PLL / 0=Pass CLKIN to PLL
         *   all other bits set to zero
         */

        r0 = CONFIG_VCO_MULT & 63;      /* Load the VCO multiplier         */
        r0 = r0 << 9;                   /* Shift it over,                  */
        r1 = CONFIG_CLKIN_HALF;        /* Do we need to divide CLKIN by 2?*/
        r0 = r1 | r0;
        r1 = CONFIG_PLL_BYPASS;         /* Bypass the PLL?                 */
        r1 = r1 << 8;                   /* Shift it over                   */
        r0 = r1 | r0;                   /* add them all together           */

        p0.h = hi(PLL_CTL);
        p0.l = lo(PLL_CTL);             /* Load the address                */
        cli r2;                         /* Disable interrupts              */
	ssync;
        w[p0] = r0.l;                   /* Set the value                   */
        idle;                           /* Wait for the PLL to stablize    */
        sti r2;                         /* Enable interrupts               */

check_again:
	p0.h = hi(PLL_STAT);
	p0.l = lo(PLL_STAT);
	R0 = W[P0](Z);
	CC = BITTST(R0,5);
	if ! CC jump check_again;

	/* Configure SCLK & CCLK Dividers */
       	r0 = (CONFIG_CCLK_ACT_DIV | CONFIG_SCLK_DIV);
        p0.h = hi(PLL_DIV);
        p0.l = lo(PLL_DIV);
        w[p0] = r0.l;
        ssync;
#endif
#endif //mh


	//Set up PLL Div
    p0.l = lo(PLL_DIV);
    p0.h = hi(PLL_DIV);
	r0.l = 0x0003; //83.333 MHz SCLK
	//r0.l = 0x0002; //125 MHz SCLK
    w[p0] = r0;
    ssync;

	// configure GPIO
	// set port f function enable register to configure PF0-PF15 as GPIO (not peripheral)
	p0.l = lo(PORTF_FER);
    p0.h = hi(PORTF_FER);
    r0.l = 0x0000;
    w[p0] = r0;

	// set port f MUX to configure PF0-PF15 as GPIO (not peripheral)
	p0.l = lo(PORTF_MUX);
    p0.h = hi(PORTF_MUX);
    r0.l = 0x0000;
    w[p0] = r0;

	// set port f direction register to enable PF0-PF15 as outputs
    p0.l = lo(PORTF_DIR_SET);
    p0.h = hi(PORTF_DIR_SET);
    r0.l = 0xFFFF;
    w[p0] = r0;
    ssync; 
        
	// set port f clear register
    p0.l = lo(PORTF_CLEAR);
    p0.h = hi(PORTF_CLEAR);
    r0.l = 0xFFFF;
    w[p0] = r0;
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
		
				
        // release the DDR controller from reset as per spec
    	//        *pEBIU_RST_CTL |= 0x0001;
        //			ssync();
        p0.l = lo(EBIU_RSTCTL);
    	p0.h = hi(EBIU_RSTCTL);
    	r0.l = 0x0001;
    	r1.l = w[p0];
    	r0 = r0 | r1;
    	w[p0] = r0;
    	ssync;
   	
		//Write DDR configuration values
    	//*pEBIU_MEM_CTL0  =  OTP_Get_Field(page_16_lower, OTP_EBIU_CTRL0, 32) & 0x3FFFFFFFlu;
		p0.l = lo(EBIU_DDRCTL0);
    	p0.h = hi(EBIU_DDRCTL0);
    	//r0.l = 0x840E; //for 133 MHz SCLK
    	//r0.l = 0x830C; //for 100 MHz SCLK
    	r0.l = 0x8287; //for 83 MHz SCLK
    	//r0.h = 0x218A; //normal operation
    	r0.h =  0x23FE; //increased tRAS & tRP to max
    	[p0] = r0;
		
    	//Write DDR configuration values
		//*pEBIU_MEM_CTL1  = (OTP_Get_Field(page_16_lower, OTP_EBIU_CTRL1, 32) & 0xF00FC3FFlu) | 0x00002000lu;	// fiels [13:12] must be 0b10
		p0.l = lo(EBIU_DDRCTL1);
    	p0.h = hi(EBIU_DDRCTL1);
    	//r0.l = 0x2323; //For increased speed change to 0x2222. tWR and tRCD are very close to requiring more than 2 SCLK cycles
    	//r0.h = 0x2002; //For increased speed change to 0x1002. tWTR is very close to requiring more than 1 SCLK cycles
    	r0.l = 0x2223; //Increased tRCD to 3, minumum setting is 2
    	r0.h = 0x1002;
    	[p0] = r0;
		ssync;    

	(P5:0) = [SP++];
	(R7:0) = [SP++];
	RETS   = [SP++];
	ASTAT  = [SP++];
	RTS;

