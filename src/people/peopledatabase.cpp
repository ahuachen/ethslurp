/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "peopledatabase.h"
#include "person.h"
#include "protectedobject.h"
#include "usergroup.h"

//----------------------------------------------------------------------------------------
CPeopleDatabase::CPeopleDatabase(void)
{
	m_FileFormat = TYPE_USER;

	m_nUsers	 = 0;
	m_users		 = NULL;

	m_nGroups    = 0;
	m_groups     = NULL;

	//m_peopleFields;
}

//----------------------------------------------------------------------------------------
CUserDatabase::CUserDatabase(void) : CPeopleDatabase()
{
	m_nPermissions	 = 0;
	m_permissions	 = NULL;

	m_nObjects		 = 0;
	m_objects    	 = NULL;

	// m_emailSettings
	// m_userSettings
	// m_subSettings
	// m_registration

	m_showHints      = FALSE;
}

//----------------------------------------------------------------------------------------
CPeopleDatabase::~CPeopleDatabase(void)
{
	if (m_users)
		delete [] m_users;
	m_users  = NULL;
	m_nUsers = 0;

	if (m_groups)
		delete [] m_groups;
	m_groups = NULL;
	m_nGroups = 0;

	ClearPersonFields();
}

//----------------------------------------------------------------------------------------
CUserDatabase::~CUserDatabase(void)
{
	if (m_permissions)
		delete [] m_permissions;
	m_permissions = NULL;
	m_nPermissions = 0;

	if (m_objects)
		delete [] m_objects;
	m_objects = NULL;
	m_nObjects = 0;
}

//----------------------------------------------------------------------------------------
void CPeopleDatabase::ClearPersonFields(void)
{
	LISTPOS fPos = m_peopleFields.GetFirstItem();
	while (fPos)
	{
		CFieldData *field = m_peopleFields.GetNextItem(fPos);
		delete field;
	}
	m_peopleFields.RemoveAll();
}

//----------------------------------------------------------------------------------------
void CPeopleDatabase::ReadDatabase(const SFString& dataPath, const SFString& curObject)
{
	// These two are only needed for user databases - not contact list databases so its OK if they are empty.
	m_dataPath  = dataPath;
	m_curObject = curObject;
	SFString contents = binaryFileToString(getFilename());
	if (!contents.IsEmpty())
	{
		LoadHeader   (contents);
		LoadUserTable(contents);
		LoadRemainder(contents);
	}
}

//----------------------------------------------------------------------------------------
inline SFString encrypt_local(const SFString& value, const SFString& def="none")
{
	SFString val = value;
	if (val.IsEmpty())
		val = def;
	char buff[8192];
	return SFString(SFos::encrypt(val, buff, 8192)) + "\n";
}

//----------------------------------------------------------------------------------------
void CUserDatabase::WriteDatabase(void)
{
	//
	// This writes the entire file
	//
	SFString filename = getFilename();

	if (Lock(filename, asciiWriteCreate, LOCK_CREATE)) // wait for lock, delete existing contents, open for writing
	{
		int i;

		// write first the user table...
		SFString version = "; file version=" + getVersionString();

		CStringExportContext ctx;
		ctx << encrypt_local("[VERSION]");
		ctx << encrypt_local(version);

		ctx << encrypt_local(";");
		ctx << encrypt_local(m_registration.toAscii());

		// DO NOT CHANGE THESE LINES: LoadHeader depends on it for sentinal to end of header
		ctx << encrypt_local(";");
		ctx << encrypt_local("; users");
		ctx << encrypt_local(::asString(m_nUsers));

		for (i=0;i<m_nUsers;i++)
				ctx << encrypt_local(m_users[i].toAscii(EMPTY));

		// ...then the permission table
		ctx << encrypt_local(";");
		ctx << encrypt_local("; permissions");
		ctx << encrypt_local(::asString(m_nPermissions));
		for (i=0;i<m_nPermissions;i++)
			if (!(m_permissions[i].getObjectID() % "all"))
				ctx << encrypt_local(m_permissions[i].toAscii());

		// ...then the group table
		ctx << encrypt_local(";");
		ctx << encrypt_local("; groups");
		ctx << encrypt_local(::asString(m_nGroups));
		for (i=0;i<m_nGroups;i++)
			ctx << encrypt_local(m_groups[i].toAscii(EMPTY));

		// ...then mail table
		ctx << encrypt_local(";");
		ctx << encrypt_local("; mail");
		ctx << encrypt_local(m_emailSettings.toAscii());

		// ...then the date settings
		ctx << encrypt_local(";");
		ctx << encrypt_local("; date settings");
		ctx << getAsciiDateSettings();

		// ...then the login settings
		ctx << encrypt_local(";");
		ctx << encrypt_local("; remote login");
		ctx << encrypt_local(m_loginSettings.toAscii());

		// ...then the submission settings
		if (m_subSettings.getReplaceStr() == "-explitive-") // for previous misspelling of this word
			m_subSettings.setReplaceStr("-expletive-");
		ctx << encrypt_local(";");
		ctx << encrypt_local("; submissions");
		ctx << encrypt_local(m_subSettings.toAscii());

		// ...then the user settings
		ctx << encrypt_local(";");
		ctx << encrypt_local("; user settings");
		ctx << encrypt_local(m_userSettings.toAscii());

		// now to the file
		WriteLine(ctx.str);
		ctx.str = "";

		Release();
	} else
	{
		// could not open file?
		LockFailure();
	}
}

