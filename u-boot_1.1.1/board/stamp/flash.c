/*
 * (C) Copyright 2004 - Analog Devices, Inc.  All rights reserved. 
 */

#include "flash-defines.h"

void flash_reset(void)
{
	ResetFlash();
}

unsigned long flash_get_size(ulong baseaddr, flash_info_t * info)
{
	int man_id = 0, dev_id = 0, i = 0;

	man_id = GetCodes();
	dev_id = man_id & 0xffff;
	man_id = (man_id >> 16) & 0xff;

	printf("Device ID of the Flash is %x\n", dev_id);
	printf("Manufacture ID of the Flash is %x\n", man_id);
	printf("Memory Map of Flash: 0x%x - 0x%x\n",CFG_FLASH0_BASE,(CFG_FLASH0_BASE+FLASH_SIZE)-1);

	info->flash_id = dev_id;
	info->sector_count = FLASH_TOT_SECT;
	info->size = FLASH_SIZE;

	info->start[0] = SECT1_ADDR; 
	info->start[1] = SECT2_ADDR;
	info->start[2] = SECT3_ADDR;
	info->start[3] = SECT4_ADDR;

	for (i = 4; i < 67; i++)
		info->start[i] = ((baseaddr + AFP_SectorSize1)
				  + ((i - 4) * AFP_SectorSize1));

	return (info->size);
}

unsigned long flash_init(void)
{
	unsigned long size_b0 = 0;
	int i;

	printf("Flash Memory Start 0x%x\n", CFG_FLASH_BASE);
	printf("Please type command flinfo for information on Sectors \n");
	init_Flags();
	for (i = 0; i < CFG_MAX_FLASH_BANKS; ++i) {
		flash_info[i].flash_id = FLASH_UNKNOWN;
	}
	size_b0 = flash_get_size(CFG_FLASH0_BASE, &flash_info[0]);
	if (flash_info[0].flash_id == FLASH_UNKNOWN || size_b0 == 0) {
		printf
		    ("## Unknown FLASH on Bank 0 - Size = 0x%08lx = %ld MB\n",
		     size_b0, size_b0 >> 20);
	}
	return size_b0;
}

void flash_print_info(flash_info_t * info)
{
	int i;

	if (info->flash_id == FLASH_UNKNOWN) {
		printf("missing or unknown FLASH type\n");
		return;
	}
	switch (info->flash_id) {
	case FLASH_DEV_ID:
		printf("ST Microelectronics ");
		break;
	default:
		printf("Unknown Vendor ");
		break;
	}
	for (i = 0; i < info->sector_count; ++i) {
		if ((i % 5) == 0)
			printf("\n   ");
		printf(" %08lX%s",
		       info->start[i],
		       info->protect[i] ? " (RO)" : "     ");
	}
	printf("\n");
	return;
}

int flash_erase(flash_info_t * info, int s_first, int s_last)
{
	int cnt = 0;
	int rc = ERR_OK;

	cnt = s_last - s_first + 1;

	if (cnt == FLASH_TOT_SECT) {
		printf(" Please Wait \n");
		rc =  EraseFlash(); 
		printf("done\n");
	} else if (cnt > 0) {
		printf("Sector erasing, Please Wait\n");
		rc = FLASH_Block_Erase(s_first,s_last);
		printf("done\n");
	}
	else {
		printf("Invalid target address\n");
		rc = ERR_INVAL;
	}
	return rc;
}

unsigned long GetOffset(int sec_num)
{
	if (sec_num == 0)
		return SECT1_ADDR;
	else if (sec_num == 1)
		return SECT2_ADDR;
	else if (sec_num == 2)
		return SECT3_ADDR;
	else if (sec_num == 3)
		return SECT4_ADDR;
	else
		return ((SECT1_ADDR + AFP_SectorSize1) +
			((sec_num - 4) * AFP_SectorSize1));
}

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

int write_word(flash_info_t *info, ulong addr, ulong data)
{
	volatile unsigned short *dest = (volatile unsigned short *)addr;
	ushort sdata = (ushort)data;
	int rcode;
	ulong flag;

	sdata = ((sdata & 0xff00) >> 8) | ((sdata & 0xff) << 8 );

	/* Check if Flash is (sufficiently) erased */
	if ((*dest & sdata) != sdata) {
        	return ERR_NOT_ERASED;
	}

	LED6_On();
	flag = disable_interrupts();
	FLASH_Base[WRITE_CMD1] = WRITE_DATA1;
	FLASH_Base[WRITE_CMD2] = WRITE_DATA2;
	FLASH_Base[WRITE_CMD3] = WRITE_DATA3;
	if(flag)
		enable_interrupts();
    	*dest = sdata;
	asm("ssync;");
	rcode = FlashDataToggle(CFG_FLASH_WRITE_TOUT);
	if (rcode != ERR_OK) {
		ResetFlash();
		return rcode;
	}
	LED6_Off();
	return rcode;
}

