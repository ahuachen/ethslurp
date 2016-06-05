#ifndef _IMPORTCONTEXT_H
#define _IMPORTCONTEXT_H
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
#include "attribute.h"
#include "sftime.h"
#include "point.h"
#include "rect.h"

class CImportContext
{
public:
	         CImportContext(void);
	virtual ~CImportContext(void) { }

	virtual CImportContext& operator>>(char& c);
	virtual CImportContext& operator>>(long& dw);
	virtual CImportContext& operator>>(float& f);
	virtual CImportContext& operator>>(double& f);
	virtual CImportContext& operator>>(char* &str);
	virtual CImportContext& operator>>(SFString& str);
	virtual CImportContext& operator>>(SFTime& tm);
	virtual CImportContext& operator>>(SFAttribute& attr);
	virtual CImportContext& operator>>(CDoublePoint& pt);
	virtual CImportContext& operator>>(CDoubleRect& rect);

	virtual SFString getNextToken(char delim) = 0;
	virtual void     refreshBuffer(void) = 0;
};

class CFileImportContext : public CImportContext
{
public:
	SFString  m_buffer;
	FILE     *m_input;

	CFileImportContext(void)
		{
			m_input = NULL;
		}

	CFileImportContext(const SFString& filename, const SFString& mode)
		{
			m_input = fopen((const char *)filename, mode);
		}

	~CFileImportContext(void)
		{
			if (m_input)
				Close();
		}

	void Close(void)
		{
			if (m_input)
			{
				fflush(m_input);
				fclose(m_input);
			}
			m_input = NULL;
		}

	SFString getNextToken  (char delim);
	void     refreshBuffer (void);
};

#endif
