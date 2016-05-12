/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "peopledatabase.h"
#include "usersettings.h"

//-------------------------------------------------------------------------
void CUserSettings::setFieldValue(const CFieldData *field, const SFString& value)
{
	SFInt32 valueI = atoi((const char *)value);

	SFString name = field->getFieldName();
	if (name.IsEmpty())
		return;

	switch (name[0])
	{
	case 'd':
		if (name == "denyPerm")          { setHighestPermission(valueI); return; }
		if (name == "defPerm")           { setDefaultPermission(valueI); return; }
		break;

	case 'g':
		if (name == "groupPerms")        { setAllowGroupPerms(valueI);   return; }
		break;
	
	case 'l':
		if (name == "leastPerm")         { setLowestPermission(valueI);  return; }
		break;

	case 'n':
		if (name == "notSame")           { setPasswordNotSame(valueI);   return; }
		break;

	case 'p':
		if (name == "pwAge")             { setPasswordAge(valueI);       return; }
		if (name == "permsPerCal")       { setAllowDiffPerms(valueI);    return; }
		if (name == "pwRemind")          { setPasswordReminders(valueI); return; }
		break;

	case 'r':
		if (name == "r_userid")          { setUseridReq(valueI);         return; }
		if (name == "r_pass")            { setPasswordReq(valueI);       return; }
		if (name == "r_email")           { setEmailReq(valueI);          return; }
		if (name == "r_name")            { setNameReq(valueI);           return; }
		if (name == "r_phone")           { setPhoneReq(valueI);          return; }
		if (name == "r_cell")            { setCellReq(valueI);           return; }
		if (name == "r_fax")             { setFaxReq(valueI);            return; }
		if (name == "r_hint")            { setHintReq(valueI);           return; }
		if (name == "resetPW")           { setResetPassword(valueI);     return; }
		break;

	case 's':
		if (name == "singleDB")          { setSingleContactDB(valueI);   return; }
		if (name == "showDropdown")      { setShowDropdown(valueI);      return; }
		if (name == "showLastLogin")     { setShowLastLogin(valueI);     return; }
		if (name == "selfEdit")          { setSelfEdit(valueI);          return; }
		break;

	case 'u':
		if (name == "useEmailID")        { setUseEmailAsID(valueI);      return; }
		break;
	}
}

//-------------------------------------------------------------------------
SFBool CUserSettings::getShowDropdown(void) const
{
#ifdef TESTING
	if (g_unitTesting && getInt32("nodrop", FALSE))
		return FALSE;
#endif
	return m_showDropdown;
}

//-------------------------------------------------------------------------
SFString CUserSettings::toAscii(SFBool prevVersion) const
{
// Default:
// 1|1|5| 3|0|0| 1|1|0| 0|1|1| 0|0|0| 0|0|1| 0|1|0| 0|

	SFString ret;
	ret =
		asString( getSelfEdit         ()) + CH_BAR +
		asString( getLowestPermission ()) + CH_BAR +
		asString( getHighestPermission()) + CH_BAR +

		asString( getDefaultPermission()) + CH_BAR +
		asString( getPasswordAge      ()) + CH_BAR +
		asString( getUseEmailAsID     ()) + CH_BAR +

		asString( getShowDropdown     ()) + CH_BAR +
		asString( getSingleContactDB  ()) + CH_BAR +
		asString( getAllowGroupPerms  ()) + CH_BAR +

		asString( getAllowDiffPerms   ()) + CH_BAR +
		asString( getUseridReq        ()) + CH_BAR +
		asString( getPasswordReq      ()) + CH_BAR +

		asString( getEmailReq         ()) + CH_BAR +
		asString( getNameReq          ()) + CH_BAR +
		asString( getPhoneReq         ()) + CH_BAR +

		asString( getCellReq          ()) + CH_BAR +
		asString( getFaxReq           ()) + CH_BAR +
		asString( getPasswordReminders()) + CH_BAR +

		asString( getResetPassword    ()) + CH_BAR +
		asString( getPasswordNotSame  ()) + CH_BAR +
		asString( getHintReq          ()) + CH_BAR;

	if (!prevVersion)
		ret += asString( getShowLastLogin()) + CH_BAR;

	return ret;
}

//-------------------------------------------------------------------------
void CUserSettings::fromAscii(const SFString& in)
{
	SFString str = in;
	setSelfEdit         (atoi((const char *)nextTokenClear(str, CH_BAR)));
	setLowestPermission (atoi((const char *)nextTokenClear(str, CH_BAR)));
	setHighestPermission(atoi((const char *)nextTokenClear(str, CH_BAR)));
	setDefaultPermission(atoi((const char *)nextTokenClear(str, CH_BAR)));
	setPasswordAge      (atoi((const char *)nextTokenClear(str, CH_BAR)));
	setUseEmailAsID     (atoi((const char *)nextTokenClear(str, CH_BAR)));
	setShowDropdown     (atoi((const char *)nextTokenClear(str, CH_BAR)));
	setSingleContactDB  (atoi((const char *)nextTokenClear(str, CH_BAR)));
	setAllowGroupPerms  (atoi((const char *)nextTokenClear(str, CH_BAR)));
	setAllowDiffPerms   (atoi((const char *)nextTokenClear(str, CH_BAR)));
	setUseridReq        (atoi((const char *)nextTokenClear(str, CH_BAR)));
	setPasswordReq      (atoi((const char *)nextTokenClear(str, CH_BAR)));
	setEmailReq         (atoi((const char *)nextTokenClear(str, CH_BAR)));
	setNameReq          (atoi((const char *)nextTokenClear(str, CH_BAR)));
	setPhoneReq         (atoi((const char *)nextTokenClear(str, CH_BAR)));
	setCellReq          (atoi((const char *)nextTokenClear(str, CH_BAR)));
	setFaxReq           (atoi((const char *)nextTokenClear(str, CH_BAR)));
	setPasswordReminders(atoi((const char *)nextTokenClear(str, CH_BAR)));
	setResetPassword    (atoi((const char *)nextTokenClear(str, CH_BAR)));
	setPasswordNotSame  (atoi((const char *)nextTokenClear(str, CH_BAR)));
	setHintReq          (atoi((const char *)nextTokenClear(str, CH_BAR)));
	setShowLastLogin    (atoi((const char *)nextTokenClear(str, CH_BAR)));
}

