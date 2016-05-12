#ifndef _PEOPLEDATABASE_H
#define _PEOPLEDATABASE_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

class CPermission;
class CPerson;
class CProtectedObject;
class CUserGroup;
class CUserDatabase;

#include "utillib.h"
#include "fielddata.h"
#include "apps.h"
#include "usersettings.h"
#include "emailsettings.h"
#include "subsettings.h"
#include "loginsettings.h"
#include "registration.h"

typedef SFBool (*CONTACTAPPLYFUNC)(CPerson *qPerson, void *data);

//-------------------------------------------------------------------------
class CPeopleDatabase : public CSharedResource
{
protected:
	CFieldList     m_peopleFields;

public:
	SFInt32            m_FileFormat;
	SFString		   m_dataPath;
	SFString		   m_curObject;

	CPerson           *m_users;
	SFInt32	           m_nUsers;

	CUserGroup        *m_groups;
	SFInt32	           m_nGroups;

	// shorthand used to store these items for speed only
	SFString            m_superList;
	SFString            m_adminList;

public:
	//-------------------------------------------------------------------------
	// Construction / Destruction
					  CPeopleDatabase   (void);
	virtual          ~CPeopleDatabase   (void);

	        void      ReadDatabase        (const SFString& path, const SFString& objectName);
	virtual void      WriteDatabase       (void) = 0;

	virtual const CRegistration *getRegistration(void) const { return NULL; }
	        SFBool    showField           (const CFieldData *field) const;
	        SFInt32   applyToAll          (CONTACTAPPLYFUNC applyFunc, void *data, SFBool single=FALSE, SFBool writeBack=TRUE);

	//-------------------------------------------------------------------------
	// User Table
	virtual CPerson  *ReturnUserByID      (const SFString& userid);
	virtual CPerson  *ReturnUserByEmail   (const SFString& email);
	SFBool            FindUserByID        (CPerson& user, const SFString& userid) const;
	SFBool            FindUserByName      (CPerson& user, const SFString& nameOrNick) const;
	SFBool            FindSuperUser       (CPerson& user) const;
	void              UpdateUser          (CPerson& user, const SFTime& changeTime, SFInt32 pendingPerm=PERMISSION_NOTSPEC);
	SFInt32           CompactifyUsers     (void);

	SFInt32           getUserCount        (void) const;
	CPerson          *getUserAt           (SFInt32 index) const;

	SFInt32           nActiveUsers        (void) const;
	SFInt32           nDeletedUsers       (void) const;
	SFInt32           nPendingUsers       (void) const;
	SFInt32           nVisibleUsers       (const CPerson *validUser) const;

	SFString          getSuperList        (void) const { return m_superList; }
	SFString          getAdminList        (void) const { return m_adminList; }

	//-------------------------------------------------------------------------
	// Group Table
	SFBool            FindGroup           (CUserGroup& user, const SFString& groupid) const;
	SFString          ListAllGroups       (const SFString& userid, SFBool showPerms=FALSE) const;
	void              UpdateGroup         (CUserGroup& group);
	SFInt32           CompactifyGroups    (void);

	SFInt32           getGroupCount       (void) const;
	CUserGroup       *getGroupAt          (SFInt32 index) const;

	SFInt32           nActiveGroups       (void) const;
	SFInt32           nDeletedGroups      (void) const;
	
	SFBool			  inSameGroup         (const SFString& user1, const SFString& user2) const;

	//-------------------------------------------------------------------------
	// Editing Support
            void        ClearPersonFields     (void);
	const   CFieldList *getFieldList          (void) const;

	void              SortUserTable       (void);
	void              SortGroupTable      (void);

	void              EditOrAddUser       (const CPerson& userIn, const SFTime& changeTime);

protected:
	virtual void      LoadHeader          (SFString& contents) = 0;
	virtual void      LoadUserTable       (SFString& contents) = 0;
	virtual void      LoadRemainder       (SFString& contents) = 0;

public:
	void              RemoveUser          (const SFString& userid);
	void              DeleteUser          (const SFString& userid, SFBool del);

protected:
	//-------------------------------------------------------------------------
	// User Group related functions
	void              LoadGroupTable      (SFString& contents);
	void              EditOrAddGroup      (const CUserGroup& user);
	void              RemoveGroup         (const SFString& groupid);

