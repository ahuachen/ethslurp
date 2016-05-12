/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "peopledatabase.h"
#include "person.h"
#include "usergroup.h"
#include "protectedobject.h"

//-------------------------------------------------------------------------
void CUserDatabase::LoadUserTable(SFString& contents)
{
	SFBool firstLine = TRUE;
	
	// Rebuild each time
	m_nUsers = 0;
	if (m_users)
	{
		delete [] m_users;
		m_users = NULL;
	}

	SFInt32 nUsers=0;
	while (!contents.IsEmpty())
	{
		SFString line = nextTokenClearDecrypt(contents, '\n');

		// Since we read the header in LoadHeader and we read until the
		// line just previous to the 'nUsers' line we assert that either
		// we are not on the first line or we are not on a comment line.
		ASSERT(!firstLine || !isComment(line));		

		if (!isComment(line)) // is this line a comment?
		{
			if (firstLine)
			{
				firstLine = FALSE;
				nUsers = atoi((const char *)line);
				// add more than we appear to need because if we need to add a new user
				// we will not have to realloc - the most we add is two new users (admin and
				// user in site license - lets get three extra slots just in case.
				if (nUsers)
				{
					m_users = new CPerson[nUsers + 3];  // add three extra to use when adding new users
					for (int i=0;i<nUsers+3;i++)
						m_users[i].setPeopleDB(this);
				}
				else
				{
					// should not happen
					ASSERT(0); // there should always be at least one user (the SUPER user)
					return;
				}
			} else
			{
				if (!line.endsWith(';'))
					line += ";";
				if (m_nUsers < nUsers)
				{
					ASSERT(m_version.isSet());
					ASSERT(m_users);
					line.ReplaceAll("\n", "");
					line.ReplaceAll("\r", "");

					CPerson *person = &m_users[m_nUsers++];
					person->fromAscii(line, m_version);
					     if (person->isRawSuper()) m_superList  =  person->getUserID(FALSE);
					else if (person->isRawAdmin()) m_adminList += (person->getUserID(FALSE) + ",");
				}
			}
		} else if (!firstLine)
		{
			// done reading this table?
			break;
		}
	}

	return;
}

//-------------------------------------------------------------------------
void CPeopleDatabase::SortUserTable(void)
{
	if (m_nUsers>1 && m_users)
		qsort(m_users, m_nUsers, sizeof(CPerson), sortByUserID);
}

//-------------------------------------------------------------------------
void CPeopleDatabase::EditOrAddUser(const CPerson& userIn, const SFTime& changeTime)
{
	CPerson user = userIn;
	if (changeTime != earliestDate)
		user.setLastEditDate(changeTime);
#ifdef TESTING
	if (getInt32("testAge", FALSE))
		user.setLastEditDate(SFTime(2005,10,1,12,0,0));
#endif

	//--------------------------------------------------------------------
	// Create the table if it does not already exists
	if (!m_users)
	{
		m_nUsers = 0;
		// add more than we appear to need because if we need to add a new user
		// we will not have to realloc - the most we add is two new users (admin and
		// user in site license - lets get three extra slots just in case.
		m_users = new CPerson[3];
		for (int i=0;i<3;i++)
			m_users[i].setPeopleDB(this);
	}

	// add the user to the user table (if not already there)
	SFBool found = FALSE;
	int i=0;
	for (i=0;i<m_nUsers;i++)
	{
		CPerson *person = &m_users[i];
		if (person->getUserID() == user.getUserID())
		{
			*person = user;
			ASSERT(person->getPeopleDB() == this);
			found = TRUE;
		}
	}

	// ....or add
	if (!found)
	{
		user.setCreateDate(Now());
		m_users[m_nUsers] = user;
		ASSERT(m_users[m_nUsers].getPeopleDB() == this);
		m_nUsers++;
	}
}

//-------------------------------------------------------------------------
void CPeopleDatabase::UpdateUser(CPerson& user, const SFTime& changeTime, SFInt32 pendingPerm)
{
	user.setPendingPerm(pendingPerm);
	EditOrAddUser(user, changeTime);
	EditOrAddPermission(user);
	WriteDatabase();
}

//-------------------------------------------------------------------------
void CPeopleDatabase::DeleteUser(const SFString& userid, SFBool del)
{
	int i=0;
	for (i=0;i<m_nUsers;i++)
	{
		if (m_users[i].getUserID() == userid)
			m_users[i].setDeleted(del);
	}
	WriteDatabase();
}