//----------------------------------------------------------------------------------------
void CUserDatabase::LoadRemainder(SFString& contents)
{
	LoadPermissionTable   (contents);
	LoadGroupTable        (contents);
	LoadMailTable         (contents);
	LoadDateSettings      (contents);
	LoadLoginSettings     (contents);
	LoadSubmissionSettings(contents);
	LoadUnusedSettings    (contents);
	LoadUserSettings      (contents);
}

//----------------------------------------------------------------------------------------
void CUserDatabase::LoadUnusedSettings(SFString& contents)
{
	// Used to be reminder settings. Didn't exists under 2.5.2...
	if (m_version.eqEarlier(2,5,2)) // didn't exist yet
		return;

	// ...no longer exists after 4.6.2
	if (m_version.eqLater(4,6,2)) // exists no more
		return;

	// was three lines prior to 4.0.1, one line afterwards...
	// in either case we simply skip over it
	SFInt32  nLines    = (m_version.eqLater(4,0,1) ? 1 : 3);
	SFInt32  linesRead = 0;
	while (!contents.IsEmpty())
	{
		SFString line = nextTokenClearDecrypt(contents, '\n');
		if (!isComment(line)) // is this line a comment?
		{
			linesRead++;
			if (linesRead == nLines)
				break;
		}
	}

	return;
}

//----------------------------------------------------------------------------------------
SFBool CUserDatabase::hasHigherPermission(const CPerson& user1, const SFString& user2Name) const
{
	CPerson user2;
	SFBool found = ValidateUser(user2, user2Name, m_curObject);
	
	// if user2 is not found then of course user1 has higher permission
	if (!found)
		return TRUE;

	// Because the same user does not have higher permission but he should have
	// permission to do anything with his/her own stuff we make this special case
	if (user1.getUserID() == user2.getUserID())
		return TRUE;

	return (user1.isTrusted() || user1.getPermission(PRM_DEFAULT) > user2.getPermission(PRM_DEFAULT));
}

//----------------------------------------------------------------------------------------
SFString CUserDatabase::getAsciiDateSettings(void) const
{
	CStringExportContext ctx;
	
	ctx << encrypt_local(SFos::asciiDateNames(0));
	ctx << encrypt_local(SFos::asciiDateNames(1));
	ctx << encrypt_local(SFos::asciiDateNames(2));
	ctx << encrypt_local(SFos::asciiDateNames(3));

	SFString dSep = SFos::getDateSep();
	dSep.ReplaceAll(";", "$");
	SFString tSep = SFos::getTimeSep();
	tSep.ReplaceAll(";", "$");

	CStringExportContext ctx1;
	ctx1 
		<< SFos::isEuropean()  << ";" 
		<< SFos::isMilitary()  << ";" 
		<< dSep                << ";" 
		<< tSep                << ";" 
		<< SFos::getFirstDay() << ";"
		<< SFos::getTimeZone();

	ctx << encrypt_local(ctx1.str);
	return ctx.str;
}

//----------------------------------------------------------------------------------------
const CFieldList *CPeopleDatabase::getFieldList(void) const
{
	if (!m_peopleFields.GetCount())
	{
		SFInt32 iterator = 0;
		const CFieldData *field = CPerson::firstPersonField(iterator);
		while (field)
		{
			CFieldData copy = *field;
			((CPeopleDatabase*)this)->m_peopleFields.AddTail(new CFieldData(copy));
			field = CPerson::nextPersonField(iterator);
		}
	}
	return &m_peopleFields;
}

#include "usergroup.h"

