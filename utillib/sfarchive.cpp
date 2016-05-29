/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
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
