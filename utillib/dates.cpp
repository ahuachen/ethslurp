/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "dates.h"
#include "sfos.h"
#include "string_table.h"
#include "aes_encrypt.h"

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
	SFInt32 year  = date.GetYear();
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
		viewTypes[VT_DAYLIST]      = LoadStringGH("Day List View");
		viewTypes[VT_DAYGRID]      = LoadStringGH("Day Grid View");
		viewTypes[VT_DAYPLANNER]   = LoadStringGH("Day Planner View");
		viewTypes[VT_DAYREPORT]    = LoadStringGH("Day Report View");
		viewTypes[VT_WEEKLIST]     = LoadStringGH("Week List View");
		viewTypes[VT_WEEKGRID]     = LoadStringGH("Week Grid View");
		viewTypes[VT_WEEKPLANNER]  = LoadStringGH("Week Planner View");
		viewTypes[VT_WEEKREPORT]   = LoadStringGH("Week Report View");
		viewTypes[VT_MONTHLIST]    = LoadStringGH("Month List View");
		viewTypes[VT_MONTHGRID]    = LoadStringGH("Month Grid View");
		viewTypes[VT_MONTHPLANNER] = LoadStringGH("Month Planner View");
		viewTypes[VT_MONTHREPORT]  = LoadStringGH("Month Report View");
		viewTypes[VT_YEARLIST]     = LoadStringGH("Year List View");
		viewTypes[VT_YEARGRID]     = LoadStringGH("Year Grid View");
		viewTypes[VT_YEARPLANNER]  = LoadStringGH("Year Planner View");
		viewTypes[VT_YEARREPORT]   = LoadStringGH("Year Report View");
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

	SFInt32 y = atoi((const char *)dStr.Mid(0,4));
	SFInt32 m = atoi((const char *)dStr.Mid(4,2));
	SFInt32 d = atoi((const char *)dStr.Mid(6,2));

	SFInt32 h  = atoi((const char *)tStr.Mid(0,2));
	SFInt32 mn = atoi((const char *)tStr.Mid(2,2));
	SFInt32 s  = atoi((const char *)tStr.Mid(4,2));

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
SFTime snagDate(const SFString& dStr, const SFTime& def)
{
	SFString str = dStr;
	if (str.IsEmpty())
		return def;
	
	SFInt32 y  = atoi(nextTokenClear(str, ';'));
	SFInt32 m  = atoi(nextTokenClear(str, ';'));
	SFInt32 d  = atoi(nextTokenClear(str, ';'));
	SFInt32 h  = atoi(nextTokenClear(str, ';'));
	SFInt32 mn = atoi(nextTokenClear(str, ';'));
	SFInt32 s  = atoi(nextTokenClear(str, ';'));

	return SFTime(y,m,d,h,mn,s);
}

//---------------------------------------------------------------------------------------
SFString unRotateStr(const SFString& iiIn, SFInt32 timeout, SFInt32 serr, SFBool oldVersion)
{
	SFString ii = iiIn;
if (!oldVersion)
{
	serr=1;
	// This thing must come in encrypted and it must have a cookie key set
	if (ii.Left(4) != "!zzE" || (serr < 1 || serr > 4))
		return ii;
	ii = hard_decrypt(iiIn, serr);
}
	if (ii.IsEmpty())
		return ii;
	
	SFString in = ii;
	SFString ret;
	SFInt32 len = atoi(in);
	if (!len)
		return "";
	len -= 12;

	SFInt32 f = in.Find("x0");
	ASSERT(f!=-1);
	in = in.Mid(f+1);
	int i=0;
	int cnt=1;
	for (i=0;i<len;i++)
	{
		ASSERT(cnt < in.GetLength());
		ret += in[cnt];
		cnt += 2;
	}
	ret.Reverse();

	SFString str  = ret;

	SFInt32 find = ret.Find("|");
	if (find != -1)
	{
		str  = ret.Left(find);
		SFInt32  secsThen = atoi((const char *)ret.Mid(find+1));
		SFInt32  secsNow  = atoi((const char *)Now().Format(FMT_SECSTODAY));
		if (secsThen > secsNow)
			secsNow += (24 * 60 * 60); // login spanned midnight
		
#ifdef _DEBUG
		if (FALSE)
#else
		// If this command is timeout minutes old or older then fail
		if (secsNow - secsThen > (timeout * 60)) 
#endif
			return "";
	}
	return str;
}

//---------------------------------------------------------------------------------------
// serr is French for 'key'
SFString rotateStr(const SFString& input, SFBool timed, SFInt32 serr, SFBool oldVersion)
{
	serr=1;

	ASSERT(input.Find("|") == -1);

	SFString in = input;
	if (timed)  // do we want this to ever timeout?
		in += ("|" + Now().Format(FMT_SECSTODAY));

	int len  = (int)in.GetLength();
	SFString ret = asString(len+12) + "x0";
	int i=0;
	for (i=len-1;i>=0;i--)
	{
		SFString s = in[i];
		SFInt32 ch = 'a' + (RandomValue(0, 250)%25);
		if (RandomValue(0,10) % 2)
			ch = toupper((char)ch);
		s += SFString((char)ch);
		ret += s;
	}

	if (oldVersion || serr < 1 || serr > 4)
		return ret;
	return hard_encrypt(ret, serr); // french for key
}

//-----------------------------------------------------------------------------------------
class CFileDater
{
public:
	SFTime   fileDate;
	SFString fileName;
	SFString ignore;
	CFileDater(void) { fileDate = earliestDate; }
};

//-----------------------------------------------------------------------------------------
extern SFBool dateTheFile(SFString& fileName, void *data);
SFBool dateTheFile(SFString& fileName, void *data)
{
	CFileDater *fd = (CFileDater*)data;
	SFTime fD = SFos::fileLastModifyDate(fileName);
	if (fD > fd->fileDate)
	{
		if (!fileName.endsWith(fd->ignore))
		{
		fd->fileDate = fD;
		fd->fileName = fileName;
	}
	}
	return TRUE;
}

#include "paths.h"
//-----------------------------------------------------------------------------------------
extern SFTime newestFileDateInFolder(const SFString& path, SFString *whichFilePtr, const SFString& ignore);
SFTime newestFileDateInFolder(const SFString& path, SFString *whichFilePtr, const SFString& ignore)
{
	CFileDater fd;
	fd.ignore = ignore;
	forAllItemsInFolder(path, dateTheFile,  &fd, F_DEFAULT|F_FULL_PATHS);
	if (whichFilePtr)
		*whichFilePtr = fd.fileName;
	return fd.fileDate;
}

/*
//-----------------------------------------------------------------------------------------
void checkFolderDate(const SFString& path, const SFString& folderIn, const SFString& fileIn)
{
	SFString folder = path + folderIn;
	SFString file   = path + fileIn;
	SFString newestFile;
	
	SFTime newestPDF = newestFileDateInFolder(folder, &newestFile, fileIn);
	SFTime fileDate  = SFos::fileLastModifyDate(file);
	if (newestPDF < fileDate)
	{
		CStringExportContext check;
		check << "\tFile date:\t"   << fileDate.Format(FMT_DEFAULT)  << " (" << file.Substitute(path, "./")       << ")\n";
		check << "\tFolder date:\t" << newestPDF.Format(FMT_DEFAULT) << " (" << newestFile.Substitute(path, "./") << ")\n";
		reportIt(check, "Folder is out of date...");
	}
}
*/
