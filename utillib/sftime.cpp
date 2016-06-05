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
#include "basetypes.h"

#include "sftime.h"

#include "dates.h"
#include "sfos.h"

//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
SFTime::SFTime()
{
  m_nSeconds = _I64_MIN;
}

//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
SFTime::SFTime(const SFTime& date)
{
  m_nSeconds  = date.m_nSeconds;
}

//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
SFTime::SFTime(SFInt32 year, SFInt32 month, SFInt32 day, SFInt32 hour, SFInt32 min, SFInt32 sec)
{
  *this = SFTime(SFDate(year, month, day), SFTimeOfDay(hour, min, sec));
}

//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
SFTime::SFTime(SFInt32 year, SFInt32 month, SFInt32 weekInMonth, SFInt32 dayOfWeek, SFInt32 hour, SFInt32 min, SFInt32 sec)
{
  *this = SFTime(SFDate(year, month, weekInMonth, dayOfWeek), SFTimeOfDay(hour, min, sec));
}

//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
SFTime::SFTime(SFInt32 days, SFInt32 hour, SFInt32 min, SFInt32 sec)
{
  *this = SFTime(SFDate(days), SFTimeOfDay(hour, min, sec));
}

//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
SFTime::SFTime(const SFDate& datePart, const SFTimeOfDay& timePart)
{
	m_nSeconds = SFInt64(datePart.GetTotalDays()) * SECS_PER_DAY + SFInt64(timePart.GetTotalSeconds());
}

//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
SFTime::SFTime(const SFString& dateStr, const SFString& fmtStr)
{
	if (dateStr.IsEmpty() || fmtStr.IsEmpty())
		*this = earliestDate;
	else
		*this = SFTime(SFDate(dateStr, fmtStr), SFTimeOfDay(dateStr, fmtStr));
}

//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
SFTime::SFTime(const SF_TIMESTRUCT& st, SFBool useDayOfWeek)
{
	SF_TIMESTRUCT sysTime = st;
	if (!sysTime.tm_year)
		sysTime.tm_year = (int)Now().GetYear();
	ASSERT(sysTime.tm_year);

	if (useDayOfWeek)
  {
	  *this = SFTime(SFDate(sysTime.tm_year, sysTime.tm_mon, sysTime.tm_mday, (sysTime.tm_wday+1)), SFTimeOfDay(sysTime));
  } else
  {
	  *this = SFTime(SFDate(sysTime), SFTimeOfDay(sysTime));
  }
}

//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
SFTime& SFTime::operator=(const SFTime& d)
{
  m_nSeconds  = d.m_nSeconds;
  return *this;
}

