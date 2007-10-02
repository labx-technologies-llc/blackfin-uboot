/*
 * RTC Masks
 */

#ifndef __BFIN_PERIPHERAL_RTC__
#define __BFIN_PERIPHERAL_RTC__

/* RTC_STAT and RTC_ALARM Masks */
#define	RTC_SEC			0x0000003F	/* Real-Time Clock Seconds */
#define	RTC_MIN			0x00000FC0	/* Real-Time Clock Minutes */
#define	RTC_HR			0x0001F000	/* Real-Time Clock Hours */
#define	RTC_DAY			0xFFFE0000	/* Real-Time Clock Days */

/*
 * RTC_ALARM Macro
 */
#define SET_ALARM(day, hour, min, sec) \
	( (((day)  & 0x7FFF) << 0x11) | \
	  (((hour) & 0x001F) << 0x0C) | \
	  (((min)  & 0x003F) << 0x06) | \
	  (((sec)  & 0x003F) << 0x00) )

/* RTC_ICTL and RTC_ISTAT Masks */
#define	STOPWATCH		0x0001	/* Stopwatch Interrupt Enable */
#define	ALARM			0x0002	/* Alarm Interrupt Enable */
#define	SECOND			0x0004	/* Seconds (1 Hz) Interrupt Enable */
#define	MINUTE			0x0008	/* Minutes Interrupt Enable */
#define	HOUR			0x0010	/* Hours Interrupt Enable */
#define	DAY			0x0020	/* 24 Hours (Days) Interrupt Enable */
#define	DAY_ALARM		0x0040	/* Day Alarm (Day, Hour, Minute, Second) Interrupt Enable */
#define	WRITE_PENDING		0x4000	/* Write Pending Status */
#define	WRITE_COMPLETE		0x8000	/* Write Complete Interrupt Enable */

/* RTC_FAST / RTC_PREN Mask */
#define PREN			0x0001	/* Enable Prescaler, RTC Runs @1 Hz */

#endif
