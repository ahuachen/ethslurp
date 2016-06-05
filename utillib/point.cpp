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