//----------------------------------------------------------------------------------------------------
SFString PRS_NAMES             = "NAMES|names|Name";
SFString PRS_PREFIX            = "NAMES|prefix|Prefix";
SFString PRS_GIVENNAME         = "NAMES|given_name|Given Name";
SFString PRS_FAMILYNAME        = "NAMES|family_name|Family Name";
SFString PRS_SUFFIX            = "NAMES|suffix|Suffix";
SFString PRS_INITIALS          = "NAMES|initials|Initials";
SFString PRS_USERTEXT1         = "NAMES|usertext1|";

SFString PRS_BUSINESS_PHONES   = "BUSINESS_PHONES|business_phones|Business Phones";
SFString PRS_B_PHONE           = "BUSINESS_PHONES|phone|Phone";
SFString PRS_B_CELL            = "BUSINESS_PHONES|cell|Cell";
SFString PRS_B_FAX             = "BUSINESS_PHONES|fax|Fax";
SFString PRS_B_PAGER           = "BUSINESS_PHONES|pager|Pager";

SFString PRS_PERSONAL_PHONES   = "PERSONAL_PHONES|personal_phones|Personal Phones";
SFString PRS_P_PHONE           = "PERSONAL_PHONES|h_phone|Phone";
SFString PRS_P_CELL            = "PERSONAL_PHONES|h_cell|Cell";
SFString PRS_P_FAX             = "PERSONAL_PHONES|h_fax|Fax";
SFString PRS_P_PAGER           = "PERSONAL_PHONES|h_pager|Pager";

SFString PRS_BUSINESS_ADDRESS  = "BUSINESS_ADDRESS|business_address|Business Address";
SFString PRS_B_COMPANY         = "BUSINESS_ADDRESS|company_name|Company Name";
SFString PRS_B_ADDRESS_1       = "BUSINESS_ADDRESS|address_1|Address 1";
SFString PRS_B_ADDRESS_2       = "BUSINESS_ADDRESS|address_2|Address 2";
SFString PRS_B_CITY            = "BUSINESS_ADDRESS|city|City";
SFString PRS_B_REGION          = "BUSINESS_ADDRESS|region|Region";
SFString PRS_B_POSTALCODE      = "BUSINESS_ADDRESS|postalcode|Postal Code";
SFString PRS_B_COUNTRY         = "BUSINESS_ADDRESS|country|Country";
SFString PRS_B_LOCALE          = "BUSINESS_ADDRESS|locale|Locale";
SFString PRS_B_ADDRESS         = "BUSINESS_ADDRESS|fulladdr|Address";

SFString PRS_PERSONAL_ADDRESS  = "PERSONAL_ADDRESS|personal_address|Personal Address";
SFString PRS_P_ADDRESS_1       = "PERSONAL_ADDRESS|h_address_1|Address 1";
SFString PRS_P_ADDRESS_2       = "PERSONAL_ADDRESS|h_address_2|Address 2";
SFString PRS_P_CITY            = "PERSONAL_ADDRESS|h_city|City";
SFString PRS_P_REGION          = "PERSONAL_ADDRESS|h_region|Region";
SFString PRS_P_POSTALCODE      = "PERSONAL_ADDRESS|h_postalcode|Postal Code";
SFString PRS_P_COUNTRY         = "PERSONAL_ADDRESS|h_country|Country";
SFString PRS_P_LOCALE          = "PERSONAL_ADDRESS|h_locale|Locale";
SFString PRS_P_ADDRESS         = "PERSONAL_ADDRESS|h_fulladdr|Address";

SFString PRS_BUSINESS_INTERNET = "BUSINESS_INTERNET|business_internet|Business Internet";
SFString PRS_B_EMAIL           = "BUSINESS_INTERNET|email|Email";
SFString PRS_B_WEBPAGE         = "BUSINESS_INTERNET|webpage|Web";

SFString PRS_PERSONAL_INTERNET = "PERSONAL_INTERNET|personal_internet|Personal Internet";
SFString PRS_P_EMAIL           = "PERSONAL_INTERNET|h_email|Email";
SFString PRS_P_WEBPAGE         = "PERSONAL_INTERNET|h_webpage|Web";

SFString PRS_COMPANY_INFO      = "COMPANY|company|Company";
SFString PRS_C_POSITION        = "COMPANY|position|Position";
SFString PRS_C_DEPT            = "COMPANY|dept|Dept";
SFString PRS_C_OFFICE          = "COMPANY|office|Office";
SFString PRS_C_DOH             = "COMPANY|doh|";
SFString PRS_C_USERTEXT2       = "COMPANY|usertext2|";

