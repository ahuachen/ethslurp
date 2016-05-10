/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "fielddata.h"
#include "dateselect.h"
#include "version.h"
#include "config.h"
#include "drops.h"
#include "string_table.h"

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

	setSizes();
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

	setSizes();
	m_resolved = TRUE;
}

//-----------------------------------------------------------------------------------------
SFBool CFieldData::Customize(const CConfig *config)
{
	// Format: prompt | default | required | export | leastPerm | access
	if (config)
	{
		CVersion version;
		config->getVersion(version);

		// we want to properly handle the empty string found in the file and ignore
		// everything if the line is not found in the file
		SFString line = config->GetProfileStringGH(getGroupName(), getFieldName(), "not-set");
		if (line != "not-set")
		{
			// if the line is found we force these first two fields
			m_prompt = nextTokenClear(line, '|');
			m_defStr = nextTokenClear(line, '|');

			// and only set the remaining fields if present
			if (line.GetLength())
				m_required = atoi((const char *)nextTokenClear(line, '|'));
			if (getFieldName() == "subject")
				m_required = TRUE;
			if (getFieldID() & E_NO_REQUIRED)
				m_required = FALSE;
			// the rest of any fields for older versions was garbage.
			if (version.eqLater(3,8,6))
			{
				if (line.GetLength())
					m_export   = atoi((const char *)nextTokenClear(line, '|'));
				if (line.GetLength())
					m_minPerms = atoi((const char *)nextTokenClear(line, '|'));
				if (line.GetLength())
					m_access   = atoi((const char *)nextTokenClear(line, '|'));
				if (line.GetLength())
					m_helpText =                    nextTokenClear(line, '|');
				// type and fieldID are not stored in config files
			}

			m_defInt = atoi((const char *)m_defStr);
			setHidden(m_prompt.IsEmpty());
		}
		// if the line was not found in the file do not customize anything
	}
	m_resolved = TRUE;

	return TRUE;
}

//-----------------------------------------------------------------------------------------
SFBool CFieldData::isFieldVisible(SFInt32 userPerm) const
{
	return (userPerm >= getMinPerms());
}

//-----------------------------------------------------------------------------------------
void CFieldData::setProblem(const SFString& prob)
{
	m_problem = prob;
	m_problem.ReplaceAll("[VALUE]",  getValue());
	m_problem.ReplaceAll("[FIELD]",  getFieldName());
	if (getPrompt().IsEmpty())
		m_problem.ReplaceAll("[PROMPT]", getFieldName());
	else
		m_problem.ReplaceAll("[PROMPT]", getPrompt());
}

