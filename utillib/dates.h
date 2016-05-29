#ifndef _H_DUTILITY
#define _H_DUTILITY
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "sftime.h"

// Adding viewtypes:
//
//	Hardest part - .ini file has hard coded items must dynamically upgrade .ini files on read
//  Array access by vt - there is only one place
//  Add help file items
//  Add menu items
//  Make sure to check .ini files since they hard code values (look for GetProfile with grep and then search for VT_ lines in resuults)
//  The rest is easy
//
#define VT_FIRST         ((long)0)
#define VT_DAYLIST       ((long)1)
#define VT_DAYGRID       ((long)2)
#define VT_DAYPLANNER    ((long)3)
#define VT_DAYREPORT     ((long)4)
#define VT_WEEKLIST      ((long)5)
#define VT_WEEKGRID      ((long)6)
#define VT_WEEKPLANNER   ((long)7)
#define VT_WEEKREPORT    ((long)8)
#define VT_MONTHLIST     ((long)9)
#define VT_MONTHGRID     ((long)10)
#define VT_MONTHPLANNER  ((long)11)
#define VT_MONTHREPORT   ((long)12)
#define VT_YEARLIST      ((long)13)
#define VT_YEARGRID      ((long)14)
#define VT_YEARPLANNER   ((long)15)
#define VT_YEARREPORT    ((long)16)
#define VT_LAST          ((long)(VT_YEARREPORT+1))

// Date display formatting shortcuts
// Internationalized
#define FMT_DEFAULT         0
#define FMT_TIMENOMINS      1
#define FMT_TIME            2
#define FMT_TIMESH          3
#define FMT_DATENOYEAR      4
#define FMT_DATE            5
#define FMT_DAY             6
#define FMT_DAYSH           7
#define FMT_WEEK            8
#define FMT_WEEKSH          9
#define FMT_WEEKSHDATE     10
#define FMT_DATEFMT        11
#define FMT_DAYNAMEDAY     12
#define FMT_MONTH          13
#define FMT_MONTHSH        14
#define FMT_YEAR           15
#define FMT_YEARSH         16
#define FMT_DAYNAME        17
#define FMT_DAYNAMESH      18
#define FMT_MONTHNAME      19
#define FMT_MONTHNAMESH    20
#define FMT_SECSTODAY      21
#define FMT_DAYNUMBER      22
#define FMT_SORTYYYYMMDD   23
#define FMT_DAYNAMEDAYL    24
#define FMT_TIME_EXPORT    25
#define FMT_DEFAULT_EXPORT 26
#define FMT_SORTALL        27
#define FMT_TIMESTAMP      28
#define FMT_DAYSH_NOYEAR   29
#define FMT_VCAL_DATE      30
#define FMT_SEMI_SEP       31

// Not a real viewtype but used in some EO and BO macros
#define VT_HOUR (VT_LAST+1)

#define BOM  FirstDayofMonth
#define EOM  LastDayofMonth
#define BOY  FirstDayofYear
#define EOY  LastDayofYear
#define BOW  FirstDayofWeek
#define EOW  LastDayofWeek
#define BOP  BeginningOfPeriod
#define EOP  EndOfPeriod

extern SFTime   Now               (void);

extern SFString getDayName        (SFInt32 dow);
extern SFString getShortDayName   (SFInt32 dow);
extern SFString getMonthName      (SFInt32 month);
extern SFString getShortMonthName (SFInt32 month);

extern SFInt32  getWeekOfYear     (const SFTime& date); 
extern SFInt32  getDayOfYear      (const SFTime& date);

extern SFInt32  getDayOfWeek      (const SFDate& date);
extern SFInt32  getWeekOfMonth    (const SFTime& date);

extern SFTime   IncrementOneDay   (const SFTime& date);
extern SFTime   DecrementOneDay   (const SFTime& date);
extern SFTime   IncrementDate     (SFInt32 type, const SFTime& date, SFInt32 n=-100);
extern SFTime   DecrementDate     (SFInt32 type, const SFTime& date, SFInt32 n=-100);

extern SFInt32  DaysInYear        (SFInt32 year);
extern SFInt32  DaysInMonth       (SFInt32 year, SFInt32 month);
extern SFInt32  DaysInYear        (const SFTime& date);
extern SFInt32  DaysInMonth       (const SFTime& date);
extern SFInt32  WeeksInMonth      (const SFTime& date);
extern SFBool   isLeap            (SFInt32 year);