SFString PRS_PERSONAL_INFO     = "OTHER_DATA|other_data|Other";
SFString PRS_P_NICKNAME        = "OTHER_DATA|nickname|Nickname";
SFString PRS_P_SPOUSE          = "OTHER_DATA|spouse|Spouse";
SFString PRS_P_CHILDREN        = "OTHER_DATA|children|Children";
SFString PRS_P_GENDER          = "OTHER_DATA|gender|Gender";
SFString PRS_P_BIRTHDATE       = "OTHER_DATA|birthdate|";
SFString PRS_P_ANNIVERSARY     = "OTHER_DATA|anniversary|";
SFString PRS_P_OPTIONSLIST     = "OTHER_DATA|options|";
SFString PRS_P_NOTIFY          = "OTHER_DATA|notify|";

SFString PRS_D_SORTNAME        = "DISPLAY|sort_name|Name";
SFString PRS_D_MAINPHONE       = "DISPLAY|main_phone|Phone";
SFString PRS_D_MAINCELL        = "DISPLAY|main_cell|Cell";
SFString PRS_D_MAINFAX         = "DISPLAY|main_fax|Fax";
SFString PRS_D_MAINPAGER       = "DISPLAY|main_pager|Pager";
SFString PRS_D_MAINEMAIL       = "DISPLAY|main_email|Email";
SFString PRS_D_MAINWEBPAGE     = "DISPLAY|main_webpage|Webpage";
SFString PRS_D_MAINLOCALE      = "DISPLAY|main_locale|Locale";
SFString PRS_D_MAINADDRESS     = "DISPLAY|main_fulladdr|Address";
SFString PRS_D_DOWNLOADNAME    = "DISPLAY|download_name|Download Name";
SFString PRS_D_DOWNLOADADDR    = "DISPLAY|download_address|Download Address";
SFString PRS_D_ALLPHONES       = "DISPLAY|all_phones|Phones";
SFString PRS_D_ALLBUSINESS     = "DISPLAY|all_business|All Business";
SFString PRS_D_ALLPERSONAL     = "DISPLAY|all_personal|All Personal";
SFString PRS_D_ALLOTHER        = "DISPLAY|all_other|All Other";
SFString PRS_D_ALLREVIEW       = "DISPLAY|all_review|All Review";
SFString PRS_D_CREATOR_STR     = "DISPLAY|creator_str|Creator";

SFString PRS_USERDB_DATA       = "USERDB_DATA|userdb_data|Owner Data";
SFString PRS_U_NAME            = "USERDB_DATA|name|Name";
SFString PRS_U_USERID          = "USERDB_DATA|userid|User ID";
SFString PRS_U_PASSWORD        = "USERDB_DATA|password|Password";
SFString PRS_U_CONFIRM         = "USERDB_DATA|confirm|Confirm";
SFString PRS_U_PERMISSION      = "USERDB_DATA|perm|Default Permission";
SFString PRS_U_PERM_SPEC       = "USERDB_DATA|permspec|Special Permission";
SFString PRS_U_PASSHINT        = "USERDB_DATA|hint|Password Hint";
SFString PRS_U_TRUSTED         = "USERDB_DATA|trusted|trusted user";
SFString PRS_U_ACCESS          = "USERDB_DATA|access|Access Level";
SFString PRS_U_DELETED         = "USERDB_DATA|deleted|Deleted";
SFString PRS_U_PENDING         = "USERDB_DATA|pending|Pending"; //NOTE: This is never used for pending submissions - only for pending user perm changes - pending submissions always look at eventedit.dat
SFString PRS_U_CREATOR         = "USERDB_DATA|creator|Owner";
SFString PRS_U_CREATEDATE      = "USERDB_DATA|create_date|Create Date";
SFString PRS_U_LASTEDITOR      = "USERDB_DATA|lasteditor|Last Editor";
SFString PRS_U_LASTCHANGE      = "USERDB_DATA|edit_date|Last Change Date";
SFString PRS_U_LASTLOGIN       = "USERDB_DATA|login_date|Last Login Date";

SFString GRP_GROUP_DATA        = "GROUP_DATA|group_data|Group Data";
SFString GRP_NAME              = "GROUP_DATA|gr_name|Group name";
SFString GRP_DESCR             = "GROUP_DATA|gr_descr|Description";
SFString GRP_MEMBERS           = "GROUP_DATA|gr_members|Members";
SFString GRP_PERMISSION        = "GROUP_DATA|gr_permission|Group Permission";
SFString GRP_SUBSCRIBE         = "GROUP_DATA|gr_subscribe|Subscribable";
SFString GRP_DELETED           = "GROUP_DATA|gr_deleted|Deleted";
SFString GRP_EXPANDED          = "GROUP_DATA|gr_expanded|Expanded";

