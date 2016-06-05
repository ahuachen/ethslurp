#ifndef __SFTIMEOFDAY_H__
#define __SFTIMEOFDAY_H__
/*--------------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2016 Great Hill Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
--------------------------------------------------------------------------------*/
#include "sfdate.h"
#include "sftimespan.h"

class SFTime;
class SFTimeOfDay;

//----------------------------------------------------------------------------
// A Time of Day class with a granularity of 1 second which
// is used in conjunction with the time class SFDate in the SFTime class
//----------------------------------------------------------------------------
class SFTimeOfDay
{
public:
				  SFTimeOfDay      (void);
				  SFTimeOfDay      (const SFTimeOfDay& tod);

private:
// Everything is private because on the SFTime class (which is exposed to end users and
// is a 'friend' can use this class
				  SFTimeOfDay      (SFInt32 h, SFInt32 m, SFInt32 s);
				  SFTimeOfDay      (const SF_TIMESTRUCT& sysTime);
				  SFTimeOfDay      (SFInt32 secs);
				  SFTimeOfDay      (const SFString& dateStr, const SFString& fmtStr);

//			     ~SFTimeOfDay      (void);

	SFTimeOfDay&  operator=        (const SFTimeOfDay& tod);

	SFInt32       GetHour          (void) const;
	SFInt32       GetMinute        (void) const;
	SFInt32       GetSecond        (void) const;
	SFInt32       GetTotalSeconds  (void) const;

	SFTimeOfDay   operator+        (const SFTimeSpan& tod) const;
	SFTimeOfDay   operator-        (const SFTimeSpan& tod) const;

	SFTimeOfDay&  operator+=       (SFTimeSpan& tod);
	SFTimeOfDay&  operator-=       (SFTimeSpan& tod);


	SFBool        operator==       (const SFTimeOfDay& tod) const;
	SFBool        operator!=       (const SFTimeOfDay& tod) const;

	SFBool        operator>        (const SFTimeOfDay& tod) const;
	SFBool        operator<        (const SFTimeOfDay& tod) const;

	SFBool        operator>=       (const SFTimeOfDay& tod) const;
	SFBool        operator<=       (const SFTimeOfDay& tod) const;

	SFString      Format           (const SFString& fmt=nullString) const;

	SFBool        IsValid          (void) const;

private:
	SFInt32 m_nSeconds;

	friend class SFTime;
	friend class SFTimeSpan;
	friend void  SetTime    (SFTime& date, const SFString& tStr);
};

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFInt32 SFTimeOfDay::GetHour() const
{
	ASSERT(IsValid());
	return (SFInt32)(m_nSeconds / SECS_PER_HOUR);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFInt32 SFTimeOfDay::GetMinute() const
{
	ASSERT(IsValid());
	SFInt32 secsInHours = GetHour() * SECS_PER_HOUR;
	return (SFInt32)((m_nSeconds - secsInHours) / SECS_PER_MIN);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFInt32 SFTimeOfDay::GetSecond() const
{
	ASSERT(IsValid());
	SFInt32 secsInHours = GetHour()   * SECS_PER_HOUR;
	SFInt32 secsInMins  = GetMinute() * SECS_PER_MIN;
	return (SFInt32)(m_nSeconds - secsInHours - secsInMins);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFInt32 SFTimeOfDay::GetTotalSeconds() const
{
	ASSERT(IsValid());
	return m_nSeconds;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool SFTimeOfDay::IsValid() const
{
	return (m_nSeconds != SECS_PER_DAY);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFTimeOfDay SFTimeOfDay::operator+(const SFTimeSpan& ts) const
{
	if (IsValid() && ts.IsValid())
	{
		SFTimeSpan t = SFTimeSpan(*this) + SFTimeSpan(0, ts.getHoursAbs(), ts.getMinutesAbs(), ts.getSecondsAbs());
		return SFTimeOfDay(t.getHoursAbs(), t.getMinutesAbs(), t.getSecondsAbs());
	}
	return SFTimeOfDay();
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFTimeOfDay SFTimeOfDay::operator-(const SFTimeSpan& ts) const
{
	if (IsValid() && ts.IsValid())
	{
		//Addition of 1 Day is used to ensure result "t" does not go negative
		SFTimeSpan t = SFTimeSpan(1, 0, 0, 0) + SFTimeSpan(*this) - SFTimeSpan(0, ts.getHoursAbs(), ts.getMinutesAbs(), ts.getSecondsAbs());
		return SFTimeOfDay(t.getHoursAbs(), t.getMinutesAbs(), t.getSecondsAbs());
	}
	return SFTimeOfDay();
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFTimeOfDay& SFTimeOfDay::operator+=(SFTimeSpan& ts)
{
	*this = *this + ts;
	return *this;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFTimeOfDay& SFTimeOfDay::operator-=(SFTimeSpan& ts)
{
	*this = *this - ts;
	return *this;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool SFTimeOfDay::operator==(const SFTimeOfDay& tod) const
{
	return (m_nSeconds == tod.m_nSeconds);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool SFTimeOfDay::operator!=(const SFTimeOfDay& tod) const
{
	return !operator==(tod);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool SFTimeOfDay::operator>(const SFTimeOfDay& tod) const
{
	return (m_nSeconds > tod.m_nSeconds);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool SFTimeOfDay::operator<(const SFTimeOfDay& tod) const
{
	return (m_nSeconds < tod.m_nSeconds);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool SFTimeOfDay::operator>=(const SFTimeOfDay& tod) const
{
	return (m_nSeconds >= tod.m_nSeconds);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool SFTimeOfDay::operator<=(const SFTimeOfDay& tod) const
{
	return (m_nSeconds <= tod.m_nSeconds);
}

#endif
