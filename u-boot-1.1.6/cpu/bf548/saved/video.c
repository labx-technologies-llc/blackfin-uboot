/************************************************************************/
/* ** HEADER FILES							*/
/************************************************************************/

#include <stdarg.h>
#include <common.h>
#include <config.h>
#include <asm/blackfin.h>
#include <i2c.h>
#include <linux/types.h>
#include <devices.h>

#ifdef CONFIG_VIDEO
#define NTSC_FRAME_ADDR 0x06000000

#include "video.h"

/* NTSC OUTPUT SIZE  720 * 240 */
#define VERTICAL	2
#define HORIZONTAL	4

int IsVBlankLine ( const int nLine )
{
        /* This array contains a single bit for each line in
           an NTSC frame. */
                                                                                                                                                             
        if (    ( nLine <= 18 ) ||
                        ( nLine >=264 && nLine <= 281 ) ||
                        ( nLine == 528 ) )
                return true;
                                                                                                                                                             
        return false;
}

int NtscVideoOutFrameBuffInit ( char *base_address )
{
        const int nNumNTSCVoutFrames = 1;  // changed from 2 to 1 (dkl)
        const int nNumNTSCLines = 525;
        char *dest = base_address;
                                                                                                                                                             
//      SetVideoBufferBaseAddress ( base_address );
                                                                                                                                                             
        int nFrameNum, nLineNum;
                                                                                                                                                             
        for ( nFrameNum = 0; nFrameNum < nNumNTSCVoutFrames; ++nFrameNum )
        {
                for ( nLineNum = 1; nLineNum <= nNumNTSCLines; ++nLineNum )
                {
                        int offset = 0;
                        int i;
                                                                                                                                                             
                        if ( IsVBlankLine ( nLineNum ) )
                                offset ++;
                                                                                                                                                             
                        if ( nLineNum > 266 || nLineNum < 3 )
                                offset += 2;
                                                                                                                                                             
                        /* Output EAV code */
                        unsigned int code = SystemCodeMap[ offset ].EAV;
                        WriteDestByte ( (char) (code >> 24) & 0xff );
                        WriteDestByte ( (char) (code >> 16) & 0xff );
                        WriteDestByte ( (char) (code >> 8) & 0xff );
                        WriteDestByte ( (char) (code) & 0xff );
                                                                                                                                                             
                        /* Output horizontal blanking */
                        for ( i = 0; i < 67*2; ++i )
                        {
                                WriteDestByte ( 0x80 );
                                WriteDestByte ( 0x10 );
                        }
                                                                                                                                                             
                        /* Output SAV */
                        code = SystemCodeMap[ offset ].SAV;
                        WriteDestByte ( (char) (code >> 24) & 0xff );
                        WriteDestByte ( (char) (code >> 16) & 0xff );
                        WriteDestByte ( (char) (code >> 8) & 0xff );
                        WriteDestByte ( (char) (code) & 0xff );
                                                                                                                                                             
                        /* Output empty horizontal data */
                        for ( i = 0; i < 360*2; ++i )
                        {
                                WriteDestByte ( 0x80 );
                                WriteDestByte ( 0x10 );
                        }
                }
        }
                                                                                                                                                             
        return dest - base_address;
}

void FillFrame (char *Frame, int Value) {

        int *OddPtr32;
        int OddLine;
        int *EvenPtr32;
        int EvenLine;
        int i;
	int *data;
	int m,n;
	   
        // fill odd and even frames
        for (OddLine = 22, EvenLine = 285; OddLine < 263; OddLine++, EvenLine++) {
                OddPtr32 = (int *)((Frame + (OddLine * 1716)) + 276);
                EvenPtr32 = (int *)((Frame + (EvenLine * 1716)) + 276);
                for (i = 0; i < 360; i++, OddPtr32++, EvenPtr32++) {
                        *OddPtr32 = Value;
                        *EvenPtr32 = Value;
                        }
        }
	
	for(m=0;m<VERTICAL;m++){
		data = (int *)u_boot_logo.data;
		for (OddLine = (22+m), EvenLine = (285+m); OddLine < (u_boot_logo.height*VERTICAL)+(22+m); OddLine+=VERTICAL, EvenLine+=VERTICAL){
			OddPtr32 = (int *)((Frame + ((OddLine) * 1716)) + 276);
			EvenPtr32 = (int *)((Frame + ((EvenLine) * 1716)) + 276);
			for(i=0;i<u_boot_logo.width/2;i++){
			/* enlarge one pixel to m x n */
					for(n=0;n<HORIZONTAL;n++){
				        *OddPtr32++ = *data;
					*EvenPtr32++= *data;
					}
					 data++;
			}
		}
	}
        // return
}

void video_putc (const char c)
{
}

void video_puts (const char *s)
{
}

/************************************************************************/
/* ** VIDEO HIGH-LEVEL FUNCTIONS					*/
/************************************************************************/

static int video_init (void)
{
	char *NTSCFrame;
        NTSCFrame = (char *)NTSC_FRAME_ADDR;
        NtscVideoOutFrameBuffInit(NTSCFrame);
        FillFrame (NTSCFrame, BLUE);
                                                                                                                                                             
        *pPPI_CONTROL = 0x0082;
        *pPPI_FRAME   = 0x020D;
                                                                                                                                                             
        *pDMA0_START_ADDR       = NTSCFrame;
        *pDMA0_X_COUNT          = 0x035A;
        *pDMA0_X_MODIFY         = 0x0002;
        *pDMA0_Y_COUNT          = 0x020D;
        *pDMA0_Y_MODIFY         = 0x0002;
        *pDMA0_CONFIG           = 0x1015;
        *pPPI_CONTROL           = 0x0083;
	return 0;
}

int drv_video_init (void)
{
	int error, devices = 1;

	device_t videodev;

	video_init ();	/* Video initialization */

/* Device initialization */

	memset (&videodev, 0, sizeof (videodev));

	strcpy (videodev.name, "video");
	videodev.ext = DEV_EXT_VIDEO;	/* Video extensions */
	videodev.flags = DEV_FLAGS_OUTPUT;	/* Output only */
	videodev.putc = video_putc;	/* 'putc' function */
	videodev.puts = video_puts;	/* 'puts' function */

	error = device_register (&videodev);

	return (error == 0) ? devices : error;
}
#endif
