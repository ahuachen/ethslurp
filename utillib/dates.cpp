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

//---------------------------------------------------------------------------------------
const SFTime latestDate   = SFTime(2200, 12, 31, 23, 59, 59);
const SFTime earliestDate = SFTime(1700,  1,  1,  0,  0,  1);

//---------------------------------------------------------------------------------------
const SFInt32 DaysPerMonth[] =
{
	31,28,31,30,31,30,31,31,30,31,30,31
};

//---------------------------------------------------------------------------------------
SFBool isLeap(SFInt32 year)
{
  if ((year%400)==0) // centuries every 400 years are leaps
		return TRUE;

  if (((year % 4)==0) && ((year % 100)!=0)) // otherwise every four years (but not centuries)
		return TRUE;

	return FALSE;
}

//---------------------------------------------------------------------------------------
SFInt32 DaysInMonth(SFInt32 year, SFInt32 month)
{
  if (month==2 && isLeap(year))
		return 29;
  return DaysPerMonth[month-1];
}

//---------------------------------------------------------------------------------------
SFInt32 DaysInMonth(const SFTime& date)
{
	return DaysInMonth(date.GetYear(), date.GetMonth());
}

//---------------------------------------------------------------------------------------
SFInt32 DaysInYear(SFInt32 year)
{
  return isLeap(year) ? ((SFInt32)366) : ((SFInt32)365);
}

//---------------------------------------------------------------------------------------
SFInt32 DaysInYear(const SFTime& date)
{
  return DaysInYear(date.GetYear());
}

//---------------------------------------------------------------------------------------
SFTime AddOneDay(const SFTime& date)
{
	if (date >= latestDate)
		return latestDate;

	SFInt32 day   = date.GetDay();
	SFInt32 month = date.GetMonth();
	SFInt32 year  = date.GetYear();
	SFInt32 min   = date.GetMinute();
	SFInt32 sec   = date.GetSecond();
	SFInt32 hour  = date.GetHour();
	SFInt32 nDays = DaysInMonth(year, month);

	if (nDays==day)
	{
		if (month==12)
		{
		 month = 1;
		 year  += 1;
		 return SFTime(year,month,1,hour,min,sec);
		} else
		{
		 month += 1;
		 return SFTime(year,month,1,hour,min,sec);
		}
	}

	return SFTime(year,month,day+1,hour,min,sec);
}

//---------------------------------------------------------------------------------------
SFTime SubtractOneDay(const SFTime& date)
{
	if (date <= earliestDate)
		return earliestDate;

	SFInt32 day   = date.GetDay();
	SFInt32 month = date.GetMonth();
	SFInt32 year  = date.GetYear();
	SFInt32 min   = date.GetMinute();
	SFInt32 sec   = date.GetSecond();
	SFInt32 hour  = date.GetHour();

	if (day==1)
	{
		if (month==1)
		{
		 month = 12;
		 year  -= 1;
		 return SFTime(year,month,DaysInMonth(year, month),hour,min,sec);
		} else
		{
		 month -= 1;
		 return SFTime(year,month,DaysInMonth(year, month),hour,min,sec);
		}
	}

	return SFTime(year,month,day-1,hour,min,sec);
}

//---------------------------------------------------------------------------------------
SFTime BeginningOfPeriod(const SFTime& date, SFInt32 vt)
{
	if (vt==VT_HOUR)
		return BOH(date);
	else if (isDayType(vt))
		return BOD(date);
	else if (isWeekType(vt)) // WEEKSTARTDAYCHANGE
		return BOW(date);
	else if (isMonthType(vt))
		return BOM(date);
	else if (isYearType(vt))
		return BOY(date);

	// should not happen
	ASSERT(0);
	return date;
}

