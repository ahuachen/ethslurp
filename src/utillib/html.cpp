/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "html.h"
#include "affil.h"
#include "cgilib.h"

//------------------------------------------------------------------------
SFString _aligns[] = { "left", "middle", "right", "top", "bottom" };

//------------------------------------------------------------------------
#ifdef TESTING
#define ADDMOUSEATTRIB()
#else
#define onMouseWheel SFString("return false;")
#define ADDMOUSEATTRIB() if (isMSIE()) { ADDATTRIB("onMouseWheel", onMouseWheel); }
#endif

//------------------------------------------------------------------------
CSelect::CSelect(CExportContext& ctx, CSelectionData *data)
: CElement(ctx)
{
	ASSERT(data && data->func);

	SFAttribute attribs[25];
	int n=0;
	ADDATTRIB      ("name",     data->name);
	ADDATTRIB      ("id",       data->name);
	ADDATTRIB      ("size",     asString(1,-1));
	ADDATTRIBSINGLE("disabled", data->disabled);
	ADDATTRIB      ("onChange", data->onChangeStr);
	ADDMOUSEATTRIB ();
	InitElement    ("select", -1, n, attribs);

	// how many?
	SFInt32 nVals = 0;
	if (data->func)
		data->func(nVals, NULL, data);

	if (nVals)
	{
		// get the name/value pairs
		SFAttribute *vals = new SFAttribute[nVals];
		data->func(nVals, vals, data);

		SFBool isIndexed = (data->selStr.IsEmpty() && (data->selInt >= 0 && data->selInt < nVals));

		// sort?
		if (data->sorted)
		{
			SFAttribute saved;
			if (isIndexed)
				saved = vals[data->selInt];

			// Leave the No Value entry alone
			if (nVals>0)
			{
				if (data->sortAll)
					qsort(&vals[0], nVals,   sizeof(SFAttribute), sortByAttributeName);
				else
					qsort(&vals[1], nVals-1, sizeof(SFAttribute), sortByAttributeName);
			}

			// find the select value again in it's new position
			if (isIndexed)
			{
				for (int i=0;i<nVals;i++)
				{
					if (vals[i] == saved)
					{
						data->selInt = i;
						break;
					}
				}
			}
		}

		if (isIndexed)
			Create(nVals, vals, data->selInt);
		else
			Create(nVals, vals, data->selStr);

		if (vals)
			delete [] vals;
	}
}

//------------------------------------------------------------------------
CSelect::CSelect(CExportContext& ctx, const SFString& name, SFInt32 size)
: CElement(ctx)
{
	SFAttribute attribs[25];
	int n=0;
	ADDATTRIB      ("name",         name);
	ADDATTRIB      ("id",           name);
	ADDATTRIB      ("size",         asString(size,-1));
	ADDMOUSEATTRIB ();
	InitElement    ("select", -1, n, attribs);
}

//------------------------------------------------------------------------
CSelect::CSelect(CExportContext& ctx, const SFString& name, SFInt32 size, const SFString& onChangeStr, SFBool disabled)
: CElement(ctx)
{
	ctx << "\n";
	SFAttribute attribs[25];
	int n=0;
	ADDATTRIB      ("name",         name);
	ADDATTRIB      ("id",           name);
	ADDATTRIB      ("size",         asString(size,-1));
	ADDATTRIBSINGLE("disabled",     disabled);
	ADDATTRIB      ("onChange",     onChangeStr);
	ADDMOUSEATTRIB ();
	InitElement    ("select", -1, n, attribs);
}

//------------------------------------------------------------------------
CSelect::CSelect(CExportContext& ctx, const SFString& name, SFInt32 nFields, SFString *vals, SFString *prompts, SFInt32 selected)
: CElement(ctx)
{
	ctx << "\n";
	SFAttribute attribs[25];
	int n=0;
	ADDATTRIB      ("name",         name);
	ADDATTRIB      ("id",           name);
	ADDATTRIB      ("size",         asString(1,-1));
	ADDMOUSEATTRIB ();
	InitElement    ("select", -1, n, attribs);
	Create(nFields, vals, prompts, selected);
}

//------------------------------------------------------------------------
CSelect::CSelect(CExportContext& ctx, SFAttribute vals[], SFInt32 nFields, const SFString& name, SFInt32 selected, const SFString& onChangeStr)
: CElement(ctx)
{
	ctx << "\n";
	SFAttribute attribs[25];
	int n=0;
	ADDATTRIB      ("name",         name);
	ADDATTRIB      ("id",           name);
	ADDATTRIB      ("size",         asString(1,-1));
	ADDATTRIB      ("onChange",     onChangeStr);
	ADDMOUSEATTRIB ();
	InitElement    ("select", -1, n, attribs);
	Create(nFields, vals, selected);
}

//------------------------------------------------------------------------
CSelect::CSelect(CExportContext& ctx, const SFString& name, SFInt32 nFields, SFString *vals, SFString *prompts, SFInt32 selected, const SFString& onChangeStr, SFBool disabled)
: CElement(ctx)
{
	ctx << "\n";
	SFAttribute attribs[25];
	int n=0;
	ADDATTRIB      ("name",         name);
	ADDATTRIB      ("id",           name);
	ADDATTRIB      ("size",         asString(1,-1));
	ADDATTRIBSINGLE("disabled",     disabled);
	ADDATTRIB      ("onChange",     onChangeStr);
	ADDMOUSEATTRIB ();
	InitElement    ("select", -1, n, attribs);
	Create(nFields, vals, prompts, selected);
}