/*
//------
// Save these for possible later use with Daylight Savings time and time zone stuff
//------
//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
SFBool SFTime::isDST()
{
  ASSERT(IsValid());
  SFBool ret;
  SFTimeFrame OldTF = getTimeFrame();
  ret = setTimeFrame(LOCAL);
  ret = ret && DST();
  ret = ret && setTimeFrame(OldTF);
  return ret;
}

//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
SFBool SFTime::DST()
{
  ASSERT(IsValid());
  SFBool ret = TRUE;
  TIME_ZONE_INFORMATION tzi;
  SFInt32 dwTZI = SFos::GetTimeZoneInformation(tzi);
  ASSERT(dwTZI != TIME_ZONE_ID_UNKNOWN);

  SFInt32 m_Year = GetYear();

  //SF_TIMESTRUCT can only accommodate a SFInt32 sized year
  //so we always return FALSE if outside of this range
  if ((m_Year > SHRT_MAX) || (m_Year < 0))
    ret = FALSE;

  if (ret)
  {
    SFTimeFrame OldTF = m_TimeFrame;
    m_TimeFrame = UTC;

    SFBool useDayOfWeek = FALSE;
    if (tzi.DaylightDate.wYear == 0)
    {
      useDayOfWeek = TRUE;
      tzi.DaylightDate.wYear = (unsigned short) m_Year;
    }

    SFTime BeginDST(tzi.DaylightDate, UTC, useDayOfWeek);

    useDayOfWeek = FALSE;
    if (tzi.StandardDate.wYear == 0)
    {
      useDayOfWeek = TRUE;
      tzi.StandardDate.wYear = (unsigned short) m_Year;
    }

    SFTime EndDST(tzi.StandardDate, UTC, useDayOfWeek);

    ret = ret && (*this > BeginDST && *this < EndDST);

    m_TimeFrame = OldTF;
  }

  return ret;
}

//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
SFBool SFTime::CurrentlyInDST()
{
  TIME_ZONE_INFORMATION tzi;
  return SFos::GetTimeZoneInformation(tzi) == TIME_ZONE_ID_DAYLIGHT;
}

//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
SFTimeSpan SFTime::DaylightBias()
{
  TIME_ZONE_INFORMATION tzi;

#ifdef _DEBUG
  SFBool bSuccess = (SFos::GetTimeZoneInformation(tzi) != TIME_ZONE_ID_UNKNOWN);
  ASSERT(bSuccess);
#else
  SFos::GetTimeZoneInformation(tzi);
#endif

  SFTimeSpan ret;
  if (tzi.DaylightBias > 0)
    ret = SFTimeSpan(0, 0, (SFInt32) tzi.DaylightBias, 0);
  else
  {
    ret = SFTimeSpan(0, 0, (SFInt32) -tzi.DaylightBias, 0);
    ret.Negate();
  }
  return ret;
}

//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
SFTimeSpan SFTime::TimezoneBias()
{
	SFTimeSpan ret;

  TIME_ZONE_INFORMATION tzi;
  if (SFos::GetTimeZoneInformation(tzi) != TIME_ZONE_ID_UNKNOWN)
	{
		SFInt32 tzBias = tzi.StandardBias + tzi.Bias;
		if (tzBias > 0)
		{
			ret = SFTimeSpan(0, 0, (SFInt32) tzBias, 0);
		} else
		{
			ret = SFTimeSpan(0, 0, (SFInt32) -tzBias, 0);
			ret.Negate();
		}
	}
  return ret;
}

//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
SFString SFTime::DaylightName()
{
	SFString str;
	SFInt32 i=0;

  TIME_ZONE_INFORMATION tzi;
  if (SFos::GetTimeZoneInformation(tzi) != TIME_ZONE_ID_UNKNOWN)
	{
		while (tzi.DaylightName[i])
			str += ((char)tzi.DaylightName[i++]);
	}
  return str;
}

//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
SFString SFTime::StandardName()
{
	SFString str;
	SFInt32 i=0;

  TIME_ZONE_INFORMATION tzi;
  if (SFos::GetTimeZoneInformation(tzi) != TIME_ZONE_ID_UNKNOWN)
	{
		while (tzi.StandardName[i])
			str += ((char)tzi.StandardName[i++]);
	}
  return str;
}

//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
void SFTime::CheckForValidLocalDate()
{
  //Should only be called when testing a LOCAL SFTime
  ASSERT(m_TimeFrame == LOCAL);

  TIME_ZONE_INFORMATION tzi;
  SFBool bSuccess = (SFos::GetTimeZoneInformation(tzi) != TIME_ZONE_ID_UNKNOWN);
  ASSERT(bSuccess);

  if (bSuccess)
  {
    //Set the SFTimeFrame to UTC so that we do not need to perform DST convert in
    //the following arithmetic
    m_TimeFrame = UTC;

    SFTime BeginDaylightTime;
    SFInt32 ThisYear = GetYear();
    if (tzi.DaylightDate.wYear == 0)  //Using day-in-month format
      BeginDaylightTime = SFTime(ThisYear, tzi.DaylightDate.wMonth, tzi.DaylightDate.wDay, (WORD)(tzi.DaylightDate.wDayOfWeek+1),
                            tzi.DaylightDate.wHour, tzi.DaylightDate.wMinute, tzi.DaylightDate.wSecond, UTC);
    else
      BeginDaylightTime = SFTime(tzi.DaylightDate.wYear, tzi.DaylightDate.wMonth, tzi.DaylightDate.wDay, tzi.DaylightDate.wHour,
                            tzi.DaylightDate.wMinute, tzi.DaylightDate.wSecond, UTC);

    SFDate BeginStandardTime;
    if (tzi.StandardDate.wYear == 0)  //Using day-in-month format
      BeginStandardTime = SFTime(ThisYear, tzi.StandardDate.wMonth, tzi.StandardDate.wDay, (WORD)(tzi.StandardDate.wDayOfWeek+1),
                            tzi.StandardDate.wHour, tzi.StandardDate.wMinute, tzi.StandardDate.wSecond, UTC);
    else
      BeginStandardTime = SFTime(tzi.StandardDate.wYear, tzi.StandardDate.wMonth, tzi.StandardDate.wDay, tzi.StandardDate.wHour,
                            tzi.StandardDate.wMinute, tzi.StandardDate.wSecond, UTC);


#ifdef _DEBUG
    SFString s = BeginDaylightTime.Format();
    s = BeginStandardTime.Format();
#endif

    if (tzi.DaylightBias > 0)
    {
      //A SFTime which specifies a date which occurs twice
      SFTime StartNonUnique(BeginDaylightTime - SFTimeSpan(0, 0, (SFInt32) tzi.DaylightBias, 0));
      if ((*this >= StartNonUnique) && (*this < BeginDaylightTime))
      {
//        TRACE0("DTime Warning: A SFTime using a LOCAL timeframe tried to be constructed which represents a non unique absolute time\n");
				m_nSeconds = _I64_MIN;
        m_TimeFrame = LOCAL;
        return;
      }

      //A SFTime which specifies a date which does not occur
      SFTime EndSkip(BeginStandardTime + SFTimeSpan(0, 0, (SFInt32) tzi.DaylightBias, 0));
      if ((*this > BeginStandardTime) && (*this < EndSkip))
      {
        TRACE0("A SFTime using a LOCAL timeframe tried to be constructed which does not occur\n");
        m_TimeFrame = LOCAL;
        return;
      }

    }
    else if (tzi.DaylightBias < 0)
    {
      //A SFTime which specifies a date which does not occur
      SFTime EndSkip(BeginDaylightTime + SFTimeSpan(0, 0, (SFInt32) -tzi.DaylightBias, 0));
      if ((*this > BeginDaylightTime) && (*this < EndSkip))
      {
        TRACE0("A SFTime using a LOCAL timeframe tried to be constructed which does not occur\n");
				m_nSeconds = _I64_MIN;
        m_TimeFrame = LOCAL;
        return;
      }

      //A SFTime which specifies a date which occurs twice
      SFTime StartNonUnique(BeginStandardTime - SFTimeSpan(0, 0, (SFInt32) -tzi.DaylightBias, 0));
      if ((*this >= StartNonUnique) &&
          (*this <= BeginStandardTime) )
      {
        TRACE0("DTime Warning: A SFTime using a LOCAL timeframe tried to be constructed which represents a non unique absolute time\n");
        m_TimeFrame = LOCAL;
        return;
      }
    }

    //return the timeframe back to local
    m_TimeFrame = LOCAL;
  }
}

*/

