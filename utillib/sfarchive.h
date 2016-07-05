#ifndef _SFARCHIVE_H
#define _SFARCHIVE_H
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
#include "exportcontext.h"
#include "importcontext.h"
#include "database.h"

//-----------------------------------------------------------------------------------------
class SFArchive : public CSharedResource
{
public:
	SFBool              m_writeDeleted;
	SFInt32             m_schema;
	SFBool              m_isReading;

	SFArchive(SFBool isReading, SFInt32 schema, SFBool writeDeleted=TRUE)
		{
			m_isReading    = isReading;
			m_schema       = schema;
			m_writeDeleted = writeDeleted;
		}
	SFBool  writeDeleted(void) const
		{
			return (m_writeDeleted);
		}
	SFInt32 getSchema(void) const
		{
			return (m_schema);
		}
	SFBool isWriting(void) const
		{
			return !m_isReading;
		}
	SFBool isReading(void) const
		{
			return m_isReading;
		}

	SFArchive& operator<<(char c);
	SFArchive& operator<<(long dw);
	SFArchive& operator<<(float f);
	SFArchive& operator<<(double f);
	SFArchive& operator<<(const SFString& str);
	SFArchive& operator<<(const SFTime& tm);
	SFArchive& operator<<(const SFAttribute& attr);
	SFArchive& operator<<(const CDoublePoint& pt);
	SFArchive& operator<<(const CDoubleRect& rect);
	SFArchive& operator<<(const char *str);

	SFArchive& operator>>(char& c);
	SFArchive& operator>>(long& dw);
	SFArchive& operator>>(float& f);
	SFArchive& operator>>(double& f);
	SFArchive& operator>>(SFString& str);
	SFArchive& operator>>(SFTime& tm);
	SFArchive& operator>>(SFAttribute& attr);
	SFArchive& operator>>(CDoublePoint& pt);
	SFArchive& operator>>(CDoubleRect& rect);
};

extern SFArchive& operator<<(SFArchive& archive, SFStringArray& array);
extern SFArchive& operator>>(SFArchive& archive, SFStringArray& array);

#endif
