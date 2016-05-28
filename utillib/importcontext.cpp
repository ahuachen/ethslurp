/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "importcontext.h"
#define DEF_DELIMITER ((char)0x5)

// Note: This code does nothing interesting
CImportContext::CImportContext(void)
{
}

CImportContext& CImportContext::operator>>(char& c)
{
	c = (char)atoi(getNextToken(DEF_DELIMITER));
	return *this;
}

CImportContext& CImportContext::operator>>(long& dw)
{
	dw = atoi(getNextToken(DEF_DELIMITER));
	return *this;
}

CImportContext& CImportContext::operator>>(float& f)
{
	f = (float)atof(getNextToken(DEF_DELIMITER));
	return *this;
}

CImportContext& CImportContext::operator>>(double& f)
{
	f = atof(getNextToken(DEF_DELIMITER));
	return *this;
}

CImportContext& CImportContext::operator>>(char* &str)
{
	//str = getNextToken(DEF_DELIMITER);
	return *this;
}

CImportContext& CImportContext::operator>>(SFString& str)
{
	str = getNextToken(DEF_DELIMITER);
	return *this;
}

CImportContext& CImportContext::operator>>(SFTime& tm)
{
	tm = SFTime(getNextToken(DEF_DELIMITER), "ydm4/");
	return *this;
}

CImportContext& CImportContext::operator>>(SFAttribute& attr)
{
	attr.setName(getNextToken(DEF_DELIMITER));
	attr.setValue(getNextToken(DEF_DELIMITER));
	return *this;
}

void CFileImportContext::refreshBuffer(void)
{
	if (countOf(((char)0x6), m_buffer) > 1)
		return;
		
	char buff[4096*8];
	fgets(buff, 4096*8, m_input);
	m_buffer += buff;
}

SFString CFileImportContext::getNextToken(char delim)
{
	refreshBuffer();
	return nextTokenClear(m_buffer, delim);
}
