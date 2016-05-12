/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "peopledatabase.h"
#include "protectedobject.h"
#include "usergroup.h"
#include "person.h"

//---------------------------------------------------------------------------------------
void CPeopleDatabase::LoadGroupTable(SFString& contents)
{
	if (m_version.eqEarlier(1,0,2))
		return;

	// clean up first
	if (m_groups)
	{
		delete [] m_groups;
		m_groups= NULL;
	}
	m_nGroups = 0;
		
	ASSERT(!m_groups);
	ASSERT(m_nGroups==0);

	SFBool firstLine = TRUE;
	SFInt32 nGroups = 0;
		
	while (!contents.IsEmpty())
	{
		SFString line = nextTokenClearDecrypt(contents, '\n');
		if (!isComment(line)) // is this line a comment?
		{
			if (firstLine)
			{
				firstLine = FALSE;
				nGroups = atoi((const char *)line);
				if (nGroups)
				{
					m_groups = new CUserGroup[nGroups + 2]; 
					for (int i=0;i<nGroups+2;i++)
						m_groups[i].setPeopleDB(this);
				} else
				{
					return; // there are no more data
				}
			} else
			{
				if (m_nGroups < nGroups)
				{
					ASSERT(m_groups);
					CUserGroup *group = &m_groups[m_nGroups++];
					group->fromAscii(line, m_version);
					if (group->getPermission() >= PERMISSION_ADMIN)
						m_adminList += (group->getName() + ",");
				}
			}
		} else if (!firstLine)
		{
			// done reading this table?
			break;
		}
	}
	
	if (!m_nGroups && m_groups)
	{
		if (m_groups)
			delete [] m_groups;
		m_groups = NULL;
	}
	
	return;
}

//---------------------------------------------------------------------------------------
static int sortByGroupID(const void *rr1, const void *rr2)
{
	SFString n1 = ((CUserGroup *)rr1)->getName();
	SFString n2 = ((CUserGroup *)rr2)->getName();
	return (int)n1.ICompare(n2);
}

//---------------------------------------------------------------------------------------
void CPeopleDatabase::SortGroupTable(void)
{
	if (m_nGroups>1 && m_groups)
		qsort(m_groups, m_nGroups, sizeof(CUserGroup), sortByGroupID);
}

//---------------------------------------------------------------------------------------
SFInt32 CPeopleDatabase::nDeletedGroups(void) const
{
	if (!m_groups)
		return 0;

	SFInt32 cnt=0;
	int i=0;
	for (i=0;i<m_nGroups;i++)
		if (m_groups[i].isDeleted())
			cnt++;

	return cnt;
}

//---------------------------------------------------------------------------------------
SFInt32 CPeopleDatabase::nActiveGroups(void) const
{
	return (m_nGroups - nDeletedGroups());
}

//---------------------------------------------------------------------------------------
CUserGroup *CPeopleDatabase::getGroupAt(SFInt32 index) const
{
	if (m_groups && index >=0 && index < m_nGroups)
	{
		ASSERT(m_groups[index].getPeopleDB() == this);
		return &m_groups[index];
	}
	return NULL;
}

//---------------------------------------------------------------------------------------
SFBool CPeopleDatabase::FindGroup(CUserGroup& group, const SFString& groupid) const
{
	// This linear search would be better as binary search, particularly as nGroups grows
	int i=0;
	for (i=0;i<m_nGroups;i++)
	{
		CUserGroup *grp = &m_groups[i];
		if (grp->getName() == groupid)
		{
			group = *grp;
			ASSERT(group.getPeopleDB() == this);
			return TRUE;
		}
	}
	return FALSE;
}

//---------------------------------------------------------------------------------------
void CPeopleDatabase::EditOrAddGroup(const CUserGroup& group)
{
	//--------------------------------------------------------------------
	// Create the table if it does not already exists
	if (!m_groups)
	{
		m_nGroups = 0;
		// add more than we appear to need because if we need to add a new group
		// we will not have to realloc - we only ever add a single new group at
		// any given time so '2' is more than enough
		m_groups = new CUserGroup[2];
		for (int i=0;i<2;i++)
			m_groups[i].setPeopleDB(this);
	}

	// add the group to the group table (if not already there)
	SFBool found = FALSE;
	int i=0;
	for (i=0;i<m_nGroups;i++)
	{
		CUserGroup *grp = &m_groups[i];
		if (grp->getName() == group.getName())
		{
			*grp = group;
			ASSERT(grp->getPeopleDB() == this);
			found = TRUE;
		}
	}

	// ....or add
	if (!found)
	{
		m_groups[m_nGroups] = group;
		ASSERT(m_groups[m_nGroups].getPeopleDB() == this);
		m_nGroups++;
	}
}

