/*
 * (C) Copyright 2000
 * Paolo Scaffardi, AIRVENT SAM s.p.a - RIMINI(ITALY), arsenio@tin.it
 * (C) Copyright 2002
 * Wolfgang Denk, wd@denx.de
 * (C) Copyright 2006
 * Aubrey Li, aubrey.li@analog.com
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

#include <stdarg.h>
#include <common.h>
#include <config.h>
#include <malloc.h>
#include <asm/blackfin.h>
#include <asm/mach-common/bits/dma.h>
#include <i2c.h>
#include <linux/types.h>
#include <devices.h>

int gunzip(void *, int, unsigned char *, unsigned long *);

#ifdef CONFIG_VIDEO

#define DMA_SIZE16	2

#if defined(__ADSPBF533__)

#include <asm/mach-common/bits/ppi.h>

#define NTSC_FRAME_ADDR 0x06000000
#include "video.h"

/* NTSC OUTPUT SIZE  720 * 240 */
#define VERTICAL	2
#define HORIZONTAL	4

int is_vblank_line(const int line)
{
	/*
	 *  This array contains a single bit for each line in
	 *  an NTSC frame.
	 */
	if ((line <= 18) || (line >= 264 && line <= 281) || (line == 528))
		return true;

	return false;
}

int NTSC_framebuffer_init(char *base_address)
{
	const int NTSC_frames = 1;
	const int NTSC_lines = 525;
	char *dest = base_address;
	int frame_num, line_num;

	for (frame_num = 0; frame_num < NTSC_frames; ++frame_num) {
		for (line_num = 1; line_num <= NTSC_lines; ++line_num) {
			unsigned int code;
			int offset = 0;
			int i;

			if (is_vblank_line(line_num))
				offset++;

			if (line_num > 266 || line_num < 3)
				offset += 2;

			/* Output EAV code */
			code = SystemCodeMap[offset].EAV;
			write_dest_byte((char)(code >> 24) & 0xff);
			write_dest_byte((char)(code >> 16) & 0xff);
			write_dest_byte((char)(code >> 8) & 0xff);
			write_dest_byte((char)(code) & 0xff);

			/* Output horizontal blanking */
			for (i = 0; i < 67 * 2; ++i) {
				write_dest_byte(0x80);
				write_dest_byte(0x10);
			}

			/* Output SAV */
			code = SystemCodeMap[offset].SAV;
			write_dest_byte((char)(code >> 24) & 0xff);
			write_dest_byte((char)(code >> 16) & 0xff);
			write_dest_byte((char)(code >> 8) & 0xff);
			write_dest_byte((char)(code) & 0xff);

			/* Output empty horizontal data */
			for (i = 0; i < 360 * 2; ++i) {
				write_dest_byte(0x80);
				write_dest_byte(0x10);
			}
		}
	}

	return dest - base_address;
}

void fill_frame(char *Frame, int Value)
{
	int *OddPtr32;
	int OddLine;
	int *EvenPtr32;
	int EvenLine;
	int i;
	int *data;
	int m, n;

	/* fill odd and even frames */
	for (OddLine = 22, EvenLine = 285; OddLine < 263; OddLine++, EvenLine++) {
		OddPtr32 = (int *)((Frame + (OddLine * 1716)) + 276);
		EvenPtr32 = (int *)((Frame + (EvenLine * 1716)) + 276);
		for (i = 0; i < 360; i++, OddPtr32++, EvenPtr32++) {
			*OddPtr32 = Value;
			*EvenPtr32 = Value;
		}
	}

	for (m = 0; m < VERTICAL; m++) {
		data = (int *)u_boot_logo.data;
		for (OddLine = (22 + m), EvenLine = (285 + m);
		     OddLine < (u_boot_logo.height * VERTICAL) + (22 + m);
		     OddLine += VERTICAL, EvenLine += VERTICAL) {
			OddPtr32 = (int *)((Frame + ((OddLine) * 1716)) + 276);
			EvenPtr32 =
			    (int *)((Frame + ((EvenLine) * 1716)) + 276);
			for (i = 0; i < u_boot_logo.width / 2; i++) {
				/* enlarge one pixel to m x n */
				for (n = 0; n < HORIZONTAL; n++) {
					*OddPtr32++ = *data;
					*EvenPtr32++ = *data;
				}
				data++;
			}
		}
	}
}