//---------------------------------------------------------------------------------------
SFTime EndOfPeriod(const SFTime& date, SFInt32 vt)
{
	if (vt==VT_HOUR)
		return EOH(date);
	else if (isDayType(vt))
		return EOD(date);
	else if (isWeekType(vt)) // WEEKSTARTDAYCHANGE
		return EOW(date);
	else if (isMonthType(vt))
		return EOM(date);
	else if (isYearType(vt))
		return EOY(date);

	// should not happen
	ASSERT(0);
	return date;
}

//---------------------------------------------------------------------------------------
SFTime IncrementDate(SFInt32 vt, const SFTime& date, SFInt32 n)
{
// VIEWTYPEARRAY
	if (n==-100)
	{
		if (vt==VT_HOUR)
			return earlierOf(latestDate, date+SFTimeSpan(0,1,0,0));
		else if (isDayType(vt))
			return earlierOf(latestDate, AddOneDay(date));
		else if (isWeekType(vt)) // WEEKSTARTDAYCHANGE
			return earlierOf(latestDate, BONW(date));
		else if (isMonthType(vt))
			return earlierOf(latestDate, BONM(date));
		else if (isYearType(vt))
			return earlierOf(latestDate, BONY(date));
	} else
	{
		SFTime d = date;
		int i=0;
		for (i=0;i<labs(n);i++)
		{
			if (n<0)
				d = DecrementDate(vt, d);
			else
				d = IncrementDate(vt, d);
		}
		return d;
	}

	// should not happen
	ASSERT(0);
	return date;
}

//---------------------------------------------------------------------------------------
SFTime DecrementDate(SFInt32 vt, const SFTime& date, SFInt32 n)
{
// VIEWTYPEARRAY
	if (n==-100)
	{
		if (vt==VT_HOUR)
			return laterOf(earliestDate, date-SFTimeSpan(0,1,0,0));
		else if (isDayType(vt))
			return laterOf(earliestDate, SubtractOneDay(date));
		else if (isWeekType(vt)) // WEEKSTARTDAYCHANGE
			return laterOf(earliestDate, BOW(EOPW(date)));
		else if (isMonthType(vt))
			return laterOf(earliestDate, BOM(EOPM(date)));
		else if (isYearType(vt))
			return laterOf(earliestDate, BOY(EOPY(date)));
	} else
	{
		SFTime d = date;
		int i=0;
		for (i=0;i<labs(n);i++)
		{
			if (n<0)
				d = IncrementDate(vt, d);
			else
				d = DecrementDate(vt, d);
		}
		return d;
	}

	// should not happen
	ASSERT(0);
	return date;
}

// WEEKSTARTDAYCHANGE
//---------------------------------------------------------------------------------------
SFInt32 WeeksInMonth(const SFTime& date)
{
  SFInt32 extraDays = DaysInMonth(date) - 28;

  // days in first week
  SFTime bom = BOM(date);
  SFInt32 days = 8 - bom.GetDayOfWeek();

  if (extraDays == 0 && days == 7)
		return 4;

	SFBool n = (4 + ((extraDays > days) ? 2 : 1));
  return n;
}

//---------------------------------------------------------------------------------------
SFTime FirstDayofMonth(const SFTime& date)
{
  // 12:01am
  SFInt32 year  = date.GetYear();
  SFInt32 month = date.GetMonth();
  return SFTime(year,month,1,0,0,0);
}

//---------------------------------------------------------------------------------------
SFTime LastDayofMonth(const SFTime& date)
{
	SFTime firstDay = FirstDayofMonth(date);

	SFInt32 month = firstDay.GetMonth();
  SFInt32 year  = firstDay.GetYear();

	return SFTime(year,month,DaysInMonth(date),23,59,59);
}

// WEEKSTARTDAYCHANGE
//---------------------------------------------------------------------------------------
SFTime FirstDayofWeek(const SFTime& date)
{
	SFInt32 y = date.GetYear();
	SFInt32 m = date.GetMonth();
	SFInt32 d = date.GetDay();
	SFTime noonTime = SFTime(y, m, d, 12, 0, 0);

	SFInt32 n   = date.GetDayOfWeek() - 1; // sun is '1'
	SFTime temp = DecrementDate(VT_DAYLIST, noonTime, n);
	SFTime ret  = BOD(temp);

	if (ret <= earliestDate)
		return earliestDate;

	return ret;
}

