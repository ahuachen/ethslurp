#ifndef __SFTIME_H__
#define __SFTIME_H__
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
#include "sftimeofday.h"

//-------------------------------------------------------------------------
// A Date class with a granularity of 1 second
//-------------------------------------------------------------------------
class SFTime
{
public:
				SFTime          (void);
				SFTime          (const SFTime& date);

				SFTime          (SFInt32 year, SFInt32 month, SFInt32 day, SFInt32 hour, SFInt32 min, SFInt32 sec);
				SFTime          (SFInt32 year, SFInt32 month, SFInt32 weekInMonth, SFInt32 dayOfWeek, SFInt32 hour, SFInt32 min, SFInt32 sec);
				SFTime          (SFInt32 days, SFInt32 hour, SFInt32 min, SFInt32 sec);
				SFTime          (const SFDate& date, const SFTimeOfDay& tod);
				SFTime          (const SF_TIMESTRUCT& sysTime, SFBool useDayOfWeek=FALSE);
				SFTime          (const SFString& dateStr, const SFString& fmtStr);

	//	       ~SFTime          (void);

	SFTime&     operator=       (const SFTime& date);

	SFInt32     GetDay          (void) const;
	SFInt32     GetMonth        (void) const;
	SFInt32     GetYear         (void) const;
	SFInt32     GetHour         (void) const;
	SFInt32     GetMinute       (void) const;
	SFInt32     GetSecond       (void) const;

	SFInt32     GetDayOfWeek    (void) const;
	SFInt64     GetTotalSeconds (void) const;
	SFInt64     GetTime         (void) const { return GetTotalSeconds(); }

	SFBool      IsValid         (void) const;

	SFTime      operator+       (const SFTimeSpan& ts) const;
	SFTime      operator-       (const SFTimeSpan& ts) const;
	SFTime&     operator+=      (const SFTimeSpan& ts);
	SFTime&     operator-=      (const SFTimeSpan& ts);

	SFTime&     operator+=      (SFInt32 days);
	SFTimeSpan  operator-       (SFTime& date);

	SFBool      operator==      (const SFTime& date) const;
	SFBool      operator!=      (const SFTime& date) const;

	SFBool      operator>       (const SFTime& date) const;
	SFBool      operator<       (const SFTime& date) const;

	SFBool      operator>=      (const SFTime& date) const;
	SFBool      operator<=      (const SFTime& date) const;

	SFBool      onTheHour       (void) const;

	SFString    Format          (SFInt32 fmt) const;
	SFString    Format          (const SFString& fmt) const;

	void        fromSortStr     (const SFString& vStr);

private:
	friend void SetTime       (SFTime& date, const SFString& tStr);
	friend void SetDate       (SFTime& date, const SFString& dStr);
	friend void SetTimeAndDate(SFTime& date, const SFString& valueStr);

public:
	SFDate      getDatePart     (void) const;
	SFTimeOfDay getTimePart     (void) const;

	// Count of seconds from same epoch as SFDate uses
	SFInt64     m_nSeconds;

//------
// Save these for possible later use with Daylight Savings time and time zone stuff
//------
//         void       CheckForValidLocalDate  (void);
//         SFBool     DST                     (void);
//         SFBool     isDST                   (void) const
//  static SFBool     CurrentlyInDST          (void);
//  static SFTimeSpan DaylightBias            (void);
//  static SFTimeSpan TimezoneBias            (void);
//  static SFString   DaylightName            (void);
//  static SFString   StandardName            (void);
};

inline SFTime& SFTime::operator+=(SFInt32 days)
{
	*this = *this + SFTimeSpan(days, 0, 0, 0);
	return *this;
}

inline SFInt32 SFTime::GetYear() const
{
	return getDatePart().GetYear();
}

inline SFInt32 SFTime::GetMonth() const
{
	return getDatePart().GetMonth();
}

inline SFInt32 SFTime::GetDay() const
{
	return getDatePart().GetDay();
}

inline SFInt32 SFTime::GetHour() const
{
	return getTimePart().GetHour();
}

inline SFInt32 SFTime::GetMinute() const
{
	return getTimePart().GetMinute();
}

inline SFInt32 SFTime::GetSecond() const
{
	return getTimePart().GetSecond();
}

inline SFInt64 SFTime::GetTotalSeconds (void) const
{
	return m_nSeconds;
}

inline SFTime& SFTime::operator+=(const SFTimeSpan& ts)
{
  *this = *this + ts;
  return *this;
}

inline SFTime& SFTime::operator-=(const SFTimeSpan& ts)
{
  *this = *this - ts;
  return *this;
}

extern SFInt32 getDayOfWeek(const SFDate& date);
inline SFInt32 SFTime::GetDayOfWeek() const
{
	return ::getDayOfWeek(getDatePart());
}

inline SFBool SFTime::operator==(const SFTime& date) const
{
  return (m_nSeconds == date.m_nSeconds);
}

inline SFBool SFTime::operator!=(const SFTime& date) const
{
	ASSERT(IsValid());
	return !operator==(date);
}

inline SFBool SFTime::operator>(const SFTime& date) const
{
  return (m_nSeconds > date.m_nSeconds);
}

inline SFBool SFTime::operator<(const SFTime& date) const
{
  return (m_nSeconds < date.m_nSeconds);
}

inline SFBool SFTime::operator>=(const SFTime& date) const
{
  return operator>(date) || operator==(date);
}

inline SFBool SFTime::operator<=(const SFTime& date) const
{
  return operator<(date) || operator==(date);
}

inline SFTime SFTime::operator+(const SFTimeSpan& ts) const
{
	SFTime ret;
	ret.m_nSeconds = m_nSeconds + ts.getSeconds_raw();
	return ret;
}

inline SFTime SFTime::operator-(const SFTimeSpan& ts) const
{
	SFTime ret;
	ret.m_nSeconds = m_nSeconds - ts.getSeconds_raw();
	return ret;
}

inline SFTimeSpan SFTime::operator-(SFTime& date)
{
	return SFTimeSpan(m_nSeconds - date.m_nSeconds);
}

inline SFBool SFTime::IsValid() const
{
	return (m_nSeconds != (SFInt64)_I64_MIN);
}

inline SFDate SFTime::getDatePart() const
{
	ASSERT(IsValid());
	return SFDate(((SFInt32)(m_nSeconds / SECS_PER_DAY)) - 2000000000L);
}

inline SFTimeOfDay SFTime::getTimePart() const
{
  ASSERT(IsValid());
  SFInt64 nSeconds = m_nSeconds - SFInt64(getDatePart().GetTotalDays()) * SECS_PER_DAY;
	return SFTimeOfDay((SFInt32)nSeconds);
}

inline SFBool SFTime::onTheHour(void) const
{
	return (GetMinute() < 5 || GetMinute() > 55);
}

inline SFString getPaddedDate(const SFTime& date)
{
	SFString ret = padNum2(date.GetMonth())+"_"+padNum2(date.GetDay());
	if (date.GetDayOfWeek()==1) ret += "Su";
	if (date.GetDayOfWeek()==2) ret += "M";
	if (date.GetDayOfWeek()==3) ret += "T";
	if (date.GetDayOfWeek()==4) ret += "W";
	if (date.GetDayOfWeek()==5) ret += "R";
	if (date.GetDayOfWeek()==6) ret += "F";
	if (date.GetDayOfWeek()==7) ret += "Sa";
	return ret;
}

extern SFString getFormatString (SFInt32 fmt, SFBool euro, SFBool mil, const SFString& dSep, const SFString& tSep);

#endif