extern SFTime   FirstDayofMonth   (const SFTime& date);
extern SFTime   LastDayofMonth    (const SFTime& date);
extern SFTime   FirstDayofWeek    (const SFTime& date);
extern SFTime   LastDayofWeek     (const SFTime& date);
extern SFTime   FirstDayofYear    (const SFTime& date);
extern SFTime   LastDayofYear     (const SFTime& date);

extern SFTime   BeginningOfPeriod   (const SFTime& date, SFInt32 type);
extern SFTime   EndOfPeriod        (const SFTime& date, SFInt32 type);
extern SFTime   AddOneDay          (const SFTime& date);
extern SFTime   SubtractOneDay     (const SFTime& date);

extern const SFTime latestDate;
extern const SFTime earliestDate;

inline SFTime BOH(const SFTime& date)
{
	// H:00:00
	return SFTime(date.GetYear(), date.GetMonth(), date.GetDay(), date.GetHour(), 0, 0);
}

inline SFTime EOH(const SFTime& date)
{
	// H:59:59
	return SFTime(date.GetYear(), date.GetMonth(), date.GetDay(), date.GetHour(), 59, 59);
}

inline SFTime BOD(const SFTime& date)
{
	// 12:00:00 am
	return SFTime(date.GetYear(), date.GetMonth(), date.GetDay(), 0, 0, 0);
}

inline SFTime EOD(const SFTime& date)
{
	// 11:59:59 pm
	return SFTime(date.GetYear(), date.GetMonth(), date.GetDay(), 23, 59, 59);
}

inline SFTime MIDDAY(const SFTime& date)
{
	// 12:00 noon
	return SFTime(date.GetYear(), date.GetMonth(), date.GetDay(), 12, 0, 0);
}

inline SFTime BONM(const SFTime& date)
{
	return BOM(IncrementDate(VT_DAYLIST, EOM(date), 2));
}

inline SFTime EOPM(const SFTime& date)
{
	return DecrementDate(VT_DAYLIST, BOM(date));
}

inline SFTime BONY(const SFTime& date)
{
	return IncrementDate(VT_DAYLIST, EOY(date));
}

inline SFTime EOPY(const SFTime& date)
{
	return DecrementDate(VT_DAYLIST, BOY(date));
}

inline SFTime BONW(const SFTime& date)
{
	SFTime eow = EOW(date);
	return IncrementDate(VT_DAYLIST, eow);
}

inline SFTime EOPW(const SFTime& date)
{
	return DecrementDate(VT_DAYLIST, BOW(date), 2);
}

inline SFTime BOND(const SFTime& date)
{
	return BOD(IncrementDate(VT_DAYLIST, date));
}

inline SFTime EOPD(const SFTime& date)
{
	return EOD(DecrementDate(VT_DAYLIST, date));
}

//-------------------------------------------------
inline SFBool isGridType(SFInt32 vt)
{
	ASSERT(vt>VT_FIRST && vt<VT_LAST);
	return (vt == VT_DAYGRID || vt == VT_WEEKGRID || vt == VT_MONTHGRID || vt == VT_YEARGRID);
}

//-------------------------------------------------
inline SFBool isListType(SFInt32 vt)
{
	ASSERT(vt>VT_FIRST && vt<VT_LAST);
	return (vt == VT_DAYLIST || vt == VT_WEEKLIST || vt == VT_MONTHLIST || vt == VT_YEARLIST);
}

//-------------------------------------------------
inline SFBool isPlannerType(SFInt32 vt)
{
	ASSERT(vt>VT_FIRST && vt<VT_LAST);
	return (vt == VT_DAYPLANNER || vt == VT_WEEKPLANNER || vt == VT_MONTHPLANNER || vt == VT_YEARPLANNER);
}

//-------------------------------------------------
inline SFBool isReportType(SFInt32 vt)
{
	ASSERT(vt>VT_FIRST && vt<VT_LAST);
	return (vt == VT_DAYREPORT || vt == VT_WEEKREPORT || vt == VT_MONTHREPORT || vt == VT_YEARREPORT);
}