static int video_init(void)
{
	char *NTSCFrame;
	NTSCFrame = (char *)NTSC_FRAME_ADDR;
	NTSC_framebuffer_init(NTSCFrame);
	fill_frame(NTSCFrame, BLUE);

	*pPPI_CONTROL = 0x0082;
	*pPPI_FRAME = 0x020D;

	*pDMA0_START_ADDR = NTSCFrame;
	*pDMA0_X_COUNT = 0x035A;
	*pDMA0_X_MODIFY = 0x0002;
	*pDMA0_Y_COUNT = 0x020D;
	*pDMA0_Y_MODIFY = 0x0002;
	*pDMA0_CONFIG = 0x1015;
	*pPPI_CONTROL = 0x0083;
	return 0;
}

#endif


#if defined(__ADSPBF52x__)

#include <asm/mach-common/bits/ppi.h>
#include <asm/mach-common/bits/timer.h>

#include "bfin_logo_230x230.h"

#define LCD_X_RES		320	/* Horizontal Resolution */
#define LCD_Y_RES		240	/* Vertical Resolution */
#define LCD_BPP			24	/* Bit Per Pixel */
#define LCD_PIXEL_SIZE		(LCD_BPP / 8)

#define	DMA_BUS_SIZE		16
#define	LCD_CLK         	(12*1000*1000)	/* 12MHz */

#define CLOCKS_PER_PIX		3

/* HS and VS timing parameters (all in number of PPI clk ticks) */
#define H_ACTPIX	(LCD_X_RES * CLOCKS_PER_PIX)	/* active horizontal pixel */
#define H_PERIOD	(408 * CLOCKS_PER_PIX)		/* HS period */
#define H_PULSE		90				/* HS pulse width */
#define H_START		204				/* first valid pixel */

#define U_LINE		1				/* Blanking Lines */

#define	V_LINES		(LCD_Y_RES + U_LINE)		/* total vertical lines */
#define V_PULSE		(3 * H_PERIOD)			/* VS pulse width (1-5 H_PERIODs) */
#define V_PERIOD	(H_PERIOD * V_LINES)		/* VS period */

#define ACTIVE_VIDEO_MEM_OFFSET	(U_LINE * H_ACTPIX)

#define PPI_TX_MODE		0x2
#define PPI_XFER_TYPE_11	0xC
#define PPI_PORT_CFG_01		0x10
#define PPI_PACK_EN		0x80
#define PPI_POLS_1		0x8000

/* enable and disable PPI functions */
void EnablePPI(void)
{
	*pPPI_CONTROL |= PORT_EN;
}

void DisablePPI(void)
{
	*pPPI_CONTROL &= ~PORT_EN;
}

void Init_Ports(void)
{
	*pPORTF_MUX &= ~PORT_x_MUX_0_MASK;
	*pPORTF_MUX |= PORT_x_MUX_0_FUNC_1;
	*pPORTF_FER |= PF0 | PF1 | PF2 | PF3 | PF4 | PF5 | PF6 | PF7;

	*pPORTG_MUX &= ~PORT_x_MUX_1_MASK;
	*pPORTG_MUX |= PORT_x_MUX_1_FUNC_1;
	*pPORTG_FER |= PG5;
}

void Init_PPI(void)
{

	*pPPI_DELAY = H_START;
	*pPPI_COUNT = (H_ACTPIX-1);
	*pPPI_FRAME = 0;

	/* PPI control, to be replaced with definitions */
	*pPPI_CONTROL = PPI_TX_MODE		|	/* output mode , PORT_DIR */
			PPI_XFER_TYPE_11	|	/* sync mode XFR_TYPE */
			PPI_PORT_CFG_01		|	/* two frame sync PORT_CFG */
			PPI_PACK_EN		|	/* packing enabled PACK_EN */
			PPI_POLS_1;			/* faling edge syncs POLS */


}

void Init_DMA(void *dst)
{
	*pDMA0_START_ADDR = dst;

	/* X count */
	*pDMA0_X_COUNT = H_ACTPIX / 2;
	*pDMA0_X_MODIFY = DMA_BUS_SIZE / 8;

	/* Y count */
	*pDMA0_Y_COUNT = V_LINES;
	*pDMA0_Y_MODIFY = DMA_BUS_SIZE / 8;

	/* DMA Config */
	*pDMA0_CONFIG =
		WDSIZE_16	|	/* 16 bit DMA */
		DMA2D 		|	/* 2D DMA */
		FLOW_AUTO;		/* autobuffer mode */
}