//---------------------------------------------------------------------------------------
// list all ****active**** groups a given user belongs to
SFString CPeopleDatabase::ListAllGroups(const SFString& userid, SFBool showPerms) const
{
	// A non-user may not belong to any group obviously
	if (userid.IsEmpty())
		return EMPTY;

	SFString ret;
	for (SFInt32 i=0;i<m_nGroups;i++)
	{
		if (!m_groups[i].isDeleted() && m_groups[i].getMembers().ContainsExact(userid, CH_COMMA))
		{
			if (!ret.IsEmpty())
				ret += ",";
			ret += m_groups[i].getName();
			if (showPerms && m_groups[i].getPermission() > PERMISSION_NOTSPEC)
				ret += (" (" + CPermission::getPermissionStrShort(m_groups[i].getPermission()) + ")");
		}
	}

	return ret;
}

//---------------------------------------------------------------------------------------
SFBool CPeopleDatabase::inSameGroup(const SFString& user1, const SFString& user2) const
{
	if (user1 == user2)
		return TRUE;
		
	SFString user1Groups = ListAllGroups(user1);
	SFString user2Groups = ListAllGroups(user2);
	
	// If one or the other of the two users is not in any
	// group then he/she cannot possibly be in the same groups
	// as the other user
	if (user1Groups.IsEmpty() || user2Groups.IsEmpty())
		return FALSE;

	SFInt32 l1 = user1Groups.GetLength();
	SFInt32 l2 = user2Groups.GetLength();
	
	SFString test, reference;
	if (l1 > l2)
	{ test = user2Groups; reference = user1Groups; }
	else
	{ test = user1Groups; reference = user2Groups; }
	
	// Spin through the shorter of the two lists testing for
	// exact matches in the group ids
	while (!test.IsEmpty())
	{
		SFString grp = Strip(nextTokenClear(test, CH_COMMA), ' ');
		if (reference.ContainsExact(grp, CH_COMMA))
			return TRUE;
	}

	// for both empty case
	return (test == reference);
}

//---------------------------------------------------------------------------------------
void CPeopleDatabase::UpdateGroup(CUserGroup& group)
{
	EditOrAddGroup(group);
	WriteDatabase();
}

//---------------------------------------------------------------------------------------
SFInt32 CPeopleDatabase::CompactifyGroups(void)
{
	SFString *removes = new SFString[m_nGroups];
	SFInt32 nRemoves = 0;
	int i=0;
	for (i=0;i<m_nGroups;i++)
	{
		if (m_groups[i].shouldRemove())
			removes[nRemoves++] = m_groups[i].getName();
	}
	ASSERT(nRemoves <= m_nGroups);
	for (i=0;i<nRemoves;i++)
		RemoveGroup(removes[i]);
	
	WriteDatabase();

	if (removes)
		delete [] removes;

	return nRemoves;
}

//---------------------------------------------------------------------------------------
void CPeopleDatabase::RemoveGroup(const SFString& groupid)
{
	// Remove the old record from the user table
	SFInt32 cnt = 0;
	int i=0;
	for (i=0;i<m_nGroups;i++)
	{
		if (m_groups[i].getName() != groupid)
			m_groups[cnt++] = m_groups[i];
	}
	m_nGroups = cnt;
}

//---------------------------------------------------------------------------------------
// Find highest permission in any group that this user belongs to
SFInt32 CUserDatabase::GetHighestGroupPerm(const SFString& userid) const
{
	if (!getUserSettings()->getAllowGroupPerms())
		return PERMISSION_NOTSPEC;

	SFInt32 ret = PERMISSION_NOTSPEC;
	for (SFInt32 i=0;i<m_nGroups;i++)
		if (!m_groups[i].isDeleted() && m_groups[i].getMembers().ContainsExact(userid, CH_COMMA))
			ret = MAX(ret, m_groups[i].getPermission());

	return ret;
}

//---------------------------------------------------------------------------------------
SFBool CUserDatabase::hasSubscribeGroups(const CPerson *person) const
{
	for (int i=0;i<m_nGroups;i++)
	{
		const CUserGroup *group = &m_groups[i];
		if (group->userMaySubscribe(person, getUserSettings()->getAllowGroupPerms()))
			return TRUE;
	}
	return FALSE;
}

