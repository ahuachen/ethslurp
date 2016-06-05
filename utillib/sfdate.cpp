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

#include "dates.h"
#include "sfos.h"

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFDate::SFDate(void)
{
  m_nDays = (SFUint32)LONG_MIN;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFDate::SFDate(const SFDate& date)
{
  m_nDays = date.m_nDays;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFDate::SFDate(SFInt32 y, SFInt32 m, SFInt32 d)
{
  setValues(y,m,d);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFDate::SFDate(SFInt32 y, SFInt32 m, SFInt32 weekInMonth, SFInt32 dayOfWeek)
{
	// assume it fails
	m_nDays = (SFUint32)LONG_MIN;

  // assume any year is OK
	if ( (m >= 1 && m <= 12) &&
			 (weekInMonth >= 1 && weekInMonth <= 4) &&
			 (dayOfWeek >= 1 && dayOfWeek <= 7))
	{
		SFDate firstDay(y, m, 1);
		ASSERT(firstDay.IsValid());

		SFInt32 dow = getDayOfWeek(firstDay);
		SFInt32 d = (SFInt32) (((dayOfWeek - dow + 7) % 7) + (7*(weekInMonth-1)) + 1);
		setValues(y, m, d);
	}

}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFDate::SFDate(SFInt32 days)
{
	if (days > 2146905911L) // Largest valid GD N
		m_nDays = (SFUint32)LONG_MIN;
	else
		m_nDays = days + 2000000000L;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFDate::SFDate(const SF_TIMESTRUCT& sysTime)
{
  setValues(sysTime.tm_year, sysTime.tm_mon, sysTime.tm_mday);
}

//-------------------------------------------------------------------------
//
// dateStr is a legal date string as defined by fmtStr.
//
// fmtStr is a 5 character string where the first 3 indicate the
// order of the month/day/year, the fourth character indicates the length
// of the year, and the fifth character indicates the separator.
//
// For example:
//
// fmtStr	  dateStr       Result
//
// "dmy2/"   "20/03/96"     20th March 1996
// "mdy4-"   "03-28-1996"   28th March 1996
//
// This feature allows for the creation of SFDate's from a string
// as entered in a masked edit field or from a parsed report.
//
//-------------------------------------------------------------------------
SFDate::SFDate(const SFString& dateStr, const SFString& fmtStr)
{
	m_nDays  = (SFUint32)LONG_MIN;
	if (fmtStr.GetLength() != 5)
	{
		*this = SFDate(Now().GetYear(), Now().GetMonth(), Now().GetDay());
		return;
	}

	SFInt32 day   = 1;
	SFInt32 month = 1;
	SFInt32 year  = Now().GetYear();

	char sep = fmtStr[4];

	SFString str = dateStr; // 12-10-1921 for example

	for (int i=0 ; i<3 && str.GetLength()>0 ; i++)
	{
		switch(fmtStr[i])
		{
		case 'd':
		case 'D':
		day = toLong(nextTokenClear(str, sep));
		break;
		case 'm':
		case 'M':
		month = toLong(nextTokenClear(str, sep));
		break;
		case 'y':
		case 'Y':
		{
			year = toLong(nextTokenClear(str, sep));
			char c = fmtStr.GetAt(3);
			ASSERT((c == '2') || (c == '4'));
			if (c == '2' || year < 100)
			{
				if (year >= 50)
					year += 1900;
				else if (year < 50)
					year += 2000;
			}
		}
		break;
		}
	}

	year  = CLAMP(year,  earliestDate.GetYear(), latestDate.GetYear());
	month = CLAMP(month, 1,                      12);
	day   = CLAMP(day,   1,                      DaysInMonth(year, month));

	setValues(year, month, day);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFDate& SFDate::operator=(const SFDate& date)
{
  m_nDays = date.m_nDays;
  return *this;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFDate& SFDate::setValues(SFInt32 y, SFInt32 m, SFInt32 d)
{
	m_nDays  = (SFUint32)LONG_MIN;

  if (m >= JANUARY && m <= DECEMBER && d <= DaysInMonth(y, m))
  {
	  //The following algorithm has been taken from from an article in
		//the March 1993 issue of the Windows / Dos Developers Journal.

	  m_nDays = (y-1) * 365 + lfloor(y-1, 4L);

	  m_nDays += lfloor(y-1, 400L) - lfloor(y-1, 100L);

		--m;
		while (m)
		{
			m_nDays += DaysInMonth(y, m);
	    m--;
		}

		m_nDays += (d + 1999422264L); //ensure all usable date values are positive by adding 2 billion to m_ldays
	}

  return *this;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFDateStruct SFDate::getDateStruct() const
{
  // See the comment in SFDate::setValues(SFInt32 Year, SFInt32 Month, SFInt32 Day)
  // for references to where the algorithm is taken from

  ASSERT(IsValid());


  SFDateStruct ds;
  SFInt32 gdn  = m_nDays - 1999422265L;
  SFInt32 y4   = 1461L;
  SFInt32 y400 = 146100L - 3*TRUE;
  SFInt32 y100 = 36525L - TRUE;

  SFBool exception=FALSE;

  ds.m_Year = 400*lfloor(gdn, y400);

  gdn -= y400*lfloor(gdn, y400);       //400 year periods

  if (gdn > 0L)
  {
    ds.m_Year += 100*lfloor(gdn, y100);  //100 year periods
    gdn -= y100*lfloor(gdn, y100);
    exception = (gdn == 0L && TRUE);
    if (gdn > 0L)
    {
      ds.m_Year += 4*lfloor(gdn, y4);     //4 year periods
      gdn -= y4*lfloor(gdn, y4);
      if (gdn > 0L)
      {
        int i=0;
        while (gdn > 365 && ++i < 4)
        {
          ds.m_Year++;
          gdn -= 365L;
        }
      }
    }
  }

  if (exception)
    gdn = 366L; //occurs once every hundred years with Gregorian calendar
  else
  {
    ds.m_Year++;
    gdn++;
  }


  ds.m_Month = 1;

  while (ds.m_Month < 13 && gdn > ((SFInt32) DaysInMonth(ds.m_Year, ds.m_Month)))
    gdn -= DaysInMonth(ds.m_Year, ds.m_Month++);

  if (ds.m_Month == 13)
  {
    ds.m_Month = 1;
    ds.m_Year++;
  }


  ds.m_Day = (SFInt32) gdn;

  return ds;
}

//-------------------------------------------------------------------------
//
// The Following Format parameters are supported
//
// %a    Abbreviated weekday name
// %A    Full weekday name
// %b    Abbreviated month name
// %B    Full month name
// %d    Day of month as decimal number (01 - 31)
// %f    Calendar being used "O.S" Old Style (Julian) or "N.S" New Style (Gregorian)
// %j    Day of year as decimal number (001 - 366)
// %m    Month as decimal number (01 - 12)
// %U    Week of year as decimal number
// %w    Weekday as decimal number (1 - 7; Sunday is 1)
// %y    Year without century, as decimal number (00 - 99)
// %Y    Year with century, as decimal number
// %#x   Long date representation, appropriate to current locale
// %#d, %#j, %#m, %#U, %#y Remove leading zeros (if any)
//
//-------------------------------------------------------------------------
SFString SFDate::Format(const SFString& fmt) const
{
  SFString fmtStr = fmt;
	ASSERT(!fmtStr.IsEmpty());

	char sBuffer[512];

	SFString ret;
  if (IsValid())
  {
    int sFmtLength = (int)fmtStr.GetLength();

    int i=0;
    for (i=0; i<sFmtLength; i++)
    {
      char c = fmtStr.GetAt(i);
      if (c == '%')
      {
        ++i;
        if (i < sFmtLength)
        {
          c = fmtStr.GetAt(i);
          switch (c)
          {
            case 'a':
            {
              ret += getShortDayName(getDayOfWeek(*this));
              break;
            }
            case 'A':
            {
              ret += getDayName(getDayOfWeek(*this));
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
#if 0
            case 'j':
            {
              sprintf(sBuffer, "%.03ld", DaysSinceJan0());
              ret += sBuffer;
              break;
            }
#endif
            case 'm':
            {
              sprintf(sBuffer, "%.02ld", GetMonth());
              ret += sBuffer;
              break;
            }
            case 'U':
            {
              SFTime tm(GetYear(), GetMonth(), GetDay(), 12, 0, 0);
							sprintf(sBuffer, "%.02ld", getWeekOfYear(tm));
              ret += sBuffer;
              break;
            }
            case 'w':
            {
              sprintf(sBuffer, "%ld", getDayOfWeek(*this));
              ret += sBuffer;
              break;
            }
            case 'y':
            {
              sprintf(sBuffer, "%.02ld", get2Digit(labs(GetYear())));
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
                c = fmtStr.GetAt(i);
                switch (c)
                {
                  case 'd':
                  {
                    sprintf(sBuffer, "%ld", GetDay());
                    ret += sBuffer;
                    break;
                  }
#if 0
                  case 'j':
                  {
                    sprintf(sBuffer, "%ld", DaysSinceJan0());
                    ret += sBuffer;
                    break;
                  }
#endif
                  case 'm':
                  {
                    sprintf(sBuffer, "%ld", GetMonth());
                    ret += sBuffer;
                    break;
                  }
                  case 'U':
                  {
			              SFTime tm(GetYear(), GetMonth(), GetDay(), 12, 0, 0);
                    sprintf(sBuffer, "%ld", getWeekOfYear(tm));
                    ret += sBuffer;
                    break;
                  }
                  case 'y':
                  {
			              sprintf(sBuffer, "%ld", get2Digit(labs(GetYear())));
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