//-----------------------------------------------------------------------------------------
SFString CFieldData::getControl(void) const
{
	SFString group = getGroupName();
	SFString name  = getFieldName();
	SFInt32  type  = getFieldType();

	SFString value  = getValue();
	SFInt32  valueI = atoi((const char *)value);

	if (isHidden())
	{
		if (!m_showHidden)
			return EMPTY;

		CStringExportContext ctx;
		if (isCheckbox(type))
		{
			if (valueI)
			{CHiddenInput hidden(ctx, name, value);
				}

		} else if (!value.IsEmpty())
		{CHiddenInput hidden(ctx, name, value);
			}

		return ctx.str;
	}

	SFString script   = getScript();
	SFString preLabel = snagFieldClear(script, "pre");
	SFString label    = snagFieldClear(script, "label");
	if (type == T_RADIO)
	{
		script   = getScript();
		label    = snagFieldClear(script, "label");
		preLabel = snagFieldClear(script, "pre");
	}

	SFString ret = value;

	// Note - do not remove the 'Now' it is an initializer
	SFTime date = Now();

	switch (type)
	{
		case T_SCRIPT:
			ret = "\n<script language=javascript>\n<!-- begin script\n" + value + "\n// end script -->\n</script>\n";  // just copy the code through
			break;

		case T_CONTROL:
			ASSERT(!isHidden())
			ret = value;  // the control already exists - just copy it through
			if (isDisabled() && !ret.Contains("disabled"))
			{
				ret.ReplaceAll("<input ",    "<input disabled readOnly ");
				ret.ReplaceAll("<select ",   "<select disabled readOnly ");
				ret.ReplaceAll("<textarea ", "<textarea disabled readOnly ");
			}
			break;

		case T_CHECKSRCH:
		case T_CHECKBOX:
			ret = getCheckboxString(name, valueI, label, script, isDisabled());
			break;

		case T_RADIO:
			ret = getRadioString(name, label, valueI, getDefInt(), isDisabled(), script);
			break;

		case T_TIME:
			SetTime(date, value);
			ret = getTimeSelectString(name, date, isDisabled(), m_extraInt);
			break;

		case T_DATE:
			// Note - the actual function call gets added to m_delayScripts in CServer
			// because of Javascript problems with trying to access an un-initalized field.
			ret = IDS_JAV_DATECONTROLSPAN;
			ret.ReplaceAll("[NAME]",  name);
			ret.ReplaceAll("[VALUE]", value);
			break;

		case T_ONOFF:
			ret = getSelectString(onOffChoose,   name, value, isDisabled(), script, m_dataPtr,  m_extraInt);
			break;

		case T_SELSEARCH:
		case T_SELECTION:
			ret =  preLabel;
			ret += getSelectString(getSelFunc(), name, value, isDisabled(), script, m_dataPtr,  m_extraInt);
			ret += label;
			break;

		case T_ICONLIST:
			script += getIconChooser(getFieldName());
			ret  = getSelectString(iconChoose,   name, value, isDisabled(), EMPTY, &m_extraStr, m_extraInt);
			// upgraded events that reference icons in old folders would break otherwise
			if (!value.IsEmpty() && !ret.Contains("value=\"" + value + "\""))
				ret.ReplaceReverse("</select>", "\n<option selected value=\"" + value + "\">" + value + "\n</select>");
			ret += script;
			break;

		case T_COLOR:
			script += getColorChooser(getFieldName());
			ret  = getSelectString(colorChoose,  name, value, isDisabled(), EMPTY, m_dataPtr,   m_extraInt);
			ret.Replace("<option selected value=\"", EMPTY);
			ret.Replace("<option value=\"", EMPTY);
			ret.Replace("\">[NAME]", EMPTY);
			ret.Replace("[OPTIONS]", "<script>getColorOptions(" + value + ");</script>");
			ret += script;
			break;

		case T_USERID:
		case T_PASSWORD:
		case T_LONGPASSWORD:
		case T_PHONE:
		case T_HALFTEXT:
		case T_SHORTTEXT:
		case T_EMAIL:
		case T_NUMBER:
		case T_TEXT:
		case T_FILEUPLOAD:
		case T_NOTES:
		case T_BUTTON:
			{
				SFInt32 maxSize  = getMaxSize();
				SFInt32 editSize = getEditSize();

				// Do not allow double quotes since they break the form, convert them to single quotes instead
				value.ReplaceAll("\"", "'");

				CStringExportContext ctx;
				if (value != SPACER)
				{
					SFString iType = "text";
					if (type == T_PASSWORD || type == T_LONGPASSWORD)
						iType = "password";
					else if (type == T_FILEUPLOAD)
						iType = "file";
					else if (type == T_BUTTON)
						iType = "submit";

					if (!preLabel.IsEmpty())
					{SFText text(ctx, preLabel);
						}

					{CInput input(ctx, iType, name, editSize, value, isDisabled(), FALSE, EMPTY, maxSize);
						} // EOI

					if (!label.IsEmpty())
					{SFText text(ctx, label);
						}

					ret = ctx.str;
					if (!script.IsEmpty())
						ret += " " + script;
				}
			}
			break;

		case T_MULTILINE:
			{
				//SFInt32 maxSize  = getMaxSize(); // not used here
				SFInt32 editSize = getEditSize();

				SFInt32 rows     = editSize / 100000;
				SFInt32 cols     = editSize - (rows * 100000);
				
				value.ReplaceAll("<", "&lt;");
				value.ReplaceAll(">", "&gt;");
				CStringExportContext ctx;
				{CTextArea textarea(ctx, name, rows, cols, isDisabled());
					ctx << value;
					}

				ret = ctx.str;
				if (!script.IsEmpty())
					ret += " " + script;
			}
			break;

		// fieldIDs not handled
		case T_NONE:
		case T_PROMPT:
		case T_LABEL:
		default:
//			ASSERT(0);
			break;
	}

	if (m_noRow)
		ret = "<norow>" + ret;

	if (isRequired() && !isDisabled())
		ret += " <div class=cw_css_required><big>*</big></div>";

	if (!m_problem.IsEmpty())
		ret += " <span " + WARNINGCOLOR + ">" + m_problem + "</span>";

	return ret;
}

//-------------------------------------------------------------------------
SFString CFieldData::toAscii(void) const
{
	SFString ret;
	ret += (m_prompt             + CH_BAR);
	ret += (m_defStr             + CH_BAR);
	ret += (asString(m_required) + CH_BAR);
	ret += (asString(m_export)   + CH_BAR);
	ret += (asString(m_minPerms) + CH_BAR);

	// do not write this data unless it non-default
	if (m_access || !m_helpText.IsEmpty())
	{
		ret += (asString(m_access) + CH_BAR);
		ret += (m_helpText         + CH_BAR);
	}

	return ret;
}

//------------------------------------------------------------------------
SFString getSelectString(CSelectionData *data)
{
	CStringExportContext ctx;
	{CSelect select(ctx, data);
		}
	return ctx.str;
}