//----------------------------------------------------------------------------------------------------
//
// %a    Abbreviated weekday name
// %A    Full weekday name
// %b    Abbreviated month name
// %B    Full month name
// %d    Day of month as decimal number (01 - 31)
// %f    Calendar being used "O.S" Old Style (Julian) or "N.S" New Style (Gregorian)
// %j    Day of year as decimal number (001 - 366)
// %m    Month as decimal number (01 - 12)
// %P    AM/PM indicator
// %p    a/p indicator
// %U    Week of year as decimal number
// %w    Weekday as decimal number (1 - 7; Sunday is 1)
// %x    Date representation for current locale,  namely
//         Date representation + " " + Time Representation for current locale
// %y    Year without century, as decimal number (00 - 99)
// %Y    Year with century, as decimal number
// %c    Year displayed using C.E.(Current Epoch) / B.C.E (Before Current Epoch) convention e.g. -1023 = 1022 BCE
//
// %H    Hours in the day
// %h    12 Hour format Hours in (00 - 12)
// %M    Minutes in the hour
// %S    Seconds in the minute
// %Q    Seconds so far this day
// %%    Percent sign
//
//
// //may also need to include full windows escape character
//
// %%    Percent sign
//
// As in the printf function, the # flag may prefix any formatting code.
// In that case, the meaning of the format code is changed as follows.
//
// Format Code Meaning
// %#x Long date representation, appropriate to current locale, namely
//       Long Date representation + " " + Time Representation for current locale
//
//
// %#d, %#j, %#m, %#U, %#w, %#y, %#H, %#h, %#M, %#S  Remove leading zeros (if any).
//----------------------------------------------------------------------------------------------------
SFString SFTime::Format(const SFString& sFormat) const
{
  SFString ret;
  if (IsValid())
  {
    char sBuffer[512];

    int sFmtLength = (int)sFormat.GetLength();

    int i=0;
		for (i=0; i<sFmtLength; i++)
    {
      char c = sFormat.GetAt(i);
      if (c == '%')
      {
        ++i;
        if (i < sFmtLength)
        {
          c = sFormat.GetAt(i);
          switch (c)
          {
            case 'Q':
            {
              SFInt32 hour = GetHour();
							SFInt32 min  = GetMinute();
							SFInt32 secs = GetSecond();
              ret += asString(hour * 60 * 60 + min * 60 + secs);
              break;
            }
            case 'H':
            {
				// military
				sprintf(sBuffer, "%.02ld", GetHour());
				ret += sBuffer;
				break;
            }
            case 'h':
            {
				SFInt32 h = GetHour();
				if (h > 12)
					h -= 12;
				if (h == 0)
					h = 12;
				sprintf(sBuffer, "%.02ld", h);
				ret += sBuffer;
				break;
            }
            case 'M':
            {
              sprintf(sBuffer, "%.02ld", GetMinute());
              ret += sBuffer;
              break;
            }
            case 'P':
            {
				ASSERT(getTimePart().IsValid());
				ret += (GetHour() >= 12 ? SFos::GetLocaleInfo(LOCALE_S2359) : SFos::GetLocaleInfo(LOCALE_S1159));
				break;
            }
            case 'p':
            {
				ASSERT(getTimePart().IsValid());
				ret += (GetHour() >= 12 ? SFos::GetLocaleInfo(LOCALE_S2359SH) : SFos::GetLocaleInfo(LOCALE_S1159SH));
				break;
            }
            case 'S':
            {
              sprintf(sBuffer, "%.02ld", GetSecond());
              ret += sBuffer;
              break;
            }
            case 'a':
            {
              ret += getShortDayName(::getDayOfWeek(getDatePart()));
              break;
            }
            case 'A':
            {
              ret += getDayName(::getDayOfWeek(getDatePart()));
              break;
            }
            case 'b':
            {
              ret += getShortMonthName(GetMonth());
              break;
            }
            case 'B':
            {
              ret += getMonthName(GetMonth());
              break;
            }
            case 'd':
            {
              sprintf(sBuffer, "%.02ld", GetDay());
              ret += sBuffer;
              break;
            }
            case 'f':
            {
              ret += "Gregorian Calendar";
              break;
            }
            case 'm':
            {
              sprintf(sBuffer, "%.02ld", GetMonth());
              ret += sBuffer;
              break;
            }
            case 'j':
            {
              sprintf(sBuffer, "%.02ld", getDayOfYear(*this));
              ret += sBuffer;
              break;
            }
            case 'U':
            {
              sprintf(sBuffer, "%.02ld", getWeekOfYear(*this));
              ret += sBuffer;
              break;
            }
            case 'w':
            {
              sprintf(sBuffer, "%ld", ::getDayOfWeek(getDatePart()));
              ret += sBuffer;
              break;
            }
            case 'x':
            {
              ret += getDatePart().Format() + " " + getTimePart().Format();
              break;
            }
            case 'y':
            {
              sprintf(sBuffer, "%.02ld", get2Digit(GetYear()));
              ret += sBuffer;
              break;
            }
            case 'Y':
            {
              sprintf(sBuffer, "%ld", GetYear());
              ret += sBuffer;
              break;
            }
            case '#':
            {
              if (i < sFmtLength)
              {
                ++i;
                c = sFormat.GetAt(i);
                switch (c)
                {
                  case 'd':
                  {
                    sprintf(sBuffer, "%ld", GetDay());
                    ret += sBuffer;
                    break;
                  }
                  case 'm':
                  {
                    sprintf(sBuffer, "%ld", GetMonth());
                    ret += sBuffer;
                    break;
                  }
				  case 'j':
				  {
				    sprintf(sBuffer, "%.ld", getDayOfYear(*this));
				    ret += sBuffer;
				    break;
				  }
                  case 'U':
                  {
                    sprintf(sBuffer, "%ld", getWeekOfYear(*this));
                    ret += sBuffer;
                    break;
                  }
                  case 'x':
                  {
                    ret += getDatePart().Format() + " " + getTimePart().Format();
                    break;
                  }
                  case 'y':
                  {
                    sprintf(sBuffer, "%ld", get2Digit(GetYear()));
                    ret += sBuffer;
                    break;
                  }
                  case 'H':
                  {
                    sprintf(sBuffer, "%ld", GetHour());
                    ret += sBuffer;
                    break;
                  }
                  case 'h':
                  {
										SFInt32 h = GetHour();
										if (h > 12)
											h -= 12;
										if (h == 0)
											h = 12;
                    sprintf(sBuffer, "%ld", h);
                    ret += sBuffer;
                    break;
                  }
                  case 'M':
                  {
                    sprintf(sBuffer, "%ld", GetMinute());
                    ret += sBuffer;
                    break;
                  }
                  case 'S':
                  {
                    sprintf(sBuffer, "%ld", GetSecond());
                    ret += sBuffer;
                    break;
                  }
                  default:
                  {
                    ret += c;
                    break;
                  }
                }
              }
              break;
            }
            default:
            {
              ret += c;
              break;
            }
          }
        }
      }
      else
      {
        ret += c;
      }
    }
  }

	return ret;
}

