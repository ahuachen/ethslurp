#ifndef _DOUBLERECT_H
#define _DOUBLERECT_H
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
#include "point.h"
//-----------------------------------------------------------------------------
#ifndef __POL_H__
class CRect;
#endif

class CDoubleRect
{
public:
  double        left;
  double        top;
  double        right;
  double        bottom;

								CDoubleRect    (void);
								CDoubleRect    (const CDoubleRect& rect);
								CDoubleRect    (const CRect& rect);
								CDoubleRect    (double l, double t, double r, double b);
								CDoubleRect    (const CDoublePoint& pt1, const CDoublePoint& pt2);

							 ~CDoubleRect    (void) {};

  CDoubleRect&  operator=      (const CDoubleRect& pt);

	SFBool        operator==     (const CDoubleRect& rect) const;
	SFBool        operator!=     (const CDoubleRect& rect) const;

  void          operator+=     (const CDoublePoint& pt);
  void          operator-=     (const CDoublePoint& pt);
  void          operator/=     (const CDoublePoint& pt);
  void          operator*=     (const CDoublePoint& pt);

  void          InflateRect    (double x, double y);

	double        Width          (void) const;
	double        Height         (void) const;
  CDoublePoint  Size           (void) const;

  CDoublePoint  Center         (void) const;

	void          operator |=    (const CDoubleRect& rect);
                operator CRect (void) const;
};

//-----------------------------------------------------------------------------
inline CDoubleRect::CDoubleRect(void)
{
	// random values
}

inline CDoubleRect::CDoubleRect(const CDoubleRect& rect)
{
	left   = rect.left;
	top    = rect.top;
	right  = rect.right;
	bottom = rect.bottom;
}

inline CDoubleRect::CDoubleRect(double l, double t, double r, double b)
{
	left   = l;
	top    = t;
	right  = r;
	bottom = b;
}

inline CDoubleRect::CDoubleRect(const CDoublePoint& pt1, const CDoublePoint& pt2)
{
	left   = pt1.x;
	top    = pt1.y;
	right  = pt2.x;
	bottom = pt2.y;
}

//-----------------------------------------------------------------------------
inline CDoubleRect& CDoubleRect::operator=(const CDoubleRect& rect)
{
	left   = rect.left;
	top    = rect.top;
	right  = rect.right;
	bottom = rect.bottom;

	return *this;
}

inline double RoundInch(double t1, double t2)
{
  long tt1 = (long)(t1 * 100000.);
  long tt2 = (long)(t2 * 100000.);

  long m = tt1 / tt2;
  return (m * tt2) / 100000.;
}

inline SFBool CDoubleRect::operator==(const CDoubleRect& rect) const
{
	if (RoundInch(left,   0.001) != RoundInch(rect.left, 0.001))   return FALSE;
	if (RoundInch(top,    0.001) != RoundInch(rect.top, 0.001))    return FALSE;
	if (RoundInch(right,  0.001) != RoundInch(rect.right, 0.001))  return FALSE;
	if (RoundInch(bottom, 0.001) != RoundInch(rect.bottom, 0.001)) return FALSE;
	return TRUE;
}

inline SFBool CDoubleRect::operator!=(const CDoubleRect& rect) const
{
	return (SFBool)((!(*this == rect)));
}

inline void CDoubleRect::operator+=(const CDoublePoint& pt)
{
	left   += pt.x;
	right  += pt.x;
	top    += pt.y;
	bottom += pt.y;
}

inline void CDoubleRect::operator-=(const CDoublePoint& pt)
{
	left   -= pt.x;
	right  -= pt.x;
	top    -= pt.y;
	bottom -= pt.y;
}

inline void CDoubleRect::operator/=(const CDoublePoint& pt)
{
	left   /= pt.x;
	right  /= pt.x;
	top    /= pt.y;
	bottom /= pt.y;
}

inline void CDoubleRect::operator*=(const CDoublePoint& pt)
{
	left   *= pt.x;
	right  *= pt.x;
	top    *= pt.y;
	bottom *= pt.y;
}

//-----------------------------------------------------------------------------
inline double CDoubleRect::Width(void) const
{
	return (right-left);
}

inline double CDoubleRect::Height(void) const
{
	return (bottom-top);
}

inline CDoublePoint CDoubleRect::Size(void) const
{
	return CDoublePoint((right-left), (bottom-top));
}

inline CDoublePoint CDoubleRect::Center(void) const
{
  CDoublePoint center = CDoublePoint(left+right, top+bottom);
  center = CDoublePoint(center.x / 2.0, center.y / 2.0);
  return center;
}

inline void CDoubleRect::InflateRect(double x, double y)
{
  left -= x;
  right += x;
  top -= y;
  bottom += y;
}

#define POS_H_T  11
#define POS_H_B  12
#define POS_H_L  13
#define POS_H_R  14

