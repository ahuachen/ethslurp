/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "fielddata.h"
#include "version.h"
#include "config.h"

extern const char* IDS_JAV_DATECONTROLSPAN;

//-----------------------------------------------------------------------------------------
CFieldData::CFieldData(void)
{
	Init();
}

//-----------------------------------------------------------------------------------------
CFieldData::CFieldData(const SFString& group, const SFString& fieldName, const SFString& prompt, const SFString& value, SFInt32 type, SFBool required, SFInt32 fieldID)
{
	Init();

	m_groupName = group;
	m_fieldName = fieldName;
	m_type      = type;
	m_prompt    = prompt;
	m_value     = value;
	m_required  = required;
	m_fieldID   = fieldID;

	// the input prompt may contain field name (for example from CUser or CUserGroup) so drop it out here
	if (m_prompt.Contains("|"))
		nextTokenClear(m_prompt, '|');

	if (m_fieldID != NOT_A_FIELD)
	{
		m_defStr    = value;
		m_defInt    = atoi(value);
	    setHidden(m_prompt.IsEmpty());
	}

	m_resolved = TRUE;
}

//-----------------------------------------------------------------------------------------
CFieldData::CFieldData(const SFString& in)
{
	Init();
	
	SFString line = in;
	ASSERT(countOf('|', line) == 11 || in == "|unknown");

	m_groupName =                    nextTokenClear(line, '|');
	m_fieldName =                    nextTokenClear(line, '|');
	m_prompt    =                    nextTokenClear(line, '|');
	m_defStr    =                    nextTokenClear(line, '|');
	m_required  = atoi((const char *)nextTokenClear(line, '|'));
	m_export    = atoi((const char *)nextTokenClear(line, '|'));
	m_minPerms  = atoi((const char *)nextTokenClear(line, '|'));
	m_access    = atoi((const char *)nextTokenClear(line, '|'));
	m_helpText  =                    nextTokenClear(line, '|');
	m_type      = atoi((const char *)nextTokenClear(line, '|'));
	m_fieldID   = atoi((const char *)nextTokenClear(line, '|'));

	setHidden(m_prompt.IsEmpty());
	m_defInt = atoi((const char *)m_defStr);

	m_resolved = TRUE;
}