SFString SFTime::Format(SFInt32 fmt) const
{
	return SFTime::Format(getFormatString(fmt, SFos::isEuropean(), SFos::isMilitary(), SFos::getDateSep(), SFos::getTimeSep()));
}

const static SFString fmts[] = {
	"%#m/%#d/%Y %#h:%M:%S%P",    // FMT_DEFAULT - 0
	"%#h%P",                     // FMT_TIMENOMINS
	"%#h:%M%P",                  // FMT_TIME
	"%#h:%M%p",                  // FMT_TIMESH
	"%#m/%#d",                   // FMT_DATENOYEAR
	"%#m/%#d/%Y",                // FMT_DATE
	"%A, %B %#d, %Y",            // FMT_DAY - 5
	"%b %#d, %y",                // FMT_DAYSH
	"Week of %A, %B %#d, %Y",    // FMT_WEEK
	"Week of %b %#d, %y",        // FMT_WEEKSH
	"Wk of %a, %b %#d, %Y",      // FMT_WEEKSHDATE
	"%a, %b %#d, %Y",            // FMT_DATEFMT - 10
	"%a %#d",                    // FMT_DAYNAMEDAY
	"%B %Y",                     // FMT_MONTH
	"%b %y",                     // FMT_MONTHSH
	"%Y",                        // FMT_YEAR
	"%y",                        // FMT_YEARSH - 15
	"%A",                        // FMT_DAYNAME
	"%a",                        // FMT_DAYNAMESH
	"%B",                        // FMT_MONTHNAME
	"%b",                        // FMT_MONTHNAMESH
	"%Q",                        // FMT_SECSTODAY - 20
	"%#d",                       // FMT_DAYNUMBER
	"%Y%m%d",                    // FMT_SORTYYYYMMDD
	"%A %#d",                    // FMT_DAYNAMEDAYL
	"%#h:%M %P",                 // FMT_TIME_EXPORT
	"%#m/%#d/%Y %#h:%M:%S %P",   // FMT_DEFAULT_EXPORT - 25
	"%Y%m%d%H%M%S",              // FMT_SORTALL
	"%A, %B %#d. %Y - %#h:%M%P", // FMT_TIMESTAMP
	"%b %#d",                    // FMT_DAYSH_NOYEAR - 28
	"%Y%m%dT%H%M%S",             // FMT_VCAL_DATE
	"%Y;%m;%d;%H;%M;%S;",        // FMT_SEMI_SEP
};