void EnableDMA(void)
{
	*pDMA0_CONFIG |= DMAEN;
}

void DisableDMA(void)
{
	*pDMA0_CONFIG &= ~DMAEN;
}


/* Init TIMER0 as Frame Sync 1 generator */
void InitTIMER0(void)
{
	*pTIMER_DISABLE |= TIMDIS0;			/* disable Timer */
	SSYNC();
	*pTIMER_STATUS  |= TIMIL0 | TOVF_ERR0 | TRUN0;	/* clear status */
	SSYNC();

	*pTIMER0_PERIOD  = H_PERIOD;
	SSYNC();
	*pTIMER0_WIDTH   = H_PULSE;
	SSYNC();

	*pTIMER0_CONFIG  = PWM_OUT |
				PERIOD_CNT   |
				TIN_SEL      |
				CLK_SEL      |
				EMU_RUN;
	SSYNC();
}

void EnableTIMER0(void)
{
	*pTIMER_ENABLE  |= TIMEN0;
	SSYNC();
}

void DisableTIMER0(void)
{
	*pTIMER_DISABLE  |= TIMDIS0;
	SSYNC();
}


void InitTIMER1(void)
{
	*pTIMER_DISABLE |= TIMDIS1;			/* disable Timer */
	SSYNC();
	*pTIMER_STATUS  |= TIMIL1 | TOVF_ERR1 | TRUN1;	/* clear status */
	SSYNC();


	*pTIMER1_PERIOD  = V_PERIOD;
	SSYNC();
	*pTIMER1_WIDTH   = V_PULSE;
	SSYNC();

	*pTIMER1_CONFIG  = PWM_OUT |
				PERIOD_CNT   |
				TIN_SEL      |
				CLK_SEL      |
				EMU_RUN;
	SSYNC();
}

void EnableTIMER1(void)
{
	*pTIMER_ENABLE  |= TIMEN1;
	SSYNC();
}

void DisableTIMER1(void)
{
	*pTIMER_DISABLE  |= TIMDIS1;
	SSYNC();
}

int video_init(void *dst)
{

	Init_Ports();
	Init_DMA(dst);
	EnableDMA();
	InitTIMER0();
	InitTIMER1();
	Init_PPI();
	EnablePPI();

	/* Frame sync 2 (VS) needs to start at least one PPI clk earlier */
	EnableTIMER1();
	/* Add Some Delay ... */
	SSYNC();
	SSYNC();
	SSYNC();
	SSYNC();

	/* now start frame sync 1 */
	EnableTIMER0();

	return 0;
}
#endif /* __ADSPBF52x__ */

#if defined(__ADSPBF54x__)

#include "bfin_logo_230x230.h"
#include <asm/mach-common/bits/eppi.h>

#define LCD_X_RES		480	/*Horizontal Resolution */
#define LCD_Y_RES		272	/* Vertical Resolution */

#define LCD_BPP			24	/* Bit Per Pixel */
#define LCD_PIXEL_SIZE		(LCD_BPP / 8)
#define	DMA_BUS_SIZE		32
#define ACTIVE_VIDEO_MEM_OFFSET 0

/* 	-- Horizontal synchronizing --
 *
 * Timing characteristics taken from the SHARP LQ043T1DG01 datasheet
 * (LCY-W-06602A Page 9 of 22)
 *
 * Clock Frequency 	1/Tc Min 7.83 Typ 9.00 Max 9.26 MHz
 *
 * Period 		TH - 525 - Clock
 * Pulse width 		THp - 41 - Clock
 * Horizontal period 	THd - 480 - Clock
 * Back porch 		THb - 2 - Clock
 * Front porch 		THf - 2 - Clock
 *
 * -- Vertical synchronizing --
 * Period 		TV - 286 - Line
 * Pulse width 		TVp - 10 - Line
 * Vertical period 	TVd - 272 - Line
 * Back porch 		TVb - 2 - Line
 * Front porch 		TVf - 2 - Line
 */

#define	LCD_CLK         	(8*1000*1000)	/* 8MHz */

/* # active data to transfer after Horizontal Delay clock */
#define EPPI_HCOUNT		LCD_X_RES

/* # active lines to transfer after Vertical Delay clock */
#define EPPI_VCOUNT		LCD_Y_RES

/* Samples per Line = 480 (active data) + 45 (padding) */
#define EPPI_LINE		525

