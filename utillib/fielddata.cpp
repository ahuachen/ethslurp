/*--------------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2016 Great Hill Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
--------------------------------------------------------------------------------*/
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
		m_defInt    = toLong(value);
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

	m_groupName =        nextTokenClear(line, '|');
	m_fieldName =        nextTokenClear(line, '|');
	m_prompt    =        nextTokenClear(line, '|');
	m_defStr    =        nextTokenClear(line, '|');
	m_required  = toLong(nextTokenClear(line, '|'));
	m_export    = toLong(nextTokenClear(line, '|'));
	m_minPerms  = toLong(nextTokenClear(line, '|'));
	m_access    = toLong(nextTokenClear(line, '|'));
	m_helpText  =        nextTokenClear(line, '|');
	m_type      = toLong(nextTokenClear(line, '|'));
	m_fieldID   = toLong(nextTokenClear(line, '|'));

	setHidden(m_prompt.IsEmpty());
	m_defInt = toLong(m_defStr);

	m_resolved = TRUE;
}