inline CDoubleRect Half(const CDoubleRect& rect, SFInt32 which=0)
{
	CDoublePoint half(rect.Width() / 2., rect.Height() / 2.);
	switch (which)
	{
	case POS_H_T: return CDoubleRect(rect.left,          rect.top,          rect.right,         rect.top + half.y);
	case POS_H_B: return CDoubleRect(rect.left,          rect.top + half.y, rect.right,         rect.bottom);
	case POS_H_L: return CDoubleRect(rect.left,          rect.top,          rect.left + half.x, rect.bottom);
	case POS_H_R: return CDoubleRect(rect.left + half.x, rect.top,          rect.right,         rect.bottom);
	}

	ASSERT(0);
	return rect;
}

#define POS_Q_TL 7
#define POS_Q_TR 8
#define POS_Q_BL 9
#define POS_Q_BR 10
inline CDoubleRect Quarter(const CDoubleRect& r, SFInt32 which=0)
{
  switch (which)
	{
	case POS_Q_TL: return Half(Half(r, POS_H_T), POS_H_L);
	case POS_Q_TR: return Half(Half(r, POS_H_T), POS_H_R);
	case POS_Q_BL: return Half(Half(r, POS_H_B), POS_H_L);
	case POS_Q_BR: return Half(Half(r, POS_H_B), POS_H_R);
	}
	ASSERT(0);
	return r;
}

#define POS_C_L  0
#define POS_C_M  1
#define POS_C_S  2
#define POS_C_LF 3
#define POS_C_TP 4
#define POS_C_RT 5
#define POS_C_BT 6
inline void CalcLocation(SFInt32 pos, CDoubleRect& innerShape, CDoublePoint& innerTrans)
{
	switch (pos)
	{
	case POS_C_L: // center large
		{
	//					innerShape.right  -= 0.25;
	//					innerShape.bottom -= 0.25;
	//					innerTrans += CDoublePoint(0.125, 0.125); //pt.x/2., pt.y/2.);
		}
		break;
	case POS_C_M: // center medium
		{
			CDoublePoint pt(innerShape.Width(), innerShape.Height());
			pt *= .3;
			innerShape.right  -= pt.x;
			innerShape.bottom -= pt.y;
			innerTrans += CDoublePoint(pt.x/2., pt.y/2.);
		}
		break;
	case POS_C_S: // center small
		{
			innerShape = Quarter(innerShape, POS_Q_BR);
			CDoublePoint trans(innerShape.left/2, innerShape.top/2);
			innerShape -= trans;
			innerTrans += trans;
		}
		break;
	case POS_C_LF: // left center
		{
			innerShape = Quarter(innerShape, POS_Q_BR);
			CDoublePoint trans(0, innerShape.top/2);
			innerShape -= trans;
			innerTrans += trans;
		}
		break;
	case POS_C_TP: // top center
		{
			innerShape = Quarter(innerShape, POS_Q_BR);
			CDoublePoint trans(innerShape.left/2, 0);
			innerShape -= trans;
			innerTrans += trans;
		}
		break;
	case POS_C_RT: // right center
		{
			innerShape = Quarter(innerShape, POS_Q_BR);
			CDoublePoint trans(innerShape.left, innerShape.top/2);
			innerShape -= trans;
			innerTrans += trans;
		}
		break;
	case POS_C_BT: // bottom center
		{
			innerShape = Quarter(innerShape, POS_Q_BR);
			CDoublePoint trans(innerShape.left/2, innerShape.top);
			innerShape -= trans;
			innerTrans += trans;
		}
		break;
	case POS_Q_TL: // top left quarter
	case POS_Q_TR: // top right quarter
	case POS_Q_BL: // bottom left quarter
	case POS_Q_BR: // bottom right quarter

		innerShape = Quarter(innerShape, pos);
		break;

	case POS_H_T: // top half
	case POS_H_B: // bottom half
	case POS_H_L: // left half
	case POS_H_R: // right half

		innerShape = Half(innerShape, pos);
		break;
	default:
		ASSERT(0);
	}
}

inline CDoublePoint CalcScale(const CDoubleRect& rect1, const CDoubleRect& rect2)
{
  // Bigger one first returns numbers greater than zero
  CDoublePoint size1 = rect1.Size();
  CDoublePoint size2 = rect2.Size();

  return CDoublePoint(size1.x/size2.x, size1.y/size2.y);
}

extern CDoubleRect ShapeFromPercents(const CDoubleRect base, const CDoubleRect& pcts);

inline CDoubleRect SquareIt(const CDoubleRect& r, SFBool largest=TRUE)
{
	CDoubleRect rect = r;
	double size = MAX(r.Width(), r.Height());
	if (!largest)
		size = MIN(r.Width(), r.Height());
	rect.right = rect.left + size;
	rect.bottom = rect.top + size;
	return rect;
}

#endif
