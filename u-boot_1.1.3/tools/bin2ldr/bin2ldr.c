/******************************************************************** 
 * BIN2LDR: convert the application binary file to LDR format file  *
 ********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include "bin2ldr.h"
#define INIT_FILE "init_sdram_bootrom_initblock.bin"
#define JUMP_FILE "jump.bin"
#define APP_FILE "app.bin"
#define OUT_FILE "app.ldr"

#define PORT_G    0x0400 >>8
#define GPIO6     0x00C0
#define ZEROFILL  0x0001
#define RESVECT   0x0002
#define INIT      0x0008
#define IGNORE    0x0010
#define FINAL     0x8000 >>8

#define BLOCK_SIZE 0x8000

int block[0x80000] = {0};
int app[0x80000]= {0};

static struct option bin2ldr_options[]={
	{"proc", required_argument, 0, 'p'},
	{"help", no_argument, 0, 'h'},
};

void usage(void)
{
	printf("----------------------------------------\n");
	printf("bin2ldr 1.0 (C) 2005-2006 Analog Devices\n");
	printf("Usage: bin2ldr --proc [target name]\n");
	printf("target name: bf531,bf532,bf533,bf534,bf535,bf536,bf537,bf561\n\n");
}

int main(int argc, char *argv[])
{
	int i=0,c,ch;
	int temp,block_temp1,block_temp2,block_temp3,block_temp4;
	int dxe_temp1,dxe_temp2,dxe_temp3,dxe_temp4;
	int app_i,app_j,app_x,app_y,start_addr;

	FILE *init_fd, *jump_fd, *app_fd, *out_fd;
	int init_n,jump_n,app_n,dxe_n;
	char *target = NULL;

	if ((out_fd = fopen(OUT_FILE,"wb")) < 0) {
		printf("Error open %s to write\n",OUT_FILE);
		exit(EXIT_FAILURE);
	}
	if ((init_fd = fopen(INIT_FILE,"rb")) < 0) {
		printf("Error open %s to read\n",INIT_FILE);
		exit(EXIT_FAILURE);
	}
	if ((jump_fd = fopen(JUMP_FILE,"rb")) < 0) {
		printf("Error open %s to read\n",JUMP_FILE);
		exit(EXIT_FAILURE);
	}
	if ((app_fd = fopen(APP_FILE,"rb")) < 0) {
		printf("Error open %s to read\n",APP_FILE);
		exit(EXIT_FAILURE);
	}

	while((c = getopt_long(argc, argv, "p:h", bin2ldr_options, (int *)0))!=EOF){
		switch(c){
			case 'p':
				target = optarg;
				break;
			case 'h':
				usage();
				break;
			default:
				printf("unsupported options\n");
		}
	}

	if(target == NULL){
		usage();
		exit(EXIT_FAILURE);
	}
	
	if(strcmp(target,"bf561") == 0){
/* BF561 needs an additional global header.
   The details can be found at page 2-39 in
   http://www.analog.com/UploadedFiles/Associated_Docs/8055513140_loader_man.pdf */
	block [i++] = 0xDF;
	block [i++] = 0x00;
	block [i++] = 0x00;
	block [i++] = 0xA0;
	}
/*
 * DXE 
 */

/* Add BLOCK 0, 10 bytes head and 4 bytes data */
	block[i++] = 0x40;
	block[i++] = 0x00;
	block[i++] = 0x80;
	block[i++] = 0xff;

	block[i++] = 0x04;
	block[i++] = 0x00;
	block[i++] = 0x00;
	block[i++] = 0x00;

#if (BFIN_BOOT_MODE == BF537_UART_BOOT)
	block[i++] = (GPIO6|IGNORE|RESVECT);
	block[i++] = PORT_G;
#else
	block[i++] = (IGNORE|RESVECT);
	block[i++] = 0;
#endif
	dxe_temp1  = i;
	block[i++] = 0x00;
	dxe_temp2  = i;
	block[i++] = 0x00;
	dxe_temp3  = i;
	block[i++] = 0x00;
	dxe_temp4  = i;
	block[i++] = 0x00;

/* Add BLOCK 1, init block */
	block[i++] = 0x00;	/* create head of init block */
	block[i++] = 0x00;
	block[i++] = 0xa0;
	block[i++] = 0xff;
	block_temp1 = i;
	block[i++] = 0x00;	/* count of init block,init to ZERO */
	block_temp2 = i;
	block[i++] = 0x00;
	block_temp3 = i;
	block[i++] = 0x00;
	block_temp4 = i;
	block[i++] = 0x00;
#if (BFIN_BOOT_MODE == BF537_UART_BOOT)
	block[i++] = (GPIO6|INIT|RESVECT);
	block[i++] = PORT_G;
#else
	block[i++] = (INIT|RESVECT);	/* flag of init block */ 
	block[i++] = 0x00;
#endif

	temp = i;		/* index for the data of init block */
	while ((ch=fgetc(init_fd))!=EOF)
		block[i++] = ch;
	
	init_n = i-temp;

/* re-write the count of init_block */
	block[block_temp1] = init_n & 0xFF;
	block[block_temp2] = (init_n & 0xFF00)     >>8;
	block[block_temp3] = (init_n & 0xFF0000)   >> 16;
	block[block_temp4] = (init_n & 0xFF000000) >> 24;

	dxe_n = i - 0x0e;
	block[dxe_temp1] = dxe_n & 0xFF;
	block[dxe_temp2] = (dxe_n & 0xFF00)	>>8;
	block[dxe_temp3] = (dxe_n & 0xFF0000)	>>16;
	block[dxe_temp4] = (dxe_n & 0xFF000000)	>> 24;