//-------------------------------------------------
inline SFBool isDayType(SFInt32 vt)
{
	ASSERT(vt>VT_FIRST && vt<VT_LAST);
	return (vt == VT_DAYGRID || 
						vt == VT_DAYLIST ||
						vt == VT_DAYPLANNER ||
						vt == VT_DAYREPORT
						);
}

inline SFBool isWeekType(SFInt32 vt)
{
	ASSERT(vt>VT_FIRST && vt<VT_LAST);
	return (vt == VT_WEEKGRID || 
						vt == VT_WEEKLIST || 
						vt == VT_WEEKPLANNER ||
						vt == VT_WEEKREPORT
						);
}

inline SFBool isMonthType(SFInt32 vt)
{
	ASSERT(vt>VT_FIRST && vt<VT_LAST);
	return (vt == VT_MONTHGRID || 
						vt == VT_MONTHLIST ||
						vt == VT_MONTHPLANNER ||
						vt == VT_MONTHREPORT
						);
}

inline SFBool isYearType(SFInt32 vt)
{
	ASSERT(vt>VT_FIRST && vt<VT_LAST);
	return (vt == VT_YEARGRID ||
						vt == VT_YEARLIST ||
						vt == VT_YEARPLANNER ||
						vt == VT_YEARREPORT
						);
}

extern SFBool isSamePeriodType(SFInt32 t1, SFInt32 t2);
extern SFBool isSameDisplayType(SFInt32 t1, SFInt32 t2);

#define VT_DAYTYPE     100
#define VT_WEEKTYPE    101
#define VT_MONTHTYPE   102
#define VT_YEARTYPE    103
#define VT_GRIDTYPE    200
#define VT_LISTTYPE    201
#define VT_PLANNERTYPE 202
#define VT_REPORTTYPE  203

inline SFInt32 getViewType(SFInt32 period, SFInt32 display)
{
	switch (display)
	{
	case VT_GRIDTYPE:
		switch (period)
		{
		case VT_DAYTYPE:   return VT_DAYGRID;
		case VT_WEEKTYPE:  return VT_WEEKGRID;
		case VT_MONTHTYPE: return VT_MONTHGRID;
		case VT_YEARTYPE:  return VT_YEARGRID;
		}
		break;
	case VT_LISTTYPE:
		switch (period)
		{
		case VT_DAYTYPE:   return VT_DAYLIST;
		case VT_WEEKTYPE:  return VT_WEEKLIST;
		case VT_MONTHTYPE: return VT_MONTHLIST;
		case VT_YEARTYPE:  return VT_YEARLIST;
		}
		break;
	case VT_PLANNERTYPE:
		switch (period)
		{
		case VT_DAYTYPE:   return VT_DAYPLANNER;
		case VT_WEEKTYPE:  return VT_WEEKPLANNER;
		case VT_MONTHTYPE: return VT_MONTHPLANNER;
		case VT_YEARTYPE:  return VT_YEARPLANNER;
		}
		break;
	case VT_REPORTTYPE:
		switch (period)
		{
		case VT_DAYTYPE:   return VT_DAYREPORT;
		case VT_WEEKTYPE:  return VT_WEEKREPORT;
		case VT_MONTHTYPE: return VT_MONTHREPORT;
		case VT_YEARTYPE:  return VT_YEARREPORT;
		}
		break;
	}
	// happens sometimes but we default OK so don't complain
	return VT_MONTHGRID;
}

inline SFInt32 getDisplayType(SFInt32 vt)
{
	if (isGridType(vt))    return VT_GRIDTYPE;
	if (isListType(vt))    return VT_LISTTYPE;
	if (isPlannerType(vt)) return VT_PLANNERTYPE;
	if (isReportType(vt))  return VT_REPORTTYPE;

	// should not happen
	ASSERT(0);
	return VT_GRIDTYPE;
}

inline SFInt32 getDisplayType(const SFString& t)
{
	if (t % "list")    return VT_LISTTYPE;
	if (t % "grid")    return VT_GRIDTYPE;
	if (t % "planner") return VT_PLANNERTYPE;
	if (t % "report")  return VT_REPORTTYPE;

	// should not happen
	ASSERT(0);
	return VT_GRIDTYPE;
}

