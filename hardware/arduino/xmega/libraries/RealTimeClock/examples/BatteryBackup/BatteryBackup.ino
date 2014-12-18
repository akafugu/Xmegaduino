/*
 **  RTC Battery-backed RTC example.
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
 **
 ** ************************* USAGE *************************
 ** 
 ** Update the OUTPUT_SERIAL_LINE/OUTPUT_SERIAL_BAUD as required, 
 ** hook up a USB serial and open a terminal... inspect the code
 ** as you watch the pretty messages scroll by.  Pretty much it.
 ** 
 ** 
 ** 
 **
 **
 ** The code is fully commented, see below.
 ** Enjoy!
 ** Pat Deegan
 */

#include <RealTimeClock.h>

#define OUTPUT_SERIAL_LINE		Serial
#define OUTPUT_SERIAL_BAUD		115200


/*
 * PERIODIC_CALLBACK_INTERVAL_SECONDS
 *
 * Interval at which we backup the current time, in seconds.
 *
 * Choosing this value is about finding the correct balance between
 * resolution (PERIODIC_CALLBACK_INTERVAL_SECONDS is the max amount of
 * time we can lose on a battery-backed RTC, like the RTC32), and how
 * much wear-and-tear you're willing to put on your NVRAM/EEPROM/whatever.
 *
 */
#define PERIODIC_CALLBACK_INTERVAL_SECONDS		5


#define OUTPUT_MESSAGE(...)		OUTPUT_SERIAL_LINE.print(__VA_ARGS__);
#define OUTPUTLN_MESSAGE(...)	OUTPUT_SERIAL_LINE.println(__VA_ARGS__);

RealTimeClock::DateTime getLastValidTime()
{
	// HERE is where you would fetch the last valid time stored in EEPROM/NVRAM...

	// fakin' it...

	// datetime(year, month, day, hour, minute, seconds)
	RealTimeClock::DateTime startDate(2014, 8, 30, 9, 35, 22); // yay, bday!
	return startDate;
}


void myPeriodicCallback()
{

	// Remember that you're in the middle of an interrupt, here, so keep it snappy and simple.
	RealTimeClock::DateTime nowTime = RealTimeClock::Device::rebaseCounter();
	
	OUTPUTLN_MESSAGE(' ');
	
	OUTPUT_MESSAGE(F("Storing last known start time ("));
	OUTPUT_MESSAGE(nowTime.unixtime());
	OUTPUTLN_MESSAGE(F(") to NVRAM (well, not really)."));



	OUTPUT_MESSAGE(F("Should we lose power, the battery-backed RTC will keep counting time and, "));
	OUTPUT_MESSAGE(F("when we reboot, we will call begin() with that value.  Thusly, the maximum time lost "));
	OUTPUT_MESSAGE(F("will be the interval at which this function is called (here "));
	OUTPUT_MESSAGE((int)PERIODIC_CALLBACK_INTERVAL_SECONDS);
	OUTPUTLN_MESSAGE(F("seconds)."));

	OUTPUTLN_MESSAGE(' ');

}






void setup()
{
	OUTPUT_SERIAL_LINE.begin(OUTPUT_SERIAL_BAUD);

	RealTimeClock::DateTime lastValid = getLastValidTime();
	RealTimeClock::Device::begin(lastValid);


	OUTPUTLN_MESSAGE(F("RTC Battery Backup example."));
	OUTPUTLN_MESSAGE(F("Copyright (C) 2014 Pat Deegan, flyingcarsandstuff.com"));
	OUTPUTLN_MESSAGE(' ');
	
	
	

	OUTPUT_MESSAGE(F("Booted up, started RTC from last valid time: "));
	OUTPUT_MESSAGE(lastValid.year());
	OUTPUT_MESSAGE('-');

	if (lastValid.month() < 10)
		OUTPUT_MESSAGE('0');
	OUTPUT_MESSAGE(lastValid.month());

	OUTPUT_MESSAGE('-');

	if (lastValid.day() < 10)
		OUTPUT_MESSAGE('0');
	OUTPUTLN_MESSAGE(lastValid.day());


	RealTimeClock::Device::setPeriodicCallback(myPeriodicCallback, PERIODIC_CALLBACK_INTERVAL_SECONDS);

}


void showCurrentTime(){

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


void loop() {

	for (uint8_t i=0; i<12; i++)
	{
		
		delay(1234);
		OUTPUT_MESSAGE(F("Just doin' my biz @ "));
		showCurrentTime();
	}
	

	OUTPUTLN_MESSAGE(F("Now really busy (long delay > 10s)..."));
	for (uint16_t i=0; i<12000; i++)
	{
		delayMicroseconds(1000);
	}

}
