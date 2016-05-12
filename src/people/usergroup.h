#ifndef _USERGROUP_H
#define _USERGROUP_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "editableobject.h"

class CPeopleDatabase;
class CPerson;

//-------------------------------------------------------
// Data needed to represent a group of users
//-------------------------------------------------------
class CUserGroup : public CEditableObject
{
private:
	      SFString         m_groupName;
	      SFString         m_description;
	      SFString         m_members;
	      SFInt32          m_permission;
	      SFBool           m_subscribe;
	      SFBool           m_deleted;
	      SFBool           m_expanded;
	const CPeopleDatabase *m_peopleDB;

public:
			    CUserGroup      (const CPeopleDatabase *db=NULL);

			    CUserGroup      (const CUserGroup& data) { Copy(data);               }
    CUserGroup& operator=       (const CUserGroup& data) { Copy(data); return *this; }
			   ~CUserGroup      (void)                   {                           }

	SFString getName            (void) const { return m_groupName;   }
	SFString getDescription     (void) const { return m_description; }
	SFString getMembers         (void) const { return m_members;     }
	SFInt32  getPermission      (void) const { return m_permission;  }
	SFBool   maySubscribe       (void) const { return m_subscribe;   }
	SFBool   isDeleted          (void) const { return m_deleted;     }
	SFBool   isExpanded         (void) const { return m_expanded;    }
	SFInt32  getMemberCount     (void) const;
	SFBool   shouldRemove       (void) const { return m_deleted == TRASHCAN_REMOVE; }

	void     setName            (const SFString &groupName)   { m_groupName   = groupName; }
	void     setDescription     (const SFString &description) { m_description = description; }
	void     setMembers         (const SFString &members);
	void     setPermission      (SFInt32 perm)                { m_permission  = perm; }
	void     setSubscribe       (SFBool sub)                  { m_subscribe   = sub; }
	void     setDeleted         (SFBool del)                  { m_deleted     = del; }
	void     setExpanded        (SFBool exp)                  { m_expanded    = exp; }

	SFBool   userMaySubscribe   (const CPerson *person, SFBool groupsPerms) const;
	void     removeMember       (const SFString& removeName);
	void     addMember          (const SFString& newMember);
	SFBool   isMember           (const SFString& qMember) const;

	SFString getFieldValueByID  (SFInt32 id,                const SFString& def) const;
	void     setFieldValueByID  (SFInt32 id,                const SFString& value);
	SFInt32  getFieldID         (const SFString& fieldName) const;

	SFString toAscii            (const SFString& format) const;
	void     fromAscii          (const SFString& in, const CVersion& version);
	void     fromAscii_4_0_1    (const SFString& in, const CVersion& version);

	const CPeopleDatabase *getPeopleDB(void) const                { return m_peopleDB; }
	void                   setPeopleDB(const CPeopleDatabase *db) { m_peopleDB = db;   }

private:
	void Init(void)
		{
//			m_groupName      = "";
//			m_description    = "";
//			m_members        = "";
//			m_unused         = "";
			m_deleted        = FALSE;
			m_expanded       = TRUE;
			m_permission     = PERMISSION_NOTSPEC;
			m_subscribe      = FALSE;
			m_peopleDB       = NULL;
		}

	void Copy(const CUserGroup& data)
		{
//			m_unused         = data.m_unused;
			m_groupName      = data.m_groupName;
			m_description    = data.m_description;
			m_members        = data.m_members;
			m_deleted        = data.m_deleted;
			m_expanded       = data.m_expanded;
			m_permission     = data.m_permission;
			m_subscribe      = data.m_subscribe;

			if (data.m_peopleDB) // never update to NULL
			m_peopleDB       = data.m_peopleDB;
		}
};

//---------------------------------------------------------------------------------------
inline void CUserGroup::removeMember(const SFString& removeName)
{
	if (isMember(removeName)) m_members.ReplaceExact(removeName, EMPTY, CH_COMMA);
}

//---------------------------------------------------------------------------------------
inline void CUserGroup::addMember(const SFString& newMember)
{
	if (!isMember(newMember)) m_members += ", " + newMember;
}

//---------------------------------------------------------------------------------------
inline SFBool CUserGroup::isMember(const SFString& qMember) const
{
	return m_members.ContainsExact(qMember, CH_COMMA);
}

#endif
