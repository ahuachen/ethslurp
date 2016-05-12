/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "peopledatabase.h"
#include "person.h"
#include "protectedobject.h"

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void CUserDatabase::LoadPermissionTable(SFString& contents)
{
	if (m_version.earlier(1,0,1))
		return;

	if (m_permissions)
	{
		delete [] m_permissions;
		m_permissions = NULL;
	}
	m_nPermissions = 0;
	
	ASSERT(!m_permissions);
	ASSERT(m_nPermissions==0);

	SFBool firstLine = TRUE;
	SFInt32 nPerms   = 0;
		
	while (!contents.IsEmpty())
	{
		SFString line = nextTokenClearDecrypt(contents, '\n');
		if (!isComment(line)) // is this line a comment?
		{
			// no...

			if (firstLine)
			{
				firstLine = FALSE;
				nPerms = atoi((const char *)line);
				// add more than we appear to need because if we need to add a new permission
				// we will not have to realloc - we only ever add a single new permission at
				// any given time so '2' is more than enough
				if (nPerms)
					m_permissions = new CPermission[nPerms + 2]; 
				else
				{
					return; // there may be more data in group table
				}
			} else
			{
				if (m_nPermissions < nPerms)
				{
					CPermission permission;
					permission.setUserID     (nextToken(line, ';'));
					permission.setObjectID   (nextToken(line, ';'));

					// ADD permission in older versions is converted to EDIT because ADD and EDIT
					// had identical behavior.
					SFInt32 perm = atoi(nextToken(line, ';'));
					if (m_version.earlier(3,5,0) && perm == PERMISSION_ADD)
						perm = PERMISSION_EDIT;
					permission.setPermission (perm);

					ASSERT(m_permissions);
					m_permissions[m_nPermissions++] = permission;
				}
			}
		} else if (!firstLine)
		{
			// done reading this table?
			break;
		}
	}

	if (!m_nPermissions && m_permissions)
	{
		delete [] m_permissions;
		m_permissions = NULL;
	}
	
	// and assign any special permissions
	if (m_nPermissions)
	{
		for (int i=0;i<m_nUsers;i++)
		{
			SFInt32 find = FindPermission(m_users[i], m_curObject);
			if (find != -1)
			{
				CPermission perm = m_permissions[find];
				
				if (m_version.eqEarlier(1,0,2))
				{
					m_users[i].setPermission(PRM_DEFAULT, perm.getPermission());
					if (!(perm.getObjectID() % "all"))
						m_users[i].setPermission(PRM_SPECIAL, perm.getPermission());
					else
						m_users[i].setPermission(PRM_SPECIAL, PERMISSION_NOTSPEC);
				} else
				{
					m_users[i].setPermission(PRM_SPECIAL, m_permissions[find].getPermission());
				}

			} else
			{
				ASSERT(m_version.later(1,0,2));
				m_users[i].setPermission(PRM_SPECIAL, PERMISSION_NOTSPEC);
			}
		}
	}

	return;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFInt32 CUserDatabase::FindPermission(const CPerson& user, const SFString& objectID) const
{
	// look for an existing record for the current calendar
	for (int i=0; i<m_nPermissions; i++)
	{
		CPermission *perm = &m_permissions[i];

		if (perm->getUserID() == user.getUserID())
		{
			if (perm->getObjectID() == objectID ||
					(m_version.eqEarlier(1,0,2) && 
						perm->getObjectID() % "all"))
			{
				return i;
			}
		}
	}
	return -1;
}

void CUserDatabase::RemovePermission(const SFString& userid)
{
	SFInt32 cnt = 0;
	for (int i=0;i<m_nPermissions;i++)
		if (m_permissions[i].getUserID() != userid)
			m_permissions[cnt++] = m_permissions[i];
	m_nPermissions = cnt;
}

void CUserDatabase::RemovePermission(SFInt32 rem)
{
	if (rem < 0 || rem > m_nPermissions-1)
		return;

	SFInt32 cnt = 0;
	for (int i=0;i<m_nPermissions;i++)
		if (i != rem)
			m_permissions[cnt++] = m_permissions[i];
	m_nPermissions = cnt;
}

void CUserDatabase::EditOrAddPermission(const CPerson& user)
{
	if (user.getPermission(PRM_DEFAULT) != user.getPermission(PRM_SPECIAL) &&
			user.getPermission(PRM_SPECIAL) != PERMISSION_NOTSPEC)
	{
		if (!m_permissions)
		{
			m_nPermissions = 0;
			// add more than we appear to need because if we need to add a new permission
			// we will not have to realloc - we only ever add a single new permission at
			// any given time so '2' is more than enough
			m_permissions = new CPermission[2];
		}

		SFInt32 find = FindPermission(user, m_curObject);
		if (find != -1)
		{
			// edit
			ASSERT(m_permissions[find].getUserID()   == user.getUserID());
			ASSERT(m_permissions[find].getObjectID() == m_curObject);
			m_permissions[find].setPermission(user.getPermission(PRM_SPECIAL));
		} else
		{
			// add
			m_permissions[m_nPermissions].setUserID    (user.getUserID());
			m_permissions[m_nPermissions].setObjectID  (m_curObject);
			m_permissions[m_nPermissions].setPermission(user.getPermission(PRM_SPECIAL));
			m_nPermissions++;
		}
	} else
	{
		// no special permission so remove any existing records for this user
		RemovePermission(FindPermission(user, m_curObject));
	}
}

//-------------------------------------------------------------------------
CPermission *CUserDatabase::getPermissionAt(SFInt32 index) const
{
	if (m_permissions && index >=0 && index < m_nPermissions)
		return &m_permissions[index];
	
	// should never happen
	ASSERT(0);
	return NULL;
}

