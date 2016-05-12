#ifndef _LOGINSETTING_H
#define _LOGINSETTING_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

class CLoginSettings
{
private:
	SFBool  m_allowRemote;
	SFInt32 m_remotePerm;
	SFInt32 m_loginTimeout;

public:
	CLoginSettings(void)
		{
			Init();
		}

	CLoginSettings(const CLoginSettings& data)
		{
			Copy(data);
		}

	CLoginSettings& operator=(const CLoginSettings& data)
		{
			Copy(data);
			return *this;
		}

	SFString toAscii       (void) const;
	void     fromAscii     (const SFString& in);

private:
	void Init(void)
		{
			m_allowRemote  = FALSE;
			m_remotePerm   = PERMISSION_VIEW;
			m_loginTimeout = 1*60;
		}

	void Copy(const CLoginSettings& data)
		{
			m_allowRemote  = data.m_allowRemote;
			m_remotePerm   = data.m_remotePerm;
			m_loginTimeout = data.m_loginTimeout;
		}

	friend class CUserDatabase;
};

#endif
