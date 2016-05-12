#ifndef _SUBSETTINGS_H
#define _SUBSETTINGS_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#define WORDLIST_DISALLOW_SUBMISSION 0
#define WORDLIST_REPLACE_SUBMISSION  1

class CSubSettings
{
private:
	SFString m_includeList;
	SFInt32	 m_subAction;
	SFBool	 m_exact;
	SFString m_replaceStr;
	SFBool	 m_Tracking;
	SFBool	 m_Verbose;

public:
	CSubSettings(void)
		{
			Init();
		}

	CSubSettings(const CSubSettings& data)
		{
			Copy(data);
		}

	CSubSettings& operator=(const CSubSettings& data)
		{
			Copy(data);
			return *this;
		}

	SFString toAscii       (void) const;
	void     fromAscii     (const SFString& in);

	SFString getIncludeList(void) const           { return m_includeList;  }
	SFInt32  getAction     (void) const           { return m_subAction;    } 
	SFBool   getExact      (void) const           { return m_exact;        }
	SFString getReplaceStr (void) const           { return m_replaceStr;   }
	SFBool   getTracking   (void) const           { return m_Tracking;     }
	SFBool   getVerbose    (void) const           { return m_Verbose;      }

	void     setIncludeList(const SFString& list) { m_includeList = list;  }
	void     setAction     (SFInt32 act)          { m_subAction   = act;   }
	void     setExact      (SFBool exact)         { m_exact       = exact; }
	void     setReplaceStr (const SFString& str)  { m_replaceStr  = str;   }
	void     setTracking   (SFBool on)            { m_Tracking    = on;    }
	void     setVerbose    (SFBool on)            { m_Verbose     = on;    }


private:
	void Init(void)
		{
			m_includeList = EMPTY;
			m_subAction   = WORDLIST_DISALLOW_SUBMISSION;
			m_exact       = TRUE;
			m_replaceStr  = "-expletive-";
			m_Tracking    = FALSE;
			m_Verbose     = FALSE;
		}

	void Copy(const CSubSettings& data)
		{
			m_includeList = data.m_includeList;
			m_subAction   = data.m_subAction;
			m_exact       = data.m_exact;
			m_replaceStr  = data.m_replaceStr;
			m_Tracking    = data.m_Tracking;
			m_Verbose     = data.m_Verbose;
		}

};

#endif
