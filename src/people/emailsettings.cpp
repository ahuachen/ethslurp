/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "peopledatabase.h"
#include "emailsettings.h"

//---------------------------------------------------------------------------------------
CEmailSettings::CEmailSettings(void)
{
	Init();
}

//---------------------------------------------------------------------------------------
CEmailSettings::CEmailSettings(const CEmailSettings& settings)
{
	Copy(settings);
}

//---------------------------------------------------------------------------------------
CEmailSettings& CEmailSettings::operator=(const CEmailSettings& settings)
{
	Copy(settings);
	return *this;
}

//---------------------------------------------------------------------------------------
void CEmailSettings::Init(void)
{
	m_Method     = EMAIL_NEVER;
	m_Server     = "none";
	m_FromAddr   = "none";
	m_Program    = "/usr/sbin/sendmail";
	m_Bcc        = (BCC_ES | BCC_US | BCC_UU | BCC_GS); // supers for all, user for users
	m_Notify     = EMPTY;
	m_AllFields  = TRUE;
	m_Comments   = FALSE;
	m_HTML       = FALSE;
	m_Logging    = FALSE;
}

//---------------------------------------------------------------------------------------
void CEmailSettings::Copy(const CEmailSettings& settings)
{
	m_Method     = settings.m_Method;
	m_Server     = settings.m_Server;
	m_FromAddr   = settings.m_FromAddr;
	m_Program    = settings.m_Program;
	m_Bcc        = settings.m_Bcc;
	m_Notify     = settings.m_Notify;
	m_AllFields  = settings.m_AllFields;
	m_Comments   = settings.m_Comments;
	m_HTML       = settings.m_HTML;
	m_Logging    = settings.m_Logging;
}

//---------------------------------------------------------------------------------------
SFString CEmailSettings::toAscii(void) const
{
	return
		asString(m_Method)    + CH_BAR +
		         m_Server     + CH_BAR +
		         m_FromAddr   + CH_BAR +
		         m_Program    + CH_BAR +
		asString(m_Bcc)       + CH_BAR +
		         m_Notify     + CH_BAR +
		asString(m_Comments)  + CH_BAR +
		asString(m_AllFields) + CH_BAR +
		asString(m_HTML)      + CH_BAR +
		asString(m_Logging)   + CH_BAR;
}

//---------------------------------------------------------------------------------------
void CEmailSettings::fromAscii(const SFString& in, const CVersion& version)
{
	SFString str = in;

	m_Method    = atoi((const char *)nextTokenClear(str, CH_BAR));
	m_Server    =                    nextTokenClear(str, CH_BAR) ;
	m_FromAddr  =                    nextTokenClear(str, CH_BAR) ;
	m_Program   =                    nextTokenClear(str, CH_BAR) ;
	m_Bcc       = atoi((const char *)nextTokenClear(str, CH_BAR));
	m_Notify    =                    nextTokenClear(str, CH_BAR) ;
	m_Comments  = atoi((const char *)nextTokenClear(str, CH_BAR));
	m_AllFields = atoi((const char *)nextTokenClear(str, CH_BAR));
	m_HTML      = atoi((const char *)nextTokenClear(str, CH_BAR));
	m_Logging   = atoi((const char *)nextTokenClear(str, CH_BAR));
}

//---------------------------------------------------------------------------------------
SFBool CEmailSettings::getBCC(SFInt32 mask) const
{
	return (m_Bcc & mask);
}

//---------------------------------------------------------------------------------------
void CEmailSettings::setBCC(SFInt32 mask, SFBool onOff)
{
	if (onOff)
		m_Bcc |= mask;
	else
		m_Bcc &= ~(mask);
}

//---------------------------------------------------------------------------------------
void CEmailSettings::setValueByField(CFieldData *field)
{
	SFString value  = field->getValue();
	SFInt32  valueI = atoi((const char *)value);

	SFString name = field->getFieldName();
	if (name.IsEmpty())
		return;

	switch (name[0])
	{
	case 'b':
		if (name == "bcc_es")     { setBCC(BCC_ES, valueI); return; }
		if (name == "bcc_ea")     { setBCC(BCC_EA, valueI); return; }
		if (name == "bcc_ec")     { setBCC(BCC_EC, valueI); return; }
		if (name == "bcc_el")     { setBCC(BCC_EL, valueI); return; }
		if (name == "bcc_us")     { setBCC(BCC_US, valueI); return; }
		if (name == "bcc_ua")     { setBCC(BCC_UA, valueI); return; }
		if (name == "bcc_uu")     { setBCC(BCC_UU, valueI); return; }
		if (name == "bcc_gs")     { setBCC(BCC_GS, valueI); return; }
		if (name == "bcc_ga")     { setBCC(BCC_GA, valueI); return; }
		if (name == "bcc_gg")     { setBCC(BCC_GG, valueI); return; }
		if (name == "blind")      { m_Notify    = value;    return; }
		break;

	case 'c':
		if (name == "comments")   { m_Comments  = valueI;   return; }
		break;

	case 'f':
		if (name == "from_email") { m_FromAddr  = value;    return; }
		break;

	case 'h':
		if (name == "html")       { m_HTML      = valueI;   return; }
		break;

	case 'm':
		if (name == "method")     { m_Method    = valueI;   return; }
		if (name == "mailServ")   { m_Server    = value;    return; }
		if (name == "maillog")    { m_Logging   = valueI;   return; }
		break;

	case 'p':
		if (name == "program")    { m_Program   = value;    return; }
		break;

	case 's':
		if (name == "sendAll")    { m_AllFields = valueI;   return; }
		break;
	}
}