//-------------------------------------------------------------------------
void CPeopleDatabase::RemoveUser(const SFString& userid)
{
	// Remove the old record from the user table
	SFInt32 cnt = 0;
	int i=0;
	for (i=0;i<m_nUsers;i++)
	{
		if (m_users[i].getUserID() != userid)
			m_users[cnt++] = m_users[i];
	}
	m_nUsers = cnt;

	// And the permissions table
	RemovePermission(userid);

	// And group memberships
	for (i=0;i<m_nGroups;i++)
		m_groups[i].removeMember(userid);
}

//-------------------------------------------------------------------------
SFInt32 CPeopleDatabase::CompactifyUsers(void)
{
	if (!m_nUsers)
		return 0;

	SFString *removes = new SFString[m_nUsers];
	SFInt32 nRemoves = 0;
	int i=0;
	for (i=0;i<m_nUsers;i++)
	{
		if (m_users[i].shouldRemove())
			removes[nRemoves++] = m_users[i].getUserID();
	}
	ASSERT(nRemoves <= m_nUsers);
	for (i=0;i<nRemoves;i++)
		RemoveUser(removes[i]);
	WriteDatabase();

	if (removes)
		delete [] removes;

	return nRemoves;
}

//-------------------------------------------------------------------------
SFBool CPeopleDatabase::FindUserByName(CPerson& user, const SFString& nameOrNick) const
{
	int i=0;
	for (i=0;i<m_nUsers;i++)
	{
		CPerson *person = &m_users[i];
		if (person->getFullName() == nameOrNick || person->getNickname() == nameOrNick)
		{
			user = *person;
			ASSERT(user.getPeopleDB() == this);
			return TRUE;
		}
	}
	return FALSE;
}

//-------------------------------------------------------------------------
SFBool CPeopleDatabase::FindSuperUser(CPerson& user) const
{
	int i=0;
	for (i=0;i<m_nUsers;i++)
	{
		CPerson *person = &m_users[i];
		if (person->isSuper() && person->getUserID() != "system")
		{
			user = *person;
			return TRUE;
		}
	}
	return FALSE;
}

//-------------------------------------------------------------------------
CPerson *CPeopleDatabase::ReturnUserByID(const SFString& userid)
{
	if (userid.IsEmpty())
		return NULL;

	// This linear search would be better as binary search, particularly as nUsers grows
	for (int i=0;i<m_nUsers;i++)
	{
		CPerson *person = &m_users[i];
		if (person->getUserID(FALSE) % userid)
			return person;
	}

	return NULL;
}

//-------------------------------------------------------------------------
CPerson *CPeopleDatabase::ReturnUserByEmail(const SFString& email)
{
	if (email.IsEmpty())
		return NULL;

	// This linear search would be better as binary search, particularly as nUsers grows
	for (int i=0;i<m_nUsers;i++)
	{
		CPerson *person = &m_users[i];
		if (person->getMainEmailNoLink() % email)
			return person;
	}

	return NULL;
}

//-------------------------------------------------------------------------
SFBool CPeopleDatabase::FindUserByID(CPerson& user, const SFString& userid) const
{
	// This linear search would be better as binary search, particularly as nUsers grows
	int i=0;
	for (i=0;i<m_nUsers;i++)
	{
		CPerson *person = &m_users[i];
		if (person->getUserID(FALSE) % userid)
		{
			user = *person;
			ASSERT(person->getPeopleDB() == this);
			return TRUE;
		}
	}
	return FALSE;
}

//-------------------------------------------------------------------------
SFInt32 CUserDatabase::QueryPermission(const CPerson& user, const CProtectedObject& object) const
{
	// returns the greater of the user's default permission, the calendars default
	// permission and the special permission record (if found)
	if (getUserSettings()->getAllowDiffPerms())
	{
		SFInt32 find = FindPermission(user, object.getObjectID());
		if (find != -1)
			return m_permissions[find].getPermission();
	}

	return MAX(user.getPermission(PRM_DEFAULT), object.getDefaultPerm());
}

