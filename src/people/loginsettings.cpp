/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "peopledatabase.h"
#include "loginsettings.h"

//-------------------------------------------------------------------------
SFString CLoginSettings::toAscii(void) const
{
	return 
		asString(m_allowRemote)  + CH_BAR +
		asString(m_remotePerm)   + CH_BAR +
		asString(m_loginTimeout) + CH_BAR;
}

//-------------------------------------------------------------------------
void CLoginSettings::fromAscii(const SFString& in)
{
	SFString str = in;
	
	m_allowRemote  = atoi((const char *)nextTokenClear(str, CH_BAR));
	m_remotePerm   = atoi((const char *)nextTokenClear(str, CH_BAR));
	m_loginTimeout = atoi((const char *)nextTokenClear(str, CH_BAR));
}

//-------------------------------------------------------------------------
/*
void CLoginSettings::setFieldValue(const CFieldData *field, const SFString& value)
{
	SFInt32 valueI = atoi((const char *)value);

	SFString name = field->getFieldName();
	if (name.IsEmpty())
		return;

	switch (name[0])
	{
	case 'a': if (name == "allowRemote")  { setAllowRemote (valueI); return; }
	case 'l': if (name == "loginTimeout") { setLoginTimeout(valueI); return; }
	case 'r': if (name == "remotePerm")   { setRemotePerm  (valueI); return; }
	}
}
*/
