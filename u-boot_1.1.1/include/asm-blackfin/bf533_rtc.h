/*
 * Copyright (C) 2004  LG Soft India 
 */

#ifndef _BF533_RTC_H_
#define _BF533_RTC_H_

void rtc_init(void);
void wait_for_complete(void);
void rtc_reset(void);

#define MIN_TO_SECS(_x_) (60 * _x_)
#define HRS_TO_SECS(_x_) (60 * 60 * _x_)
#define DAYS_TO_SECS(_x_) (24 * 60 * 60 * _x_)

#define NUM_SECS_IN_DAY (24 * 3600)
#define NUM_SECS_IN_HOUR (3600)
#define NUM_SECS_IN_MIN (60)

/* Shift values for RTC_STAT register */
#define DAY_BITS_OFF	17
#define HOUR_BITS_OFF	12
#define MIN_BITS_OFF	6
#define SEC_BITS_OFF	0

#endif
