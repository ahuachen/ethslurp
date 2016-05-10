#ifndef _PERMISSION_H
#define _PERMISSION_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#define PRM_APPARENT 0
#define PRM_DEFAULT  1
#define PRM_SPECIAL  2
#define PRM_EDIT     3

// Do not change the ordering the permissions are hierarchical
#define PERMISSION_NOTSPEC -1
#define PERMISSION_NONE     0
#define PERMISSION_VIEW     1
#define PERMISSION_ADD		2
#define PERMISSION_EDIT     3
#define PERMISSION_ADMIN    4
#define PERMISSION_SUPER    5

//--------------------------------------------------------------------------------
extern const SFString perms[];
extern const SFString permsPrompts[];

//--------------------------------------------------------------------------------
class CPermission
{
private:
	SFString m_userid;
	SFString m_objectid;
	SFInt32  m_permission;

public:
	CPermission(void)
		{
			Init();
		}
	CPermission(const CPermission& data)
		{
			Copy(data);
		}
	CPermission& operator=(const CPermission& data)
		{
			Copy(data);
			return *this;
		}

	SFString getUserID       (void) const { return m_userid; }
	SFString getObjectID     (void) const { return m_objectid; }
	SFInt32  getPermission   (void) const { return m_permission; }
	SFString getPermissionStr(void) const;

	void     setUserID     (const SFString& userid)   { m_userid = userid; }
	void     setObjectID   (const SFString& objectid) { m_objectid = objectid; }
	void     setPermission (SFInt32 perm)             { m_permission = perm; }

	SFString toAscii(void) const
		{
			return m_userid + ";" + 
							m_objectid + ";" + 
							::asString(m_permission);
		}

	static SFString getPermissionStrShort(SFBool type);
	static SFString getPermissionStrLong (SFBool type);

private:
	void Init(void)
		{
//			m_userid     = "";
//			m_objectid   = "";
			m_permission = PERMISSION_VIEW;
		}

	void Copy(const CPermission& data)
		{
			m_userid     = data.m_userid;
			m_objectid   = data.m_objectid;
			m_permission = data.m_permission;
		}
};

#endif
