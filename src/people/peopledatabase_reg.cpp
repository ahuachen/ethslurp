/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "peopledatabase.h"
#include "person.h"
#include "usergroup.h"

//-------------------------------------------------------------------------
SFBool CUserDatabase::registerUser(const CRegistration& helper, const SFString& newPWifNeeded)
{
	m_registration           = helper;
	m_registration.m_date    = Now();
	m_registration.m_todo    = (helper.m_userid % "demo" ? TRUE  : ((crackApps(helper.m_code) & APP_TODOLIST)    != 0));
	m_registration.m_contact = (helper.m_userid % "demo" ? TRUE  : ((crackApps(helper.m_code) & APP_CONTACTLIST) != 0));
	m_registration.m_branded = (helper.m_userid % "demo" ? TRUE  : ((crackApps(helper.m_code) & APP_BRANDED)     != 0));

	// update the super user
	CPerson oldSuper;
	FindSuperUser(oldSuper); // May not find - may be empty - its ok

	SFString oldPass  = oldSuper.getPassword();
	SFString oldName  = oldSuper.getFullName();
	SFString oldEmail = oldSuper.getOffice().getEmail();
	SFString oldPhone = oldSuper.getOffice().getPhone();

	SFString newPass, newName, newEmail, newPhone;
	newPass  = (oldPass % "demo" || oldPass.IsEmpty() ? newPWifNeeded : oldPass);
	newName  = (!helper.m_name.IsEmpty () ? helper.m_name  : (oldName.IsEmpty() ? LoadStringGH(IDS_WRD_REGISTRATION_USER) : oldName));
	newEmail = (!helper.m_email.IsEmpty() ? helper.m_email :  oldEmail);
	newPhone = (!helper.m_phone.IsEmpty() ? helper.m_phone :  oldPhone);

	CPerson newSuper;
	newSuper.setPeopleDB         ( this            );
	newSuper.setUserID           ( helper.m_userid );
	newSuper.setPassword         ( newPass         );
	newSuper.setFullName         ( newName         );
	newSuper.getOffice().setEmail( newEmail        );
	newSuper.getOffice().setPhone( newPhone        );
	newSuper.setPermission       ( PRM_DEFAULT, PERMISSION_SUPER   );
	newSuper.getOffice().setFax  ( oldSuper.getOffice().getFax  () ); // these next few are possibly empty, preserve what's found
	newSuper.getOffice().setCell ( oldSuper.getOffice().getCell () );
	newSuper.setPassHint         ( oldSuper.getPassHint         () );

	if (oldSuper.getUserID() != newSuper.getUserID())
	{
		RemoveUser(oldSuper.getUserID());
		// adjust group memberships
		for (int i=0;i<m_nGroups;i++)
		{
			getGroupAt(i)->removeMember(oldSuper.getUserID());
			getGroupAt(i)->addMember   (newSuper.getUserID());
		}
	}

	UpdateUser(newSuper, Now());
			
	return TRUE;
}

//-----------------------------------------------------------------------------------
SFBool CUserDatabase::mayAddLicense(void) const
{
	return nAccessibleObjects() < m_registration.nRegistered();
}