// WEEKSTARTDAYCHANGE
//---------------------------------------------------------------------------------------
SFTime LastDayofWeek(const SFTime& date)
{
	// 11:59pm
	SFInt32 y = date.GetYear();
	SFInt32 m = date.GetMonth();
	SFInt32 d = date.GetDay();
	SFTime noonTime = SFTime(y, m, d, 12, 0, 0);

	SFTime temp = IncrementDate(VT_DAYLIST, noonTime, 7);
	SFTime ret = FirstDayofWeek(temp) - SFTimeSpan(0,0,0,1);

	if (ret >= latestDate)
		return latestDate;
	return ret;
}

//---------------------------------------------------------------------------------------
SFTime FirstDayofYear(const SFTime& date)
{
  // 12:01am
  return BOD(SFTime(date.GetYear(),1,1,0,0,0));
}

//---------------------------------------------------------------------------------------
SFTime LastDayofYear(const SFTime& date)
{
  // 11:59pm
  return EOD(SFTime(date.GetYear(),12,31,0,0,0));
}

//---------------------------------------------------------------------------------------
SFBool isSamePeriodType(SFInt32 t1, SFInt32 t2)
{
	if (t1==-1 || t2==-1) return FALSE;
	if (isDayType  (t1))  return isDayType  (t2);
	if (isWeekType (t1))  return isWeekType (t2);
	if (isMonthType(t1))  return isMonthType(t2);
	if (isYearType (t1))  return isYearType (t2);

	// should not happen
	ASSERT(0);
	return FALSE;
}

//---------------------------------------------------------------------------------------
SFBool isSameDisplayType(SFInt32 t1, SFInt32 t2)
{
	if (t1==-1 || t2==-1)  return FALSE;
	if (isGridType   (t1)) return isGridType   (t2);
	if (isListType   (t1)) return isListType   (t2);
	if (isPlannerType(t1)) return isPlannerType(t2);
	if (isReportType (t1)) return isReportType (t2);

	// should not happen
	ASSERT(0);
	return FALSE;
}

//---------------------------------------------------------------------------------------
SFString getDayName(SFInt32 dow)
{
  switch (dow)
  {
    case MONDAY:    return SFos::GetLocaleInfo(LOCALE_SDAYNAME1); break;
    case TUESDAY:   return SFos::GetLocaleInfo(LOCALE_SDAYNAME2); break;
    case WEDNESDAY: return SFos::GetLocaleInfo(LOCALE_SDAYNAME3); break;
    case THURSDAY:  return SFos::GetLocaleInfo(LOCALE_SDAYNAME4); break;
    case FRIDAY:    return SFos::GetLocaleInfo(LOCALE_SDAYNAME5); break;
    case SATURDAY:  return SFos::GetLocaleInfo(LOCALE_SDAYNAME6); break;
    case SUNDAY:    return SFos::GetLocaleInfo(LOCALE_SDAYNAME7); break;
    default: ASSERT(FALSE); break;
  }
  return SFString();
}

//---------------------------------------------------------------------------------------
SFString getShortDayName(SFInt32 dow)
{
  SFString ret;

  switch (dow)
  {
    case MONDAY:    return SFos::GetLocaleInfo(LOCALE_SABBREVDAYNAME1); break;
    case TUESDAY:   return SFos::GetLocaleInfo(LOCALE_SABBREVDAYNAME2); break;
    case WEDNESDAY: return SFos::GetLocaleInfo(LOCALE_SABBREVDAYNAME3); break;
    case THURSDAY:  return SFos::GetLocaleInfo(LOCALE_SABBREVDAYNAME4); break;
    case FRIDAY:    return SFos::GetLocaleInfo(LOCALE_SABBREVDAYNAME5); break;
    case SATURDAY:  return SFos::GetLocaleInfo(LOCALE_SABBREVDAYNAME6); break;
    case SUNDAY:    return SFos::GetLocaleInfo(LOCALE_SABBREVDAYNAME7); break;
    default: ASSERT(FALSE); break;
  }

  return ret;
}