//------------------------------------------------------------------------
SFString getSelectString(SELFUNC func, const SFString& name, const SFString& selected, SFInt32 disabled, const SFString& onChange, const void *dataPtr, SFInt32 extraInt)
{
	CSelectionData selector;

	if (selected == "0" || atoi((const char*)selected) != 0)
		selector.set(func, name, (const char*)selected, disabled);
	else
		selector.set(func, name, selected, disabled);
	selector.extraInt    = extraInt;
	selector.dataVal     = dataPtr;
	selector.onChangeStr = onChange;

	return getSelectString(&selector);
}

//------------------------------------------------------------------------
SFString getRadioString(const SFString& name, const SFString& label, SFInt32 value, SFBool checked, SFBool disabled, const SFString& script)
{
	CStringExportContext ctx;

	{CRadio radio(ctx, name, asString(value), disabled, checked, script);
		if (!label.IsEmpty())
		{
			ctx << "&nbsp;";
			{CLabel lab(ctx, name, label);
				} // EOL
		}
		} // 

	return ctx.str;
}

//------------------------------------------------------------------------
SFString getCheckboxString(const SFString& name, SFInt32 val, const SFString& prompt, const SFString& onClickStr, SFBool disabled)
{
	CStringExportContext ctx;

	{CCheckbox cb(ctx, name, asString(val), disabled, (val), onClickStr);
		if (!prompt.IsEmpty())
		{
			ctx << "&nbsp;";
			{CLabel label(ctx, name, prompt);
				} // EOL
		}
		} // 

	return ctx.str;
}

