/*
 * U-boot - BF537.c
 *
 * Copyright (c) 2005 blackfin.uclinux.org
 *
 * (C) Copyright 2000-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
#include <config.h>
#include <asm/blackfin.h>

#define POST_WORD_ADDR 0xFF903FFC

int checkboard(void)
{
	printf("CPU:   ADSP BF537 Rev.: 0.%d\n", *pCHIPID >>28);
	printf("Board: ADI BF537 stamp board\n");
	printf("       Support: http://blackfin.uclinux.org/\n");
	return 0;
}

long int initdram(int board_type)
{
	DECLARE_GLOBAL_DATA_PTR;
#ifdef DEBUG
	int brate;
	char *tmp = getenv("baudrate");
	brate = simple_strtoul(tmp, NULL, 16);
	printf("Serial Port initialized with Baud rate = %x\n",brate);
	printf("SDRAM attributes:\n");
	printf("tRCD %d SCLK Cycles,tRP %d SCLK Cycles,tRAS %d SCLK Cycles"
	       "tWR %d SCLK Cycles,CAS Latency %d SCLK cycles \n",
	       3, 3, 6, 2, 3);
	printf("SDRAM Begin: 0x%x\n", CFG_SDRAM_BASE);
	printf("Bank size = %d MB\n", CFG_MAX_RAM_SIZE >> 20);
#endif
	gd->bd->bi_memstart = CFG_SDRAM_BASE;
	gd->bd->bi_memsize = CFG_MAX_RAM_SIZE;
	return CFG_MAX_RAM_SIZE;
}

#if defined(CONFIG_MISC_INIT_R)
/* miscellaneous platform dependent initialisations */
int misc_init_r(void)
{
	return 0;
}
#endif

#ifdef CONFIG_POST
/* Using sw10-PF5 as the hotkey */
int post_hotkeys_pressed(void)
{
	int delay = 3;
	int i;
	unsigned short value;
	
	*pPORTF_FER   &= ~PF5;
	*pPORTFIO_DIR &= ~PF5;
	*pPORTFIO_INEN|=  PF5;
	
	printf("########Press SW10 to enter Memory POST########: %2d ",delay);	
	while(delay--){
		for(i=0;i<100;i++){
			value = *pPORTFIO & PF5;
			if(value != 0){
				break;
				}
			udelay(10000);		
			}
		printf("\b\b\b%2d ",delay);
	}
	printf("\b\b\b 0");
	printf("\n");
	if(value == 0)
		return 0;
	else
		{
		printf("Hotkey has been pressed, Enter POST . . . . . .\n");
		return 1;
		}
}
#endif

#if defined(CONFIG_POST) || defined(CONFIG_LOGBUFFER)
void post_word_store(ulong a)
{
	volatile ulong *save_addr =
		(volatile ulong *)POST_WORD_ADDR;
	*save_addr = a;
}

ulong post_word_load(void)
{
	volatile ulong *save_addr = 
		(volatile ulong *)POST_WORD_ADDR;
	return *save_addr;
}
#endif

#ifdef CONFIG_POST
int uart_post_test(int flags)
{
	return 0;
}

int flash_post_test(int flags)
{
	return 0;
}

/****************************************************
 * LED1 ---- PF6	LED2 ---- PF7		    *
 * LED3 ---- PF8	LED4 ---- PF9		    *
 * LED5 ---- PF10	LED6 ---- PF11		    *
 ****************************************************/
int led_post_test(int flags)
{
	*pPORTF_FER   	&= ~(PF6|PF7|PF8|PF9|PF10|PF11);
        *pPORTFIO_DIR 	|= PF6|PF7|PF8|PF9|PF10|PF11;
        *pPORTFIO_INEN	&= ~(PF6|PF7|PF8|PF9|PF10|PF11);	
	*pPORTFIO 	&= ~(PF6|PF7|PF8|PF9|PF10|PF11);
	udelay(1000000);
	printf("LED1 on");
	*pPORTFIO	|= PF6;
	udelay(1000000);
	printf("\b\b\b\b\b\b\b");
	printf("LED2 on");
	*pPORTFIO	|= PF7;
	udelay(1000000);
	printf("\b\b\b\b\b\b\b");
	printf("LED3 on");
        *pPORTFIO       |= PF8;
        udelay(1000000);
	printf("\b\b\b\b\b\b\b");
	printf("LED4 on");
        *pPORTFIO       |= PF9;
	udelay(1000000);
	printf("\b\b\b\b\b\b\b");
	printf("LED5 on");
        *pPORTFIO       |= PF10;
        udelay(1000000);
	printf("\b\b\b\b\b\b\b");
	printf("lED6 on");
        *pPORTFIO       |= PF11;
	printf("\b\b\b\b\b\b\b ");
	return 0;
}

/************************************************
 *  SW10 ---- PF5	SW11 ---- PF4		*
 *  SW12 ---- PF3	SW13 ---- PF2		*
 ************************************************/
int button_post_test(int flags)
{
	int i,delay = 5;
	unsigned short value = 0;	
	int result = 0;

        *pPORTF_FER   &= ~(PF5|PF4|PF3|PF2);
        *pPORTFIO_DIR &= ~(PF5|PF4|PF3|PF2);
        *pPORTFIO_INEN|=  (PF5|PF4|PF3|PF2);

        printf("\n--------Press SW10: %2d ",delay);
        while(delay--){
                for(i=0;i<100;i++){
                        value = *pPORTFIO & PF5;
                        if(value != 0){
                                break;
                                }
                        udelay(10000);
                        }
                printf("\b\b\b%2d ",delay);
        }
	if(value!=0)
		printf("\b\bOK");
	else{
		result = -1;
		printf("\b\bfailed");
	}	

	delay = 5;
	printf("\n--------Press SW11: %2d ",delay);
        while(delay--){
                for(i=0;i<100;i++){
                        value = *pPORTFIO & PF4;
                        if(value != 0){
                                break;
                                }
                        udelay(10000);
                        }
                printf("\b\b\b%2d ",delay);
        }
        if(value!=0)
                printf("\b\bOK");
        else{
                result = -1;
		printf("\b\bfailed");
	}

	delay = 5;
        printf("\n--------Press SW12: %2d ",delay);
        while(delay--){
                for(i=0;i<100;i++){
                        value = *pPORTFIO & PF3;
                        if(value != 0){
                                break;
                                }
                        udelay(10000);
                        }
                printf("\b\b\b%2d ",delay);
        }
        if(value!=0)
                printf("\b\bOK");
        else{
                result = -1;
                printf("\b\bfailed");
        }

	delay = 5;
        printf("\n--------Press SW13: %2d ",delay);
        while(delay--){
                for(i=0;i<100;i++){
                        value = *pPORTFIO & PF2;
                        if(value != 0){
                                break;
                                }
                        udelay(10000);
                        }
                printf("\b\b\b%2d ",delay);
        }
        if(value!=0)
                printf("\b\bOK");
        else{
                result = -1;
                printf("\b\bfailed");
        }
	printf("\n");
	return result;
}
#endif