int ResetFlash()
{
	FLASH_Base[WRITE_CMD1] = WRITE_DATA1;
	FLASH_Base[WRITE_CMD2] = WRITE_DATA2;
	FLASH_Base[ANY_OFF] = RESET_DATA;

	/* Pause for 10 micro seconds */
	udelay(10);	

	return ERR_OK;
}

int EraseFlash()
{
	FLASH_Base[WRITE_CMD1] = ERASE_DATA1; 
	FLASH_Base[WRITE_CMD2] = ERASE_DATA2;
	FLASH_Base[WRITE_CMD3] = ERASE_DATA3;
	FLASH_Base[WRITE_CMD1] = ERASE_DATA4;
	FLASH_Base[WRITE_CMD2] = ERASE_DATA5;
	FLASH_Base[WRITE_CMD3] = ERASE_DATA6;

	return FlashDataToggle(CFG_FLASH_ERASE_TOUT);
}

int FlashDataToggle(unsigned long timeout)
{
	unsigned int u1,u2;
	unsigned long start,now,last;

	start = get_timer(0);
	last = get_timer(start);
	do {
		now = get_timer(start);

		/* show that we're waiting */
		if ((now - last) > 1000) {	/* every second */
			putc ('.');
			last = now;
		}

		u1 = FLASH_Base[ANY_OFF];
		u2 = FLASH_Base[ANY_OFF];
		if((u1 & 0x0040) == (u2 & 0x0040)) {
			return ERR_OK;
		}
		if((u2 & 0x0020) == 0x0000)
			continue;
		u1 = FLASH_Base[ANY_OFF];
		if((u2 & 0x0040) == (u1 & 0x0040)) {
			return ERR_OK;
		}
		else {
			printf("Data Toggle Error\n");
			ResetFlash();
			return ERR_PROG_ERROR;
		}
	}while(now < timeout);

	printf("TimeOut - DataToggle\n");	
	return ERR_TIMOUT;
}

int GetCodes()
{
	unsigned long dev_id = 0;
	unsigned long man_id = 0;

	FLASH_Base[WRITE_CMD1] = WRITE_DATA1;
	FLASH_Base[WRITE_CMD2] = WRITE_DATA2;
	FLASH_Base[WRITE_CMD3] = GETCODE_DATA;
	asm("ssync;");
	man_id = FLASH_Base[ANY_OFF];
	dev_id = *(unsigned short *) FB;
	asm("ssync;");

	FLASH_Base[ANY_OFF] = RESET_DATA;
	return ((man_id << 16) | dev_id);
}

int FLASH_Block_Erase(unsigned long s_first,unsigned long s_last)
{
	int i,rc,timeout_flag = 0;
	unsigned long Off,flag;
  	unsigned long start,now,timeout;

	flag = disable_interrupts();
	FLASH_Base[WRITE_CMD1] = ERASE_DATA1;
	FLASH_Base[WRITE_CMD2] = ERASE_DATA2;
	FLASH_Base[WRITE_CMD3] = ERASE_DATA3;
	FLASH_Base[WRITE_CMD1] = ERASE_DATA4;
	FLASH_Base[WRITE_CMD2] = ERASE_DATA5;

	/* Timer critical section every block must be added in 50 ms*/
	for (i = s_first; i <= s_last; i++) {
		Off = (GetOffset(i) - CFG_FLASH0_BASE);
		*(volatile unsigned short *) (CFG_FLASH0_BASE + Off) = 0x30;
		asm("ssync;");

		/* Check for Erase Timeout Period (is bit DQ3 set?) */
                if((FLASH_Base[0] & 0x0008) == 0x0008) {
			timeout_flag = 1;
                        break; /* Cannot see any more blocks due to timeout */
		}
	}

	if(flag)
		enable_interrupts();

	if (timeout_flag) {
		printf("Time out - All blocks couldn't be added\n");
		return ERR_TIMOUT;
	}

	start = get_timer(0);
	timeout = CFG_FLASH_ERASEBLOCK_TOUT * (s_last - s_first + 1);
	/* Wait for the Erase Timer Bit (DQ3) to be set */
	while(1) {
		now = get_timer(start);
		if((FLASH_Base[0] & 0x0008) == 0x0008) 
			break; /* Break when device starts the erase cycle */
		if (now > timeout) {
			printf("Time out - Erase cycle is not started\n");
			return ERR_TIMOUT;
		}
	}

	rc = FlashDataToggle(timeout);

	if(rc != ERR_OK) {
		ResetFlash();
		return rc;
	}
	return ERR_OK;
}

