/*
 **  RTCDateTime.h, part of the RealTimeClock library.
 **
 ** http://www.flyingcarsandstuff.com/projects/RealTimeClock
 **
 ** This code, with minor mods, is by JeeLabs (http://news.jeelabs.org/code/)
 ** and under the public domain.  Included here because it's useful and
 ** already familiar to many.
 **
 **
*/
#ifndef RTCDATETIME_H_
#define RTCDATETIME_H_
#include "RTCIncludes.h"

namespace RealTimeClock {

class TimeSpan;

// Simple general-purpose date/time class (no TZ / DST / leap second handling!)
class DateTime {
public:
    DateTime (uint32_t t =0);
    DateTime (uint16_t year, uint8_t month, uint8_t day,
                uint8_t hour =0, uint8_t min =0, uint8_t sec =0);
    DateTime (const DateTime& copy);
    DateTime (const char* date, const char* time);
    DateTime (const __FlashStringHelper* date, const __FlashStringHelper* time);
    uint16_t year() const       { return 2000 + yOff; }
    uint8_t month() const       { return m; }
    uint8_t day() const         { return d; }
    uint8_t hour() const        { return hh; }
    uint8_t minute() const      { return mm; }
    uint8_t second() const      { return ss; }
    uint8_t dayOfWeek() const;

    // 32-bit times as seconds since 1/1/2000
    long secondstime() const;
    // 32-bit times as seconds since 1/1/1970
    uint32_t unixtime(void) const;

    DateTime operator+(const TimeSpan& span);
    DateTime operator-(const TimeSpan& span);
    TimeSpan operator-(const DateTime& right);

protected:
    uint8_t yOff, m, d, hh, mm, ss;
};

} /* namespace RTC */



#endif /* RTCDATETIME_H_ */