/* Lines per Frame = 272 (active data) + 14 (padding) */
#define EPPI_FRAME		286

/* FS1 (Hsync) Width (Typical)*/
#define EPPI_FS1W_HBL		41

/* FS1 (Hsync) Period (Typical) */
#define EPPI_FS1P_AVPL		EPPI_LINE

/* Horizontal Delay clock after assertion of Hsync (Typical) */
#define EPPI_HDELAY		43

/* FS2 (Vsync) Width    = FS1 (Hsync) Period * 10 */
#define EPPI_FS2W_LVB		(EPPI_LINE * 10)

 /* FS2 (Vsync) Period   = FS1 (Hsync) Period * Lines per Frame */
#define EPPI_FS2P_LAVF		(EPPI_LINE * EPPI_FRAME)

/* Vertical Delay after assertion of Vsync (2 Lines) */
#define EPPI_VDELAY		12

#define EPPI_CLIP		0xFF00FF00

/* EPPI Control register configuration value for RGB out
 * - EPPI as Output
 * GP 2 frame sync mode,
 * Internal Clock generation disabled, Internal FS generation enabled,
 * Receives samples on EPPI_CLK raising edge, Transmits samples on EPPI_CLK falling edge,
 * FS1 & FS2 are active high,
 * DLEN = 6 (24 bits for RGB888 out) or 5 (18 bits for RGB666 out)
 * DMA Unpacking disabled when RGB Formating is enabled, otherwise DMA unpacking enabled
 * Swapping Enabled,
 * One (DMA) Channel Mode,
 * RGB Formatting Enabled for RGB666 output, disabled for RGB888 output
 * Regular watermark - when FIFO is 100% full,
 * Urgent watermark - when FIFO is 75% full
 */

#define EPPI_CONTROL		(0x20136E2E)

static inline u16 get_eppi_clkdiv(u32 target_ppi_clk)
{
	u32 sclk = get_sclk();

	/* EPPI_CLK = (SCLK) / (2 * (EPPI_CLKDIV[15:0] + 1)) */

	return (((sclk / target_ppi_clk) / 2) - 1);
}

void Init_PPI(void)
{
	u16 eppi_clkdiv = get_eppi_clkdiv(LCD_CLK);

	bfin_write_EPPI0_FS1W_HBL(EPPI_FS1W_HBL);
	bfin_write_EPPI0_FS1P_AVPL(EPPI_FS1P_AVPL);
	bfin_write_EPPI0_FS2W_LVB(EPPI_FS2W_LVB);
	bfin_write_EPPI0_FS2P_LAVF(EPPI_FS2P_LAVF);
	bfin_write_EPPI0_CLIP(EPPI_CLIP);

	bfin_write_EPPI0_FRAME(EPPI_FRAME);
	bfin_write_EPPI0_LINE(EPPI_LINE);

	bfin_write_EPPI0_HCOUNT(EPPI_HCOUNT);
	bfin_write_EPPI0_HDELAY(EPPI_HDELAY);
	bfin_write_EPPI0_VCOUNT(EPPI_VCOUNT);
	bfin_write_EPPI0_VDELAY(EPPI_VDELAY);

	bfin_write_EPPI0_CLKDIV(eppi_clkdiv);

/*
 * DLEN = 6 (24 bits for RGB888 out) or 5 (18 bits for RGB666 out)
 * RGB Formatting Enabled for RGB666 output, disabled for RGB888 output
 */
#if defined(CONFIG_VIDEO_RGB666)
		bfin_write_EPPI0_CONTROL((EPPI_CONTROL & ~DLENGTH) | DLEN_18 |
					 RGB_FMT_EN);
#else
		bfin_write_EPPI0_CONTROL(((EPPI_CONTROL & ~DLENGTH) | DLEN_24) &
					 ~RGB_FMT_EN);
#endif

}

#define               DEB2_URGENT  0x2000     /* DEB2 Urgent */

void Init_DMA(void *dst)
{

#if defined(CONFIG_DEB_DMA_URGENT)
	*pEBIU_DDRQUE |= DEB2_URGENT;
#endif

	*pDMA12_START_ADDR = dst;

	/* X count */
	*pDMA12_X_COUNT = (LCD_X_RES * LCD_BPP) / DMA_BUS_SIZE;
	*pDMA12_X_MODIFY = DMA_BUS_SIZE / 8;

	/* Y count */
	*pDMA12_Y_COUNT = LCD_Y_RES;
	*pDMA12_Y_MODIFY = DMA_BUS_SIZE / 8;

	/* DMA Config */
	*pDMA12_CONFIG =
		WDSIZE_32	|	/* 32 bit DMA */
		DMA2D 		|	/* 2D DMA */
		FLOW_AUTO;		/* autobuffer mode */
}

