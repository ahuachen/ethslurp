/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "peopledatabase.h"
#include "usergroup.h"
#include "person.h"

//---------------------------------------------------------------------------------------
void CUserDatabase::LoadUserSettings(SFString& contents)
{
	if (m_version.earlier(3,5,0))
		return;

	CUserSettings *settings = getUserSettingsEdit();

	SFInt32  linesRead = 0;
	while (!contents.IsEmpty())
	{
		SFString line = nextTokenClearDecrypt(contents, '\n');
		if (!isComment(line)) // is this line a comment?
		{
			switch (linesRead)
			{
			case 0:
				settings->fromAscii(line);
				break;
			default:
				// should not happen
				ASSERT(0);
				return;
			}
			linesRead++;
			if (linesRead==1)
				break;
		}
	}

	if (m_version.earlier(4,0,1))
		upgradeFrom401(this);

	// Maintain the m_adminsList further
	if (!settings->getAllowGroupPerms())
	{
		// replace reverse in case there is a group with same name as user id
		for (int i=0;i<m_nGroups;i++)
			m_adminList.ReplaceReverse(m_groups[i].getName()+",", EMPTY);
	}

	if (settings->getAllowDiffPerms())
	{
		// Pick up any newly minted admin users for the admin list
		for (int i=0; i<m_nPermissions; i++)
		{
			CPermission *perm = &m_permissions[i];
			if (perm->getObjectID() == m_curObject)
			{
				if (perm->getPermission() == PERMISSION_ADMIN)
				{
					m_adminList += (perm->getUserID() + ",");
				} else
				{
					// remove it if this user is bumped down
					m_adminList.Replace(perm->getUserID()+",", EMPTY);
				}
			}
		}
	}

	return;
}

//---------------------------------------------------------------------------------------
void CUserDatabase::upgradeFrom401(CUserDatabase *userDB)
{
	// Now we set specific settings for a regular user database
	CUserSettings *settings = getUserSettingsEdit();
	settings->setSingleContactDB  (FALSE); // earlier we only had contact db per calendar
	settings->setPasswordNotSame  (TRUE ); // improve security
	settings->setAllowDiffPerms   (getPermissionCount() > 0);
	settings->setPasswordReminders(userDB->mailEnabled());
	userDB  ->setLoginTimeout     (userDB->getLoginTimeout() == NEVER_TIMEOUT ? 1*60 : userDB->getLoginTimeout());

	WriteDatabase ();
}