#if defined(CONFIG_MISC_INIT_R)
/* miscellaneous platform dependent initialisations */
int misc_init_r(void)
{
	int i;
	int cf_stat = 0;

	/* Check whether CF card is inserted */
	*(volatile unsigned short *) pFIO_EDGE = FIO_EDGE_CF_BITS;
	*(volatile unsigned short *) pFIO_POLAR = FIO_POLAR_CF_BITS;
	for (i=0; i < 0x300 ; i++) asm("nop;");
			
	if ( (*(volatile unsigned short *) pFIO_FLAG_S) & CF_STAT_BITS) {
		cf_stat = 0;
	}
	else {
		cf_stat = 1;
	}

	*(volatile unsigned short *) pFIO_EDGE  = FIO_EDGE_BITS;
	*(volatile unsigned short *) pFIO_POLAR = FIO_POLAR_BITS;
	

	if (cf_stat) {
                printf ("Booting from COMPACT flash\n");

		/* Set cycle time for CF */
		*(volatile unsigned long *) ambctl1 = CF_AMBCTL1VAL;


		pll_set(CF_CONFIG_VCO, CF_CONFIG_CRYSTAL_FREQ, CF_PLL_DIV_FACTOR);

		pll_div_fact = CF_PLL_DIV_FACTOR;

		for (i=0; i < 0x1000 ; i++) asm("nop;");
		for (i=0; i < 0x1000 ; i++) asm("nop;");
		for (i=0; i < 0x1000 ; i++) asm("nop;");

		serial_setbrg();
		ide_init();

                setenv ("bootargs", "");
                setenv ("bootcmd", "fatload ide 0:1 0x1000000 uImage-stamp;bootm 0x1000000;bootm 0x20100000" );
        } else {
                printf ("Booting from FLASH\n");
        }

	return 1;
}
#endif

#ifdef CONFIG_STAMP_CF

void cf_outb(unsigned char val, volatile unsigned char* addr)
{
	/* 
	 * Set PF1 PF0 respectively to 0 1 to divert address
	 * to the expansion memory banks  
	 */
        *(volatile unsigned short *) pFIO_FLAG_S = CF_PF0;
        *(volatile unsigned short *) pFIO_FLAG_C = CF_PF1;
        asm("ssync;");

        *(addr) = val;
        asm("ssync;");

	/* Setback PF1 PF0 to 0 0 to address external 
	 * memory banks  */
        *(volatile unsigned short *) pFIO_FLAG_C = CF_PF1_PF0;
        asm("ssync;");
}

unsigned char cf_inb(volatile unsigned char *addr)
{
	volatile unsigned char c;

	*(volatile unsigned short *) pFIO_FLAG_S = CF_PF0;
	*(volatile unsigned short *) pFIO_FLAG_C = CF_PF1;
	asm("ssync;");

	c = *(addr);
	asm("ssync;");

	*(volatile unsigned short *) pFIO_FLAG_C = CF_PF1_PF0;
	asm("ssync;");

	return c;
}

void cf_insw(unsigned short *sect_buf, unsigned short *addr, int words)
{
        int i;

        *(volatile unsigned short *) pFIO_FLAG_S = CF_PF0;
        *(volatile unsigned short *) pFIO_FLAG_C = CF_PF1;
        asm("ssync;");

        for (i = 0;i < words; i++) {
                *(sect_buf + i) = *(addr);
                asm("ssync;");
        }

        *(volatile unsigned short *) pFIO_FLAG_C = CF_PF1_PF0;
        asm("ssync;");
}

void cf_outsw(unsigned short *addr, unsigned short *sect_buf, int words)
{
        int i;

        *(volatile unsigned short *) pFIO_FLAG_S = CF_PF0;
        *(volatile unsigned short *) pFIO_FLAG_C = CF_PF1;
        asm("ssync;");

        for (i = 0;i < words; i++) {
                *(addr) = *(sect_buf + i);
                asm("ssync;");
        }

        *(volatile unsigned short *) pFIO_FLAG_C = CF_PF1_PF0;
        asm("ssync;");
}
#endif