//-------------------------------------------------------------------------
SFBool CUserDatabase::ValidateUser(CPerson& user, const SFString& userid, const SFString& curObject) const
{
	// This function assigns APPARENT permission based user's default permission, special per calendar
	// permission, user's group permission, remote login permission, default calendar permission
	// which applies.
	//
	// Rule is:
	//		per calendar special permission first,
	//		user's default permission or remote login permission next,
	//			unless user's group permission is higher then user's group permission,
	//		finally - calendar's default permission.
	//		finally - finally - impose minimum calendar permission (if present)
	CProtectedObject object;

	SFBool  obFound   = FindObjectByID (object, curObject, HIDDEN_ALSO);
	SFBool  userFound = FindUserByID   (user, userid);
	SFInt32 groupPerm = GetHighestGroupPerm(userid);

	SFBool  retVal    = -1;
	SFInt32 permToSet = PERMISSION_NONE;

	if (userFound && obFound)
	{
		// a user and an object were found - get the permission
		// first from the user (including special permissions for this
		// calendar) and then, if special permission was not applied
		// apply the group permission

		SFInt32 defPerm = user.getPermission(PRM_DEFAULT);
		SFInt32 newPerm = QueryPermission(user, object);
		if (defPerm != newPerm)
			permToSet = newPerm;
		else if (groupPerm > defPerm)
			permToSet = groupPerm;
		else
			permToSet = defPerm;

		user.setInternal(TRUE);
		ASSERT(user.getPeopleDB() == this);
		retVal = TRUE;

	} else if (!userid.IsEmpty() && allowRemote())
	{
		user.setUserID(userid);
		permToSet = getRemotePerm();
		if (permToSet < groupPerm)
			permToSet = groupPerm;
//		ASSERT(user.getPeopleDB() == this);
		retVal = TRUE;

	} else
	{
		// If the user or object were not found and the user was not assigned because of remote login
		user.setPeopleDB(this);
		permToSet = PERMISSION_VIEW;
		if (obFound)
			permToSet = object.getDefaultPerm();
		retVal = FALSE;
	}

	ASSERT(permToSet != BAD_NUMBER);
	// If calendar's minimum permission is greater than the apparent permission we fall back to 
	// the fallback permission (or calendar default if that's not set).  min_perm means the user
	// must have at least this permission to be granted any permission.
	if (obFound && object.getMinPerm() > permToSet)
		permToSet = MIN(permToSet, object.getFallback());
	user.setPermission(PRM_APPARENT, permToSet);

	ASSERT(retVal != BAD_NUMBER);
	return retVal;
}

//-------------------------------------------------------------------------
SFInt32 CPeopleDatabase::nPendingUsers(void) const
{
	ASSERT(m_users || m_nUsers == 0);
	
	SFInt32 cnt=0;
	int i=0;
	for (i=0;i<m_nUsers;i++)
		if (m_users[i].isPending() && !m_users[i].isDeleted())
			cnt++;

	return cnt;
}

//-------------------------------------------------------------------------
SFInt32 CPeopleDatabase::nDeletedUsers(void) const
{
	ASSERT(m_users || m_nUsers == 0);
	
	SFInt32 cnt=0;
	int i=0;
	for (i=0;i<m_nUsers;i++)
		if (m_users[i].isDeleted())
			cnt++;

	return cnt;
}

//-------------------------------------------------------------------------
SFInt32 CPeopleDatabase::nActiveUsers(void) const
{
	return (m_nUsers - nDeletedUsers() - nPendingUsers());
}

//-------------------------------------------------------------------------
SFInt32 CPeopleDatabase::nVisibleUsers(const CPerson *validUser) const
{
	//
	// Called from User Manager only this function shows the number of users that
	// are visible to the given user.  The user is either the real super user or
	// he/she is a trusted admin.  The real super user sees everyone.  The trusted 
	// admin sees everyone except the real super user
	//
	return (validUser->isSuper() ? nActiveUsers() : nActiveUsers() - 1);
}

//-------------------------------------------------------------------------
CPerson *CPeopleDatabase::getUserAt(SFInt32 index) const
{
	if (m_users && index >=0 && index < m_nUsers)
		return &m_users[index];
	return NULL;
}

//-------------------------------------------------------------------------
SFBool CUserDatabase::isVisible(const SFString& folderName, const CPerson *user) const
{
	CProtectedObject object;
	if (!FindObjectByID(object, folderName, HIDDEN_ALSO))
		return FALSE;
	
	// Deleted are invisible (to calendar dropdown and merges)
	if (object.isDeleted()) return FALSE;

	// Unhidden calendars are also visible (obviously).
	if (!object.isHidden()) return TRUE;

	// All that is left are hidden calendars
	ASSERT(user && object.isHidden());

	// Get the user's apparent permissions
	CPerson person;
	if (!ValidateUser(person, user->getUserID(), folderName))
		return FALSE;

	// Only admins may see hidden calendars
	return person.isAdmin();
}
