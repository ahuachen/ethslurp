/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "sftimespan.h"
#include "sftimeofday.h"

//-------------------------------------------------------------------------
SFTimeSpan::SFTimeSpan()
{
	m_nSeconds = _I64_MIN;
}

//-------------------------------------------------------------------------
SFTimeSpan::SFTimeSpan(const SFTimeSpan& lts)
{
	m_nSeconds = lts.m_nSeconds;
}

//-------------------------------------------------------------------------
SFTimeSpan::SFTimeSpan(SFInt32 d, SFInt32 h, SFInt32 m, SFInt32 s)
{
	if (d >= 0)
		m_nSeconds = SFInt64(d)*SECS_PER_DAY + SFInt64(h)*SECS_PER_HOUR + SFInt64(m)*SECS_PER_MIN + s;
	else
		m_nSeconds = SFInt64(d)*SECS_PER_DAY - SFInt64(h)*SECS_PER_HOUR - SFInt64(m)*SECS_PER_MIN - s;
}

//-------------------------------------------------------------------------
SFTimeSpan::SFTimeSpan(const SFTimeOfDay& tod)
{
	m_nSeconds = SFInt64(0)*SECS_PER_DAY + SFInt64(tod.GetHour())*SECS_PER_HOUR + SFInt64(tod.GetMinute())*SECS_PER_MIN + tod.GetSecond();
}
      
//-------------------------------------------------------------------------
SFTimeSpan::SFTimeSpan(SFInt64 s)
{
	m_nSeconds = s;
}

//-------------------------------------------------------------------------
SFTimeSpan& SFTimeSpan::operator=(const SFTimeSpan& ts)
{ 
	m_nSeconds = ts.m_nSeconds;
	return *this;
}

//-------------------------------------------------------------------------
SFBool SFTimeSpan::isPositive(void) const
{
	return (m_nSeconds > 0);
}

//-------------------------------------------------------------------------
SFBool SFTimeSpan::isNegative(void) const
{
	return (m_nSeconds < 0);
}

//-------------------------------------------------------------------------
SFBool SFTimeSpan::isZero(void) const
{
	return (m_nSeconds == 0);
}

//-------------------------------------------------------------------------
// Note: only days are represented as negative.  Hours, mins, secs are always positive.
//-------------------------------------------------------------------------
SFInt32 SFTimeSpan::getTotalDays() const
{
	ASSERT(IsValid());
	return (SFInt32)(m_nSeconds/SECS_PER_DAY);
}

//-------------------------------------------------------------------------
SFInt32 SFTimeSpan::getDaysAbs() const
{
	ASSERT(IsValid());
	if (m_nSeconds == 0) return 0;
	if (isPositive())
	{
		return (SFInt32)(m_nSeconds/SECS_PER_DAY);
	} else
	{
		SFTimeSpan n(*this);n.Negate();
		return n.getDaysAbs();
	}  
}

//-------------------------------------------------------------------------
SFInt32 SFTimeSpan::getHoursAbs() const
{
	ASSERT(IsValid());
	if (m_nSeconds == 0) return 0;
	if (isPositive())
	{
		return (SFInt32) ((m_nSeconds - SECS_PER_DAY * SFInt64(getTotalDays()))/SECS_PER_HOUR);
	} else
	{
		SFTimeSpan n(*this);n.Negate();
		return n.getHoursAbs();
	}  
}

//-------------------------------------------------------------------------
SFInt32 SFTimeSpan::getMinutesAbs() const
{
	ASSERT(IsValid());
	if (m_nSeconds == 0) return 0;
	if (isPositive())
	{
		return (SFInt32) ((m_nSeconds - SECS_PER_DAY*SFInt64(getTotalDays()) - SECS_PER_HOUR*getHoursAbs())/SECS_PER_MIN);
	} else
	{
		SFTimeSpan n(*this);n.Negate();
		return n.getMinutesAbs();
	}  
}

//-------------------------------------------------------------------------
SFInt32 SFTimeSpan::getSecondsAbs() const
{
	ASSERT(IsValid());
	if (m_nSeconds == 0) return 0;
	if (isPositive())
	{
		return (SFInt32) (m_nSeconds - SECS_PER_DAY*SFInt64(getTotalDays()) - SECS_PER_HOUR*getHoursAbs() - SECS_PER_MIN*getMinutesAbs());
	} else
	{
		SFTimeSpan n(*this);n.Negate();
		return n.getSecondsAbs();
	}  
}                             

//-----------------------------------------------------------------------------
SFString getTimeSpanString(const SFTimeSpan& ts)
{
	if (!ts.IsValid())
		return "Not valid timespan";

	SFInt32 days  = ts.getTotalDays ();
	SFInt32 hours = ts.getHoursAbs  ();
	SFInt32 mins  = ts.getMinutesAbs();

	SFString fmt;
	if (days)          fmt += "%Dd ";
	if (hours || mins) fmt += "%#Hh ";
	if (mins)          fmt += "%#Mm";

	return ts.Format(fmt.IsEmpty() ? "%#Hh %#Mm" : fmt);
}

//-----------------------------------------------------------------------------
// The Following Format parameters are supported        
// %D							Total days in this SFTimeSpan
// %H							Hours in this SFTimeSpan (00 - 23)
// %M							Minutes in the current hour in this SFTimeSpan (00 - 59)
// %S							Seconds in the current minute in this SFTimeSpan (00 - 59)
// %%							Percent sign
// %#H, %#M, %#S	Remove leading zeros (if any).
//-----------------------------------------------------------------------------
SFString SFTimeSpan::Format(const SFString& fmt) const
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
            case 'D':
            {
				SFInt32 Days = getTotalDays();
				sprintf(sBuffer, "%ld", Days);
				if (m_nSeconds < 0 && Days == 0) //handle neagtive 0
					ret += "-";
				ret += sBuffer;
				break;
            }
            case 'H':
            {
              sprintf(sBuffer, "%.02ld", getHoursAbs());
              ret += sBuffer;
              break;
            }
            case 'M':
            {
              sprintf(sBuffer, "%.02ld", getMinutesAbs());
              ret += sBuffer;
              break;
            }
            case 'S':
            {
              sprintf(sBuffer, "%.02ld", getSecondsAbs());
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
                    sprintf(sBuffer, "%ld", getHoursAbs());
                    ret += sBuffer;
                    break;
                  }
                  case 'M':
                  {
                    sprintf(sBuffer, "%ld", getMinutesAbs());
                    ret += sBuffer;
                    break;
                  }
                  case 'S':
                  {
                    sprintf(sBuffer, "%ld", getSecondsAbs());
                    ret += sBuffer;
                    break;
                  }
                  default:
                  {
                    ret += c;
                    break;
                  }
                }
              break;
              }
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

#include "sftime.h"
//-----------------------------------------------------------------------------
SFString getTimeSpanString(const SFTime& t1In, const SFTime& t2In)
{
	SFTime t1 = t1In;
	SFTime t2 = t2In;
	if (t1 == t2)
		return "0h 0m";
	return getTimeSpanString(SFTimeSpan(t1-t2));
}
