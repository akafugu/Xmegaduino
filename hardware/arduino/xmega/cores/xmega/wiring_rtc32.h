#ifndef WIRING_RTC32_IMPL_H_
#define WIRING_RTC32_IMPL_H_

#include "wiring_private.h"

/*
 *
 * RTC32 real-time clock implementation for Arduino/Xmegaduino.
 *  Copyright (c) 2014 Pat Deegan, flyingcarsandstuff.com
 *
 *
 * This program library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 * This code leverages the RTC32, battery-backed, real-time clock
 * to keep track of (you guessed it) time.
 *
 * It keeps interrupt activity down to a minimum, only accessing the
 * RTC when required to, and is smart enough to do setup on first time
 * power-up while leaving things running smoothly when power returns
 * after an interruption (assuming you have a functional battery tied
 * to VBAT).
 *
 * For this all to work, you must have a battery *and* you must configure
 * brown-out detection.  In essence, you need to set the BODACT to Enabled,
 * BODPD (powerdown) to enabled or sampled (to save power) and the BODLEVEL
 * to something sensible.  Here are the settings used while testing on an 
 * xmega256A3BU:
 *
 *   FUSE2:	0xFD 	(sampled powerdown BOD)
 *   FUSE5:	0xEB	(BODACT enabled, BODLEVEL 011/2.6V)
 *
 *
*/

#if defined(USE_RTC)
// we want the on-board RTC
#ifdef RTC32

#include "wiring_rtc.h"


#ifdef __cplusplus
extern "C"{
#endif

#define RTC32_PERIODICCALLBACK_MAX_INTERVAL_SECONDS	(66*60)

uint8_t wiring_rtc32_needed_reset(void);
void wiring_rtc32_set_periodic_callback(RTCPeriodicCallback cb, uint16_t every_seconds);

void wiring_rtc32_zero_clock(void);




#ifdef __cplusplus
} /* extern C */
#endif



#define VBAT_BACKUP_INTERVAL_SECONDS		2


#define RTC32_IS_BUSY() 			(RTC32.SYNCCTRL & RTC32_SYNCBUSY_bm)
#define RTC32_WAIT_UNTIL_READY()		while(RTC32_IS_BUSY()) 


#define RTC32_SET_COUNTER(count) \
	/* stahp! */ \
	RTC32.CTRL = 0; \
	RTC32_WAIT_UNTIL_READY(); \
	RTC32.CNT = count; \
	/* ok, continue */ \
	RTC32.CTRL = RTC32_ENABLE_bm;


#define RTC32_FETCH_COUNTER(intoVar)	\
	RTC32.SYNCCTRL = RTC32_SYNCCNT_bm; \
	RTC32_WAIT_UNTIL_READY(); \
	intoVar = RTC32.CNT;


#define BACKUP_SECONDS_COUNT(secs) \
	{ \
	VBAT.BACKUP1 = (0xff & (((secs)/VBAT_BACKUP_INTERVAL_SECONDS) >> 8)); \
	VBAT.BACKUP0 = (((secs)/VBAT_BACKUP_INTERVAL_SECONDS) & 0xff); \
	}



#endif /* USE_RTC */
#endif /* RTC32 */
#endif /* WIRING_RTC32_IMPL_H_ */