//---------------------------------------------------------------------------------------
SFString getMonthName(SFInt32 month)
{
  SFString ret;

  switch (month)
  {
    case JANUARY:   return SFos::GetLocaleInfo(LOCALE_SMONTHNAME1); break;
    case FEBRUARY:  return SFos::GetLocaleInfo(LOCALE_SMONTHNAME2); break;
    case MARCH:     return SFos::GetLocaleInfo(LOCALE_SMONTHNAME3); break;
    case APRIL:     return SFos::GetLocaleInfo(LOCALE_SMONTHNAME4); break;
    case MAY:       return SFos::GetLocaleInfo(LOCALE_SMONTHNAME5); break;
    case JUNE:      return SFos::GetLocaleInfo(LOCALE_SMONTHNAME6); break;
    case JULY:      return SFos::GetLocaleInfo(LOCALE_SMONTHNAME7); break;
    case AUGUST:    return SFos::GetLocaleInfo(LOCALE_SMONTHNAME8); break;
    case SEPTEMBER: return SFos::GetLocaleInfo(LOCALE_SMONTHNAME9); break;
    case OCTOBER:   return SFos::GetLocaleInfo(LOCALE_SMONTHNAME10); break;
    case NOVEMBER:  return SFos::GetLocaleInfo(LOCALE_SMONTHNAME11); break;
    case DECEMBER:  return SFos::GetLocaleInfo(LOCALE_SMONTHNAME12); break;
    default: ASSERT(FALSE); break;
  }

  return ret;
}

//---------------------------------------------------------------------------------------
SFString getShortMonthName(SFInt32 month)
{
  SFString ret;

  switch (month)
  {
    case JANUARY:   return SFos::GetLocaleInfo(LOCALE_SABBREVMONTHNAME1); break;
    case FEBRUARY:  return SFos::GetLocaleInfo(LOCALE_SABBREVMONTHNAME2); break;
    case MARCH:     return SFos::GetLocaleInfo(LOCALE_SABBREVMONTHNAME3); break;
    case APRIL:     return SFos::GetLocaleInfo(LOCALE_SABBREVMONTHNAME4); break;
    case MAY:       return SFos::GetLocaleInfo(LOCALE_SABBREVMONTHNAME5); break;
    case JUNE:      return SFos::GetLocaleInfo(LOCALE_SABBREVMONTHNAME6); break;
    case JULY:      return SFos::GetLocaleInfo(LOCALE_SABBREVMONTHNAME7); break;
    case AUGUST:    return SFos::GetLocaleInfo(LOCALE_SABBREVMONTHNAME8); break;
    case SEPTEMBER: return SFos::GetLocaleInfo(LOCALE_SABBREVMONTHNAME9); break;
    case OCTOBER:   return SFos::GetLocaleInfo(LOCALE_SABBREVMONTHNAME10); break;
    case NOVEMBER:  return SFos::GetLocaleInfo(LOCALE_SABBREVMONTHNAME11); break;
    case DECEMBER:  return SFos::GetLocaleInfo(LOCALE_SABBREVMONTHNAME12); break;
    default: ASSERT(FALSE); break;
  }

  return ret;
}

