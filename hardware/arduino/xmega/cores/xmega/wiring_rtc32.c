#include "wiring_rtc32.h"

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

/* we want the on-board RTC */

#ifdef RTC32
/* Yes, this is 32-bit RTC */

extern volatile unsigned long seconds_count;

RTCPeriodicCallback rtc32_periodic_callback = NULL;



uint8_t wiring_rtc32_needed_reset(void)
{


	uint8_t backupBatStatus = VBAT.STATUS;
	uint8_t needToReset = 1;
	uint8_t potentialIssues = (VBAT_BBPWR_bm | VBAT_BBPORF_bm | VBAT_BBBORF_bm );
	if (!(backupBatStatus & potentialIssues)) {
		// we *aren't without battery power, no BBPOR probs, no backup brown-out
		VBAT.CTRL = VBAT_ACCEN_bm;
		if (! (backupBatStatus & VBAT_XOSCFAIL_bm))
		{
			// no osc failure...
			// looks pretty good, lets NOT reset
			needToReset = 0;
		}
	}

	return needToReset;

}


/* and we are indeed on a platform with 32-bit RTC built-in. */
void wiring_setup_rtc_clocksource(void)
{
	if (! wiring_rtc32_needed_reset())
		return;

	// Disable RTC and wait until ready, make certain it's off...
	RTC32.CTRL = 0;
	RTC32_WAIT_UNTIL_READY();

	// enable RTC (i.e. disable power reduction for it)	
	PR.PRGEN &= ~PR_RTC_bm;



	// VBAT access
	VBAT.CTRL |= VBAT_ACCEN_bm;

	// reset backup registers
	VBAT.BACKUP1 = 0;
	VBAT.BACKUP0 = 0;

        CCP = CCP_IOREG_gc; // Secret handshake 
	VBAT.CTRL = VBAT_RESET_bm;

	VBAT.CTRL |= VBAT_XOSCFDEN_bm;
	// Let the backup system stabilize
	delayMicroseconds(220);
	VBAT.CTRL |= VBAT_XOSCEN_bm | VBAT_XOSCSEL_bm; // enable x osc, and select 1024Hz

	while (!(VBAT.STATUS & VBAT_XOSCRDY_bm));


}


void wiring_rtc32_set_periodic_callback(RTCPeriodicCallback cb, uint16_t every_seconds)
{

	// Disable RTC and wait until ready, make certain it's off...
	RTC32.CTRL = 0;
	RTC32_WAIT_UNTIL_READY();

	if (cb && every_seconds > 0 && every_seconds <= RTC32_PERIODICCALLBACK_MAX_INTERVAL_SECONDS)
	{
	
		RTC32.COMP = (1024UL * every_seconds);

		// make sure we catch overflows *and* compare INTs
		RTC32.INTCTRL = RTC32_OVFINTLVL_LO_gc | RTC32_COMPINTLVL_LO_gc;
		RTC32.INTFLAGS = RTC32_OVFIF_bm | RTC32_COMPIF_bm;
	} else {

		RTC32.INTCTRL = RTC32_OVFINTLVL_LO_gc | RTC32_COMPINTLVL_OFF_gc;
		RTC32.INTFLAGS = RTC32_OVFIF_bm ;
	}	

	rtc32_periodic_callback = cb;

	RTC32.CTRL = RTC32_ENABLE_bm;
	// sync it
	RTC32_WAIT_UNTIL_READY();
}




void wiring_setup_rtc(void)
{

	if (! wiring_rtc32_needed_reset())
	{
		// we don't need a reset, but we need to restore our seconds counter, as best we can...
		seconds_count = ((VBAT.BACKUP1 * VBAT_BACKUP_INTERVAL_SECONDS) << 8);
		seconds_count += (VBAT.BACKUP0 * VBAT_BACKUP_INTERVAL_SECONDS);

		return;
	}


	RTC32.PER = 0xffffffff;

	// setup max period,
	// and start at 0, as we're resetting...
	RTC32.CNT = 0;
	
	// RTC32.COMP = (1024UL * VBAT_BACKUP_INTERVAL_SECONDS);

	// sync it before proceeding
	RTC32_WAIT_UNTIL_READY();

	// make sure we catch overflows, and don't loose our seconds every 48 days or so
	RTC32.INTCTRL = RTC32_OVFINTLVL_LO_gc | RTC32_COMPINTLVL_OFF_gc;
	// RTC32.INTCTRL = RTC32_OVFINTLVL_LO_gc | RTC32_COMPINTLVL_LO_gc;


	RTC32.INTFLAGS = RTC32_OVFIF_bm ;
	// RTC32.INTFLAGS = RTC32_OVFIF_bm | RTC32_COMPIF_bm;

	RTC32.CTRL = RTC32_ENABLE_bm;

	// sync it
	RTC32_WAIT_UNTIL_READY();

}

