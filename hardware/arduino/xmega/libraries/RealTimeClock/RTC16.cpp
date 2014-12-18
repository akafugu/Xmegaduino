/*
 **  RTC16.cpp, part of the RealTimeClock library.
 **  RTC (16bit) implementation of the RealTimeClock::Device.
 **  Copyright (C) 2014 Pat Deegan.  All rights reserved.
 **
 ** http://www.flyingcarsandstuff.com/projects/RealTimeClock
 **
 **
 ** This library is free software;
 ** you can redistribute it and/or modify it under the terms of
 ** the GNU Lesser General Public License as published by the
 ** Free Software Foundation; either version 3 of the License,
 ** or (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See file LICENSE.txt for further informations on licensing terms.
 **
 ** The library incorporates public domain code, namely the DateTime and
 ** TimeSpan implementations by JeeLabs (http://news.jeelabs.org/code/),
 ** and retain their original licensing.
 **
 ** ************************* OVERVIEW *************************
 ** The RealTimeClock library leverages the on-board RTC available
 ** on certain chips, like the Atmel XMEGA.
 **
 ** Along with modifications to the Xmegaduino library's wiring
 ** implementation, this will allow you to easily deal with an
 ** RTC-based time system.  Use this library to get and set the
 ** real datetime, and use millis()/micros() as usual.
 **
 **
 ** Assumptions:
 ** 	1) You are on a platform that has either an RTC or RTC32
 **
 ** 	2) You are using a variant that actually enables the RTC
 ** 		(defines USE_RTC in the pins_arduino.h)
 **
 ** 	3) You are using an Xmegaduino (or other Arduino compatible
 ** 		sdk) that has implemented my mods for the RTC wiring
 ** 		functionality (e.g. from https://github.com/psychogenic/Xmegaduino
 ** 		or from Xmegaduino main trunk, if they've merged my pull request).
 **
 **
 **
 ** Pat Deegan http://flyingcarsandstuff.com
 */


#include "includes/RTCDevice.h"

#ifdef USE_RTC
#ifndef RTC32

#include "wiring_rtc.h"


namespace RealTimeClock {

uint32_t Device::offset = 0;


bool Device::wasResetAtPowerup()
{
	 return true; // always true, no battery backup...

}


DateTime Device::rebaseCounter()
{

	DateTime nowTime = now();
	adjust(nowTime, true);
	return nowTime;
}
void Device::adjust(const DateTime& dt, bool resetRTCCounter)
{
	if (resetRTCCounter)
	{
		wiring_rtc_zero_clock();
	}

	offset = dt.unixtime() - (millis() / 1000);

}
void Device::setPeriodicCallback(PeriodicCallback cb, uint16_t every_seconds)
{
	wiring_rtc_set_periodic_callback(cb, every_seconds);

}


void Device::clearPeriodicCallback()
{

	wiring_rtc_set_periodic_callback(NULL, 0);
}




} /* namespace RealTimeClock */


#endif /* NOT RTC32 */
#endif /* using RTC */