inline SFInt32 getPeriodType(SFInt32 vt)
{
	if (isDayType(vt))   return VT_DAYTYPE;
	if (isWeekType(vt))  return VT_WEEKTYPE;
	if (isMonthType(vt)) return VT_MONTHTYPE;
	if (isYearType(vt))  return VT_YEARTYPE;

	// should not happen
	ASSERT(0);
	return VT_MONTHTYPE;
}

inline SFInt32 getPeriodType(const SFString& t)
{
	if (t % "day")   return VT_DAYTYPE;
	if (t % "week")  return VT_WEEKTYPE;
	if (t % "month") return VT_MONTHTYPE;
	if (t % "year")  return VT_YEARTYPE;

	// should not happen
	ASSERT(0);
	return VT_MONTHTYPE;
}

inline SFInt32 sameDisplayType(SFInt32 vt, SFInt32 perType)
{
	return getViewType(perType, getDisplayType(vt));

}

inline SFInt32 samePeriodType(SFInt32 vt, SFInt32 disType)
{
	return getViewType(getPeriodType(vt), disType);
}

inline void SetMonth(SFTime& date, SFInt32 val)
{
	ASSERT(date.IsValid());
	SFInt32 m = MIN(12, MAX(1, val));
	date = SFTime(date.GetYear(), 
								m, 
								MIN(DaysInMonth(date.GetYear(), m), date.GetDay()),
								date.GetHour(), 
								date.GetMinute(), 
								0);
	ASSERT(date.IsValid());
}

inline void SetDay(SFTime& date, SFInt32 val)
{
	ASSERT(date.IsValid());
	date = SFTime(date.GetYear(), 
								date.GetMonth(), 
								MIN(DaysInMonth(date.GetYear(), date.GetMonth()), MAX(1, val)), 
								date.GetHour(), 
								date.GetMinute(), 
								0);
	ASSERT(date.IsValid());
}

inline void SetYear(SFTime& date, SFInt32 val)
{
	ASSERT(date.IsValid());
	SFInt32 y = MIN(latestDate.GetYear(), MAX(earliestDate.GetYear(),  val));
	SFInt32 m = date.GetMonth();
	SFInt32 d = MIN(DaysInMonth(y, m), date.GetDay());
	date = SFTime(y, m, d,
								date.GetHour(), 
								date.GetMinute(), 
								0);
	ASSERT(date.IsValid());
}

inline void SetHour(SFTime& date, SFInt32 val)
{
	ASSERT(date.IsValid());
	date = SFTime(date.GetYear(),
								date.GetMonth(), 
								date.GetDay(), 
								MIN(23, MAX(0, val)), 
								date.GetMinute(), 
								0);
	ASSERT(date.IsValid());
}

inline void SetMinute(SFTime& date, SFInt32 val)
{
	ASSERT(date.IsValid());
	date = SFTime(date.GetYear(),
								date.GetMonth(), 
								date.GetDay(), 
								date.GetHour(),
								MIN(59, MAX(0, val)), 
								0);
	ASSERT(date.IsValid());
}

extern void SetDate           (SFTime& date, const SFString& dateStr);
extern void SetTime           (SFTime& date, const SFString& timeStr);
extern void SetTimeAndDate    (SFTime& date, const SFString& valueStr);

inline SFTime addDays(const SFTime& date, SFInt32 nDays)
{
	int i=0;

	SFTime ret = date;
	if (nDays>0)
	{
		for (i=0;i<nDays;i++)
			ret = AddOneDay(ret);
		return ret;
	} else if (nDays<0)
	{
		for (i=0;i<labs(nDays);i++)
			ret = SubtractOneDay(ret);
		return ret;
	}
	return date;
}

inline SFTime addWeeks(const SFTime& date, SFInt32 nWeeks)
{
	if (nWeeks == 0)
	{
		return date;

	} else if (labs(nWeeks) == 1)
	{
		SFInt32 day   = date.GetDay();
		SFInt32 month = date.GetMonth();
		SFInt32 year  = date.GetYear();
		SFInt32 nDays = DaysInMonth(SFTime(year,month,1,0,0,0));
		if (nWeeks<0)
		{
			day -= 7;
			if (day < 1)
			{
				month--;
				if (month < 1)
				{
					year--;
					month = 12;
				}
				nDays = DaysInMonth(SFTime(year,month,1,0,0,0));
				day += nDays;
			}
		} else
		{
			day += 7;
			if (day > nDays)
			{
				month++;
				if (month > 12)
				{
					year++;
					month = 1;
				}
				day -= nDays;
			}
		}
		return SFTime(year,month,day,date.GetHour(),date.GetMinute(),date.GetSecond()); 
	} else
	{
		// multiples
		SFTime ret = date;
		int i=0;
		for (i=0;i<labs(nWeeks);i++)
			if (nWeeks>0)
				ret = addWeeks(ret, 1);
			else
				ret = addWeeks(ret, -1);
		return ret;
	}
}

