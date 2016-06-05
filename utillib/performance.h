#ifndef __TIME_H3D
#define __TIME_H3D
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
#include <sys/timeb.h>

/*----------------------------------------------------------------------
CLASS
	CPerformanceClock

	For doing time calculations.

DESCRIPTION
	<ul>
	The CPerformanceClock class manages the time.  This is used mostly
	by the vrBrowser class to initiate vrTimeSensor events.
	</ul>

NOTES
	<ul>
	</ul>

EXAMPLE CODE
	<pre>
	// This class allows calculations on time values:
	CPerformanceClock time1 = vrNow();

	// wait around a while....

	CPerformanceClock time2 = vrNow();

	ASSERT(time2 > time1); // comparison
	ASSERT((time2-time1) > 0.0); // subtraction, convert to double
	</pre>

MEMBERS
----------------------------------------------------------------------*/
class CPerformanceClock
{
protected:
  //<doc>------------------------------------------------------------
  // <dd>Number of seconds since 12:00am January 1, 1970.
	// [default:] 0
	//
	long m_Seconds;

  //<doc>------------------------------------------------------------
  // <dd>Millionths of a second (only acurate to 1,000 of a second).
	// [default:] 0
	//
	long m_uSeconds;

public:

  //<doc>------------------------------------------------------------
  // <dd>Default constructor.
	//
	CPerformanceClock(void)
		{
			m_Seconds  = 0;
			m_uSeconds = 0;
		}

  //<doc>------------------------------------------------------------
  // <dd>Copy constructor.
	//
	CPerformanceClock(const CPerformanceClock& t)
		{
			m_Seconds  = t.m_Seconds;
			m_uSeconds = t.m_uSeconds;
		}

  //<doc>------------------------------------------------------------
  // <dd>Constructor.
	//
	// [in] secs: Number of seconds since 1/1/1970.
	// [in] usecs: Number of milliseconds.
	//
	CPerformanceClock(long secs, long usecs)
		{
			m_Seconds  = secs;
			m_uSeconds = usecs;
		}

  //<nodoc>------------------------------------------------------------
  // <dd>Constructor.
	//
	// [in] secs: Number of seconds since 1/1/1970.
	//
//	CPerformanceClock(double secs)
//		{
//			m_Seconds  = secs;
//			m_uSeconds = 0;
//		}

  //<doc>------------------------------------------------------------
  // <dd>Equals operator.
	//
	CPerformanceClock& operator=(const CPerformanceClock& t)
		{
			m_Seconds  = t.m_Seconds;
			m_uSeconds = t.m_uSeconds;
			return *this;
		}

  //<doc>------------------------------------------------------------
  // <dd>Equals operator.
	//
	CPerformanceClock& operator=(double t)
		{
			m_Seconds  = (long)t; // truncate
			m_uSeconds = (long)((t - m_Seconds) / 1000000.0);
			return *this;
		}

  //<doc>------------------------------------------------------------
  // <dd>Adds two CPerformanceClock values together.
	//
	// [in] t1: The first time.
	// [in] t2: The second time.
	//
	friend CPerformanceClock operator+(const CPerformanceClock& t1, const CPerformanceClock& t2)
	{
		CPerformanceClock t;
		t.m_uSeconds = t1.m_uSeconds + t2.m_uSeconds;

		if(t.m_uSeconds >= 1000000)
		{
			t.m_uSeconds -= 1000000;
			t.m_Seconds = t1.m_Seconds + t2.m_Seconds + 1;
		} else
		{
			t.m_Seconds = t1.m_Seconds + t2.m_Seconds;
			if((t.m_Seconds >= 1) && ((t.m_uSeconds < 0)))
			{
				t.m_Seconds--;
				t.m_uSeconds += 1000000;
			}
		}
		return t;
	}

  //<doc>------------------------------------------------------------
  // <dd>Subtracts two CPerformanceClock values.
	//
	// [in] t1: The first time.
	// [in] t2: The second time.
	//
	friend CPerformanceClock operator-(const CPerformanceClock& t1, const CPerformanceClock& t2)
	{
		CPerformanceClock t;
		t.m_uSeconds = t1.m_uSeconds - t2.m_uSeconds;

		if(t.m_uSeconds < 0)
		{
			t.m_uSeconds += 1000000;
			t.m_Seconds = t1.m_Seconds - t2.m_Seconds - 1;
		} else
		{
			 t.m_Seconds = t1.m_Seconds - t2.m_Seconds;
		}
		return t;
	}

  //<doc>------------------------------------------------------------
  // <dd>Casts a CPerformanceClock value to a double.
	//
	operator double(void) const
	{
		return ((double)m_Seconds + ((double)m_uSeconds / 1000000.0));
	}

  //<doc>------------------------------------------------------------
  // <dd>Returns TRUE if this object is earlier than the query time q.
	//
	// [in] q: The query time.
	//
	SFBool operator>(const CPerformanceClock& q) const
	{
		return ((m_Seconds > q.m_Seconds) ||
						((m_Seconds == q.m_Seconds) && (m_uSeconds > q.m_uSeconds)));
	}

  //<doc>------------------------------------------------------------
  // <dd>Returns TRUE if this object is later than the query time q.
	//
	// [in] q: The query time.
	//
	SFBool operator<(const CPerformanceClock& q) const
	{
		return ((m_Seconds < q.m_Seconds) ||
						((m_Seconds == q.m_Seconds) && (m_uSeconds < q.m_uSeconds)));
	}

  //<doc>------------------------------------------------------------
  // <dd>Returns TRUE if this object is earlier than or equal to the query time q.
	//
	// [in] q: The query time.
	//
	SFBool operator>=(const CPerformanceClock& q) const
		{
			return ((m_Seconds >= q.m_Seconds) ||
							((m_Seconds == q.m_Seconds) && (m_uSeconds >= q.m_uSeconds)));
		}

  //<doc>------------------------------------------------------------
  // <dd>Returns TRUE if this object is later than or equal to the query time q.
	//
	// [in] q: The query time.
	//
	SFBool operator<=(const CPerformanceClock& q) const
		{
			return ((m_Seconds <= q.m_Seconds) ||
							((m_Seconds == q.m_Seconds) && (m_uSeconds <= q.m_uSeconds)));
		}

  //<doc>------------------------------------------------------------
  // <dd>Equality operator.
	//
	// [in] q: The query time.
	//
	SFBool operator==(const CPerformanceClock& q)
		{
			return ((m_Seconds == q.m_Seconds) &&
							(m_uSeconds == q.m_uSeconds));
		}

  //<doc>------------------------------------------------------------
  // <dd>Inequality operator.
	//
	// [in] q: The query time.
	//
	SFBool operator!=(const CPerformanceClock& q)
		{
			return !operator==(q);
		}

  //<doc>------------------------------------------------------------
  // <dd>Current time of day (in seconds since 12:00am January 1, 1970).
	//
	// <dd><B><I>Note:</B></I> Syntactic sugar for CPerformanceClock::Now is <B>vrNow</B> macro.
	static CPerformanceClock Now(void)
		{
			struct timeb _t;
			ftime(&_t);
			return CPerformanceClock((long)_t.time, _t.millitm * 1000);
		}
};

#define vrNow CPerformanceClock::Now
//#define SFTime CPerformanceClock
//typedef double SFTime;

#endif