/*
 * DXE 1
 */

	temp = i;	/* save the start index of DXE 1*/
/* Add BLOCK 0, 10 bytes head and 4 bytes data */
	block[i++] = 0x40;
	block[i++] = 0x00;
	block[i++] = 0x80;
	block[i++] = 0xff;

	block[i++] = 0x04;
	block[i++] = 0x00;
	block[i++] = 0x00;
	block[i++] = 0x00;

#if (BFIN_BOOT_MODE == BF537_UART_BOOT)
	block[i++] = (GPIO6|IGNORE|RESVECT);
	block[i++] = PORT_G;
#else
	block[i++] = (IGNORE|RESVECT);
	block[i++] = 0;
#endif

	dxe_temp1  = i;
	block[i++] = 0x00;
	dxe_temp2  = i;
	block[i++] = 0x00;
	dxe_temp3  = i;
	block[i++] = 0x00;
	dxe_temp4  = i;
	block[i++] = 0x00;

/* Add BLOCK 1, jump block */
	block[i++]  = 0x00;
	block[i++]  = 0x00;
	block[i++]  = 0xa0;
	block[i++]  = 0xff;

	block_temp1 = i;
	block[i++]  = 0x00;
	block_temp2 = i;
	block[i++]  = 0x00;
	block_temp3 = i;
	block[i++]  = 0x00;
	block_temp4 = i;
	block[i++]  = 0x00;

#if (BFIN_BOOT_MODE == BF537_UART_BOOT)
	block[i++] = (GPIO6|RESVECT);
	block[i++] = PORT_G;
#else
	block[i++] = RESVECT;
	block[i++] = 0;
#endif
	jump_n = i;
	while ((ch=fgetc(jump_fd))!=EOF)
		block[i++] = ch;
	jump_n = i - jump_n;

/* re-write the count of init_block */
	block[block_temp1] = jump_n & 0xFF;
	block[block_temp2] = (jump_n & 0xFF00)     >>8;
	block[block_temp3] = (jump_n & 0xFF0000)   >> 16;
	block[block_temp4] = (jump_n & 0xFF000000) >> 24;

/* Add BLOCK 2, app block */
	app_n = 0;
	while ((ch=fgetc(app_fd))!=EOF)
		app[app_n++] = ch;

	app_x = app_n / BLOCK_SIZE;
	app_y = app_n % BLOCK_SIZE;
	for (app_i = 0;app_i < app_x;app_i++) {
		start_addr = APP_ENTRY + app_i*BLOCK_SIZE;
		block[i++] = start_addr & 0xFF;
		block[i++] = (start_addr & 0xFF00)     >> 8;
		block[i++] = (start_addr & 0xFF0000)   >> 16;
		block[i++] = (start_addr & 0xFF000000) >> 24;
		block[i++] = BLOCK_SIZE & 0xFF;
		block[i++] = (BLOCK_SIZE & 0xFF00)     >> 8;
		block[i++] = (BLOCK_SIZE & 0xFF0000)   >> 16;
		block[i++] = (BLOCK_SIZE & 0xFF000000) >> 24;
#if (BFIN_BOOT_MODE == BF537_UART_BOOT)
		block[i++] = (GPIO6|RESVECT);
		block[i++] = PORT_G;
#else
		block[i++] = RESVECT;
		block[i++] = 0;
#endif
		for (app_j = app_i*BLOCK_SIZE; app_j < (app_i+1)*BLOCK_SIZE; app_j++)
			block[i++] = app[app_j];
	}
	start_addr = APP_ENTRY + app_i*BLOCK_SIZE;
	block[i++] = start_addr & 0xFF;
	block[i++] = (start_addr & 0xFF00)     >> 8;
	block[i++] = (start_addr & 0xFF0000)   >> 16;
	block[i++] = (start_addr & 0xFF000000) >> 24;
	block[i++] = app_y & 0xFF;
	block[i++] = (app_y & 0xFF00)          >> 8;
	block[i++] = (app_y & 0xFF0000)        >> 16;
	block[i++] = (app_y & 0xFF000000)      >> 24;

#if (BFIN_BOOT_MODE == BF537_UART_BOOT)
	block[i++] = (GPIO6|RESVECT);
	block[i++] = PORT_G|FINAL;
#else
	block[i++] = RESVECT;
	block[i++] = FINAL;
#endif
	for (app_j = app_i*BLOCK_SIZE;app_j < app_n; app_j ++)
		block[i++] = app[app_j];

/* re-write the count of the whole file */
	dxe_n = i-temp-0x0e;
	block[dxe_temp1] = dxe_n & 0xFF;
	block[dxe_temp2] = (dxe_n & 0xFF00)     >>8;
	block[dxe_temp3] = (dxe_n & 0xFF0000)   >> 16;
	block[dxe_temp4] = (dxe_n & 0xFF000000) >> 24;

	dxe_n = i;

	for (i=0;i<dxe_n;i++)
		fputc(block[i],out_fd);
	fclose(app_fd);
	fclose(jump_fd);
	fclose(init_fd);
	fclose(out_fd);

	printf("Done!\n");
	return EXIT_SUCCESS;
}