//---------------------------------------------------------------------------------------
SFString getRest(SFInt32 exp, SFInt32 type, SFInt32 id, const SFString& def, SFInt32 req)
{
	// format is 'default|required|export|leastPerm|access|help|type|id|'
	SFString ret = "|DE|RE|EX|LP|AC|HP|TY|ID|";
	ret.Replace("|DE|", "|" + def                       + "|"); // default
	ret.Replace("|RE|", "|" + asString(req)             + "|"); // required
	ret.Replace("|EX|", "|" + asString(exp)             + "|"); // export
	ret.Replace("|LP|", "|" + asString(PERMISSION_NONE) + "|"); // least permission
	ret.Replace("|AC|", "|" + asString(0)               + "|"); // access
	ret.Replace("|HP|", "|" + EMPTY                     + "|"); // field help id
	ret.Replace("|TY|", "|" + asString(type)            + "|"); // type
	ret.Replace("|ID|", "|" + asString(id)              + "|"); // fieldID
	return ret;
}

//----------------------------------------------------------------------------------------
CFieldData person_items[] =
{
	// Do not move this entry.  Userid is the only field assured to be non-empty.  If empty
	// field is first field in line the line is a comment in the data file and not read.
	CFieldData( PRS_U_USERID          + getRest( TRUE,  T_TEXT,      U_USERID,			EMPTY, FALSE)),

	CFieldData( PRS_NAMES             + getRest( FALSE, T_PROMPT,    NAMES,				EMPTY, FALSE)),
	CFieldData( PRS_PREFIX            + getRest( TRUE,  T_SELSEARCH, U_PREFIX,			EMPTY, FALSE)),
	CFieldData( PRS_GIVENNAME         + getRest( TRUE,  T_SHORTTEXT, U_GIVENNAME,		EMPTY, FALSE)),
	CFieldData( PRS_FAMILYNAME        + getRest( TRUE,  T_TEXT,      U_FAMILYNAME,		EMPTY, TRUE )),
	CFieldData( PRS_SUFFIX            + getRest( TRUE,  T_SELSEARCH, U_SUFFIX,			EMPTY, FALSE)),
	CFieldData( PRS_INITIALS          + getRest( TRUE,  T_SHORTTEXT, U_INITIALS,		EMPTY, FALSE)),
	CFieldData( PRS_USERTEXT1         + getRest( TRUE,  T_SHORTTEXT, U_USERTEXT1,		EMPTY, FALSE)),

	CFieldData( PRS_BUSINESS_PHONES   + getRest( FALSE, T_PROMPT,    BUSINESS_PHONES,	EMPTY, FALSE)),
	CFieldData( PRS_B_PHONE           + getRest( TRUE,  T_PHONE,     B_PHONE,			EMPTY, FALSE)),
	CFieldData( PRS_B_CELL            + getRest( TRUE,  T_PHONE,     B_CELL,			EMPTY, FALSE)),
	CFieldData( PRS_B_FAX             + getRest( TRUE,  T_PHONE,     B_FAX,				EMPTY, FALSE)),
	CFieldData( PRS_B_PAGER           + getRest( TRUE,  T_PHONE,     B_PAGER,			EMPTY, FALSE)),

	CFieldData( PRS_PERSONAL_PHONES   + getRest( FALSE, T_PROMPT,    PERSONAL_PHONES,	EMPTY, FALSE)),
	CFieldData( PRS_P_PHONE           + getRest( TRUE,  T_PHONE,     P_PHONE,			EMPTY, FALSE)),
	CFieldData( PRS_P_CELL            + getRest( TRUE,  T_PHONE,     P_CELL,			EMPTY, FALSE)),
	CFieldData( PRS_P_FAX             + getRest( TRUE,  T_PHONE,     P_FAX,				EMPTY, FALSE)),
	CFieldData( PRS_P_PAGER           + getRest( TRUE,  T_PHONE,     P_PAGER,			EMPTY, FALSE)),

	CFieldData( PRS_BUSINESS_ADDRESS  + getRest( FALSE, T_PROMPT,    BUSINESS_ADDRESS,	EMPTY, FALSE)),
	CFieldData( PRS_B_COMPANY         + getRest( TRUE,  T_TEXT,      B_COMPANY,			EMPTY, FALSE)),
	CFieldData( PRS_B_ADDRESS_1       + getRest( TRUE,  T_TEXT,      B_ADDRESS_1,		EMPTY, FALSE)),
	CFieldData( PRS_B_ADDRESS_2       + getRest( TRUE,  T_TEXT,      B_ADDRESS_2,		EMPTY, FALSE)),
	CFieldData( PRS_B_CITY            + getRest( TRUE,  T_SHORTTEXT, B_CITY,			EMPTY, FALSE)),
	CFieldData( PRS_B_REGION          + getRest( TRUE,  T_SELSEARCH, B_REGION,			EMPTY, FALSE)),
	CFieldData( PRS_B_POSTALCODE      + getRest( TRUE,  T_SHORTTEXT, B_POSTALCODE,		EMPTY, FALSE)),
	CFieldData( PRS_B_COUNTRY         + getRest( TRUE,  T_SELSEARCH, B_COUNTRY,			EMPTY, FALSE)),
	CFieldData( PRS_B_LOCALE          + getRest( FALSE, T_TEXT,      B_LOCALE,			EMPTY, FALSE)),
	CFieldData( PRS_B_ADDRESS         + getRest( FALSE, T_TEXT,      B_ADDRESS,			EMPTY, FALSE)),

	CFieldData( PRS_PERSONAL_ADDRESS  + getRest( FALSE, T_PROMPT,    PERSONAL_ADDRESS,	EMPTY, FALSE)),
	CFieldData( PRS_P_ADDRESS_1       + getRest( TRUE,  T_TEXT,      P_ADDRESS_1,		EMPTY, FALSE)),
	CFieldData( PRS_P_ADDRESS_2       + getRest( TRUE,  T_TEXT,      P_ADDRESS_2,		EMPTY, FALSE)),
	CFieldData( PRS_P_CITY            + getRest( TRUE,  T_SHORTTEXT, P_CITY,			EMPTY, FALSE)),
	CFieldData( PRS_P_REGION          + getRest( TRUE,  T_SELSEARCH, P_REGION,			EMPTY, FALSE)),
	CFieldData( PRS_P_POSTALCODE      + getRest( TRUE,  T_SHORTTEXT, P_POSTALCODE,		EMPTY, FALSE)),
	CFieldData( PRS_P_COUNTRY         + getRest( TRUE,  T_SELSEARCH, P_COUNTRY,			EMPTY, FALSE)),
	CFieldData( PRS_P_LOCALE          + getRest( FALSE, T_TEXT,      P_LOCALE,		    EMPTY, FALSE)),
	CFieldData( PRS_P_ADDRESS         + getRest( FALSE, T_TEXT,      P_ADDRESS,			EMPTY, FALSE)),

	CFieldData( PRS_BUSINESS_INTERNET + getRest( TRUE,  T_PROMPT,    BUSINESS_INTERNET,	EMPTY, FALSE)),
	CFieldData( PRS_B_EMAIL           + getRest( TRUE,  T_EMAIL,     B_EMAIL,			EMPTY, FALSE)),
	CFieldData( PRS_B_WEBPAGE         + getRest( TRUE,  T_TEXT,      B_WEBPAGE,			"http://", FALSE)),

	CFieldData( PRS_PERSONAL_INTERNET + getRest( TRUE,  T_PROMPT,    PERSONAL_INTERNET,	EMPTY, FALSE)),
	CFieldData( PRS_P_EMAIL           + getRest( TRUE,  T_EMAIL,     P_EMAIL,			EMPTY, FALSE)),
	CFieldData( PRS_P_WEBPAGE         + getRest( TRUE,  T_TEXT,      P_WEBPAGE,			"http://", FALSE)),

	CFieldData( PRS_COMPANY_INFO      + getRest( FALSE, T_PROMPT,    COMPANY_INFO,		EMPTY, FALSE)),
	CFieldData( PRS_C_POSITION        + getRest( TRUE,  T_TEXT,      C_POSITION,		EMPTY, FALSE)),
	CFieldData( PRS_C_DEPT            + getRest( TRUE,  T_SHORTTEXT, C_DEPT,			EMPTY, FALSE)),
	CFieldData( PRS_C_OFFICE          + getRest( TRUE,  T_SHORTTEXT, C_OFFICE,			EMPTY, FALSE)),
	CFieldData( PRS_C_DOH             + getRest( TRUE,  T_DATE,      C_DOH,				EMPTY, FALSE)),
	CFieldData( PRS_C_USERTEXT2       + getRest( TRUE,  T_SHORTTEXT, C_USERTEXT2,		EMPTY, FALSE)),

	CFieldData( PRS_PERSONAL_INFO     + getRest( FALSE, T_PROMPT,    PERSONAL_INFO,		EMPTY, FALSE)),
	CFieldData( PRS_P_NICKNAME        + getRest( TRUE,  T_SHORTTEXT, P_NICKNAME,		EMPTY, FALSE)),
	CFieldData( PRS_P_SPOUSE          + getRest( TRUE,  T_TEXT,      P_SPOUSE,			EMPTY, FALSE)),
	CFieldData( PRS_P_CHILDREN        + getRest( TRUE,  T_TEXT,      P_CHILDREN,		EMPTY, FALSE)),
	CFieldData( PRS_P_GENDER          + getRest( TRUE,  T_SELSEARCH, P_GENDER,			EMPTY, FALSE)),
	CFieldData( PRS_P_BIRTHDATE       + getRest( TRUE,  T_DATE,      P_BIRTHDATE,		EMPTY, FALSE)),
	CFieldData( PRS_P_ANNIVERSARY     + getRest( TRUE,  T_DATE,      P_ANNIVERSARY,		EMPTY, FALSE)),
	CFieldData( PRS_P_OPTIONSLIST     + getRest( TRUE,  T_SHORTTEXT, P_OPTIONSLIST,		EMPTY, FALSE)),
	CFieldData( PRS_P_NOTIFY          + getRest( TRUE,  T_TEXT,      P_NOTIFY,	        EMPTY, FALSE)),

	// Display only
	CFieldData( PRS_D_SORTNAME        + getRest( FALSE, T_PROMPT,    D_SORTNAME,		EMPTY, FALSE)),
	CFieldData( PRS_D_MAINPHONE       + getRest( FALSE, T_PROMPT,    D_MAINPHONE,		EMPTY, FALSE)),
	CFieldData( PRS_D_MAINCELL        + getRest( FALSE, T_PROMPT,    D_MAINCELL,		EMPTY, FALSE)),
	CFieldData( PRS_D_MAINFAX         + getRest( FALSE, T_PROMPT,    D_MAINFAX,			EMPTY, FALSE)),
	CFieldData( PRS_D_MAINPAGER       + getRest( FALSE, T_PROMPT,    D_MAINPAGER,		EMPTY, FALSE)),
	CFieldData( PRS_D_MAINEMAIL       + getRest( FALSE, T_PROMPT,    D_MAINEMAIL,		EMPTY, FALSE)),
	CFieldData( PRS_D_MAINWEBPAGE     + getRest( FALSE, T_PROMPT,    D_MAINWEBPAGE,		EMPTY, FALSE)),
	CFieldData( PRS_D_MAINLOCALE      + getRest( FALSE, T_PROMPT,    D_MAINLOCALE,		EMPTY, FALSE)),
	CFieldData( PRS_D_MAINADDRESS     + getRest( FALSE, T_PROMPT,    D_MAINADDRESS,		EMPTY, FALSE)),
	CFieldData( PRS_D_DOWNLOADNAME    + getRest( FALSE, T_PROMPT,    D_DOWNLOADNAME,    EMPTY, FALSE)),
	CFieldData( PRS_D_DOWNLOADADDR    + getRest( FALSE, T_PROMPT,    D_DOWNLOADADDR,    EMPTY, FALSE)),
	CFieldData( PRS_D_ALLBUSINESS     + getRest( FALSE, T_PROMPT,    D_ALLBUSINESS,     EMPTY, FALSE)),
	CFieldData( PRS_D_ALLPERSONAL     + getRest( FALSE, T_PROMPT,    D_ALLPERSONAL,     EMPTY, FALSE)),
	CFieldData( PRS_D_ALLOTHER        + getRest( FALSE, T_PROMPT,    D_ALLOTHER,        EMPTY, FALSE)),
	CFieldData( PRS_D_ALLREVIEW       + getRest( FALSE, T_PROMPT,    D_ALLREVIEW,       EMPTY, FALSE)),
	CFieldData( PRS_D_ALLPHONES       + getRest( FALSE, T_PROMPT,    D_ALLPHONES,       EMPTY, FALSE)),
	CFieldData( PRS_D_CREATOR_STR     + getRest( FALSE, T_PROMPT,    D_CREATOR_STR,		EMPTY, FALSE)),

	// User data
	CFieldData( PRS_USERDB_DATA       + getRest( FALSE, T_PROMPT,    USERDB_DATA,		EMPTY, FALSE)),
	CFieldData( PRS_U_NAME            + getRest( TRUE,  T_TEXT,      U_NAME,			EMPTY, FALSE)),
	CFieldData( PRS_U_PASSWORD        + getRest( TRUE,  T_LABEL,     U_PASSWORD,		EMPTY, FALSE)),
	CFieldData( PRS_U_CONFIRM         + getRest( TRUE,  T_LABEL,     U_CONFIRM,			EMPTY, FALSE)),
	CFieldData( PRS_U_PERMISSION      + getRest( TRUE,  T_LABEL,     U_PERMISSION,		EMPTY, FALSE)),
	CFieldData( PRS_U_PERM_SPEC       + getRest( TRUE,  T_LABEL,     U_PERM_SPEC,		EMPTY, FALSE)),
	CFieldData( PRS_U_ACCESS          + getRest( TRUE,  T_SELSEARCH, U_ACCESS,		    EMPTY, FALSE)),
	CFieldData( PRS_U_PASSHINT        + getRest( TRUE,  T_LABEL,     U_PASSHINT,		EMPTY, FALSE)),
	CFieldData( PRS_U_TRUSTED         + getRest( TRUE,  T_LABEL,     U_TRUSTED,			EMPTY, FALSE)),
	CFieldData( PRS_U_DELETED         + getRest( TRUE,  T_CHECKSRCH, U_DELETED,			EMPTY, FALSE)),
	CFieldData( PRS_U_PENDING         + getRest( TRUE,  T_CHECKSRCH, U_PENDING,			"-1",  FALSE)),
	CFieldData( PRS_U_CREATOR         + getRest( TRUE,  T_TEXT,      U_CREATOR,			"{USER_USERID}", TRUE ) ),
	CFieldData( PRS_U_CREATEDATE      + getRest( TRUE,  T_LABEL,     U_CREATEDATE,		EMPTY, FALSE)),
	CFieldData( PRS_U_LASTEDITOR      + getRest( TRUE,  T_TEXT,      U_LASTEDITOR,		EMPTY, TRUE )),
	CFieldData( PRS_U_LASTCHANGE      + getRest( TRUE,  T_LABEL,     U_LASTCHANGE,		EMPTY, FALSE)),
	CFieldData( PRS_U_LASTLOGIN       + getRest( TRUE,  T_LABEL,     U_LASTLOGIN,		EMPTY, FALSE)),

	// Group data
	CFieldData( GRP_GROUP_DATA        + getRest( FALSE, T_PROMPT,    GROUP_DATA,		EMPTY, FALSE)),
	CFieldData( GRP_NAME              + getRest( TRUE,  T_TEXT,      GR_NAME,			EMPTY, FALSE)),
	CFieldData( GRP_DESCR             + getRest( TRUE,  T_TEXT,      GR_DESCR,			EMPTY, FALSE)),
	CFieldData( GRP_MEMBERS           + getRest( TRUE,  T_TEXT,      GR_MEMBERS,		EMPTY, FALSE)),
	CFieldData( GRP_PERMISSION        + getRest( TRUE,  T_TEXT,      GR_PERMISSION,		EMPTY, FALSE)),
	CFieldData( GRP_SUBSCRIBE         + getRest( TRUE,  T_TEXT,      GR_SUBSCRIBE,		EMPTY, FALSE)),
	CFieldData( GRP_DELETED           + getRest( TRUE,  T_TEXT,      GR_DELETED,		EMPTY, FALSE)),
	CFieldData( GRP_EXPANDED          + getRest( TRUE,  T_TEXT,      GR_EXPANDED,		EMPTY, FALSE)),
};
const SFInt32 nPersonItems = sizeof(person_items) / sizeof(CFieldData);

