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

#include "sfarchive.h"

///////////////////////////////////////////////////////////////////
SFArchive& SFArchive::operator<<(char c)
{
	if (isWriting())
		*m_writeFile << c << m_fieldEnd;
	return *this;
}

SFArchive& SFArchive::operator<<(long dw)
{
	if (isWriting())
		*m_writeFile << dw << m_fieldEnd;
	return *this;
}

SFArchive& SFArchive::operator<<(float f)
{
	if (isWriting())
		*m_writeFile << f << m_fieldEnd;
	return *this;
}

SFArchive& SFArchive::operator<<(double f)
{
	if (isWriting())
		*m_writeFile << f << m_fieldEnd;
	return *this;
}

SFArchive& SFArchive::operator<<(const char *str)
{
	if (isWriting())
		*m_writeFile << str << m_fieldEnd;
	return *this;
}

SFArchive& SFArchive::operator<<(const SFString& str)
{
	if (isWriting())
		*m_writeFile << str << m_fieldEnd;
	return *this;
}

SFArchive& SFArchive::operator<<(const SFTime& tm)
{
	if (isWriting())
		*m_writeFile << tm << m_fieldEnd;
	return *this;
}

SFArchive& SFArchive::operator<<(const SFAttribute& attr)
{
	if (isWriting())
		*m_writeFile << attr << m_fieldEnd;
	return *this;
}

SFArchive& SFArchive::operator<<(const CDoublePoint& pt)
{
	if (isWriting())
		*m_writeFile << pt << m_fieldEnd;
	return *this;
}

SFArchive& SFArchive::operator<<(const CDoubleRect& rect)
{
	if (isWriting())
		*m_writeFile << rect << m_fieldEnd;
	return *this;
}

///////////////////////////////////////////////////////////////////
SFArchive& SFArchive::operator>>(char& c)
{
	if (isReading())
		*m_readFile >> c;
	return *this;
}

SFArchive& SFArchive::operator>>(long& dw)
{
	if (isReading())
		*m_readFile >> dw;
	return *this;
}

SFArchive& SFArchive::operator>>(float& f)
{
	if (isReading())
		*m_readFile >> f;
	return *this;
}

SFArchive& SFArchive::operator>>(double& f)
{
	if (isReading())
		*m_readFile >> f;
	return *this;
}

SFArchive& SFArchive::operator>>(char* &str)
{
	if (isReading())
		*m_readFile >> str;
	return *this;
}

SFArchive& SFArchive::operator>>(SFString& str)
{
	if (isReading())
		*m_readFile >> str;
	return *this;
}

SFArchive& SFArchive::operator>>(SFTime& tm)
{
	if (isReading())
		*m_readFile >> tm;
	return *this;
}

SFArchive& SFArchive::operator>>(SFAttribute& attr)
{
	if (isReading())
		*m_readFile >> attr;
	return *this;
}

SFArchive& SFArchive::operator>>(CDoublePoint& pt)
{
	if (isReading())
		*m_readFile >> pt;
	return *this;
}

SFArchive& SFArchive::operator>>(CDoubleRect& rect)
{
	if (isReading())
		*m_readFile >> rect;
	return *this;
}

SFArchive& operator<<(SFArchive& archive, SFStringArray& array)
{
/*
	archive << array.getCount();
	for (int i=0;i<array.getCount();i++)
		archive << array[i];
*/
	return archive;
}

SFArchive& operator>>(SFArchive& archive, SFStringArray& array)
{
/*
	SFInt32 count;
	archive >> count;
	for (int i=0;i<count;i++)
		archive >> array[i];
*/
	return archive;
}