	virtual void      EditOrAddPermission (const CPerson& user) {};
	virtual void      RemovePermission    (const SFString& userid) {};

private:
	//-------------------------------------------------------------------------
	// Cannot copy this database
	                  CPeopleDatabase     (const CPeopleDatabase& db);
	CPeopleDatabase&  operator=           (const CPeopleDatabase& db);
};

//-------------------------------------------------------------------------
class CUserDatabase : public CPeopleDatabase
{
protected:
	CEmailSettings      m_emailSettings;
	CUserSettings		m_userSettings;
	CSubSettings        m_subSettings;
	CLoginSettings      m_loginSettings;
	CRegistration       m_registration;

	CProtectedObject   *m_objects;
	SFInt32				m_nObjects;

	CPermission        *m_permissions;
	SFInt32				m_nPermissions;

	SFBool              m_showHints;

public:
	//-------------------------------------------------------------------------
	// Construction / Destruction
					  CUserDatabase         (void);
					 ~CUserDatabase         (void);

	//-------------------------------------------------------------------------
	void              WriteDatabase         (void);
	
	//-------------------------------------------------------------------------
	// Object Table
	SFBool            FindObjectByID        (CProtectedObject& object, const SFString& objectid, SFBool type=FALSE) const;

	SFInt32           getObjectCount        (void) const;
	CProtectedObject *getObjectAt           (SFInt32 index) const;

	SFInt32           nVisibleObjects       (void) const;
	SFInt32           nAccessibleObjects    (void) const;
	SFInt32           nDeletedObjects       (void) const;
	SFInt32           nHiddenObjects        (void) const;
	SFInt32           nLicensesLeft         (void) const;

	SFBool            isVisible             (const SFString& objectID, const CPerson *user) const;

	//-------------------------------------------------------------------------
	// Permission Table
	SFBool            ValidateUser          (CPerson& user, const SFString& userid, const SFString& curObject) const;
	SFBool            hasHigherPermission   (const CPerson& user1, const SFString& user2Name) const;
	SFInt32           getPermissionCount    (void) const;
	CPermission      *getPermissionAt       (SFInt32 index) const;
	SFBool            hasSubscribeGroups    (const CPerson *person) const;
	SFInt32           GetHighestGroupPerm   (const SFString& userid) const;

	//-------------------------------------------------------------------------
	// User Settings
	      CUserSettings *getUserSettingsEdit(void)       { return &m_userSettings; }
	const CUserSettings *getUserSettings    (void) const { return &m_userSettings; }

protected:
	//-------------------------------------------------------------------------
	// The file's header
	void              LoadHeader            (SFString& contents);

	//-------------------------------------------------------------------------
	// User related functions
	void              LoadUserTable         (SFString& contents);
	void              LoadRemainder         (SFString& contents);

	//-------------------------------------------------------------------------
	// Object related functions
	void              LoadObjectTable       (void);

	//-------------------------------------------------------------------------
	// Permission Table related items
	void              LoadPermissionTable   (SFString& contents);
	void              EditOrAddPermission   (const CPerson& user);
	void              RemovePermission      (const SFString& userid);
	void              RemovePermission      (SFInt32 rem);
	SFInt32           QueryPermission       (const CPerson& user, const CProtectedObject& ob) const;
	SFInt32           FindPermission        (const CPerson& user, const SFString& objectID) const;

	//-------------------------------------------------------------------------
	// Date display settings
	void              LoadDateSettings      (SFString& contents);
public:
	SFString          getAsciiDateSettings  (void) const;

protected:
	//-------------------------------------------------------------------------
	// Mail related settings
	void              LoadMailTable         (SFString& contents);

	//-------------------------------------------------------------------------
	// Login settings
	void              LoadLoginSettings     (SFString& contents);

	//-------------------------------------------------------------------------
	// Submission settings
	void              LoadSubmissionSettings(SFString& contents);

	//-------------------------------------------------------------------------
	// Submission settings
	void              LoadUnusedSettings    (SFString& contents);

	//-------------------------------------------------------------------------
	// Warning settings
	void              LoadUserSettings      (SFString& contents);
public:
		void          upgradeFrom401        (CUserDatabase *userDB);

private:
	friend  void     createUserFile         (const SFString& userFile, const SFString& userName);

public:
	//-------------------------------------------------------------------------
	// Licensing (available for top level (absolute data root) level only)
	SFBool    registerUser          (const CRegistration& helper, const SFString& newPWifNeeded);
	SFBool    mayAddLicense         (void) const;