void wiring_rtc32_zero_clock(void)
{
	// Disable RTC and wait until ready, make certain it's off...
	RTC32.CTRL = 0;
	RTC32_WAIT_UNTIL_READY();
	
	seconds_count = 0;
	RTC32.CNT = 0;
	// sync it before proceeding
	RTC32_WAIT_UNTIL_READY();

	RTC32.CTRL = RTC32_ENABLE_bm;
	// sync it
	RTC32_WAIT_UNTIL_READY();


}



/*
 * millis()
 * rather than interrupt every time we get a ~1kHz tick, we get the actual
 * value in the RTC counter when asked for it, adding greater expense to the 
 * op when it's used but basically costing nothing the rest of the time.
 *
 * NOTE: the value is "real" in that we don't simply rely on 1 tick being ~equal
 * to 1ms (because it isn't, viz the 1024Hz clock).  Instead, we do a little
 * math, basically taking the RTC count * 1000/1024.  This introduces an 
 * issue: anything beyond 0x20000000 (i.e. about 68 minutes) will result in 
 * an overflow (if we use 32bit ints, similar issues using floats).  So: 
 * when the millis() function is called, if it's been more than 32 seconds
 * since the last time, we add the full seconds into seconds_count and only
 * keep the remainder in our RTC counter.
 *
 * This all works nicely, as long as you stick to the Arduino SDK functions.
 * If you want to play directly with the RTC32 stuff, you'll have to account
 * for this, perhaps using 
 *   extern volatile unsigned long seconds_count;
 * directly, or avoiding the millis()/micros() calls, or fix this here somehow.
 */

#define SECONDS_COUNTER_BLOCK_SIZE_BITSHIFT	10
unsigned long millis()
{
 	unsigned long count, tmp;
 	
 
	// disable interrupts while we read ?
#ifdef RTC32_DISABLE_INTERRUPTS_WHILE_READING_CNT
 	uint8_t oldSREG = SREG;
	cli();
#endif

	RTC32_FETCH_COUNTER(count);


#ifdef RTC32_DISABLE_INTERRUPTS_WHILE_READING_CNT
	SREG = oldSREG;
#endif

	if ((!rtc32_periodic_callback) && (count > (1<<SECONDS_COUNTER_BLOCK_SIZE_BITSHIFT)))
	{
		// we don't want to overflow, so we count seconds and clear
		tmp = (count>>SECONDS_COUNTER_BLOCK_SIZE_BITSHIFT); // number of whole seconds

		count -= (tmp<<SECONDS_COUNTER_BLOCK_SIZE_BITSHIFT); // keep remainder
		seconds_count += tmp;

		RTC32_SET_COUNTER(count);

		/*
		if (seconds_count % VBAT_BACKUP_INTERVAL_SECONDS == 0)
			BACKUP_SECONDS_COUNT(seconds_count);
		*/


		
	}
	// actual is corrected to account for 1024Hz clock
	// so we multiply by 1000 and then divide by 1024 (right shifting)
	tmp = (seconds_count * 1000UL) + ((count * 1000UL) >> 10);

	return tmp;

}

unsigned long micros(void) {
	// TODO: Get real micros and not just millis*1000

	return millis() * 1000UL;

	/* slightly more precise version, but doesn't handle our 
	   overflow situation (described above millis()) so fuggedaboutit...

	unsigned long pseudomillis;
	RTC32_FETCH_COUNTER(pseudomillis);
	return (seconds_count * 1000000UL) + (pseudomillis * 977UL); // approximation based on 1024Hz counter
	*/

}


ISR(RTC32_COMP_vect)
{
	
 	unsigned long count, secs_elapsed;
 	// do we need cli()?

	RTC32_FETCH_COUNTER(count);
	secs_elapsed = count>>SECONDS_COUNTER_BLOCK_SIZE_BITSHIFT;
	seconds_count += secs_elapsed;
	count -= (secs_elapsed<<SECONDS_COUNTER_BLOCK_SIZE_BITSHIFT);
	RTC32_SET_COUNTER(count);


	if (rtc32_periodic_callback)
		rtc32_periodic_callback();

	// BACKUP_SECONDS_COUNT(seconds_count);




}

ISR(RTC32_OVF_vect)
{
	// wow, we've been doing nothing for a long time...
	seconds_count += 0x400000UL; // 0xffffffff clocks @ 0x2000 Hz, so about 48.5 days.
}


#endif /* RTC32 */
#endif /* USE_RTC */