//----------------------------------------------------------------------------------------
SFInt32 CPerson::nPersonFields(void)
{
	return nPersonItems;
}

//----------------------------------------------------------------------------------------
const CFieldData *CPerson::firstPersonField(SFInt32& iterator, SFInt32 start)
{
	iterator = start;
	return CPerson::nextPersonField(iterator);
}

//----------------------------------------------------------------------------------------
const CFieldData *CPerson::peekPersonField(SFInt32& iterator, SFInt32 item)
{
	if (item == -1)
		item = iterator-1;
	return &person_items[item];
}

//----------------------------------------------------------------------------------------
const CFieldData *CPerson::nextPersonField(SFInt32& iterator, SFInt32 stop)
{
	if (iterator == ((stop == -1) ? nPersonFields() : stop))
		return NULL;

	CFieldData *ret = &person_items[iterator];
	iterator++;
	return ret;
}

//---------------------------------------------------------------------------------------
SFBool CPeopleDatabase::showField(const CFieldData *field) const
{
	return !field->isHidden();
}

//---------------------------------------------------------------------------------------
SFInt32 CPeopleDatabase::applyToAll(CONTACTAPPLYFUNC applyFunc, void *data, SFBool single, SFBool writeBack)
{
	ASSERT(applyFunc && data);
	SFInt32 cnt=0;
	ReadDatabase(m_dataPath, m_curObject);
	for (int i=0;i<m_nUsers;i++)
	{
		if (applyFunc(&m_users[i], data))
		{
			cnt++;
			if (single)
			{
				if (writeBack)
					WriteDatabase();
				return cnt;
			}
		}
	}

	if (writeBack)
		WriteDatabase();

	return cnt;
}
