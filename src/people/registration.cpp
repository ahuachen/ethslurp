/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "peopledatabase.h"
#include "registration.h"

//-------------------------------------------------------------------------
// An arbitrary non-printing character to make sure we don't hit against any 'plain vanilla' deliminator
#define DELIMITER ((char)0x5)

//-------------------------------------------------------------------------
static SFString sep = "[:}"; // something that will not be found in the registration code

//-------------------------------------------------------------------------
SFString CRegistration::toAscii(SFBool encrypt) const
{
	// for current versions
	SFString ret;
	ret += "; license=";
	ret +=           m_userid    + sep;
	ret +=           m_code      + sep;
	ret +=           m_machine   + sep;
	ret += asString( m_nLics   ) + sep;
	ret += asString( 0         ) + sep; // used to be nUsed
	ret += asString( m_todo    ) + sep;
	ret += asString( m_contact ) + sep;
	ret += asString( 0         ) + sep; // used to be site
	ret += asString( m_branded ) + sep;
	ret +=           m_date.Format(FMT_SEMI_SEP) + sep;
	ret +=           getVersionString('.') + sep;

#ifdef TESTING
	if (g_unitTesting && !encryptRegistration())
		return ret;
#endif

	if (!encrypt)
		return ret;

	return hard_encrypt(ret, PW_KEY);
}

//-------------------------------------------------------------------------
void CRegistration::fromAscii(SFString& line, const CVersion& version)
{
	line = hard_decrypt(line, PW_KEY);
	if (!line.Contains("; license="))
	{
		fromAscii_4_0_2(line);
		return;
	}

	// for current versions
	line.Replace   ("; license=", EMPTY);
	line.ReplaceAll(sep, DELIMITER);

	m_userid  =                   nextTokenClear(line, DELIMITER);
	m_code    =                   nextTokenClear(line, DELIMITER);
	m_machine =                   nextTokenClear(line, DELIMITER);
	m_nLics   = atoi((const char*)nextTokenClear(line, DELIMITER));
								  nextTokenClear(line, DELIMITER);  // used to be nUsed
	m_todo    = atoi((const char*)nextTokenClear(line, DELIMITER));
	m_contact = atoi((const char*)nextTokenClear(line, DELIMITER));
	                              nextTokenClear(line, DELIMITER);  // used to be site
	m_branded = atoi((const char*)nextTokenClear(line, DELIMITER));
	m_date    =          snagDate(nextTokenClear(line, DELIMITER));
								  nextTokenClear(line, DELIMITER);  // used to be version for upgrade checking, no longer used
}

//-------------------------------------------------------------------------
void CRegistration::fromAscii_4_0_2(SFString& line)
{
	if (line.Contains("; reg"))
	{
		ASSERT(line.Contains("[]") && line.Contains("{}"));

		nextTokenClear(line, '=');
		line.Replace("[]", DELIMITER);
		line.Replace("{}", DELIMITER);
		m_userid  = nextTokenClear(line, DELIMITER);
		m_code    = nextTokenClear(line, DELIMITER);
		m_machine = nextTokenClear(line, DELIMITER);

	} else if (line.Contains("; nlics") || line.Contains("; nused"))
	{
		line.ReplaceAll(" :",    ":");
		line.Replace   ("; ",     EMPTY);
		line.ReplaceAny("\r\n\t", EMPTY);
		while (!line.IsEmpty())
		{
			SFString field  = nextTokenClear(line, '=');
			SFString value;
			if (field == "date")
			{
				value = line;
				line  = EMPTY;
			} else
				value = nextTokenClear(line, ':');
			SFInt32  valueI = atoi((const char*)value);

			if (field % "nlics"  ) m_nLics   = valueI;
			if (field % "todo"   ) m_todo    = valueI;
			if (field % "addr"   ) m_contact = valueI;
			if (field % "branded") m_branded = valueI;
			if (field % "date"   )
			{
				SFString dateStr = value + line; // gobble the rest
				SFTime d1 = SFTime(dateStr, "mdy4/");
				SFTime d2 = SFTime(dateStr, "dmy4/");
				m_date = earlierOf(d1, d2);
			}
			// ignored --> unused, sync, ressched
		}
	}
}

// Even though this is a bitmap we never test for more than one thing
// at a time so its OK to test for equality here.
//-----------------------------------------------------------------------------------
SFBool CRegistration::isRegistered(SFInt32 whichApp) const
{
	SFBool isBranded =  isDemo() || m_branded;
#ifdef TESTING
	if (g_unitTesting)
	{
		if (getInt32("test_branded", BAD_NUMBER) != BAD_NUMBER)
			isBranded = getInt32("test_branded", TRUE);
	}
#endif

	switch (whichApp)
	{
	case APP_CALENDAR:	  return  isDemo() || m_nLics;
	case APP_TODOLIST:    return  isDemo() || m_todo;
	case APP_CONTACTLIST: return  isDemo() || m_contact;
	case APP_BRANDED:     return  isBranded;
	}

	return FALSE;
}

//-------------------------------------------------------------------------
SFBool CRegistration::needsUpgrade(void) const
{
	SFBool oldDate = (m_date < SFTime(2014, 6, 1, 0, 0, 0));

#ifdef TESTING
	if (getInt32("test_ForceOutdate", FALSE))
		return TRUE;
	oldDate = (m_date < SFTime(2006, 1, 1, 0, 0, 0));
#endif

	return oldDate;
}

//-------------------------------------------------------------------------
SFInt32 crackApps(const SFString& codeIn)
{
	// code stores USERNAME_NLICS_xUSERNAMELEN+17_xUSERNAMELEN+APPS
	SFString code = codeIn;
	if (!code.Contains("_x"))
		return 0;

	code = code.Mid(code.Find("_x")+2);
	SFInt32 len  = atoi((const char *)code)-17; ASSERT(len);
	SFInt32 find = code.Find("_x"); ASSERT(find != -1);
	code = code.Mid(find+2);
	return (atoi((const char *)code)-len);
}

//-------------------------------------------------------------------------
SFString generateLicenseCode(const SFString& machine, const SFString& userid, SFInt32 nLics, SFBool apps)
{
	// Machine is not used - it is only used to scare people
	SFString user = userid;
	user.ReplaceAll(" ", "_");
	
	char code[100];
	sprintf(code, "%s_%03ld", (const char *)user, nLics);

	SFString ret = code;
	ret.MakeUpper();

	SFString rr;
	for (int i=0;i<ret.GetLength();i++)
	{
		if (i == 5 || i == 7)
		{
			char a[10];
			sprintf(a, "%02d", (int)ret[i]);
			rr += a;
		} else
		{
			rr += (char)(ret[i] + 4);
		}
	}

	if (apps)
	{
		char aStr[100];
		sprintf(aStr, "_x%03ld_x%03ld", user.GetLength()+17, user.GetLength() + apps);
		rr += aStr;
	}

	return rr;
}

//-------------------------------------------------------------------------
SFBool encryptRegistration(SFBool onOff)
{
	static SFBool shouldEncrypt=TRUE;
	if (onOff == BAD_NUMBER)
		return shouldEncrypt;
	shouldEncrypt = onOff;
	return shouldEncrypt;
}