#ifdef _DEBUG
const SFInt32 nFields = sizeof(fmts) / sizeof(SFString);
#endif

SFString getFormatString(SFInt32 fmt, SFBool euro, SFBool mil, const SFString& dSep, const SFString& tSep)
{
	ASSERT(fmt>-1 && fmt<nFields);
	SFString str = fmts[fmt];

	if (euro)
	{
		switch (fmt)
		{
		case FMT_DEFAULT:
		case FMT_DATENOYEAR:
		case FMT_DATE:
			str.Replace("%#m/%#d", "%#d/%#m");
			break;

		case FMT_DAY:
		case FMT_DAYSH:
		case FMT_DAYSH_NOYEAR:
		case FMT_WEEK:
		case FMT_WEEKSH:
		case FMT_WEEKSHDATE:
		case FMT_DATEFMT:
		case FMT_TIMESTAMP:
			str.Replace("%B %#d", "%#d %B");
			str.Replace("%b %#d", "%#d %b");
			break;
		}
	}

	if (mil)
	{
		switch (fmt)
		{
			case FMT_DEFAULT:
			case FMT_TIMENOMINS:
			case FMT_TIME:
			case FMT_TIMESH:
			case FMT_TIMESTAMP:
			case FMT_TIME_EXPORT:
			str.Replace("%#h", "%#H");
			break;
		}
	}

	if (dSep != "/")
	{
		switch (fmt)
		{
		case FMT_DEFAULT:
		case FMT_DATENOYEAR:
		case FMT_DATE:
			str.ReplaceAll("/", dSep);
			break;
		}
	}

	if (tSep != ":")
	{
		switch (fmt)
		{
		case FMT_DEFAULT:
		case FMT_TIME:
		case FMT_TIMESH:
		case FMT_TIMESTAMP:
			str.ReplaceAll(":", tSep);
			break;
		}
	}

	return str;
}
