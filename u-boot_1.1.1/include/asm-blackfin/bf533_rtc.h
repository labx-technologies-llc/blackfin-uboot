#ifndef _BF533_RTC_H_
#define _BF533_RTC_H_

void rtc_init(void);
void wait_for_complete(void);
void rtc_reset(void);

#define DAY_BITS_OFF	17
#define HOUR_BITS_OFF	12
#define MIN_BITS_OFF	6
#define SEC_BITS_OFF	0

static unsigned long g_year = 0;
static unsigned long g_mon = 0;
static unsigned long g_last_day = 0;

static const unsigned char days_in_mo[] =
	{ 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

#endif
