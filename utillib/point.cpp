/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "point.h"

SFString CDoublePoint::ToString(void) const
{
  SFString str;
  str.Format("%3.4f %3.4f\n", x, y);
  return str;
}

CDoublePoint RotatePoint(const CDoublePoint& pt, double angle, const CDoublePoint& center, double fac)
{
  if (angle == 0.0)
    return pt;
  
  CDoublePoint pt1 = pt;
  
  // Move the point to the origin
  //pt1.Offset(-center);
  
  // Rotate the point about the origin
  double radians = (double)(angle*PI)/180.0;
  double cosine = cos(radians);
  double sine   = sin(radians);
  double x, y;

  double ffxadd;
  double ffyadd;

  ffxadd = center.x + (-center.x * cosine) - (center.y * sine);
  ffyadd = center.y + ( center.x * sine  ) - (center.y * cosine);

  x = ((double)pt1.x * cosine * fac + (double)pt1.y * sine   + ffxadd) / fac;
  y =  (double)pt1.x * -sine  * fac + (double)pt1.y * cosine + ffyadd;

  // Move it back
  pt1 = CDoublePoint((long)x, (long)y);
  //pt1.Offset(center);
  
  return pt1;
}