	//-------------------------------------------------------------------------
	// Registration
	const CRegistration *getRegistration    (void) const { return &m_registration; }

	//-------------------------------------------------------------------------
	// Mail Table
	SFBool    mailEnabled           (void) const;
	SFBool    mailDisabled          (void) const;
	SFBool    mailMethod            (void) const;
	const CEmailSettings *getEmailSettings  (void) const { return &m_emailSettings; }

	//-------------------------------------------------------------------------
	// Submission Settings
	      CSubSettings *getSubSettingsEdit  (void)       { return &m_subSettings; }
	const CSubSettings *getSubSettings      (void) const { return &m_subSettings; }

	//-------------------------------------------------------------------------
	// Remote Login (top level only)
	SFInt32   getRemotePerm         (void) const;
	void      setRemotePerm         (SFInt32 perm);
	SFInt32   getLoginTimeout       (void) const;
	void      setLoginTimeout       (SFInt32 mins);
	SFInt32   allowRemote           (void) const;
	void      setAllowRemote        (SFInt32 allow);
	const CLoginSettings *getLoginSettings(void) const { return &m_loginSettings; }

private:
	//-------------------------------------------------------------------------
	// Cannot copy this database
	CUserDatabase         (const CUserDatabase& db);
	CUserDatabase&    operator=             (const CUserDatabase& db);
};

//-------------------------------------------------------------------------
#define LOGIN_METHOD_CGI          0
#define LOGIN_METHOD_REMOTE       1
#define LOGIN_METHOD_SPECIFY      2
#define LOGIN_METHOD_SPECIFYADMIN 3

//-------------------------------------------------------------------------
inline SFInt32 CUserDatabase::getPermissionCount(void) const
{
	return m_nPermissions;
}

//-------------------------------------------------------------------------
inline SFInt32 CPeopleDatabase::getGroupCount(void) const
{
	return m_nGroups;
}

//-------------------------------------------------------------------------
inline SFInt32 CPeopleDatabase::getUserCount(void) const
{
	return m_nUsers;
}

//-------------------------------------------------------------------------
inline SFInt32 CUserDatabase::getObjectCount(void) const
{
	return m_nObjects;
}

//-------------------------------------------------------------------------
inline SFInt32 CUserDatabase::allowRemote(void) const
{
	return m_loginSettings.m_allowRemote;
}

//-------------------------------------------------------------------------
inline void CUserDatabase::setAllowRemote(SFInt32 allow)
{
	m_loginSettings.m_allowRemote = allow;
}

//-------------------------------------------------------------------------
inline SFInt32 CUserDatabase::getRemotePerm(void) const
{
	return m_loginSettings.m_remotePerm;
}

//-------------------------------------------------------------------------
inline SFInt32 CUserDatabase::getLoginTimeout(void) const
{
	return m_loginSettings.m_loginTimeout;
}

//-------------------------------------------------------------------------
inline void CUserDatabase::setRemotePerm(SFInt32 perm)
{
	m_loginSettings.m_remotePerm = perm;
}

//-------------------------------------------------------------------------
inline void CUserDatabase::setLoginTimeout(SFInt32 mins)
{
	m_loginSettings.m_loginTimeout = mins;
}

//-------------------------------------------------------------------------
inline SFInt32 CUserDatabase::nVisibleObjects(void) const
{
	return m_nObjects - nDeletedObjects() - nHiddenObjects();
}

//-------------------------------------------------------------------------
inline SFInt32 CUserDatabase::nAccessibleObjects(void) const
{
	return m_nObjects - nDeletedObjects();
}

//-------------------------------------------------------------------------
inline SFBool isComment(const SFString& line)
{
	return (!line.GetLength() || line[0]==';' || line[0]=='[');
}

extern SFString nextTokenClearDecrypt (SFString& contents, char token);
extern SFString getRest               (SFInt32 exp, SFInt32 type, SFInt32 id, const SFString& def, SFInt32 req);

#define UPRE     SFString("id_")
#define UUPRE(n) SFString("&userid=id_") + asString(n)

#endif
