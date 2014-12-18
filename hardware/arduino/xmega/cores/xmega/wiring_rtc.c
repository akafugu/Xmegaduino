#include "wiring_rtc.h"



#if defined(USE_RTC)
/* we want the on-board RTC */

#ifndef RTC32
/* and this ISN'T and RTC32 implementation */




#define RTC_MAX_PERIOD_TICKS	0xffff
#ifdef __cplusplus
extern "C"{
#endif

RTCPeriodicCallback rtc_periodic_callback = NULL;
void wiring_rtc_shift_count_to_rtc_millis(void);


#ifdef __cplusplus
} // extern "C"
#endif




#define RTC_TURN_OFF()	RTC.CTRL = ( RTC.CTRL & ~RTC_PRESCALER_gm ) | RTC_PRESCALER_OFF_gc
#define RTC_TURN_ON()	RTC.CTRL = ( RTC.CTRL & ~RTC_PRESCALER_gm ) | RTC_PRESCALER_DIV1_gc;


void wiring_setup_rtc_clocksource(void)
{
	/* Turn on internal 32kHz. */
	OSC.CTRL |= OSC_RC32KEN_bm;

	do {
		/* Wait for the 32kHz oscillator to stabilize. */
	} while ( ( OSC.STATUS & OSC_RC32KRDY_bm ) == 0);
		

	/* Set internal 32kHz oscillator as clock source for RTC. */
	CLK.RTCCTRL = CLK_RTCSRC_RCOSC_gc | CLK_RTCEN_bm;/* 1kHz */
}


void wiring_setup_rtc(void)
{
	RTC_WAIT_UNTIL_READY();

	/* Configure RTC period to "infinity" -- we'll only take cpu time when either:

		* a request is made to know the millis/micros
		* we need to inform the periodic callback that time has passed, or
		* we overflow, and run out of space in our 16bit counter. 
	*/
	RTC.PER = RTC_MAX_PERIOD_TICKS; /* longest period possible */
	RTC.CNT = 0;
	RTC.COMP = 0;
	RTC_TURN_ON();

	/* Enable overflow interrupt, start with compare interrupt disabled */	
	RTC.INTCTRL = ( RTC.INTCTRL & ~( RTC_COMPINTLVL_gm | RTC_OVFINTLVL_gm ) ) |
	              RTC_OVFINTLVL_LO_gc |
	              RTC_COMPINTLVL_OFF_gc;
}


/*! \brief Set a callback to be triggered every N seconds.
 *
 * Set (or clear, if passing null) a callback to be triggered every X seconds.
 * this is most useful if you want to keep an NVRAM cache of the last known time,
 * to mitigate power failures, etc.
 */
void wiring_rtc_set_periodic_callback(RTCPeriodicCallback cb, uint16_t every_seconds)
{

	RTC_WAIT_UNTIL_READY();
 	uint8_t oldSREG = SREG;
	/* disable interrupts */
	cli();
	if (cb && every_seconds)
	{
		wiring_rtc_shift_count_to_rtc_millis();
		/* set our compare to trigger when interval expires */
		RTC.COMP = (1000UL * every_seconds);
		RTC.INTCTRL = ( RTC.INTCTRL & ~RTC_COMPINTLVL_gm) | RTC_COMPINTLVL_LO_gc;
	} else {
		RTC.COMP = 0;
		/* clear the compare interrupt */
		RTC.INTCTRL = ( RTC.INTCTRL & ~RTC_COMPINTLVL_gm) | RTC_COMPINTLVL_OFF_gc;
		
	}

	rtc_periodic_callback = cb;
	
	SREG = oldSREG;
}


/*
 * Zero the clock.
 */
void wiring_rtc_zero_clock(void)
{
	
 	uint8_t oldSREG = SREG;
	/* disable interrupts */
	cli();
	rtc_millis = 0;
	SREG = oldSREG;
}




/*! \brief returns current RTC milliseconds count.
 *
 * The ms counter has 0-cost while running in the background,
 * and will update it's idea of the time based on the RTC 
 * counter when millis() is actually called (and in a few other
 * circumstances, namely on overflow and when a periodic callback 
 * has been requested).
*/
unsigned long millis()
{
 	uint32_t m, tmp;


	RTC_WAIT_UNTIL_READY();

 	uint8_t oldSREG = SREG;
	/* disable interrupts */
	cli();

	tmp = RTC.CNT;
	if (rtc_periodic_callback || tmp < 0x4000)
	{
		// still small (or have callback and assume user is taking care of things), so just return the sum value
		m = rtc_millis + tmp;
	} else {
		// been a long time, reset the counter to avoid overflow
		wiring_rtc_shift_count_to_rtc_millis();
		m = rtc_millis;
	} 

	
	SREG = oldSREG;
	return m;
}


unsigned long micros(void) {
	/* TODO: Get real micros and not just millis*1000 */
	unsigned long m = millis();
        return m*1000;
}



/* wiring_rtc_shift_count_to_rtc_millis
 * augments the rtc_millis counter with whatever time has passed
 * and resets the counter to 0.
 */
void wiring_rtc_shift_count_to_rtc_millis(void)
{
	// turn it off...
	RTC_TURN_OFF();
	RTC_WAIT_UNTIL_READY();


 	uint8_t oldSREG = SREG;
	cli();	

	rtc_millis += RTC.CNT;
	RTC.CNT = 0;


	SREG = oldSREG;

	RTC_TURN_ON();
}





/*! \brief RTC overflow interrupt service routine.
 *
 *  This ISR keeps track of RTC counter overflows, so
 *  we don't just lose 16bits-worth of millis.
 */
ISR(RTC_OVF_vect)
{
	rtc_millis += (RTC_MAX_PERIOD_TICKS);

}

/*! \brief RTC compare interrupt service routine.
 * 
 * This ISR is only triggered when the use has requested
 * a periodic callback.  Time is updated prior to triggering.
*/
ISR(RTC_COMP_vect)
{

	if (rtc_periodic_callback)
	{
		wiring_rtc_shift_count_to_rtc_millis();
		rtc_periodic_callback();
	}

}



#endif /* NOT RTC32 */
#endif /* USE_RTC */