//---------------------------------------------------------------------------------------
//
// For these algorithms please see: http://www.pauahtun.org/CalendarFAQ
//
static SFInt32 JulianDay(const SFTime& date)
{
	SFInt32 year;//  = date.GetYear();
	SFInt32 month = date.GetMonth();
	SFInt32 day   = date.GetDay();

	SFInt32 a = (14 - month) / 12; // expect integer division

	// This is an ancient bug that I didn't want to fix on Linux - Jay 4/12/2013
	SFInt32 y = year = 4800 - a;
	SFInt32 m = month + (12 * a) - 3;

	SFInt32 JD = day +
								(((153 * m) + 2) / 5) +
								(365 * y) +
								(y / 4) -
								(y / 100) +
								(y / 400) -
								32045;
	return JD;
}

//---------------------------------------------------------------------------------------
static SFInt32 finalCheck(SFInt32 d, SFInt32 w, const SFTime& date)
{
	SFInt32 add = 0;

	if (BOY(date).GetDayOfWeek() < 5 || BOY(date).GetDayOfWeek() == 7)
		add = 1;

	if (w >= 1 && w <= 52)
		return w + add;

	if (w == 53)
	{
		if (d == 6)
			return w + add;

		if (d == 5 && isLeap(date.GetYear()))
			return w + add;
	}

	return 1; // week 1 of following year
}

//---------------------------------------------------------------------------------------
SFInt32 getDayOfYear(const SFTime& dtIn)
{
	SFTime dt   = dtIn;
	SFTime jan1 = BOY(dt);
	return (dt - jan1).getTotalDays();
}

//---------------------------------------------------------------------------------------
SFInt32 getWeekOfYear(const SFTime& dt)
{
	if (BOW(dt) < BOY(dt))
		return (BOY(dt).GetDayOfWeek() < 5);

	SFTime date = BOW(dt);

	SFInt32 jb = JulianDay(BOY(dt));
	SFInt32 jt = JulianDay(date);

	SFInt32 d = (jb + 3) % 7;
	SFInt32 w = (jt + d - jb + 4) / 7; //expect integer division

	if (w != 0)
		return finalCheck(d, w, dt);

	jb = JulianDay(BOY(EOPY(date)));
	jt = JulianDay(date);

	d = (jb + 3) % 7;
	w = (jt + d - jb + 4) / 7; //expect integer division

	return finalCheck(d, w, dt);
}

//---------------------------------------------------------------------------------------
SFInt32 getWeekOfMonth(const SFTime& date)
{
  ASSERT(date.IsValid());
  return getWeekOfYear(date) - getWeekOfYear(BOM(date)) + 1;
}

//---------------------------------------------------------------------------------------
static SFInt32 WeekOfDayModulo(SFInt32 x, SFInt32 y)
{
	if (x >= 0)
	{
		return x % y;

	} else
	{
		return (y - labs(x % y)) % y;

	}
}

//---------------------------------------------------------------------------------------
SFInt32 getDayOfWeek(const SFDate& date)
{
	ASSERT(date.IsValid());
	return (SFInt32) (WeekOfDayModulo(date.GetTotalDays(), 7) + 1);
}

//---------------------------------------------------------------------------------------
// Return the day number of day in the month 'date' at the
// row/col location.  Useful only for month calendar display.
//---------------------------------------------------------------------------------------
SFTime GetDay(const SFTime& date, SFInt32 row, SFInt32 col)
{
	SFTime ret = date;

	row++;
	col++;

	// row>0 and col>0
	SFInt32 dow    = BOM(date).GetDayOfWeek();
	SFInt32 dayNum = ((col - dow + 1) + ((row - 1) * 7));

	if (dayNum < 1)
	{
		// Dead day in previous month
		dayNum = DaysInMonth(EOPM(date)) + dayNum;
		ret    = BOM(EOPM(date));

	} else if (dayNum > DaysInMonth(date))
	{
		// Dead day in next month
		dayNum -= DaysInMonth(date);
		ret     = BONM(date);

	}

	SetDay(ret, dayNum);
	return ret;
}

//---------------------------------------------------------------------------------------
// Get the day number as a string at a particular row and col.
//---------------------------------------------------------------------------------------
SFInt32 GetDayText(const SFTime& date, SFString& text, SFInt32 row, SFInt32 col)
{
	SFTime ret = GetDay(date, row, col);
	char tt[10];
	sprintf(tt, "%ld", ret.GetDay());
	text = tt;
	return (ret.GetMonth()!=date.GetMonth());
}

