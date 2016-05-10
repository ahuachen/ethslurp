#ifndef _ELEMENT_H
#define _ELEMENT_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "exportcontext.h"

#define SINGLETON -100

//----------------------------------------------------
//
class CElement
{
protected:
	CExportContext& m_ctx;
	SFInt32         m_nls;
	SFInt32         m_singleton;
	SFString        m_tag;

public:
	void InitElement(const SFString& t, SFInt32 n=-1, SFInt32 nAttribs=0, SFAttribute attribs[]=NULL)
		{
			m_tag = t;
			m_singleton = (n==SINGLETON);
			if (m_singleton)
				n = 1;
    		m_nls = labs(n);

			if (!m_tag.IsEmpty())
			{
				m_ctx << "<" << m_tag;
				if (nAttribs)
				{
					int i=0;
					for (i=0;i<nAttribs;i++)
						m_ctx << attribs[i]; //.Write(m_ctx);
				}
				m_ctx << ">";
			}

			if (n<0 && !m_singleton)
				m_ctx << "\n";
		}

public:
	CElement(CExportContext& ctx) : m_ctx(ctx)
		{ 
			m_ctx       = ctx;
			m_tag       = nullString;
			m_nls       = 1;
			m_singleton = 0;
		}

	CElement(CExportContext& ctx, const SFString& t, int n=-1, int nAttribs=0, SFAttribute attribs[]=NULL) : m_ctx(ctx)
		{
			m_ctx       = ctx;
			m_tag       = nullString;
			m_nls       = 1;
			m_singleton = 0;
			InitElement(t, n, nAttribs, attribs);
		}

	CElement(CExportContext& ctx, const SFString& t, const SFString& attribStr, SFBool nl=FALSE) : m_ctx(ctx)
		{
			m_ctx       = ctx;
			m_tag       = nullString;
			m_nls       = 1;
			m_singleton = 0;
	
			if (nl)
				m_ctx << "\n";

			SFAttribute attribs[25];
			SFInt32     nAttribs = 0;

			SFString str = attribStr;
			if (str.Contains("=\"") && str.Contains("\"|"))
			{
				str.ReplaceAll("=\"", "=");
				str.ReplaceAll("\"|", "|");
			}
			while (!str.IsEmpty())
			{
				attribs[nAttribs].setName (nextTokenClear(str, '='));
				attribs[nAttribs].setValue(nextTokenClear(str, '|'));
				nAttribs++;
			}
			InitElement(t, -1, nAttribs, attribs);
		}

	~CElement(void)
		{
			if (!m_singleton)
			{
				if (!m_tag.IsEmpty())
					m_ctx << "</" << m_tag << ">";

				int i=0;
				for (i=0;i<m_nls;i++)
					m_ctx << "\n";
			}
		}

	void setNewLineCount(int nl);

protected:
	// no copies or default construction
						CElement  (void);
						CElement  (const CElement& el);
	CElement& operator= (const CElement& el);
};

#define ADDATTRIB(nm, val) if (!(val).IsEmpty()) { attribs[n++].set((nm), (val)); }

#define ADDATTRIBSINGLE(nm, val) if (val) { attribs[n++].set((nm), nullString); }

#endif