//------------------------------------------------------------------------
SFBool onOffChoose(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	nVals = 2;
	if (!attrs)
		return TRUE;

	attrs[0].set(" Off ", asString(FALSE));
	attrs[1].set(" On ",  asString(TRUE));

	return TRUE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
SFBool sortOrderChoose(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	nVals = 2;
	if (!attrs)	
		return TRUE;
		
	attrs[0].set(" Reverse Chronological ", asString(SORTORDER_REVCHRON));
	attrs[1].set(" Chronological ",         asString(SORTORDER_CHRON));

	return TRUE;
}

//------------------------------------------------------------------------
SFBool alignChoose(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	nVals = 4;
	if (!attrs)
		return TRUE;

	attrs[0].set(" None ",   asString(ALIGN_NONE));
	attrs[1].set(" Left ",   asString(ALIGN_LEFT));
	attrs[2].set(" Middle ", asString(ALIGN_MIDDLE));
	attrs[3].set(" Right ",  asString(ALIGN_RIGHT));
	return TRUE;
}

//------------------------------------------------------------------------
SFBool colorChoose(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	nVals = 1;
	if (!attrs)
		return TRUE;
	attrs[0].set("[NAME]", "[OPTIONS]");
	return TRUE;
}

//------------------------------------------------------------------------
static const SFString fontFaces[] =
{
	"Default",
	// best defaults
	"Verdana, Arial, Sans",
	"Georgia, Times New Roman, Serif",
	// support for older versions
	"Verdana",
	"Arial",
	"Georgia",
	"Times New Roman",
	// other popular microsoft fonts
	"Trebuchet",
	"Comic Sans Serif",
	"Courier New",
	// mac fonts
	"Helvetica",
	"Times",
	"Comic",
	"Courier",
	// last bastion of bad design!
	"Sans",
	"Serif",
	// The following are old font faces which are stored directly in each
	// event (as strings) they will match and should become selected (in other
	// words we do not store index into array in old data) so this should work
	"Arial Black",
	"Comic Sans MS",
	"Trebuchet MS",
	"Andale Mono",
	"System",
};
static const SFInt32 nFonts = sizeof(fontFaces) / sizeof(SFString);

//------------------------------------------------------------------------
SFBool fontChoose(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	nVals = nFonts;
	if (!attrs)
		return TRUE;

	for (int i=0;i<nFonts;i++)
	{
		SFString dFont = fontFaces[i];
		SFString aFont = dFont;
		if (aFont == "Default")
			aFont = EMPTY;
		attrs[i].set(" " + dFont + " ", aFont);
	}

	return TRUE;
}

//------------------------------------------------------------------------
SFBool relSizeChoose(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	nVals = 7;
	if (!attrs)
		return TRUE;

	SFString names[] = {
		" smallest ",
		" smaller ",
		" small ",
		" normal ",
		" large ",
		" larger ",
		" largest "
	};

	for (int i=0;i<7;i++)
		attrs[i].set(names[i], asString(i-3));

	return TRUE;
}

//------------------------------------------------------------------------
SFBool allowChangeChoose(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	nVals = 4;
	if (!attrs)	
		return TRUE;

	attrs[0].set(" never ",                              OWNERCHANGE_NOONE );
	attrs[1].set(" if they are logged in ",              OWNERCHANGE_ANYONE);
	attrs[2].set(" if they are an admin or super user ", OWNERCHANGE_ADMINS);
	attrs[3].set(" if they are a super user ",           OWNERCHANGE_SUPER );
		
	return TRUE;
}

//------------------------------------------------------------------------
SFBool iconChoose(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	ASSERT(data && data->dataVal);

	SFString str       = *(SFString*)data->dataVal;

	SFString root      = nextTokenClear(str, '|');
	SFString subFolder = nextTokenClear(str, '|');
	SFString prompt    = nextTokenClear(str, '|');
	SFBool   addPrompt = !prompt.IsEmpty();

	SFString dir = root + subFolder;
	ASSERT(dir.endsWith('/'));
	SFInt32 nGIFs = nFilesInFolder(dir + "*.gif");
	SFInt32 nJPGs = nFilesInFolder(dir + "*.jpg");
	SFInt32 nPNGs = nFilesInFolder(dir + "*.png");

	nVals = nGIFs + nJPGs + nPNGs + addPrompt;

	if (!attrs)
		return TRUE;

	SFInt32 cnt = 0;
	if (!prompt.IsEmpty())
		attrs[cnt++].set(prompt, EMPTY);

	if (nGIFs>0)
	{
		SFString *gFiles = new SFString[nGIFs];
		if (gFiles)
		{
			SFos::listFiles(nGIFs, gFiles, dir + "*.gif");
			for (int i=0;i<nGIFs; i++)
				if (!gFiles[i].Contains("blank.gif"))
					attrs[cnt++].set(gFiles[i], subFolder + gFiles[i]);
			delete [] gFiles;
		}
	}
	
	if (nJPGs>0)
	{
		SFString *jFiles = new SFString[nJPGs];
		if (jFiles)
		{
			SFos::listFiles(nJPGs, jFiles, dir + "*.jpg");
			for (int i=0;i<nJPGs; i++)
				attrs[cnt++].set(jFiles[i], subFolder + jFiles[i]);
			delete [] jFiles;
		}
	}
	
	if (nPNGs>0)
	{
		SFString *pFiles = new SFString[nPNGs];
		if (pFiles)
		{
			SFos::listFiles(nPNGs, pFiles, dir + "*.png");
			for (int i=0;i<nPNGs; i++)
				attrs[cnt++].set(pFiles[i], subFolder + pFiles[i]);
			delete [] pFiles;
		}
	}

	nVals = cnt;
	qsort(&attrs[addPrompt], nVals-addPrompt, sizeof(SFAttribute), sortByAttributeName);
		
	return TRUE;
}

//------------------------------------------------------------------------
const SFAttribute reminders[] =
{
	SFAttribute("none",       asString   (0) ),
    SFAttribute("5 minutes",  asString   (5) ),
	SFAttribute("10 minutes", asString  (10) ),
	SFAttribute("15 minutes", asString  (15) ),
	SFAttribute("30 minutes", asString  (30) ),
	SFAttribute("1 hour",     asString  (60) ),
	SFAttribute("2 hours",    asString (120) ),
	SFAttribute("3 hours",    asString (180) ),
	SFAttribute("4 hours",    asString (240) ),
	SFAttribute("5 hours",    asString (300) ),
	SFAttribute("6 hours",    asString (360) ),
	SFAttribute("7 hours",    asString (420) ),
	SFAttribute("8 hours",    asString (480) ),
	SFAttribute("9 hours",    asString (540) ),
	SFAttribute("10 hours",   asString (600) ),
	SFAttribute("11 hours",   asString (660) ),
	SFAttribute(".5 day",     asString (720) ),
	SFAttribute("1 day",      asString(1440) ),
	SFAttribute("2 days",     asString(2880) ),
};
const SFInt32 nReminders = sizeof(reminders) / sizeof(SFAttribute);

//------------------------------------------------------------------------
static SFInt32 getReminderIndex(SFInt32 minutes)
{
	for (int i=0;i<nReminders;i++)
		if (atoi((const char *)reminders[i].getValue()) == minutes)
			return i;
	return 0;
}

//------------------------------------------------------------------------
SFString getReminderString(SFInt32 minutes)
{
	ASSERT(getReminderIndex(minutes) >= 0 && getReminderIndex(minutes) < nReminders);
	return reminders[getReminderIndex(minutes)].getName();
}

//------------------------------------------------------------------------
SFBool reminderChoose(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	nVals = nReminders;
	if (!attrs)	
		return TRUE;

	for (int i=0;i<nReminders;i++)
		attrs[i] = reminders[i];

	return TRUE;
}

//------------------------------------------------------------------------
SFBool specPermChoose(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	nVals = 6;
	if (!attrs)
		return TRUE;

	SFInt32 cnt=0;
	attrs[cnt++].set(CPermission::getPermissionStrLong(PERMISSION_NOTSPEC), asString(PERMISSION_NOTSPEC));
	attrs[cnt++].set(CPermission::getPermissionStrLong(PERMISSION_NONE),    asString(PERMISSION_NONE)   );
	attrs[cnt++].set(CPermission::getPermissionStrLong(PERMISSION_VIEW),    asString(PERMISSION_VIEW)   );
	attrs[cnt++].set(CPermission::getPermissionStrLong(PERMISSION_ADD),     asString(PERMISSION_ADD)    );
	attrs[cnt++].set(CPermission::getPermissionStrLong(PERMISSION_EDIT),    asString(PERMISSION_EDIT)   );
	attrs[cnt++].set(CPermission::getPermissionStrLong(PERMISSION_ADMIN),   asString(PERMISSION_ADMIN)  );

	return TRUE;
}

#define DO_ONE_OPTION(perm) \
if (perm < maxPerm) attrs[nVals++].set(CPermission::getPermissionStrLong(perm), asString(perm));

//------------------------------------------------------------------------
SFBool permChoose(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	SFInt32 maxPerm = (data && data->dataVal ? (SFInt32)data->dataVal : PERMISSION_ADMIN+1);

	// over count them
	nVals = 6;
	if (!attrs)
		return TRUE;

	// recount them
	nVals = 0;

	DO_ONE_OPTION( PERMISSION_NONE  );
	DO_ONE_OPTION( PERMISSION_VIEW  );
	DO_ONE_OPTION( PERMISSION_ADD   );
	DO_ONE_OPTION( PERMISSION_EDIT  );
	DO_ONE_OPTION( PERMISSION_ADMIN );
	DO_ONE_OPTION( PERMISSION_SUPER );

	return TRUE;
}

//------------------------------------------------------------------------
static SFBool customizedChoose(SFInt32& nVals, SFAttribute *attrs, const SFString& group, SFInt32 nItems, SFString *defNames, SFString *defVals, CConfig *config, SFBool noMax=TRUE)
{
	// Do not change next two lines - dur_nDurs is used elsewhere for example
	SFString chars3   = toLower(group.Left(3));
	SFString countStr = chars3 + "_n" + toProper(chars3) + "s";

	nVals = (config ? config->GetProfileIntGH(group, countStr, nItems) : nItems);
	if (!attrs)
		return TRUE;

	SFInt32 maxVal = 1000*1000; // just some large number
	if (!noMax && config && group == "DURATIONS")
	{
		maxVal = config->GetProfileIntGH(group, "max_" + toLower(group), BAD_NUMBER);
		if (maxVal == BAD_NUMBER || maxVal <= 0)
			maxVal = 1000*1000; // some huge number
	}

	nItems = 0;
	if (config && config->GetProfileIntGH(group, countStr, BAD_NUMBER) != BAD_NUMBER)
	{
		// This means the thing has been customized so we use it from the customization file
		for (int i=0;i<nVals;i++)
		{
			SFString name = config->GetProfileStringGH(group, chars3 + "_" + asString(i+1), EMPTY);
			if (!name.IsEmpty())
			{
				SFString iName = nextTokenClear(name, '|');
				SFString iVal  = nextTokenClear(name, '|');
				SFInt32  minsI = atoi((const char*)iVal);
				if (minsI <= maxVal)
					attrs[nItems++].set(iName, iVal);
			}
		}

	} else
	{
		// This means the thing has not been customized so we use the default values sent it.
		for (int i=0;i<nVals;i++)
		{
			SFString iName = defNames[i];
			SFString iVal  = defVals [i]; if (iVal % "select...") iVal = EMPTY;
			SFInt32  minsI = atoi((const char*)iVal);
			if (minsI <= maxVal)
				attrs[nItems++].set(iName, iVal);
		}
	}
	nVals=nItems;

	return TRUE;
}

//------------------------------------------------------------------------
SFBool durationChooseMax(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	static SFString defDurNames[] =
	{
		"5 mins",   "10 mins",   "15 mins",    "20 mins",    "30 mins",
		"45 mins",  "1 hour",    "1 1/4 hour", "1 1/2 hour", "1 3/4 hour",
		"2 hours",  "3 hours",   "4 hours",    "5 hours",    "6 hours",
		"7 hours",  "8 hours",   "9 hours",    "10 hours",   "11 hours",
		"12 hours", "13 hours",  "14 hours",   "15 hours",   "16 hours",
		"17 hours", "18 hours",  "19 hours",   "20 hours",   "21 hours",
		"22 hours", "23 hours",  "24 hours",
	};
	
	static SFString defDurVals[] =
	{
		"5",    "10",   "15",   "20",   "30",
		"45",   "60", 	 "75",   "90",   "105",
		"120",  "180",  "240",  "300",  "360",
		"420",  "480",  "540",  "600",  "660",
		"720",  "780",  "840",  "900",  "960",
		"1020", "1080", "1140", "1200", "1260",
		"1320", "1380", "1440"
	};
	SFInt32 nItems = sizeof(defDurVals) / sizeof(SFString);
	ASSERT((sizeof(defDurNames) / sizeof(SFString)) == nItems);
	
	CConfig *config = (CConfig *)data->dataVal;
	return customizedChoose(nVals, attrs, "DURATIONS", nItems, defDurNames, defDurVals, config, FALSE);
}

//------------------------------------------------------------------------
SFBool durationChooseNoMax(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	static SFString defDurNames[] =
	{
		"5 mins",   "10 mins",   "15 mins",    "20 mins",    "30 mins",
		"45 mins",  "1 hour",    "1 1/4 hour", "1 1/2 hour", "1 3/4 hour",
		"2 hours",  "3 hours",   "4 hours",    "5 hours",    "6 hours",
		"7 hours",  "8 hours",   "9 hours",    "10 hours",   "11 hours",
		"12 hours", "13 hours",  "14 hours",   "15 hours",   "16 hours",
		"17 hours", "18 hours",  "19 hours",   "20 hours",   "21 hours",
		"22 hours", "23 hours",  "24 hours",
	};

	static SFString defDurVals[] =
	{
		 "5",    "10",   "15",   "20",   "30",
		 "45",   "60", 	 "75",   "90",   "105",
		 "120",  "180",  "240",  "300",  "360",
		 "420",  "480",  "540",  "600",  "660",
		 "720",  "780",  "840",  "900",  "960",
		"1020", "1080", "1140", "1200", "1260",
		"1320", "1380", "1440"
	};
	SFInt32 nItems = sizeof(defDurVals) / sizeof(SFString);
	ASSERT((sizeof(defDurNames) / sizeof(SFString)) == nItems);

	CConfig *config = (CConfig *)data->dataVal;
	return customizedChoose(nVals, attrs, "DURATIONS", nItems, defDurNames, defDurVals, config);
}

//------------------------------------------------------------------------
SFBool prefixChoose(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	static SFString prefixVals[] = { "Select...", "Mrs.", "Mr.", "Ms.", "Miss", "Mr. & Mrs.", "Messrs.", "Prof.", "Dr.", "Gen.", "Rep.", "Sen." };
	SFInt32 nItems = sizeof(prefixVals) / sizeof(SFString);

	CConfig *config = (CConfig *)data->dataVal;
	return customizedChoose(nVals, attrs, "PREFIX", nItems, prefixVals, prefixVals, config);
}

//------------------------------------------------------------------------
SFBool suffixChoose(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	static SFString suffixVals[] = { "Select...", "Sr.", "Jr.", "III", "Ph.D.", "M.D.", "B.A.", "M.A.", "D.D.S." };
	SFInt32 nItems = sizeof(suffixVals) / sizeof(SFString);

	CConfig *config = (CConfig *)data->dataVal;
	return customizedChoose(nVals, attrs, "SUFFIX", nItems, suffixVals, suffixVals, config);
}

//------------------------------------------------------------------------
extern const char* IDS_SEL_STATES;
extern const char* IDS_SEL_COUNTRIES;

//------------------------------------------------------------------------
SFBool regionChoose(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	SFString stateList = LoadStringGH(IDS_SEL_STATES);
	SFInt32 count = countOf('|', stateList);

	SFInt32 nItems = 0;
	SFString *stateVals = new SFString[count];
	SFString *stateAbrs = new SFString[count];
	while (!stateList.IsEmpty())
	{
		SFString abrv = nextTokenClear(stateList, '|');
		SFString full = nextTokenClear(abrv, '+');
		if (abrv.IsEmpty())
			abrv = full;
		stateVals[nItems] = full;
		stateAbrs[nItems] = abrv;
		nItems++;
	}
	ASSERT(nItems == count);

	CConfig *config = (CConfig *)(data ? data->dataVal : NULL);
	SFBool ret = customizedChoose(nVals, attrs, "REGIONS", nItems, stateVals, stateAbrs, config);
	delete [] stateVals;
	delete [] stateAbrs;
	return ret;
}

//------------------------------------------------------------------------
SFBool countryChoose(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	SFString countryList = LoadStringGH(IDS_SEL_COUNTRIES);
	SFInt32 count = countOf('|', countryList);

	SFInt32 nItems = 0;
	SFString *countryVals = new SFString[count];
	while (!countryList.IsEmpty())
		countryVals[nItems++] = nextTokenClear(countryList, '|');
	ASSERT(nItems == count);

	CConfig *config = (CConfig *)data->dataVal;
	SFBool ret = customizedChoose(nVals, attrs, "COUNTRIES", nItems, countryVals, countryVals, config);
	delete [] countryVals;
	return ret;
}

//------------------------------------------------------------------------
SFBool genderChoose(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data)
{
	static SFString genderVals[] = { "Select...", "Female", "Male", };
	SFInt32 nItems = sizeof(genderVals) / sizeof(SFString);

	CConfig *config = (CConfig *)data->dataVal;
	return customizedChoose(nVals, attrs, "GENDERS", nItems, genderVals, genderVals, config);
}

//------------------------------------------------------------------------
void CFieldData::setSizes(void)
{
	if (m_type == T_MULTILINE)
	{				 
        // store rows/cols in editSize
		m_editSize = (10 * 100000 + 65);
        m_maxSize  = 4096;
		return;
	}

	m_maxSize  = LONG_STRING_LEN;
	m_editSize = 64;

	if (m_type == T_PHONE)
	{
		m_maxSize = PHONE_STRING_LEN;
		m_editSize = 28;

	} else if (m_type == T_HALFTEXT)
	{
		m_maxSize  = MID_STRING_LEN;

	} else if (m_type == T_USERID)
	{
		m_maxSize  = USERID_STRING_LEN;
		m_editSize = 32;

	} else if (m_type == T_PASSWORD || m_type == T_SHORTTEXT)
	{
		m_maxSize  = 32;
		m_editSize = 32;

	} else if (m_type == T_NUMBER)
	{
		m_maxSize  = 10;
		m_editSize = 10;

	} else if (m_type == T_LONGPASSWORD)
	{
		m_maxSize  = 64;
		m_editSize = 64;
	}
}

//--------------------------------------------------------------------------------------
SFString getTimeSelectString(const SFString& name, const SFTime& date, SFBool disabled, SFInt32 step)
{
	CStringExportContext ctx;

	// ranges from 0 at 12:00am to 23 at 11:00pm --> convert to 12-1-....-11
	SFInt32 hour = date.GetHour();

	if (SFos::isMilitary()) // military time?
	{
		CSelectionData selector;

		selector.set(hourChoose24_24, name+"_hour", hour, disabled);
		ctx << getSelectString(&selector);

		selector.set(minChoose, name+"_min", date.GetMinute()/step, disabled);
		selector.dataVal = (void*)&step;
		selector.extraInt = FALSE;
		ctx << getSelectString(&selector);

	} else
	{
		SFBool isAM = (hour < 12);
		if (hour > 12) hour -= 12;
		if (hour == 0) hour = 12;

		CSelectionData selector;

		selector.set(hourChoose12, name+"_hour", hour-1, disabled);
		ctx << getSelectString(&selector);

		selector.set(minChoose, name+"_min", date.GetMinute()/step, disabled);
		selector.dataVal = (void*)&step;
		ctx << getSelectString(&selector);

		// am/pm
		selector.set(ampmChoose, name+"_ampm", ((isAM)?0:1), disabled);
		SFString ampm = "am/pm"; // must contain the '/'
		selector.dataVal = &ampm;
		ctx << getSelectString(&selector);
	}
	return ctx.str;
}

//--------------------------------------------------------------------------------------
SFString getIconChooser(const SFString& fieldName)
{
	return "<script>makePopupSelect('Choose Icon', '" + fieldName + "', baseRoot + 'javascript/iconPop.gif', iconRenderFunc);</script>\n";
}

//--------------------------------------------------------------------------------------
SFString getColorChooser(const SFString& fieldName)
{
	return "<script>makePopupSelect('Choose Color', '" + fieldName + "', baseRoot + 'javascript/colorPop.gif', colorRenderFunc);</script>\n";
}

//-------------------------------------------------------------------------
const char* IDS_JAV_DATECONTROLSPAN =
"\n<span id=[NAME]_span><input readonly name=[NAME] id=[NAME] type=text value=\"[VALUE]\"></span>\n";

const char* IDS_SEL_STATES=
"Select...|"
"Alabama+AL|Alaska+AK|American Samoa+AS|Arizona+AZ|Arkansas+AR|"
"California+CA|Colorado+CO|Connecticut+CT|Delaware+DE|District of Columbia+DC|"
"Federated States of Micronesia+FM|Florida+FL|Georgia+GA|Guam+GU|Hawaii+HI|"
"Idaho+ID|Illinois+IL|Indiana+IN|Iowa+IA|Kansas+KS|"
"Kentucky+KY|Louisiana+LA|Maine+ME|Marshall Islands+MH|Maryland+MD|"
"Massachusetts+MA|Michigan+MI|Minnesota+MN|Mississippi+MS|Missouri+MO|"
"Montana+MT|Nebraska+NE|Nevada+NV|New Hampshire+NH|New Jersey+NJ|"
"New Mexico+NM|New York+NY|North Carolina+NC|North Dakota+ND|Northern Mariana Islands+MP|"
"Ohio+OH|Oklahoma+OK|Oregon+OR|Palau+PW|Pennsylvania+PA|"
"Puerto Rico+PR|Rhode Island+RI|South Carolina+SC|South Dakota+SD|Tennessee+TN|"
"Texas+TX|Utah+UT|Vermont+VT|Virgin Islands+VI|Virginia+VA|"
"Washington+WA|West Virginia+WV|Wisconsin+WI|Wyoming+WY|";

const char* IDS_SEL_COUNTRIES=
"Select...|United States|United Kingdom|Afghanistan|Albania|"
"Algeria|American Samoa|Andorra|Angola|Anguilla|"
"Antarctica|Antigua and Barbuda|Argentina|Armenia|Aruba|"
"Australia|Austria|Azerbaijan|Bahamas|Bahrain|"
"Bangladesh|Barbados|Belarus|Belgium|Belize|"
"Benin|Bermuda|Bhutan|Bolivia|Bosnia and Herzegovina|"
"Botswana|Bouvet Island|Brazil|British Indian Ocean Territory|Brunei Darussalam|"
"Bulgaria|Burkina Faso|Burundi|Cambodia|Cameroon|"
"Canada|Cape Verde|Cayman Islands|Central African Republic|Chad|"
"Chile|China|Christmas Island|Cocos (Keeling) Islands|Colombia|"
"Comoros|Congo|Congo, The Democratic Republic of The|Cook Islands|Costa Rica|"
"Cote D'ivoire|Croatia|Cuba|Cyprus|Czech Republic|"
"Denmark|Djibouti|Dominica|Dominican Republic|Ecuador|"
"Egypt|El Salvador|Equatorial Guinea|Eritrea|Estonia|"
"Ethiopia|Falkland Islands (Malvinas)|Faroe Islands|Fiji|Finland|"
"France|French Guiana|French Polynesia|French Southern Territories|Gabon|"
"Gambia|Georgia|Germany|Ghana|Gibraltar|"
"Greece|Greenland|Grenada|Guadeloupe|Guam|"
"Guatemala|Guinea|Guinea-bissau|Guyana|Haiti|"
"Heard Island and Mcdonald Islands|Holy See (Vatican City State)|Honduras|Hong Kong|Hungary|"
"Iceland|India|Indonesia|Iran, Islamic Republic of|Iraq|"
"Ireland|Israel|Italy|Jamaica|Japan|"
"Jordan|Kazakhstan|Kenya|Kiribati|Korea, Democratic People's Republic of|"
"Korea, Republic of|Kuwait|Kyrgyzstan|Lao People's Democratic Republic|Latvia|"
"Lebanon|Lesotho|Liberia|Libyan Arab Jamahiriya|Liechtenstein|"
"Lithuania|Luxembourg|Macao|Macedonia, The Former Yugoslav Republic of|Madagascar|"
"Malawi|Malaysia|Maldives|Mali|Malta|"
"Marshall Islands|Martinique|Mauritania|Mauritius|Mayotte|"
"Mexico|Micronesia, Federated States of|Moldova, Republic of|Monaco|Mongolia|"
"Montserrat|Morocco|Mozambique|Myanmar|Namibia|"
"Nauru|Nepal|Netherlands|Netherlands Antilles|New Caledonia|"
"New Zealand|Nicaragua|Niger|Nigeria|Niue|"
"Norfolk Island|Northern Mariana Islands|Norway|Oman|Pakistan|"
"Palau|Palestinian Territory, Occupied|Panama|Papua New Guinea|Paraguay|"
"Peru|Philippines|Pitcairn|Poland|Portugal|"
"Puerto Rico|Qatar|Reunion|Romania|Russian Federation|"
"Rwanda|Saint Helena|Saint Kitts and Nevis|Saint Lucia|Saint Pierre and Miquelon|"
"Saint Vincent and The Grenadines|Samoa|San Marino|Sao Tome and Principe|Saudi Arabia|"
"Senegal|Serbia and Montenegro|Seychelles|Sierra Leone|Singapore|"
"Slovakia|Slovenia|Solomon Islands|Somalia|South Africa|"
"South Georgia and The South Sandwich Islands|Spain|Sri Lanka|Sudan|Suriname|"
"Svalbard and Jan Mayen|Swaziland|Sweden|Switzerland|Syrian Arab Republic|"
"Taiwan, Province of China|Tajikistan|Tanzania, United Republic of|Thailand|Timor-leste|"
"Togo|Tokelau|Tonga|Trinidad and Tobago|Tunisia|"
"Turkey|Turkmenistan|Turks and Caicos Islands|Tuvalu|Uganda|"
"Ukraine|United Arab Emirates|United Kingdom|United States|United States Minor Outlying Islands|"
"Uruguay|Uzbekistan|Vanuatu|Venezuela|Viet Nam|"
"Virgin Islands, British|Virgin Islands, U.S.|Wallis and Futuna|Western Sahara|Yemen|"
"Zambia|Zimbabwe|";

//----------------------------------------------------------------------------------------------------
const SFString permsPrompts[] =
{ "No Permission", "View Events", "Add Events", "Edit Events", "Administer Calendars", "Super User" };

//----------------------------------------------------------------------------------------------------
const SFString perms[] =
{ "NONE", "VIEW", "ADD", "EDIT", "ADMIN", "SUPER" };

//---------------------------------------------------------------------------------------
const char* IDS_WRD_RESET=
"reset";
