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
