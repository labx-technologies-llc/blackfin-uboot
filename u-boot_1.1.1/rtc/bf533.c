/*
 * (C) Copyright 2001
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

**********************************************************************************************************

                        PROJECT                 :       BFIN
                        VERISON                 :       2.0
                        FILE                    :       bf533.c
                        MODIFIED DATE           :       29 jun 2004
                        AUTHOR                  :       BFin Project-ADI
                        LOCATION                :       LG Soft India,Bangalore

***********************************************************************************************************/


#include <common.h>
#include <command.h>
#include <rtc.h>
#include <asm/blackfin.h>

#if defined(CONFIG_RTC_BF533) && (CONFIG_COMMANDS & CFG_CMD_DATE)

#include <asm/bf533_rtc.h>

void rtc_init()	{

	*(volatile unsigned short *)RTC_PREN = 0x1;
	wait_for_complete();
}

void rtc_set (struct rtc_time *rtc_tm)
{
	unsigned char mon,day,hrs,min,sec,leap_yr,wday;
	unsigned int yrs;

	
	yrs = rtc_tm->tm_year;
	mon = rtc_tm->tm_mon + 1;
	day = rtc_tm->tm_mday;
        hrs = rtc_tm->tm_hour;
        min = rtc_tm->tm_min;
        sec = rtc_tm->tm_sec;
	wday = rtc_tm->tm_wday;
	
	leap_yr = ((!(yrs % 4) && (yrs % 100)) || !(yrs % 400));

	g_year = yrs ;

        g_mon =  mon - 1;

        day --;
        g_last_day = 0;

	rtc_tm->tm_mday -= g_last_day;
	rtc_tm->tm_mday ++;

	rtc_tm->tm_mon = g_mon;
	rtc_tm->tm_year = g_year;

	*(volatile unsigned long *)RTC_STAT= ((day<<DAY_BITS_OFF) | (hrs<<HOUR_BITS_OFF) | (min<<MIN_BITS_OFF) | (sec<<SEC_BITS_OFF));
        wait_for_complete();
}

void rtc_get (struct rtc_time *rtc_tm)
{
	unsigned long cur_rtc_stat;
	unsigned long day;
	unsigned long cur_stat;
	int leap_yr1;
	int mon2;

	cur_rtc_stat = *(volatile unsigned long *)RTC_STAT;

	rtc_tm->tm_sec = (cur_rtc_stat>>SEC_BITS_OFF) & 0x3f;
	rtc_tm->tm_min = (cur_rtc_stat>>MIN_BITS_OFF) & 0x3f;
	rtc_tm->tm_hour = (cur_rtc_stat>>HOUR_BITS_OFF) & 0x1f;
	rtc_tm->tm_mday = (cur_rtc_stat>>DAY_BITS_OFF) & 0x7fff;
	
	day = (cur_rtc_stat>>24) & 0xff;
	mon2 = 0;

        leap_yr1 = ((!(g_year % 4) && (g_year % 100)) || !(g_year % 400));

        if((g_mon == 1) && leap_yr1)
            mon2 = 1;
       
        if((day-g_last_day) >= (days_in_mo[g_mon+1] + mon2))    {
       		if((255 - day) <= 31)   {
       			day = 0;
       		}
	        g_last_day = day;
	        g_mon ++;
	        if(g_mon > 12) {
	        	g_year++;
       			g_mon = 0;
	       	}
       		if(day == 0)	{
	       		cur_rtc_stat = (cur_stat&0x00ffffff);
        	       	*(volatile unsigned long *)RTC_STAT = cur_rtc_stat;
       			wait_for_complete();
       		}
	}
	
	rtc_tm->tm_mday -= g_last_day;
	rtc_tm->tm_mday ++;

	rtc_tm->tm_mon = g_mon;
	rtc_tm->tm_year = g_year;
	
	GregorianDay(rtc_tm);
}

void rtc_reset (void)
{
	return;	/* nothing to do */
}

void wait_for_complete(void)
{   
	while(!(*(volatile unsigned short *)RTC_ISTAT & 0x8000)) {
		printf("");
    	}
	*(volatile unsigned short *)RTC_ISTAT = 0x80000;	
}

#endif	/* CONFIG_RTC_BF533 && CFG_CMD_DATE */
