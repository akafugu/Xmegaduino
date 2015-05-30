/*
 **  RTCTimespan.cpp, part of the RealTimeClock library.
 **
 ** http://www.flyingcarsandstuff.com/projects/RealTimeClock
 **
 ** This code, with minor mods, is by JeeLabs (http://news.jeelabs.org/code/)
 ** and under the public domain.  Included here because it's useful and
 ** already familiar to many.
 **
 **
*/


#include "includes/RTCTimespan.h"

namespace RealTimeClock {


TimeSpan::TimeSpan (int32_t seconds):
  _seconds(seconds)
{}

TimeSpan::TimeSpan (int16_t days, int8_t hours, int8_t minutes, int8_t seconds):
  _seconds(days*86400L + hours*3600 + minutes*60 + seconds)
{}

TimeSpan::TimeSpan (const TimeSpan& copy):
  _seconds(copy._seconds)
{}

TimeSpan TimeSpan::operator+(const TimeSpan& right) {
  return TimeSpan(_seconds+right._seconds);
}

TimeSpan TimeSpan::operator-(const TimeSpan& right) {
  return TimeSpan(_seconds-right._seconds);
}



} /* namespace RTC */






