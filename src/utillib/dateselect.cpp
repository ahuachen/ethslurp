/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "dateselect.h"
#include "string_table.h"

//--------------------------------------------------------------------------------------
SFBool hourChoose12(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	nVals = 12;
	if (!attrs)
		return TRUE;
		
	for (int i=1;i<=12;i++)
		attrs[i-1].set(asString(i), asString(i));

	return TRUE;
}

//--------------------------------------------------------------------------------------
SFBool hourChoose24(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	nVals = 24;
	if (!attrs)
		return TRUE;
		
	ASSERT(data);
	for (int i=1;i<=24;i++)
		attrs[i-1].set(asString(i - data->extraInt), asString(i-1));

	return TRUE;
}

//--------------------------------------------------------------------------------------
SFBool hourChoose24_24(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	data->extraInt = TRUE;
	return hourChoose24(nVals, attrs, data);
}

//--------------------------------------------------------------------------------------
SFBool minChoose(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	SFInt32 step = 5;
	if (data->dataVal)
		step = *(SFInt32*)data->dataVal;

	SFInt32 start = 0;
	if (data->extraInt)
	{
		step  = 5;
		start = 5;
	}

	nVals = ((60-start)/step);
	if (!attrs)
		return TRUE;
		
	for (SFInt32 i=start;i<60;i+=step)
		attrs[(i-start)/step].set(padLeft(asString(i), 2, '0'), asString(i));

	return TRUE;
}

//--------------------------------------------------------------------------------------
SFBool ampmChoose(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	nVals = 2;
	if (!attrs)
		return TRUE;
		
	ASSERT(data && data->dataVal);
	SFString vals = *(SFString*)data->dataVal;
	ASSERT(vals.Contains("/"));

	attrs[0].set(nextTokenClear(vals, '/'), "0");
	attrs[1].set(nextTokenClear(vals, '/'), "1");

	return TRUE;
}

//--------------------------------------------------------------------------------------
SFBool directionChoose(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	nVals = 2;
	if (!attrs)
		return TRUE;
	attrs[0].set("before",  asString(FALSE));
	attrs[1].set("after",   asString(TRUE) );
	return TRUE;
}

//--------------------------------------------------------------------------------------
SFBool isMenuShowing1(const SFString& val, SFInt32 period, SFInt32 type)
{
	SFInt32 enables = strtoul(((const char *)val), NULL, 16);

	// Just some double checks against changing these since they now become dependant on their values
	ASSERT(VT_DAYTYPE     == VT_WEEKTYPE-1);
	ASSERT(VT_WEEKTYPE    == VT_MONTHTYPE-1);
	ASSERT(VT_MONTHTYPE   == VT_YEARTYPE-1);
	ASSERT(VT_GRIDTYPE    == VT_LISTTYPE-1);
	ASSERT(VT_LISTTYPE    == VT_PLANNERTYPE-1);
	ASSERT(VT_PLANNERTYPE == VT_REPORTTYPE-1);

	SFInt32 pShift = period - VT_DAYTYPE;
	SFInt32 tShift = type - VT_GRIDTYPE;
	SFInt32 nibble = ((enables >> (tShift*4)) & 0xf);

	return ((nibble >> pShift) & 0x1);
}

//--------------------------------------------------------------------------------------
SFBool isMenuShowing2(const SFString& val, SFInt32 vt)
{
	return isMenuShowing1(val, getPeriodType(vt), getDisplayType(vt));
}
