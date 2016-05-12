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
CUserGroup::CUserGroup(const CPeopleDatabase *db)
{
	Init();
	m_peopleDB = db;
}

//---------------------------------------------------------------------------------------
SFInt32 CUserGroup::getFieldID(const SFString& fieldName) const
{
	ASSERT(getPeopleDB() && getPeopleDB()->getFieldList());
	const CFieldData *field = getPeopleDB()->getFieldList()->getFieldByName(fieldName);
	return (field ? field->getFieldID() : NOT_A_FIELD);
}

//---------------------------------------------------------------------------------------
SFString CUserGroup::getFieldValueByID(SFInt32 id, const SFString& def) const
{
	SFString permStr = CPermission::getPermissionStrLong(getPermission());
	SFString str;
	switch (id)
	{
	case GR_NAME:           str = getName();                 break;
	case GR_DESCR:          str = getDescription();          break;
	case GR_MEMBERS:        str = getMembers();              break;
	case GR_DELETED:        str = asString(isDeleted());     break;
	case GR_EXPANDED:       str = asString(isExpanded());    break;
	case GR_PERMISSION:     str = asString(getPermission()); break;
	case GR_PERMISSION_STR: str = permStr;                   break;
	case GR_SUBSCRIBE:      str = asString(maySubscribe());  break;
	default:
		break; 
	}

	if (str.IsEmpty())
		str = def;

	return str;
}

//---------------------------------------------------------------------------------------
void CUserGroup::setFieldValueByID(SFInt32 id, const SFString& value)
{
	SFInt32 valueI = atoi((const char *)value);
	switch (id)
	{
	case GR_NAME:       setName       (value);  break;
	case GR_DESCR:      setDescription(value);  break;
	case GR_MEMBERS:    setMembers    (value);  break;
	case GR_DELETED:    setDeleted    (valueI); break;
	case GR_EXPANDED:   setExpanded   (valueI); break;
	case GR_PERMISSION: setPermission (valueI); break;
	case GR_SUBSCRIBE:  setSubscribe  (valueI); break;
	}
}

//---------------------------------------------------------------------------------------
// This is OK to be static global since it will never change once its set
static SFInt32 firstGroupField = 0;

//---------------------------------------------------------------------------------------
SFString CUserGroup::toAscii(const SFString& format) const
{
	ASSERT(getPeopleDB());

	SFString ret;

	SFInt32 iterator = firstGroupField;
	const CFieldData *field = CPerson::firstPersonField(iterator); ASSERT(field);
	while (field)
	{
		if (field->getFieldType() != T_PROMPT && field->getFieldID() & TYPE_GROUP)
		{
			// for performance only
			if (firstGroupField == 0)
				firstGroupField = iterator;

			ASSERT(field);
			SFInt32 id   = field->getFieldID  ();
			SFInt32 type = field->getFieldType();

			if (type != T_PROMPT)
			{
				SFString prompt = toProper(field->getPrompt   ());
				SFString value  =          getFieldValueByID  (id, EMPTY);

				if (format.IsEmpty())
				{
					value.ReplaceAll(SEP_SEMI, EMPTY);
					ret += (value + SEP_SEMI);

				} else
				{
					if (id == GR_PERMISSION)
						value = getFieldValueByID(GR_PERMISSION_STR, EMPTY);
					if (!value.IsEmpty() && id != GR_DELETED && id != GR_EXPANDED)
					{
						ret += format;
						ret.Replace("{PROMPT}", prompt);
						ret.Replace("{VALUE}",  value);
					}
				}
			}
		}
		field = CPerson::nextPersonField(iterator);
	}

	return ret;
}

//---------------------------------------------------------------------------------------
void CUserGroup::fromAscii(const SFString& in, const CVersion& version)
{
	ASSERT(getPeopleDB());
	if (version.earlier(4,0,2))
	{
		fromAscii_4_0_1(in, version);
		return;
	}

	SFString line     = in;
	SFInt32  iterator = firstGroupField;
	const CFieldData *field = CPerson::firstPersonField(iterator); ASSERT(field);
	while (field)
	{
		if (field->getFieldType() != T_PROMPT && field->getFieldID() & TYPE_GROUP)
		{
			if (firstGroupField == 0)
				firstGroupField = iterator;
			setFieldValueByID(field->getFieldID(), nextTokenClear(line, CH_SEMI));
		}
		field = CPerson::nextPersonField(iterator);
	}
}

//---------------------------------------------------------------------------------------
SFBool CUserGroup::userMaySubscribe(const CPerson *person, SFBool groupPerms) const
{
	return (maySubscribe() && !isDeleted() && (!groupPerms || getPermission() <= person->getPermission(PRM_APPARENT)));
}

//---------------------------------------------------------------------------------------
void CUserGroup::setMembers(const SFString &members)     
{
	SFString in = members;
	in.ReplaceAll("\r",  EMPTY);
	in.ReplaceAny("\n;", ",");
	in.ReplaceAll(", ",  ",");
	in.ReplaceAll(",,",  ",");
	m_members = Strip(in, CH_COMMA);
	m_members.ReplaceAll(",", ", ");
}

//---------------------------------------------------------------------------------------
SFInt32 CUserGroup::getMemberCount(void) const
{
	if (getMembers().IsEmpty())
		return 0;
	return countOf(CH_COMMA, getMembers())+1;
}
