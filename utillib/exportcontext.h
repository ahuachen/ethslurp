#ifndef _EXPORTCONTEXT_H
#define _EXPORTCONTEXT_H
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
#include "sfstring.h"
#include "sftime.h"
#include "point.h"
#include "rect.h"

class CExportContext
{
public:
	SFInt32  nTabs;
    SFString fmt;
	char     tCh;

	         CExportContext(void);
	virtual ~CExportContext(void) { }

	virtual CExportContext& operator<<(bool b);
	virtual CExportContext& operator<<(char c);
	virtual CExportContext& operator<<(long dw);
	virtual CExportContext& operator<<(unsigned int ui);
	virtual CExportContext& operator<<(int i);
	virtual CExportContext& operator<<(float f);
	virtual CExportContext& operator<<(double f);
	virtual CExportContext& operator<<(const char *str);
	virtual CExportContext& operator<<(const SFString& str);
	virtual CExportContext& operator<<(const SFTime& tm);
	virtual CExportContext& operator<<(const SFAttribute& attr);
	virtual CExportContext& operator<<(const CDoublePoint& pt);
	virtual CExportContext& operator<<(const CDoubleRect& rect);

	virtual SFString tabs      (SFInt32 add=0) { return SFString(tCh, nTabs+add); }
	virtual SFString inc       (void)          { SFString ret = SFString(tCh, nTabs); nTabs++; return ret; }
	virtual SFString dec       (void)          { nTabs--;  return SFString(tCh, nTabs); }

	virtual void     setOutput (void *output)        = 0;
	virtual void*    getOutput (void) const          = 0;
	virtual void     Output    (const SFString& str) = 0;
	virtual void     Flush     (void)                = 0;
};

// Handy for debugging
class CFileExportContext : public CExportContext
{
public:
	FILE *m_output;

	CFileExportContext(void *output=NULL)
		{
			m_output = ((output == NULL) ? stdout : (FILE*)output);
		}

	CFileExportContext(const SFString& filename, const SFString& mode)
		{
			m_output = fopen((const char *)filename, mode);
			if (!m_output)
				m_output = stdout;
		}

	~CFileExportContext(void)
		{
			Close();
		}

	void  setOutput (void *output);
	void *getOutput (void) const { return m_output; };
	void  Output    (const SFString& str);
	void  Flush     (void)
		{
			ASSERT(m_output)
			fflush(m_output);
		}
	void Close(void)
		{
			Flush();
			if (m_output != stdout && m_output != stderr)
				fclose(m_output);
			m_output = stdout;
		}
};

class CErrorExportContext : public CFileExportContext
{
public:
	CErrorExportContext(void) : CFileExportContext(stderr) {}
};

// Handy for generating HTML into strings
class CStringExportContext : public CExportContext
{
public:
	SFString str;

	CStringExportContext(void) {}

	void  setOutput (void *output);
	void* getOutput (void) const { return NULL; };
	void  Output    (const SFString& s);
	void  Flush     (void) { } // do nothing

	operator SFString(void) const;
};

inline CStringExportContext::operator SFString(void) const
{
   return str;
}

#define Fmt(a) (SFString("`")+SFString(a))

#endif
