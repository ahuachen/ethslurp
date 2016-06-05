#ifndef _DOUBLEPOINT_H
#define _DOUBLEPOINT_H
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
#ifndef __POL_H__
class CPoint;
#endif

class CDoublePoint
{
public:
  double        x;
  double        y;

public:
								CDoublePoint    (void);
                CDoublePoint    (const CDoublePoint& pt);
                CDoublePoint    (const CPoint& pt);
                CDoublePoint    (double xx, double yy);

               ~CDoublePoint    (void) {};

  CDoublePoint& operator=       (const CDoublePoint& pt);

	SFBool        operator==      (const CDoublePoint& pt) const;
	SFBool        operator!=      (const CDoublePoint& pt) const;

	void					operator+=      (const CDoublePoint& pt);
	void          operator-=      (const CDoublePoint& pt);
	void          operator/=      (double s);
	void					operator*=      (const CDoublePoint& pt);
	void					operator*=      (double s);

	void          Offset          (const CDoublePoint& pt) { operator+=(pt); }

  CDoublePoint  operator+       (void) const;
  CDoublePoint  operator-       (void) const;

	              operator CPoint (void) const;

  SFString      ToString        (void) const;

	friend CDoublePoint operator* (const CDoublePoint& A, const CDoublePoint& B);
};

//-----------------------------------------------------------------------------
inline CDoublePoint::CDoublePoint(void)
{
	// random values
}

inline CDoublePoint::CDoublePoint(const CDoublePoint& pt)
{
	x = pt.x;
	y = pt.y;
}

inline CDoublePoint::CDoublePoint(double xx, double yy)
{
	x = xx;
	y = yy;
}

//-----------------------------------------------------------------------------
inline CDoublePoint& CDoublePoint::operator=(const CDoublePoint& pt)
{
	x = pt.x;
	y = pt.y;
	return *this;
}

inline SFBool CDoublePoint::operator==(const CDoublePoint& pt) const
{
	return (SFBool)((x == pt.x && y == pt.y));
}

inline SFBool CDoublePoint::operator!=(const CDoublePoint& pt) const
{
	return (SFBool)((!(*this == pt)));
}

inline void CDoublePoint::operator+=(const CDoublePoint& pt)
{
	x += pt.x;
	y += pt.y;
}

inline void CDoublePoint::operator-=(const CDoublePoint& pt)
{
	x -= pt.x;
	y -= pt.y;
}

inline void CDoublePoint::operator*=(const CDoublePoint& pt)
{
	x *= pt.x;
	y *= pt.y;
}

inline void CDoublePoint::operator/=(double s)
{
	x /= s;
	y /= s;
}

inline void CDoublePoint::operator*=(double s)
{
	x *= s;
	y *= s;
}

inline CDoublePoint operator* (const CDoublePoint& A, const CDoublePoint& B)
{
  return CDoublePoint(A.x*B.x, A.y*B.y);
}

inline CDoublePoint CDoublePoint::operator+ () const
{
  return *this;
}

inline CDoublePoint CDoublePoint::operator- () const
{
  return CDoublePoint(-x, -y);
}

extern CDoublePoint RotatePoint(const CDoublePoint& pt, double angle, const CDoublePoint& center, double fac);

inline CDoublePoint Lerp(const CDoublePoint& pt1, const CDoublePoint& pt2, double ratio)
{
	CDoublePoint start = pt1;
	CDoublePoint diff = pt2;
	diff -= pt1;
	diff *= ratio;

	start += diff;

	return start;
}

#endif
