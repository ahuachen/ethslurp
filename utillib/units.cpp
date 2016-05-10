/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "units.h"

SFString GetUnitsString(long units, double val, long den)
{
	if (units==INCHES)
	{
		SFString str;
		long num = 0;

		SFString negStr("");
		if (val < 0.0)
		{
			val *= -1.0;
			negStr = "-";
		}
	
		double v = 0.0;
		double step  = 1.0 / (double)den;
		while (val > v)
		{
			if (val == v)
				break;
			v += step;
			num++;
		}
		// given val must be an even multiple of step size or fail!
		//ASSERT(v == val);
		// Factorization (assumes 'den' is even multiple of 2)
		while (den != 1 && num && !(num%2))
		{
			num = (num >> 1);
			den = (den >> 1);
		}

		SFString inchStr("    "), fracStr("");
		long inches=0;
		if (num >= den)
		{
			inches = (num / den); // assumes long will truncate
			num -= (inches * den);
			if (num == 0 || den == 1)
			{
				inchStr.Format("  % 2d", inches);
			} else
			{
				inchStr.Format("  % 2d - ", inches);
			}
		}
		// all that's left is a fraction less than 1
		if (num == 0)
		{
			if (!inches) fracStr.Format("%d/%d", num, den);
		} else if (den != 1)
		{
			fracStr.Format("%d/%d", num, den);
		} 
		str.Format("%s%s%s\"", (const char *)negStr, (const char *)inchStr, (const char *)fracStr);
		return str;
	} else
	{
		ASSERT(units==CENTIMETERS);
		SFString str;
		str.Format(" %3.3f %s", val, "cm");
		return str;
	}
}