inline SFTime addMonths(const SFTime& date, SFInt32 nMonths)
{
	if (labs(nMonths) == 1)
	{
		SFInt32 day   = date.GetDay();
		SFInt32 month = date.GetMonth();
		SFInt32 year  = date.GetYear();

		if (nMonths > 0)
		{
			month++;
			if (month > 12)
			{
				year++;
				month = 1;
			}
		} else
		{
			month--;
			if (month < 1)
			{
				year--;
				month = 12;
			}
		}
		// go to last day of month if there is no such day
		SFInt32 nDays = DaysInMonth(SFTime(year,month,1,0,0,0));
		day = MIN(nDays, day);
		return SFTime(year,month,day,date.GetHour(),date.GetMinute(),date.GetSecond()); 
	} else
	{
		// multiples
		SFTime ret = date;
		int i=0;
		for (i=0;i<labs(nMonths);i++)
			if (nMonths>0)
				ret = addMonths(ret, 1);
			else
				ret = addMonths(ret, -1);
		return ret;
	}
}

inline SFTime addYears(const SFTime& date, SFInt32 nYears)
{
	SFInt32 day   = date.GetDay();
	SFInt32 month = date.GetMonth();
	SFInt32 year  = date.GetYear()+nYears;

	// go to last day of month if there is no such day (feb 29 for example)
	long nDays = DaysInMonth(SFTime(year,month,1,0,0,0));
	day = MIN(nDays, day);

	return SFTime(year,month,day,date.GetHour(),date.GetMinute(),date.GetSecond()); 
}

inline SFTime makeSameTime(const SFTime& date, const SFTime& time)
{
	return SFTime(date.GetYear(), date.GetMonth(), date.GetDay(),
								time.GetHour(), time.GetMinute(), time.GetSecond());
}

inline SFTime makeSameDay(const SFTime& date, long origDay)
{
	// adjust a date to reference the given origDay
	SFTime newDate = date;
	long day = newDate.GetDay();
	if (origDay == -1 || origDay == day)
		return newDate;
	ASSERT(origDay>day);
	day = MIN(origDay, DaysInMonth(newDate));
	return SFTime(newDate.GetYear(), 
				newDate.GetMonth(), 
				day, 
				newDate.GetHour(), 
				newDate.GetMinute(), 
				newDate.GetSecond());
}

inline SFBool isSameDay(const SFTime& d1, const SFTime& d2)
{
	// If the end date is midnight of the start date then assume
	// that these two dates are the same date even though technically
	// their day is different
	if (d2 == BOND(d1))
		return TRUE;

	return ((d1.GetYear()  == d2.GetYear()) &&
			(d1.GetMonth() == d2.GetMonth()) &&
			(d1.GetDay()   == d2.GetDay()));
}

inline SFBool isSameTime(const SFTime& t1, const SFTime& t2)
{
	return ((t1.GetHour()   == t2.GetHour()) &&
					(t1.GetMinute() == t2.GetMinute()) &&
					(t1.GetSecond() == t2.GetSecond()));
}

inline SFBool isSameMerid(const SFTime& d1, const SFTime& d2)
{
	if (d1.GetHour() < 12 && d2.GetHour() < 12)
		return TRUE;
	if (d1.GetHour() > 11 && d2.GetHour() > 11)
		return TRUE;
	return FALSE;
}

inline SFTime firstOccurenceOf(SFInt32 weekday, const SFTime& month)
{
	SFInt32 firstDay = BOM(month).GetDayOfWeek();
	SFInt32 day = weekday - firstDay + 2;
	if (day < 1)
		day += 7;
	ASSERT(day>0&&day<8);
	return SFTime(month.GetYear(), month.GetMonth(), day, month.GetHour(), month.GetMinute(), month.GetSecond());
}

