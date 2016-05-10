#ifndef __SFDATE_H__
#define __SFDATE_H__
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "datetypes.h"

class SFDate
{
private:
	// Count of days since 15 October 1582 (start of Gregorian Calendar) 
	SFUint32   m_nDays;

public:
						 SFDate                 (void);
						 SFDate                 (const SFDate& dt);

private:
// Everything is private because on the SFTime class (which is exposed to end users and
// is a 'friend' can use this class
						 SFDate                 (SFInt32 year, SFInt32 month, SFInt32 day);
						 SFDate                 (SFInt32 year, SFInt32 month, SFInt32 weekInMonth, SFInt32 dayOfWeek); 
						 SFDate                 (SFInt32 days);
						 SFDate                 (const SF_TIMESTRUCT& sysTime);
						 SFDate                 (const SFString& dateStr, const SFString& fmtStr);

//				  ~SFDate                 (void);

  SFDate&    operator=              (const SFDate& date);

  SFInt32    GetYear                (void) const;
  SFInt32    GetMonth               (void) const;
  SFInt32    GetDay                 (void) const;
  SFInt32    GetTotalDays           (void) const;

  SFDate		 operator+              (SFInt32 days) const;
  SFDate     operator-              (SFInt32 days) const;
  SFInt32    operator-              (const SFDate& date) const;

  SFDate&    operator+=             (SFInt32 days);
  SFDate&    operator-=             (SFInt32 days);

  SFBool     operator==             (const SFDate& date) const;
  SFBool     operator!=             (const SFDate& date) const;

  SFBool     operator>              (const SFDate& date) const;
  SFBool     operator<              (const SFDate& date) const;

  SFBool     operator>=             (const SFDate& date) const;
  SFBool     operator<=             (const SFDate& date) const;

  SFString   Format                 (const SFString& fmt=nullString) const;
  
  SFBool     IsValid                (void) const;          
                                        
	SFDate&      setValues              (SFInt32 y, SFInt32 m, SFInt32 date);
  SFDateStruct getDateStruct          (void) const;

	friend class SFTime;
	friend SFInt32 getWeekOfYear (const SFTime& date);
	friend SFInt32 getWeekOfMonth(const SFDate& date);
	friend SFInt32 getDayOfWeek  (const SFDate& date);
	friend void    SetDate       (SFTime& date, const SFString& dStr);
};

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFInt32 SFDate::GetDay() const
{
  ASSERT(IsValid());
  return getDateStruct().m_Day;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFInt32 SFDate::GetMonth() const
{
  ASSERT(IsValid());
  return getDateStruct().m_Month;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFInt32 SFDate::GetYear() const
{
  ASSERT(IsValid());
  return getDateStruct().m_Year;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFInt32 SFDate::GetTotalDays() const
{
	ASSERT(IsValid());
	return m_nDays;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool SFDate::IsValid() const
{
  return (m_nDays != (SFUint32)LONG_MIN);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFDate SFDate::operator+(SFInt32 days) const
{
  return SFDate((m_nDays - 2000000000L) + days);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFInt32 SFDate::operator-(const SFDate& date) const
{
  return m_nDays - date.m_nDays;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFDate SFDate::operator-(SFInt32 days) const
{
  return SFDate((m_nDays - 2000000000L) - days);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFDate& SFDate::operator+=(SFInt32 days)
{
  *this = *this + days;
  return *this;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFDate& SFDate::operator-=(SFInt32 days)
{
  *this = *this - days;
  return *this;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool SFDate::operator==(const SFDate& date) const
{
  return (m_nDays == date.m_nDays);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool SFDate::operator!=(const SFDate& date) const
{
  return !operator==(date);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool SFDate::operator>(const SFDate& date) const
{
  return m_nDays > date.m_nDays;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool SFDate::operator<(const SFDate& date) const
{
  return m_nDays < date.m_nDays;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool SFDate::operator>=(const SFDate& date) const
{
  return m_nDays >= date.m_nDays;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool SFDate::operator<=(const SFDate& date) const
{
  return m_nDays <= date.m_nDays;
}

#endif
