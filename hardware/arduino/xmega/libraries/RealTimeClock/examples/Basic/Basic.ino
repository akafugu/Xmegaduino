/*
 **  RTC Basic Example
 **  Copyright (C) 2014 Pat Deegan.  All rights reserved.
 **
 ** http://www.flyingcarsandstuff.com/projects/RealTimeClock
 **
 **
 ** This program (and accompanying library) is free software;
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
 ** ************************* USAGE *************************
 ** 
 ** Update the OUTPUT_SERIAL_LINE/OUTPUT_SERIAL_BAUD as required, 
 ** hook up a USB serial and open a terminal... inspect the code
 ** as you watch the pretty messages scroll by.  Pretty much it.
 ** 
 **
 ** The code is fully commented, see below.
 ** Enjoy!
 ** Pat Deegan
 */

#include <RealTimeClock.h>

#define OUTPUT_SERIAL_LINE		Serial
#define OUTPUT_SERIAL_BAUD		115200

#define OUTPUT_MESSAGE(...)		OUTPUT_SERIAL_LINE.print(__VA_ARGS__);
#define OUTPUTLN_MESSAGE(...)	OUTPUT_SERIAL_LINE.println(__VA_ARGS__);

void showCurrentTime(){
	OUTPUT_MESSAGE(F("*** "));
	
	RealTimeClock::DateTime nowTime = RealTimeClock::Device::now();
	
	if (nowTime.hour() < 10)
		OUTPUT_MESSAGE('0');
	OUTPUT_MESSAGE((int)nowTime.hour());
	
	OUTPUT_MESSAGE(':');
	
	if (nowTime.minute() < 10)
		OUTPUT_MESSAGE('0');
	OUTPUT_MESSAGE((int)nowTime.minute());

	OUTPUT_MESSAGE(':');
	if (nowTime.second() < 10)
		OUTPUT_MESSAGE('0');
	OUTPUTLN_MESSAGE((int)nowTime.second());
	

}


void myPeriodicCallback()
{
	// Remember that you're in the middle of an interrupt, here, so keep it snappy and simple.
	// this is mainly useful for rebasing the counter and storing the current time to NVRAM.
	// however, you can do whatever you need to in a timely fashion, here.
	OUTPUT_MESSAGE(F("CALLBACK: It is now "));
	showCurrentTime();


	OUTPUT_MESSAGE(F("... a good time rebaseCounter()"));
	OUTPUTLN_MESSAGE(F(" and/or backup time to NVRAM (if you have a backup battery)."));
	OUTPUTLN_MESSAGE(' ');

}

void setup()
{
	// setup the serial line
	OUTPUT_SERIAL_LINE.begin(OUTPUT_SERIAL_BAUD);

	// begin the RTC with some valid date.
	// datetime(year, month, day, hour, minute, seconds)
	RealTimeClock::DateTime startDate(2014, 8, 30, 9, 35, 22); // yay, bday!
	RealTimeClock::Device::begin(startDate);
	
	// you may always RealTimeClock::Device::adjust() it later.
	

	// hello message
	OUTPUTLN_MESSAGE(F("RTC Basic example."));
	OUTPUTLN_MESSAGE(F("Copyright (C) 2014 Pat Deegan, flyingcarsandstuff.com"));
	OUTPUTLN_MESSAGE(' ');

	OUTPUT_MESSAGE(F("Booted up, started RTC @ "));
	OUTPUT_MESSAGE(startDate.year());
	OUTPUT_MESSAGE('-');

	if (startDate.month() < 10)
		OUTPUT_MESSAGE('0');
	OUTPUT_MESSAGE(startDate.month());

	OUTPUT_MESSAGE('-');

	if (startDate.day() < 10)
		OUTPUT_MESSAGE('0');
	OUTPUTLN_MESSAGE(startDate.day());


	RealTimeClock::Device::setPeriodicCallback(myPeriodicCallback, 3);

}

void loop() {

	// periodic callback should be set, we'll just wait a bit, using
	// delayMicroseconds() to avoid triggering millis() at all

	// wait for ~600*10000 us (i.e. 6 seconds)

	OUTPUTLN_MESSAGE(F("A few delayMicroseconds (w/cb enabled)"));
	for (uint16_t u=0; u<6100; u++)
	{
		delayMicroseconds(1000UL);
	}

	showCurrentTime();

	OUTPUTLN_MESSAGE(F("A delay() (w/cb enabled)"));
	// do it again, using delay()
	delay(6000UL);
	showCurrentTime();

	// clear the callback, to stop getting interrupted.
	RealTimeClock::Device::clearPeriodicCallback();

	OUTPUTLN_MESSAGE(F("A couple of delayMicroseconds (no cb)"));
	for (uint8_t i=0; i<2; i++)
	{
		// wait for ~2 seconds, using delayMicro
		for (uint16_t u=0; u<2000; u++)
		{
			delayMicroseconds(1000UL);
		}

		showCurrentTime();

	}

	// a few more waits, this time with delay()
	OUTPUTLN_MESSAGE(F("A couple of delays (no cb)"));
	for (uint8_t i=0; i<2; i++)
	{
		delay(2000UL);
		showCurrentTime();
	}

	
	// Re-enabling the callback
	OUTPUTLN_MESSAGE(F("Re-enabling periodic callback every 3 seconds"));
	RealTimeClock::Device::setPeriodicCallback(myPeriodicCallback, 3);
	
	OUTPUTLN_MESSAGE(F("Now for one loooong delay... (20 secs)"));
	delay(20000UL);
	
	OUTPUT_MESSAGE(F("Done long delay @ "));
	showCurrentTime();
	

}