void Init_Ports(void)
{
	*pPORTF_MUX = 0x00000000;
	*pPORTF_FER |= 0xFFFF; /* PPI0..15 */

	*pPORTG_MUX &= ~(PORT_x_MUX_0_MASK | PORT_x_MUX_1_MASK | PORT_x_MUX_2_MASK | PORT_x_MUX_3_MASK | PORT_x_MUX_4_MASK);
	*pPORTG_FER |= PG0 | PG1 | PG2 | PG3 | PG4; /* CLK, FS1, FS2, PPI16..17  */

#if !defined(CONFIG_VIDEO_RGB666)
	*pPORTD_MUX &= ~(PORT_x_MUX_0_MASK | PORT_x_MUX_1_MASK | PORT_x_MUX_2_MASK | PORT_x_MUX_3_MASK | PORT_x_MUX_4_MASK | PORT_x_MUX_5_MASK);
	*pPORTD_MUX |= (PORT_x_MUX_0_FUNC_4 | PORT_x_MUX_1_FUNC_4 | PORT_x_MUX_2_FUNC_4 | PORT_x_MUX_3_FUNC_4 | PORT_x_MUX_4_FUNC_4 | PORT_x_MUX_5_FUNC_4);
	*pPORTD_FER |= PD0 | PD1 | PD2 | PD3 | PD4 | PD5; /* PPI18..23  */
#endif

	*pPORTE_FER &= ~PE3; /* DISP */
	*pPORTE_DIR_SET = PE3;
	*pPORTE_SET  = PE3;

}

void EnableDMA(void)
{
	*pDMA12_CONFIG |= DMAEN;
}

void DisableDMA(void)
{
	*pDMA12_CONFIG &= ~DMAEN;
}

/* enable and disable PPI functions */
void EnablePPI(void)
{
	bfin_write_EPPI0_CONTROL(bfin_read_EPPI0_CONTROL() | EPPI_EN);
}

void DisablePPI(void)
{
	bfin_write_EPPI0_CONTROL(bfin_read_EPPI0_CONTROL() & ~EPPI_EN);
}

int video_init(void *dst)
{
	Init_Ports();
	Init_DMA(dst);
	EnableDMA();
	Init_PPI();
	EnablePPI();

	return 0;
}

#endif /* __ADSPBF54x__ */

#ifdef bfin_write_MDMA1_D0_IRQ_STATUS /* Make BF561 Happy */
# define bfin_write_MDMA_D0_IRQ_STATUS bfin_write_MDMA1_D0_IRQ_STATUS
# define bfin_write_MDMA_D0_START_ADDR bfin_write_MDMA1_D0_START_ADDR
# define bfin_write_MDMA_D0_X_COUNT    bfin_write_MDMA1_D0_X_COUNT
# define bfin_write_MDMA_D0_X_MODIFY   bfin_write_MDMA1_D0_X_MODIFY
# define bfin_write_MDMA_D0_Y_COUNT    bfin_write_MDMA1_D0_Y_COUNT
# define bfin_write_MDMA_D0_Y_MODIFY   bfin_write_MDMA1_D0_Y_MODIFY
# define bfin_write_MDMA_D0_CONFIG     bfin_write_MDMA1_D0_CONFIG
# define bfin_write_MDMA_S0_START_ADDR bfin_write_MDMA1_S0_START_ADDR
# define bfin_write_MDMA_S0_X_COUNT    bfin_write_MDMA1_S0_X_COUNT
# define bfin_write_MDMA_S0_X_MODIFY   bfin_write_MDMA1_S0_X_MODIFY
# define bfin_write_MDMA_S0_Y_COUNT    bfin_write_MDMA1_S0_Y_COUNT
# define bfin_write_MDMA_S0_Y_MODIFY   bfin_write_MDMA1_S0_Y_MODIFY
# define bfin_write_MDMA_S0_CONFIG     bfin_write_MDMA1_S0_CONFIG
# define bfin_write_MDMA_D0_IRQ_STATUS bfin_write_MDMA1_D0_IRQ_STATUS
# define bfin_read_MDMA_D0_IRQ_STATUS  bfin_read_MDMA1_D0_IRQ_STATUS
#endif

