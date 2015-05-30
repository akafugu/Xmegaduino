/*
 **  RTCTimespan.h, part of the RealTimeClock library.
 **
 ** http://www.flyingcarsandstuff.com/projects/RealTimeClock
 **
 ** This code, with minor mods, is by JeeLabs (http://news.jeelabs.org/code/)
 ** and under the public domain.  Included here because it's useful and
 ** already familiar to many.
 **
 **
*/

#ifndef RTCTIMESPAN_H_
#define RTCTIMESPAN_H_


#include "RTCIncludes.h"


namespace RealTimeClock {

// Timespan which can represent changes in time with seconds accuracy.
class TimeSpan {
public:
    TimeSpan (int32_t seconds = 0);
    TimeSpan (int16_t days, int8_t hours, int8_t minutes, int8_t seconds);
    TimeSpan (const TimeSpan& copy);
    int16_t days() const         { return _seconds / 86400L; }
    int8_t  hours() const        { return _seconds / 3600 % 24; }
    int8_t  minutes() const      { return _seconds / 60 % 60; }
    int8_t  seconds() const      { return _seconds % 60; }
    int32_t totalseconds() const { return _seconds; }

    TimeSpan operator+(const TimeSpan& right);
    TimeSpan operator-(const TimeSpan& right);

protected:
    int32_t _seconds;
};


} /* namespace RTC */



#endif /* RTCTIMESPAN_H_ */
