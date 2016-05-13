#ifndef _IMPORTCONTEXT_H
#define _IMPORTCONTEXT_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "attribute.h"
#include "sftime.h"

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