static void dma_bitblit(void *dst, fastimage_t *logo, int x, int y)
{
	if (dcache_status())
		blackfin_dcache_flush_range(logo->data, logo->data + logo->size);

	bfin_write_MDMA_D0_IRQ_STATUS(DMA_DONE | DMA_ERR);

	/* Setup destination start address */
	bfin_write_MDMA_D0_START_ADDR(dst + ((x & -2) * LCD_PIXEL_SIZE)
					+ (y * LCD_X_RES * LCD_PIXEL_SIZE));
	/* Setup destination xcount */
	bfin_write_MDMA_D0_X_COUNT(logo->width * LCD_PIXEL_SIZE / DMA_SIZE16);
	/* Setup destination xmodify */
	bfin_write_MDMA_D0_X_MODIFY(DMA_SIZE16);

	/* Setup destination ycount */
	bfin_write_MDMA_D0_Y_COUNT(logo->height);
	/* Setup destination ymodify */
	bfin_write_MDMA_D0_Y_MODIFY((LCD_X_RES - logo->width) * LCD_PIXEL_SIZE + DMA_SIZE16);


	/* Setup Source start address */
	bfin_write_MDMA_S0_START_ADDR(logo->data);
	/* Setup Source xcount */
	bfin_write_MDMA_S0_X_COUNT(logo->width * LCD_PIXEL_SIZE / DMA_SIZE16);
	/* Setup Source xmodify */
	bfin_write_MDMA_S0_X_MODIFY(DMA_SIZE16);

	/* Setup Source ycount */
	bfin_write_MDMA_S0_Y_COUNT(logo->height);
	/* Setup Source ymodify */
	bfin_write_MDMA_S0_Y_MODIFY(DMA_SIZE16);


	/* Enable source DMA */
	bfin_write_MDMA_S0_CONFIG(DMAEN | WDSIZE_16 | DMA2D);
	SSYNC();
	bfin_write_MDMA_D0_CONFIG(WNR | DMAEN  | WDSIZE_16 | DMA2D);

	while (bfin_read_MDMA_D0_IRQ_STATUS() & DMA_RUN);

	bfin_write_MDMA_S0_IRQ_STATUS(bfin_read_MDMA_S0_IRQ_STATUS() | DMA_DONE | DMA_ERR);
	bfin_write_MDMA_D0_IRQ_STATUS(bfin_read_MDMA_D0_IRQ_STATUS() | DMA_DONE | DMA_ERR);

}

void video_putc(const char c)
{
}

void video_puts(const char *s)
{
}

int drv_video_init(void)
{
	int error, devices = 1;
	device_t videodev;

	u8 *dst;
	u32 fbmem_size = LCD_X_RES * LCD_Y_RES * LCD_PIXEL_SIZE + ACTIVE_VIDEO_MEM_OFFSET;

	dst = malloc(fbmem_size);

	if (dst == NULL) {
		printf("Failed to alloc FB memory\n");
		return -1;
	}

#ifdef EASYLOGO_ENABLE_GZIP
	unsigned char *data = EASYLOGO_DECOMP_BUFFER;
	unsigned long src_len = EASYLOGO_ENABLE_GZIP;
	if (gunzip(data, bfin_logo.size, bfin_logo.data, &src_len)) {
		puts("Failed to decompress logo\n");
		free(dst);
		return -1;
	}
	bfin_logo.data = data;
#endif

	memset(dst + ACTIVE_VIDEO_MEM_OFFSET, bfin_logo.data[0], fbmem_size - ACTIVE_VIDEO_MEM_OFFSET);

	dma_bitblit(dst + ACTIVE_VIDEO_MEM_OFFSET, &bfin_logo,
			(LCD_X_RES - bfin_logo.width) / 2,
			(LCD_Y_RES - bfin_logo.height) / 2);

	video_init(dst);		/* Video initialization */

	memset(&videodev, 0, sizeof(videodev));

	strcpy(videodev.name, "video");
	videodev.ext = DEV_EXT_VIDEO;	/* Video extensions */
	videodev.flags = DEV_FLAGS_SYSTEM;	/* No Output */
	videodev.putc = video_putc;	/* 'putc' function */
	videodev.puts = video_puts;	/* 'puts' function */

	error = device_register(&videodev);

	return (error == 0) ? devices : error;
}

#endif
