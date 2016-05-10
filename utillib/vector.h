#ifndef _VECTOR_H
#define _VECTOR_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "point.h"

class CVector : public CDoublePoint
{
private:
	CVector() {};

public:
	       CVector   (const CDoublePoint& pt1, const CDoublePoint& pt2);

	double Length    (void) const;
	double DotProd   (const CVector& B) const;
	void   Normalize (void);
	double Angle     (const CVector& v) const;
};

inline CVector::CVector(const CDoublePoint& pt1, const CDoublePoint& pt2)
{
	x = pt2.x - pt1.x;
	y = pt2.y - pt1.y;
}

inline double CVector::Length(void) const
{
  double a = x*x;
  double b = y*y;
  double c = sqrt(a+b);
	return c;
}
inline double CVector::DotProd(const CVector& B) const
{
  double a = x * B.x;
  double b = y * B.y;
  double c = a + b;
  return c;
}

inline void CVector::Normalize(void)
{
  double len = Length();
	if (len == 0.0)
		return;

	ASSERT(len != 0.0);

	x = x / len;
	y = y / len;

  return;
}

inline double CVector::Angle(const CVector& v) const
{
  double l1 = Length();
  double l2 = v.Length();
  double len = l1 * l2;

  if (len>0)
  {
    double dot = DotProd(v);
		double val = -acos(dot);

   	if (v.y > 0.0)
   	{
				return RAD2DEG(val);

		} else
		{
   		return -RAD2DEG(val);
		}

  }

  return 0.0;
}

#endif
