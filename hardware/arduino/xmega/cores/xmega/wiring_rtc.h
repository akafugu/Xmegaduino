#ifndef WIRING_RTC_IMPL_H_
#define WIRING_RTC_IMPL_H_

/*
 *
 * RTC real-time clock mods
 *  Copyright (c) 2014 Pat Deegan, flyingcarsandstuff.com
 *
 * This is simply the USE_RTC stuff from wiring.c, isolated
 * from the main code in order to simplify variations on the
 * implementation (e.g. the RTC32 available on some xmega).
 * 
 * It is only slightly modified and retains it's original 
 * license, as
 *
 *
 *
 * Copyright (c) 2005-2006 David A. Mellis
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 *
*/

#include "wiring_private.h"

#if defined(USE_RTC)
/* we want the on-board RTC */


#ifdef __cplusplus
extern "C"{
#endif


typedef void(*RTCPeriodicCallback)(void);
void wiring_setup_rtc_clocksource(void);
void wiring_setup_rtc(void);


extern volatile unsigned long seconds_count ;

#ifndef RTC32
extern volatile unsigned long rtc_millis;

void wiring_rtc_set_periodic_callback(RTCPeriodicCallback cb, uint16_t every_seconds);
void wiring_rtc_zero_clock(void);


#define RTC_IS_BUSY() 			(RTC.STATUS & RTC_SYNCBUSY_bm)
#define RTC_WAIT_UNTIL_READY()		while(RTC_IS_BUSY()) 

#endif /* NOT RTC32 */


#ifdef __cplusplus
} /* extern C */
#endif


#endif /* USE_RTC */
#endif /* WIRING_RTC_IMPL_H_ */


