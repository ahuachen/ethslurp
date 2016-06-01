/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "sfos.h"
#include "dates.h"

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFTimeOfDay::SFTimeOfDay()
{
  // create an invalid value (would be the next day otherwise!)
  m_nSeconds = SECS_PER_DAY;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFTimeOfDay::SFTimeOfDay(const SFTimeOfDay& tod)
{
	m_nSeconds = tod.m_nSeconds;
}                                

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFTimeOfDay::SFTimeOfDay(SFInt32 h, SFInt32 m, SFInt32 s)
{
  m_nSeconds = h * SECS_PER_HOUR + m * SECS_PER_MIN + s;
  m_nSeconds = MIN(SECS_PER_DAY, m_nSeconds); // make it invalid of overrun
}    

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFTimeOfDay::SFTimeOfDay(SFInt32 secs)
{
  m_nSeconds = secs;
  m_nSeconds = MIN(SECS_PER_DAY, m_nSeconds); // make it invalid of overrun
}                                

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFTimeOfDay::SFTimeOfDay(const SF_TIMESTRUCT& sysTime)
{        
  m_nSeconds = sysTime.tm_hour * SECS_PER_HOUR + sysTime.tm_min * SECS_PER_MIN + sysTime.tm_sec;
  m_nSeconds = MIN(SECS_PER_DAY, m_nSeconds); // make it invalid of overrun
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFTimeOfDay& SFTimeOfDay::operator=(const SFTimeOfDay& tod)
{               
  m_nSeconds = tod.m_nSeconds;
  return *this;
}

//-------------------------------------------------------------------------
//
// dateStr is a legal date string as defined by fmtStr.
//
// fmtStr is a 4 character string where the first 3 indicate the
// order of the hour/min/sec and the fourth character indicates the separator.
// The date string should have a space between the time and meridian (if
// meridian is not present then assume AM.  If hour > 12 then assume military
//
// For example:
//
// fmtStr	  dateStr       Result
//
// "hms:"   "12:10 "    12:10 pm
// "hms-"   "0-10-0"   12:10:00 am
//
// This feature allows for the creation of SFTimeOfDay's from a string
// as entered in a masked edit field or from a parsed report.
//
//-------------------------------------------------------------------------
SFTimeOfDay::SFTimeOfDay(const SFString& timeStr, const SFString& fmtStr)
{
	m_nSeconds = SECS_PER_DAY;
	if (fmtStr.GetLength() != 4)
	{
		*this = SFTimeOfDay(12, 0, 0);
		return;
	}

	SFInt32 hour = 12;
	SFInt32 min  = 0;
	SFInt32 secs = 0;
	SFBool  am   = TRUE;

	char sep = fmtStr[3];

	SFString str = timeStr; // 12:12:12 am for example

	for (int i=0 ; i<3 && str.GetLength() > 0 ; i++)
	{
		switch(fmtStr[i])
		{
		case 'h':
		case 'H':
			hour = toLong(nextTokenClear(str, sep));
			break;

		case 'm':
		case 'M':
			min = toLong(nextTokenClear(str, sep));
			break;

		case 's':
		case 'S':
			secs = toLong(nextTokenClear(str, sep));
			break;
		}
	}

	SFInt32 find = timeStr.Find(" ");
	if (find != -1)
	{
		str = timeStr.Mid(find);
		str.MakeLower();
		if (str.Find("p") != -1)
			am = FALSE;
	}

	if (!SFos::isMilitary())
	{
		if (!am && hour < 12)
			hour += 12;

		else if (am && hour == 12)
			hour = 0;
	}

	*this = SFTimeOfDay(hour, min, secs);
}

//-------------------------------------------------------------------------
//
// The Following Format parameters are supported        
//
// %H    Hours in the current day
// %M    Minutes in the current hour
// %h    12 Hour format Hours in this SFTimeSpan (00 - 12)
// %P    AM / PM indicator
// %p    AM / PM indicator
// %S    Seconds in the current minute
// %%    Percent sign       
// %#H, %#h, %#M, %#S  Remove leading zeros (if any).
//-------------------------------------------------------------------------
SFString SFTimeOfDay::Format(const SFString& fmt) const
{ 
  SFString sFormat = fmt;
	ASSERT(!sFormat.IsEmpty());
	
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
            case 'H':
            {
              sprintf(sBuffer, "%.02ld", GetHour());
              ret += sBuffer;
              break;
            }
            case 'M':
            {
              sprintf(sBuffer, "%.02ld", GetMinute());
              ret += sBuffer;
              break;
            }
            case 'h':
            {
							SFInt32 h = GetHour();
							if (h > 12)
								h -= 12;
              sprintf(sBuffer, "%.02ld", h);
              ret += sBuffer;
              break;
            }
            case 'P':
            {
				ASSERT(IsValid());
				ret += (GetHour() >= 12 ? SFos::GetLocaleInfo(LOCALE_S2359) : SFos::GetLocaleInfo(LOCALE_S1159));
				break;
            }
            case 'p':
            {
				ASSERT(IsValid());
				ret += (GetHour() >= 12 ? SFos::GetLocaleInfo(LOCALE_S2359SH) : SFos::GetLocaleInfo(LOCALE_S1159SH));
				break;
            }
            case 'S':
            {
              sprintf(sBuffer, "%.02ld", GetSecond());
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
