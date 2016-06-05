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

//-------------------------------------------------------------------------
void CFieldList::Release(void) const
{
	((CFieldList*)this)->m_group = EMPTY;
}

//-------------------------------------------------------------------------
LISTPOS CFieldList::GetFirstItem(const SFString& filter) const
{
	if (filter != nullString)
		((CFieldList*)this)->m_group = filter;
	return SFList<CFieldData*>::GetHeadPosition();
};

//-------------------------------------------------------------------------
CFieldData *CFieldList::GetNextItem(LISTPOS& rPosition) const
{
	CFieldData *data = SFList<CFieldData*>::GetNext(rPosition);
	if (m_group.IsEmpty())
		return data;

	while (rPosition && !(data->getGroupName() % m_group))
		data = SFList<CFieldData*>::GetNext(rPosition);

	return data;
}

//-------------------------------------------------------------------------
const CFieldData *CFieldList::getFieldByID(SFInt32 id) const
{
	if (id == NOT_A_FIELD)
		return NULL;

	LISTPOS lPos = GetFirstItem();
	while (lPos)
	{
		CFieldData *field = GetNextItem(lPos);
		if (field->getFieldID() == id)
			return field;
	}

	//ASSERT(0); // should never happen
	return NULL;
}

//-------------------------------------------------------------------------
static OTHER_ID_FUNC otherIDFunc = NULL;

//-------------------------------------------------------------------------
static SFInt32 getOtherID(const SFString& token)
{
	if (otherIDFunc)
		return (otherIDFunc)(token);
	return NOT_A_FIELD;
}

//-------------------------------------------------------------------------
void setOtherIDFunc(OTHER_ID_FUNC func)
{
	otherIDFunc = func;
}

//-------------------------------------------------------------------------
const CFieldData *CFieldList::getFieldByName(const SFString& fieldString) const
{
	SFString fieldName = fieldString; fieldName = nextTokenClear(fieldName, '|'); // the input may contain more than just fieldName

	const CFieldData *field = NULL;
	LISTPOS lPos = GetFirstItem();
	while (lPos)
	{
		CFieldData *current = GetNextItem(lPos);
		if (current->getFieldName() % fieldName)
		{
			field = current;
			continue;
		}
	}

	if (!field && !fieldName.IsEmpty())
	{
		// File import needs to match on the field's prompt
		// since the end user may well be importing a file
		// that has been exported.
		LISTPOS llPos = GetFirstItem();
		while (llPos)
		{
			CFieldData *fieldL = GetNextItem(llPos);
			if (fieldL->getPrompt() % fieldName)
				return fieldL;
		}

		// We have not found the field yet.  Look use this map to try to pick off
		// fields that have simply been renamed always read this string from
		// the event config only (even for task list)
		SFString fieldMap = ALTERNATE_FIELDMAP; //(config ? config->GetProfileStringGH("EVENT_DATA", "fieldmap", ALTERNATE_FIELDMAP) : ALTERNATE_FIELDMAP);
		while (!fieldMap.IsEmpty())
		{
			SFString altName = nextTokenClear(fieldMap, '|');
			SFString name    = nextTokenClear(fieldMap, '|');
			if (fieldName % altName)
				return getFieldByName(name);
		}

		// Now we try to pick up some other fields - this mostly helps
		// with import from Outlook - these items are not customizable
		// We will only find it for events at this point because of the CEvent below
		SFString fieldMap2 = ALTERNATE_FIELDMAP2;
		while (!fieldMap2.IsEmpty())
		{
			SFString importName = nextTokenClear(fieldMap2, '|');
			SFString prompt     = nextTokenClear(fieldMap2, '|');
			SFString token      = nextTokenClear(fieldMap2, '|');
			if (fieldName % importName)
			{
				// this is bogus - this is shared global data but we will
				// copy it out in the caller in about a ten-billionth
				// of a second.  Only would be a problem under .dll version and
				// only if both users are hitting the program -- for import --
				// at exactly the same second....who cares!
				static CFieldData temp;
				temp.setFieldName(importName);
				temp.setPrompt(prompt);
				temp.setFieldID(getOtherID(token));
				temp.setResolved(TRUE);
				return &temp;
			}
		}

		// Scraping the bottom of the barrel now...
		// We will only find it for events at this point because of the CEvent below
		SFString fieldMap3 = ALTERNATE_FIELDMAP3;
		while (!fieldMap3.IsEmpty())
		{
			SFString importName = nextTokenClear(fieldMap3, '|');
			SFString prompt     = importName;
			SFString token      = importName;

			if (fieldName % importName)
			{
				// this is bogus - this is shared global data but we will
				// copy it out in the caller in about a ten-billionth
				// of a second.  Only would be a problem under .dll version and
				// only if both users are hitting the program -- for import --
				// at exactly the same second....who cares!
				static CFieldData temp;
				temp.setFieldName(importName);
				prompt.Replace("_", " ");
				temp.setPrompt(toProper(prompt));
				temp.setFieldID(getOtherID(token));
				temp.setResolved(TRUE);
				return &temp;
			}
		}
	}

	if (!field)
	{
		// always return something
		static const CFieldData non_field = CFieldData("|unknown");
		field = &non_field;
		((CFieldData*)field)->setFieldID(NOT_A_FIELD);
	}

	return field;
}