inline SFBool isInRange(const SFTime& ref, const SFTime& start, const SFTime& end)
{
	return (start <= ref && end >= ref);
}

inline SFBool isWeekend(const SFTime& date)
{
	return date.GetDayOfWeek()==1 || date.GetDayOfWeek()==7;
}

inline SFBool isSunday(const SFTime& date)
{
        return (date.GetDayOfWeek()==1);
}

inline SFTime earlierOf(const SFTime& one, const SFTime& two)
{
	if (one < two)
		return one;
	else if (two < one)
		return two;
	return one;
}

inline SFTime laterOf(const SFTime& one, const SFTime& two)
{
	if (one > two)
		return one;
	else if (two > one)
		return two;
	return one;
}

inline SFBool periodsOverlap(
	const SFTime& eStart, const SFTime& eStop, 
	const SFTime& cStart, const SFTime& cStop)
{
	if (eStart > cStop)
		return FALSE;
	if (eStop < cStart)
		return FALSE;
	return TRUE;
}

inline SFInt32 getCentury(SFInt32 year)
{
	return ((year / 100) * 100);
}

inline SFInt32 get2Digit(SFInt32 year)
{
	return year - getCentury(year);
}

//-------------------------------------------------------------------------
inline SFTime snapToStart(const SFTime& start, const SFTime& date, SFInt32 perLen)
{
	if (date <= start)
		return date;

	SFTime d = date;
	SFTime s = start;
	
	SFTimeSpan span = d - s;
	SFInt32 mins = SFInt32(span.getSeconds_double() / 60.);
	SFInt32 pers = mins / perLen;
	return start + SFTimeSpan(0, 0, pers * perLen, 0);
}

extern SFTime   GetDay      (const SFTime& qDate, SFInt32 row, SFInt32 col);
extern SFInt32  GetDayText  (const SFTime& qDate, SFString& text, SFInt32 row, SFInt32 col);
extern SFBool   isToday     (const SFTime& qDate, SFInt32 row, SFInt32 col);
extern SFBool   isToday     (const SFTime& qDate, const SFTime& nDate);

extern SFString getFormatedDateString (const SFTime& date, SFInt32 vt);
extern SFString getViewTypeString     (SFInt32 vt);
extern SFBool   isMenuShowing1        (const SFString& val, SFInt32 period, SFInt32 type);
extern SFBool   isMenuShowing2        (const SFString& val, SFInt32 viewType);

//---------------------------------------------------------------------------------------
inline SFString getMonthName(const SFTime& date, SFBool yearIfDiff=FALSE)
{
	SFString monthName = date.Format(FMT_MONTHNAME);
	if (yearIfDiff && date.GetYear() != Now().GetYear())
		monthName += (SPACER + "'" + asString(date.GetYear()).Right(2));
	return monthName;
}

//---------------------------------------------------------------------------------------
inline SFString getDayNames(SFInt32 len=2)
{
	SFString dayNames;
	SFTime date1 = BOW(Now()); // any week will do
	for (int i=0;i<7;i++)
	{
		dayNames += (date1.Format(FMT_DAYNAME).Left(len) + " ");
		date1 = IncrementDate(VT_DAYLIST, date1);
	}
	return dayNames;
}

//---------------------------------------------------------------------------------------
inline SFInt32 isThisWeek(const SFTime& qDate, const SFTime& refDate, SFInt32 row)
{
	if (qDate.GetMonth() != refDate.GetMonth())
		return FALSE;
	return (BOW(GetDay(refDate, row, 1)) == BOW(qDate));
}

//---------------------------------------------------------------------------------------
inline SFString displayTime1(const SFTime& tm)
{
	SFString time = tm.Format(FMT_TIME);
	ASSERT(time == toLower(time));
	return "<font size=-1>" + SPACER + time.Left(time.GetLength()-1) + SPACER + "</font>";
}

#define stripSeconds(a) SFTime(a.GetYear(), a.GetMonth(), a.GetDay(), a.GetHour(), a.GetMinute(), 0)

//---------------------------------------------------------------------------------------------
extern SFTime snagDate(const SFString& str, const SFTime& def=Now());
//extern SFTime newestFileDateInFolder(const SFString& path, SFString *whichFilePtr, const SFString& ignore);

#endif
