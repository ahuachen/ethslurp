#ifndef _DATETYPES_H
#define _DATETYPES_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

enum MONTH {
	JANUARY=1,
	FEBRUARY,
	MARCH,
	APRIL,
	MAY,
	JUNE,
	JULY,
	AUGUST,
	SEPTEMBER,
	OCTOBER,
	NOVEMBER,
	DECEMBER
};

enum DayOfWeek {
	SUNDAY=1,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY
};

struct SFDateStruct
{
  SFInt32     m_Year;
  SFInt32     m_Month;
  SFInt32     m_Day;
  SFInt32     m_Hour;
  SFInt32     m_Minute;
  SFInt32     m_Second;
};

class SFTime;
#endif