//-------------------------------------------------------------------------
// These items must lead to an actual event field (not a display only field such as access_str).
SFString ALTERNATE_FIELDMAP =
"event_id|eventid|"
"contact_id|userid|"   // contact list data (the only one)
"topic|subject|"
"subject_nolink|subject|"
"start date|start_date|"
"date_shortest|start_date|"
"date|start_date|"
"begin|start_date|"
"start time|start_time|"
"time|start_time|"
"time_shortest|start_time|"
"end date|end_date|"
"end time|end_time|"
"stop date|end_date|"
"stop time|end_time|"
"untimed|allday|"
"all day event|allday|"
"categories|category|"
"meeting organizer|name|"
"contact e-mail|email|"
"contact|name|"
"contact name|name|"
"contact phone|phone|"
"meeting organizer2|name2|"
"contact2 e-mail|email2|"
"contact2|name2|"
"contact2 name|name2|"
"contact2 phone|phone2|"
"attendees|notify|"
"notification|notify|"
"participants|notify|"
"reminder on/off|reminder|"
"private|access|"
"creator|creator_id|"
"lasteditor|lastedit_id|"
"repeat_end|repeat_end_date|"
"rep_enddate|repeat_end_date|"
"task|istodo|";

//-------------------------------------------------------------------------
// This field map should list all fields that might be imported (from Outlook for example)
SFString ALTERNATE_FIELDMAP2 =
"date_time_all|Date/Time|display_date|"
"display_date|Date/Time|display_date|"
"duration_str|Duration|duration_str|"
"age|Age|age|"
"todo_age|Age|todo_age|"
"sd|Start Day|start_day|"
"sh|Start Hour|start_hour|"
"smn|Start Minute|start_minute|"
"sm|Start Month|start_month|"
"start_day|Start Day|start_day|"
"start_hour|Start Hour|start_hour|"
"start_min|Start Minute|start_minute|"
"start_month|Start Month|start_month|"
"start_year|Start Year|start_year|"
"sy|Start Year|start_year|"
"end_hour|End Hour|end_hour|"
"end_min|End Minute|end_minute|"
"comment|Notes|notes|"
"description|Notes|notes|"
"full description|Notes|notes|";

//-------------------------------------------------------------------------
// Needed so report view can pick stuff up
SFString ALTERNATE_FIELDMAP3 =
"access_str|"
"start_minute|"
"end_month|"
"end_day|"
"end_year|"
"end_minute|"
"date_short|"
"time_short|"
"date_long|"
"time_long|"
"edate_short|"
"edate_shortest|"
"etime_short|"
"edate_long|"
"etime_long|"
"day|"
"day_short|"
"eday|"
"eday_short|"
"event_age|"
"date_sort|"
"edate_sort|"
"subject_review|"
"subject_review_nolink|"
"subject_foremail|"
"subject_nodec|"
"subject_urllink|"
"subject_url_noreview|"
"review_link|"
"url_nolink|"
"email_nolink|"
"email2_nolink|"
"allday_str|"
"reminder_str|"
"creator_str|"
"back_color_str|"
"text_color_str|"
"pending_str|"
"notes_nodec|"
"notes_nolink|"
"notes_by_id|"
"edit_icon|"
"delete_icon|"
"dup_icon|"
"artwork_icon|"
"notes_icon|"
"print_icon|"
"ical_icon|"
"vcal_icon|"
"vcard_icon|"
"location_icon|"
"priority_icon|"
"category_icon|"
"status_icon|"
"userdrop1_icon|"
"userdrop2_icon|"
"userdrop3_icon|"
"userdrop4_icon|"
"style|"
"location_style|"
"priority_style|"
"category_style|"
"status_style|"
"userdrop1_style|"
"userdrop2_style|"
"userdrop3_style|"
"userdrop4_style|"
"calendar_name|"
"calendar_group|"
"creator_name|"
"creator_groups|"
"editor_name|";