//---------------------------------------------------------------------------------------
// Return TRUE if the given date (qDate) is the same as the
// day represented by row/col in the month in which qDate is in.
//---------------------------------------------------------------------------------------
SFBool isToday(const SFTime& qDate, SFInt32 row, SFInt32 col)
{
	return (MIDDAY(GetDay(qDate, row, col)) == MIDDAY(qDate));
}

//---------------------------------------------------------------------------------------
SFBool isToday(const SFTime& qDate, const SFTime& nDate)
{
	return (MIDDAY(nDate) == MIDDAY(qDate));
}

//---------------------------------------------------------------------------------------
static SFInt32 _formats[] =
{
	FMT_DEFAULT,
	FMT_DAY,
	FMT_DAY,
	FMT_DAY,
	FMT_DAY,
	FMT_WEEK,
	FMT_WEEK,
	FMT_WEEK,
	FMT_WEEK,
	FMT_MONTH,
	FMT_MONTH,
	FMT_MONTH,
	FMT_MONTH,
	FMT_YEAR,
	FMT_YEAR,
	FMT_YEAR,
	FMT_YEAR,
};

//---------------------------------------------------------------------------------------
SFString getFormatedDateString(const SFTime& date, SFInt32 vt)
{
	return date.Format(_formats[vt]);
}

//---------------------------------------------------------------------------------------
SFString getViewTypeString(SFInt32 vt)
{
	static SFBool   viewTypesOK = FALSE;
	static SFString viewTypes[VT_LAST];

	if (!viewTypesOK)
	{
		viewTypes[VT_FIRST]        = EMPTY;
		viewTypes[VT_DAYLIST]      = ("Day List View");
		viewTypes[VT_DAYGRID]      = ("Day Grid View");
		viewTypes[VT_DAYPLANNER]   = ("Day Planner View");
		viewTypes[VT_DAYREPORT]    = ("Day Report View");
		viewTypes[VT_WEEKLIST]     = ("Week List View");
		viewTypes[VT_WEEKGRID]     = ("Week Grid View");
		viewTypes[VT_WEEKPLANNER]  = ("Week Planner View");
		viewTypes[VT_WEEKREPORT]   = ("Week Report View");
		viewTypes[VT_MONTHLIST]    = ("Month List View");
		viewTypes[VT_MONTHGRID]    = ("Month Grid View");
		viewTypes[VT_MONTHPLANNER] = ("Month Planner View");
		viewTypes[VT_MONTHREPORT]  = ("Month Report View");
		viewTypes[VT_YEARLIST]     = ("Year List View");
		viewTypes[VT_YEARGRID]     = ("Year Grid View");
		viewTypes[VT_YEARPLANNER]  = ("Year Planner View");
		viewTypes[VT_YEARREPORT]   = ("Year Report View");
		viewTypesOK = TRUE;
	};

	return viewTypes[vt];
}

//---------------------------------------------------------------------------------------
void SetDate(SFTime& date, const SFString& dStr)
{
	if (dStr.IsEmpty())
		return;

	SFString dateStr = dStr;
	dateStr.MakeLower();

	SFString format = getFormatString(FMT_DATE, SFos::isEuropean(), SFos::isMilitary(), SFos::getDateSep(), SFos::getTimeSep());

	// convert to "mdy4/" or "dmy4-"
	format.ReplaceAll("%", "");
	format.ReplaceAll("#", "");
	format.ReplaceAll(SFos::getDateSep(), "");
	format.Replace("y", "y2"); // order matters
	format.Replace("Y", "y4");
	format += SFos::getDateSep();

	date = SFTime(SFDate(dateStr, format), date.getTimePart());

	ASSERT(date.IsValid());
}

