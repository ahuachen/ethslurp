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

#include "importcontext.h"
#define DEF_DELIMITER ((char)0x5)

// Note: This code does nothing interesting
CImportContext::CImportContext(void)
{
}

CImportContext& CImportContext::operator>>(char& c)
{
	c = (char)toLong(getNextToken(DEF_DELIMITER));
	return *this;
}

CImportContext& CImportContext::operator>>(long& dw)
{
	dw = toLong(getNextToken(DEF_DELIMITER));
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

CImportContext& CImportContext::operator>>(CDoublePoint& pt)
{
	*this >> pt.x;
	*this >> pt.y;
	return *this;
}

CImportContext& CImportContext::operator>>(CDoubleRect& rect)
{
	*this >> rect.top;
	*this >> rect.left;
	*this >> rect.bottom;
	*this >> rect.right;
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
