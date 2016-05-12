/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "peoplelib.h"
#include "simpleuserdb.h"

//-----------------------------------------------------------------------------------------
#define EXPOSE_PW 1
#undef EXPOSE_PW

//--------------------------------------------------------------------------------------------
CSimpleUserDB::CSimpleUserDB(void) : CPeopleDatabase()
{
	m_curUser = &m_noUser;
}

//--------------------------------------------------------------------------------------------
void CSimpleUserDB::WriteDatabase (void)
{
	SFString out = asString(m_nUsers) + "\n";
	for (int i=0;i<m_nUsers;i++)
	{
#ifdef EXPOSE_PW
		m_users[i].setNickname(m_users[i].getPassword());
#endif
		out += m_users[i].toAscii(EMPTY) + "\n";
	}
	stringToAsciiFile(getFilename(), out);
}

//--------------------------------------------------------------------------------------------
void CSimpleUserDB::LoadUserTable (SFString& contents)
{
	m_version.setVersion(VERSION_MAJOR, VERSION_MINOR1, VERSION_MINOR2);
	m_nUsers = atoi((const char*)nextTokenClear(contents, '\n'));
	if (m_nUsers)
	{
		m_users = new CPerson[m_nUsers+3];
		if (m_users)
		{
			for (int i=0;i<m_nUsers;i++)
				m_users[i].setPeopleDB(this);

			SFInt32 cnt=0;
			while (!contents.IsEmpty() && cnt <= m_nUsers)
			{
				m_users[cnt].fromAscii(nextTokenClear(contents, '\n'), m_version);
				cnt++;
			}
			m_nUsers = cnt;
		}
	}
}

//-----------------------------------------------------------------------------------------
SFString CSimpleUserDB::generateUserID(const SFString& email)
{
	return "u_"+Now().Format(FMT_SORTALL)+asString(RandomValue(10,50));
}

//-----------------------------------------------------------------------------------------
void CSimpleUserDB::updateUser(const CPerson& user)
{
	CPerson u = user;
	EditOrAddUser(u, Now());
	WriteDatabase();
}

//--------------------------------------------------------------------------------------------
CPerson *CSimpleUserDB::getCurrentUser(void)
{
    return m_curUser;
}

//--------------------------------------------------------------------------------------------
void CSimpleUserDB::setCurrentUser( CPerson *user )
{
	m_curUser = user;
    if (!m_curUser)
        m_curUser = &m_noUser;
    
}