//---------------------------------------------------------------------------------------
void SetTime(SFTime& date, const SFString& tStr)
{
	if (tStr.IsEmpty())
		return;

	SFString timeStr = tStr;
	timeStr.MakeLower();
	timeStr.Replace("p", " p"); // need at least one space between time and meridian
	timeStr.Replace("a", " a"); // need at least one space between time and meridian

	SFString format = getFormatString(FMT_TIME, SFos::isEuropean(), SFos::isMilitary(), SFos::getDateSep(), SFos::getTimeSep());

	// convert to "hms:" or "hms-"
	format.ReplaceAll("%", "");
	format.ReplaceAll("#", "");
	format.ReplaceAll(SFos::getTimeSep(), "");
	format.Replace("P", "");
	format.Replace("M", "m");
	format += "s" + SFos::getTimeSep();

	date = SFTime(date.getDatePart(), SFTimeOfDay(timeStr, format));
	ASSERT(date.IsValid());
}

//---------------------------------------------------------------------------------------
void SetTimeAndDate(SFTime& date, const SFString& vStr)
{
	if (vStr.IsEmpty())
		return;

	SFString datePart, timePart;
	timePart = vStr;
	datePart = nextTokenClear(timePart, ' ');
	if (timePart.IsEmpty())
		timePart = "12:00:00pm";

	date = Now();
	SetDate(date, datePart);
	SetTime(date, timePart);
}

//---------------------------------------------------------------------------------------
void SFTime::fromSortStr(const SFString& vStr)
{
	if (vStr.IsEmpty())
		return;

	// This is required because some pre 2.5.1 data contains
	// junky stuff in this field - if it doesn't at least
	// start with a digit its definitely junk
	char first = vStr[0];
	if (!isdigit(first))
		return;

	ASSERT(vStr.GetLength() == 14);
	SFString dStr = vStr.Mid(0,8);
	SFString tStr = vStr.Mid(8,6);

	SFInt32 y = toLong(dStr.Mid(0,4));
	SFInt32 m = toLong(dStr.Mid(4,2));
	SFInt32 d = toLong(dStr.Mid(6,2));

	SFInt32 h  = toLong(tStr.Mid(0,2));
	SFInt32 mn = toLong(tStr.Mid(2,2));
	SFInt32 s  = toLong(tStr.Mid(4,2));

	*this = SFTime(SFDate(y,m,d), SFTimeOfDay(h,mn,s));
	ASSERT(IsValid());
}

SFTime newNow = SFTime(2006,2,1,12,0,0);
//---------------------------------------------------------------------------------------
SFTime Now(void)
{
	SFInt32 tzOffset = SFos::getTimeZone();

	SFTime ret;
#ifdef TESTING
	if (g_unitTesting)
	{
		ret = newNow;
	} else
#endif
	{
		SF_TIMESTRUCT sysTime;
		SFos::GetLocalTime(sysTime);
		ret = SFTime(sysTime, FALSE);
	}

	if (tzOffset)
		return (ret + SFTimeSpan(0,0,tzOffset,0));
	return ret;
}

//---------------------------------------------------------------------------------------
SFTime snagDate(const SFString& strIn, const SFTime& def, SFBool dir)
{
	if (strIn.IsEmpty())
		return def;

	SFString str = strIn;
	str.ReplaceAll(";",EMPTY);
	if (str.GetLength() != 14)
	{
		if (dir==-1) str += "000001";
		if (dir== 0) str += "120000";
		if (dir== 1) str += "235959";
	}

	SFInt32 y  = toLong(str.Left(4));
	SFInt32 m  = toLong(str.Mid( 4,2));
	SFInt32 d  = toLong(str.Mid( 6,2));
	SFInt32 h  = toLong(str.Mid( 8,2));
	SFInt32 mn = toLong(str.Mid(10,2));
	SFInt32 s  = toLong(str.Mid(12,2));

	return SFTime(y,m,d,h,mn,s);
}
