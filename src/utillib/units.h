#ifndef _H_UNITS
#define _H_UNITS
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

enum { INCHES,
				CENTIMETERS,
				PIXELS,
				PNTS,
        PRINTER,
				SCREEN };

SFString GetUnitsString(long units, double val, long den);

#endif
