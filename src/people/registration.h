#ifndef _REGISTRATION_H
#define _REGISTRATION_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "apps.h"

//------------------------------------------------------------------------
class CRegistration
{
private:
	SFTime   m_date;
	SFString m_userid;
	SFString m_code;
	SFString m_machine;
	SFString m_name;
	SFString m_email;
	SFString m_phone;
	SFInt32	 m_todo;
	SFInt32	 m_contact;
	SFInt32	 m_branded;
	SFInt32	 m_nLics;

public:
	CRegistration(void)
		{
			Init();
		}

	CRegistration(const SFString& username, const SFString& license, SFInt32 nLics, const SFString& name=EMPTY, const SFString& email=EMPTY, const SFString& phone=EMPTY, const SFString& machine="any")
		{
			Init();
			m_userid   = username;
			m_code     = license;
			m_name     = name;
			m_email    = email;
			m_phone    = phone;
			m_machine  = machine;
			m_nLics    = nLics;
		}

	CRegistration(const CRegistration& data)
		{
			Copy(data);
		}

	CRegistration& operator=(const CRegistration& data)
		{
			Copy(data);
			return *this;
		}

	SFString toAscii         (SFBool encrypt=TRUE) const;
	void     fromAscii       (SFString& line, const CVersion& version);
    void     fromAscii_4_0_2 (SFString& line);

	SFBool   needsUpgrade    (void) const;
	SFBool   isRegistered    (SFInt32 whichApp) const;
	SFBool   isDemo          (void) const;
	SFInt32  nRegistered     (void) const;

	SFString getDateString   (const SFString& noDateMsg) const;
	SFString getRegisteredID (void) const;
	SFString getCode         (void) const;
	SFTime   getDate         (void) const;

	friend class CUserDatabase;

private:
	void Init(void)
		{
			m_date       = earliestDate;
			// m_userid  = "";
			// m_code    = "";
			// m_machine = "";
			// m_name    = "";
			// m_email   = "";
			// m_phone   = "";
			m_todo       = FALSE;
			m_contact    = FALSE;
			m_branded    = TRUE;
			m_nLics	     = 0;
		}

	void Copy(const CRegistration& data)
		{
			m_date       = data.m_date;
			m_userid     = data.m_userid;
			m_code       = data.m_code;
			m_machine    = data.m_machine;
			m_name       = data.m_name;
			m_email      = data.m_email;
			m_phone      = data.m_phone;
			m_todo       = data.m_todo;
			m_contact    = data.m_contact;
			m_branded    = data.m_branded;
			m_nLics      = data.m_nLics;
		}
};

//-------------------------------------------------------------------------
#define PW_KEY 1

//-------------------------------------------------------------------------
extern SFString generateLicenseCode(const SFString& machine, const SFString& userid, SFInt32 nLics, SFBool apps);
extern SFInt32  crackApps          (const SFString& codeIn);

//-------------------------------------------------------------------------
inline SFString CRegistration::getDateString(const SFString& noDateMsg) const
{
	if (m_date < SFTime(1980,1,1,12,0,0))
		return noDateMsg;
	return m_date.Format(FMT_DAY);
}

//-------------------------------------------------------------------------
inline SFString CRegistration::getRegisteredID(void) const
{
	return m_userid;
}

//-------------------------------------------------------------------------
inline SFString CRegistration::getCode(void) const
{
	return m_code;
}

//-------------------------------------------------------------------------
inline SFInt32 CRegistration::nRegistered(void) const
{
	return m_nLics;
}

//-------------------------------------------------------------------------
inline SFBool CRegistration::isDemo(void) const
{
	return (getRegisteredID() == "demo");
}

//-------------------------------------------------------------------------
inline SFTime CRegistration::getDate(void) const
{
	return m_date;
}

//-------------------------------------------------------------------------
extern SFBool encryptRegistration(SFBool onOff=BAD_NUMBER);

#endif
