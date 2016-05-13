/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "exportcontext.h"
#include "dates.h"

//-------------------------------------------------------------
CExportContext::CExportContext(void) : tCh('\t')
{
    nTabs=0;
}

//-------------------------------------------------------------
CExportContext& CExportContext::operator<<(bool b)
{
	char val[10];
	sprintf(val, "%c", b ? '1' : '0');
	Output(val);
	return *this;
}

//-------------------------------------------------------------
CExportContext& CExportContext::operator<<(char c)
{
	char val[10];
	sprintf(val, "%c", c);
	Output(val);
	return *this;
}

//-------------------------------------------------------------
CExportContext& CExportContext::operator<<(long dw)
{
	char val[10];
	sprintf(val, (fmt.IsEmpty() ? "%ld" : (const char*)fmt), dw);
	Output(val);
	return *this;
}

//-------------------------------------------------------------
CExportContext& CExportContext::operator<<(unsigned int ui)
{
	char val[10];
	sprintf(val, (fmt.IsEmpty() ? "%d" : (const char*)fmt), ui);
	Output(val);
	return *this;
}

//-------------------------------------------------------------
CExportContext& CExportContext::operator<<(int i)
{
	char val[10];
	sprintf(val, (fmt.IsEmpty() ? "%d" : (const char*)fmt), i);
	Output(val);
	return *this;
}

//-------------------------------------------------------------
CExportContext& CExportContext::operator<<(float f)
{
	char val[10];
	sprintf(val, (fmt.IsEmpty() ? "%f" : (const char*)fmt), f);
	Output(val);
	return *this;
}

//-------------------------------------------------------------
CExportContext& CExportContext::operator<<(double f)
{
	char val[10];
	sprintf(val, (fmt.IsEmpty() ? "%g" : (const char*)fmt), f);
	Output(val);
	return *this;
}

//-------------------------------------------------------------
CExportContext& CExportContext::operator<<(const char *str)
{
	Output(str);
	return *this;
}

//-------------------------------------------------------------
CExportContext& CExportContext::operator<<(const SFString& str)
{
	if (str.GetLength()>=3 && str.Left(2)=="`%" && str.GetAt(2)!='%')
		fmt = str.Mid(1,1000);
	else if (str=="%")
		fmt = EMPTY;
	else
		Output(str);
	return *this;
}

//-------------------------------------------------------------
CExportContext& CExportContext::operator<<(const SFTime& tm)
{
	Output(tm.Format(FMT_DEFAULT));
	return *this;
}

//-------------------------------------------------------------
CExportContext& CExportContext::operator<<(const SFAttribute& attr)
{
	SFString name = attr.getName();
	if (!name.IsEmpty())
	{
		*this << " " << name;
		SFString value = attr.getValue();
		if (!value.IsEmpty())
		{
			*this << "=\"" << value << "\"";
		}
	}
	return *this;
}

//-------------------------------------------------------------
void CFileExportContext::setOutput(void *output)
{
	Close(); // just in case
	m_output = output == NULL ? stdout : (FILE*)output;
}

//-------------------------------------------------------------
void CFileExportContext::Output(const SFString& sss)
{
	ASSERT(m_output);
	fprintf(m_output, "%s", (const char *)sss);
}

//-------------------------------------------------------------
void CStringExportContext::setOutput(void *output)
{
	// should not happen
	ASSERT(0);
}

void CStringExportContext::Output(const SFString& sss)
{
	str += sss;
}
