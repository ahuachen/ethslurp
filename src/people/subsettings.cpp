/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "peopledatabase.h"
#include "subsettings.h"

//-------------------------------------------------------------------------
SFString CSubSettings::toAscii(void) const
{
	ASSERT(!m_includeList.Contains(CH_BAR));
	ASSERT(!m_replaceStr.Contains (CH_BAR));

	return
		         m_includeList + CH_BAR +
		asString(m_subAction)  + CH_BAR +
		asString(m_exact)      + CH_BAR +
                 m_replaceStr  + CH_BAR +
		asString(m_Tracking)   + CH_BAR +
		asString(m_Verbose)    + CH_BAR;
}

//-------------------------------------------------------------------------
void CSubSettings::fromAscii(const SFString& in)
{
	SFString str = in;

	m_includeList =                    nextTokenClear(str, CH_BAR);
	m_subAction   = atoi((const char *)nextTokenClear(str, CH_BAR));
	m_exact       = atoi((const char *)nextTokenClear(str, CH_BAR));
    m_replaceStr  =                    nextTokenClear(str, CH_BAR);
	m_Tracking    = atoi((const char *)nextTokenClear(str, CH_BAR));
	m_Verbose     = atoi((const char *)nextTokenClear(str, CH_BAR));
}

//-------------------------------------------------------------------------
/*
void CSubSettings::setFieldValue(const CFieldData *field, const SFString& value)
{
	SFInt32 valueI = atoi((const char *)value);

	SFString name = field->getFieldName();
	if (name.IsEmpty())
		return;

	switch (name[0])
	{
	case 'a': if (name == "action")           { setAction(valueI);     return; }
	case 'e': if (name == "exact")            { setExact(valueI);      return; }
	case 'i': if (name == "includeList")      { setIncludeList(value); return; }
	case 'r': if (name == "replacestr")       { setReplaceStr(value);  return; }
	case 't': if (name == "tracksubmissions") { setTracking(valueI);   return; }
	case 'v': if (name == "verbose")          { setVerbose(valueI);    return; }
	}
}
*/
