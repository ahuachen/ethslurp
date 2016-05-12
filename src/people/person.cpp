/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "peopledatabase.h"
#include "person.h"

//----------------------------------------------------------------------------------------------------
CPerson::CPerson(const CPeopleDatabase *db)
{
	Init();
	m_peopleDB = db;
}

//----------------------------------------------------------------------------------------------------
SFInt32 CPerson::daysSinceLastPW(void) const
{
#ifdef TESTING
	if (g_unitTesting && getInt32("test_forcePWAge", FALSE))
		return 1000;
#endif
	
	SFTime now = BOD(Now());
	SFTime lc  = BOD(getLastEditDate());
	SFTimeSpan ts = (now - lc);
	return ts.getTotalDays();
}

//----------------------------------------------------------------------------------------------------
SFInt32 CPerson::daysSinceLastLogin(void) const
{
	SFTime now = BOD(Now());
	SFTime lc  = BOD(getLastLoginDate());
	SFTimeSpan ts = (now - lc);
	return ts.getTotalDays();
}

//----------------------------------------------------------------------------------------------------
SFBool CPerson::needsReset(SFInt32 age, SFInt32 notSame, const SFString& testVal) const
{
	SFBool isReset = (getPassword() % testVal);
	SFBool isStale = (age > 0 && daysSinceLastPW() > age);
	SFBool isSame  = (notSame && hasSamePWasUID());

	if (isReset)
		return TRUE;
	if (isSuper())
		return FALSE;
	return (isStale || isSame);
}

//----------------------------------------------------------------------------------------------------
SFInt32 CPerson::getPermission(SFBool type) const
{
	// SUPERs are SUPERs everywhere
	if (m_defaultPerm == PERMISSION_SUPER)
		return PERMISSION_SUPER;
		
	if (type == PRM_DEFAULT)
		return m_defaultPerm;
	if (type == PRM_SPECIAL)
		return m_specialPerm;
	if (type == PRM_EDIT)
		return (isPending() ? getPendingPerm() : m_defaultPerm);

	ASSERT(type == PRM_APPARENT);
	return m_apparentPerm;
}

//----------------------------------------------------------------------------------------------------
void CPerson::setPermission(SFBool type, SFInt32 perm)
{
	if (type == PRM_DEFAULT)
		m_defaultPerm = perm;
	else if (type == PRM_SPECIAL)
		m_specialPerm = perm;
	else
	{
		ASSERT(type == PRM_APPARENT);
		m_apparentPerm = perm;
	}
}


//----------------------------------------------------------------------------------------------------
SFString CPerson::getFullName(SFBool asSort) const
{
	SFString prefix = getPrefix();
	SFString given  = getGiven();
	SFString family = getFamily();
	SFString suffix = getSuffix();

	// Chicago Manual of Style: Do not show both suffix and prefix
	if (!prefix.IsEmpty() && !suffix.IsEmpty() && !asSort)
		suffix = EMPTY;

	// Chicago Manual of Style: Do not show Jr. or Sr. if last name first name is empty
	if (given.IsEmpty() && (suffix % "jr." || suffix % "sr."))
		suffix = EMPTY;

	// Chicago Manual of Style: Do not use comma after name and before suffix
	SFString ret = Strip(Strip(prefix + "|" + given + "|" + family + "|" + suffix, '|'), ',');
	if (asSort)
		ret = Strip(Strip(family + "|" + suffix + ",|" + given, '|'), ',');
	ret.ReplaceAll("|,|", ",|");
	ret.ReplaceAll("|,|", EMPTY);
	ret.ReplaceAll("||",  "|");
	ret.ReplaceAll("|",   SEP_SPACE);
	if (ret == ",") return EMPTY;
	return ret;
}

//----------------------------------------------------------------------------------------------------
SFString CPerson::getSortName(void) const
{
	SFString ret = getFullName(TRUE);
	if (ret.IsEmpty())
	{
		// send user id...
		ret = getUserID();

		// unless nickname is not empty...
		if (!getNickname().IsEmpty())
			ret = getNickname();

		// unless company name is not empty...
		if (!m_company.getCompany().IsEmpty())
			ret = m_company.getCompany();
	}
	return ret;
}

//--------------------------------------------------------------------------------------------------------------
SFString CPostalInfo::getFullAddress(void) const
{
	SFString ret = "[ADDR1][ADDR2][ADDR3]";
	ret.Replace("[ADDR1]", ( m_address1.IsEmpty() ? EMPTY : m_address1 + "|"));
	ret.Replace("[ADDR2]", ( m_address2.IsEmpty() ? EMPTY : m_address2 + "|"));
	ret.Replace("[ADDR3]", (getLocale().IsEmpty() ? EMPTY : getLocale()));
	ret = Strip(ret, '|');
	ret.ReplaceAll("|", "<br>");
	return ret;
}

//--------------------------------------------------------------------------------------------------------------
SFString CPostalInfo::getDownloadAddr(void) const
{
	SFString ret = "[ADDR2];;[ADDR1];[CITY];[REGION];[ZIP];[COUNTRY]";
	ret.Replace("[ADDR1]",   m_address1  );
	ret.Replace("[ADDR2]",   m_address2  );
	ret.Replace("[CITY]",    m_city      );
	ret.Replace("[REGION]",  m_region    );
	ret.Replace("[ZIP]",     m_postalcode);
	ret.Replace("[COUNTRY]", m_country   );
	return ret;
}

//--------------------------------------------------------------------------------------------------------------
SFString CPostalInfo::getLocale(void) const
{
	SFString ret = Strip(Strip(m_city + "|" + m_region + "+" + m_postalcode, '|'), '+');
	ret.Replace("|+", " ");
	ret.Replace("|", ", ");
	ret.Replace("+", " ");
	return ret;
}

//----------------------------------------------------------------------------------------------------
static SFString getMainValue(const SFString& office, const SFString& home)
{
	SFBool o = !office.IsEmpty();
	SFBool h = !home.IsEmpty();
	if (o && !h) return office;
	if (h && !o) return home;
	if (o && h)
		return office + "<br>" + home;
	return EMPTY;
}

//----------------------------------------------------------------------------------------------------
SFString CPerson::getMainPhone(void) const
{
	return getMainValue(m_office.getPhone(), m_home.getPhone());
}

//----------------------------------------------------------------------------------------------------
SFString CPerson::getMainCell(void) const
{
	return getMainValue(m_office.getCell(), m_home.getCell());
}

//----------------------------------------------------------------------------------------------------
SFString CPerson::getMainFAX(void) const
{
	return getMainValue(m_office.getFax(), m_home.getFax());
}

//----------------------------------------------------------------------------------------------------
SFString CPerson::getMainPager(void) const
{
	return getMainValue(m_office.getPager(), m_home.getPager());
}

//----------------------------------------------------------------------------------------------------
SFString CPerson::getMainEmail(void) const
{
	return getMainValue(snagEmail(m_office.getEmail()), snagEmail(m_home.getEmail()));
}

//----------------------------------------------------------------------------------------------------
SFString CPerson::getMainEmailNoLink(void) const
{
	return getMainValue(m_office.getEmail(), m_home.getEmail());
}

//----------------------------------------------------------------------------------------------------
SFString CPerson::getMainWebpage(void) const
{
	return getMainValue(snagURL(m_office.getWebPage()), snagURL(m_home.getWebPage()));
}

//----------------------------------------------------------------------------------------------------
SFString CPerson::getMainWebpageNoLink(void) const
{
	return getMainValue(m_office.getWebPage(), m_home.getWebPage());
}

//----------------------------------------------------------------------------------------------------
SFString CPerson::getMainAddress(void) const
{
	return getMainValue(m_office.getFullAddress(), m_home.getFullAddress());
}

//----------------------------------------------------------------------------------------------------
SFString CPerson::getMainLocale(void) const
{
	return getMainValue(m_office.getLocale(), m_home.getLocale());
}

//----------------------------------------------------------------------------------------------------
const char* IDS_MSG_CREATOR_NEW=
"<font size=-1><i>created by: [CR] (new event)</i></font>";

const char* IDS_MSG_CREATOR_STR=
"<font size=-1><i>created: [CRDATE] by [CR] - edited: [LEDATE] by [LE]</i></font>";

const char* IDS_WRD_NOBODY=
"nobody";

//----------------------------------------------------------------------------------------------------
SFString CPerson::getCreatorString(void) const
{
	SFString fmt = LoadStringGH(IDS_MSG_CREATOR_STR);

	fmt.Replace("[CRDATE]", getCreateDate  ().Format (FMT_DATE) + displayTime1(getCreateDate  ()));
	fmt.Replace("[LEDATE]", getLastEditDate().Format (FMT_DATE) + displayTime1(getLastEditDate()));

	fmt.Replace("[CR]",     getCreator     ().IsEmpty() ? LoadStringGH(IDS_WRD_NOBODY) : getCreator   ());
	fmt.Replace("[LE]",     getLastEditor  ().IsEmpty() ? LoadStringGH(IDS_WRD_NOBODY) : getLastEditor());

	return fmt;
}

//----------------------------------------------------------------------------------------------------
SFString CPerson::getMainContact(void) const
{
	SFString ret = Strip(getFullName() + "|" + getMainPhone() + "|" + getMainCell() + "|" + getMainEmail() + "|" + getMainWebpage(), ' ');
	ret.ReplaceAll(LoadStringGH(IDS_WRD_REGISTRATION_USER), EMPTY);
	ret.ReplaceAll("||", "|");
	ret.ReplaceAll("|", "<br>");
	if (ret == "<br>")
		ret = EMPTY;
	return ret;
}

//----------------------------------------------------------------------------------------------------
void CPerson::setFieldValueByID(SFInt32 id, const SFString& value)
{
	SFInt32 valueI = atoi((const char*)value);
	SFTime date    = earliestDate;

#define oneURL(url) if (value % "http://") {} else { url.setWebPage(value); }
	switch (id)
	{
	case U_PREFIX:        setPrefix(value);                          break;
	case U_GIVENNAME:     setGiven(value);                           break;
	case U_FAMILYNAME:    setFamily(value);                          break;
	case U_SUFFIX:        setSuffix(value);                          break;
	case U_INITIALS:      setInitials(value);                        break;
	case U_USERTEXT1:     setUserText1(value);                       break;

	case B_PHONE:         m_office.setPhone(value);                  break;
	case B_FAX:           m_office.setFax(value);                    break;
	case B_CELL:          m_office.setCell(value);                   break;
	case B_PAGER:         m_office.setPager(value);                  break;

	case P_PHONE:         m_home.setPhone(value);                    break;
	case P_FAX:           m_home.setFax(value);                      break;
	case P_CELL:          m_home.setCell(value);                     break;
	case P_PAGER:         m_home.setPager(value);                    break;

	case B_COMPANY:       m_company.setCompany(value);               break;
	case B_ADDRESS_1:     m_office.setAddress1(value);               break;
	case B_ADDRESS_2:     m_office.setAddress2(value);               break;
	case B_CITY:          m_office.setCity(value);                   break;
	case B_REGION:        m_office.setRegion(value);                 break;
	case B_POSTALCODE:    m_office.setPostalCode(value);             break;
	case B_COUNTRY:       m_office.setCountry(value);                break;

	case P_ADDRESS_1:     m_home.setAddress1(value);                 break;
	case P_ADDRESS_2:     m_home.setAddress2(value);                 break;
	case P_CITY:          m_home.setCity(value);                     break;
	case P_REGION:        m_home.setRegion(value);                   break;
	case P_POSTALCODE:    m_home.setPostalCode(value);               break;
	case P_COUNTRY:       m_home.setCountry(value);                  break;

	case B_EMAIL:         m_office.setEmail(value);                  break;
	case B_WEBPAGE:       oneURL(m_office);                          break;

	case P_EMAIL:         m_home.setEmail(value);                    break;
	case P_WEBPAGE:       oneURL(m_home);                            break;

	case C_POSITION:      m_company.setPosition(value);              break;
	case C_DEPT:          m_company.setDept(value);                  break;
	case C_OFFICE:        m_company.setOffice(value);                break;
	case C_DOH:           m_company.m_doh.fromSortStr(value);        break;
	case C_USERTEXT2:     setUserText2(value);                       break;

	case P_NICKNAME:      setNickname(value);                        break;
	case P_SPOUSE:        m_personal.setSpouse(value);               break;
	case P_CHILDREN:      m_personal.setChildren(value);             break;
	case P_GENDER:        m_personal.setGender(value);               break;
	case P_BIRTHDATE:     m_personal.m_birthdate.fromSortStr(value); break;
	case P_ANNIVERSARY:   m_personal.m_anniversary.fromSortStr(value);break;
	case P_OPTIONSLIST:   setOptionsList(value);                     break;
	case P_NOTIFY:        setNotifyList(value);                      break;

	// display only
	// case D_SORTNAME:
	// case D_MAINPHONE:
	// case D_MAINCELL:
	// case D_MAINFAX:
	// case D_MAINPAGER:
	// case D_MAINEMAIL:
	// case D_MAINWEBPAGE:
	// case D_MAINADDRESS:
	// case D_MAINLOCALE:
	// case B_LOCALE:
	// case B_ADDRESS:
	// case P_LOCALE:
	// case P_ADDRESS:
	// case D_DOWNLOADNAME:
	// case D_DOWNLOADADDR:
	// case D_ALLPHONES:
	// case D_ALLBUSINESS:
	// case D_ALLPERSONAL:
	// case D_ALLOTHER:
	// case D_ALLREVIEW:
	// case D_CREATOR_STR:

	case U_NAME:          setFullName(value);                        break;
	case U_USERID:        setUserID(value);                          break;
	case U_PASSWORD:      setPassword(value);                        break;
	// case U_CONFIRM:
	case U_PERMISSION:    setPermission(PRM_DEFAULT, valueI);        break;
	case U_PERM_SPEC:     setPermission(PRM_SPECIAL, valueI);        break;
	case U_PASSHINT:      setPassHint(value);                        break;
	case U_TRUSTED:       setTrusted(valueI);                        break;
	case U_ACCESS:        setAccess(valueI);                         break;
	case U_DELETED:       setDeleted(valueI);                        break;
	case U_PENDING:       setPendingPerm(valueI);                    break;
	case U_CREATOR:       setCreator(value);                         break;
	case U_CREATEDATE:    m_createDate.fromSortStr(value);           break;
	case U_LASTEDITOR:    setLastEditor(value);                      break;
	case U_LASTCHANGE:    m_lastEditDate.fromSortStr(value);         break;
	case U_LASTLOGIN:     m_lastLoginDate.fromSortStr(value);        break;
	}
}

//----------------------------------------------------------------------------------------------------
SFString CPerson::getFieldValueByID(SFInt32 id, const SFString& def) const
{
	SFString str;
	
// hide bad data
#define oneDate(dt) ((dt > SFTime(1900,1,1,12,0,0)) ? dt.Format(FMT_SORTALL) : EMPTY);

	switch (id)
	{
	case U_PREFIX:		  str =          getPrefix                ();  break;
	case U_GIVENNAME:	  str =          getGiven                 ();  break;
	case U_FAMILYNAME:	  str =          getFamily                ();  break;
	case U_SUFFIX:		  str =          getSuffix                ();  break;
	case U_INITIALS:      str =          getInitials              ();  break;
	case U_USERTEXT1:     str =          getUserText1             ();  break;

	case B_PHONE:		  str =          m_office.getPhone        ();  break;
	case B_FAX:			  str =          m_office.getFax          ();  break;
	case B_CELL:		  str =          m_office.getCell         ();  break;
	case B_PAGER:		  str =          m_office.getPager        ();  break;

	case P_PHONE:		  str =          m_home.getPhone          ();  break;
	case P_FAX:			  str =          m_home.getFax            ();  break;
	case P_CELL:		  str =          m_home.getCell           ();  break;
	case P_PAGER:		  str =          m_home.getPager          ();  break;

	case B_COMPANY:		  str =          m_company.getCompany     ();  break;
	case B_ADDRESS_1:	  str =          m_office.getAddress1     ();  break;
	case B_ADDRESS_2:	  str =          m_office.getAddress2     ();  break;
	case B_CITY:		  str =          m_office.getCity         ();  break;
	case B_REGION:		  str =          m_office.getRegion       ();  break;
	case B_POSTALCODE:	  str =          m_office.getPostalCode   ();  break;
	case B_COUNTRY:		  str =          m_office.getCountry      ();  break;

	case P_ADDRESS_1:	  str =          m_home.getAddress1       ();  break;
	case P_ADDRESS_2:	  str =          m_home.getAddress2       ();  break;
	case P_CITY:		  str =          m_home.getCity           ();  break;
	case P_REGION:		  str =          m_home.getRegion         ();  break;
	case P_POSTALCODE:	  str =          m_home.getPostalCode     ();  break;
	case P_COUNTRY:		  str =          m_home.getCountry        ();  break;

	case B_EMAIL:		  str =          m_office.getEmail        ();  break;
	case B_WEBPAGE:		  str =          m_office.getWebPage      ();  break;

	case P_EMAIL:		  str =          m_home.getEmail          ();  break;
	case P_WEBPAGE:		  str =          m_home.getWebPage        ();  break;

	case C_POSITION:	  str =          m_company.getPosition    ();  break;
	case C_DEPT:		  str =          m_company.getDept        ();  break;
	case C_OFFICE:		  str =          m_company.getOffice      ();  break;
	case C_DOH:			  str = oneDate (m_company.getDOH         ()); break;
	case C_USERTEXT2:     str =          getUserText2             ();  break;

	case P_NICKNAME:	  str =          getNickname              ();  break;
	case P_SPOUSE:		  str =          m_personal.getSpouse     ();  break;
	case P_CHILDREN:	  str =          m_personal.getChildren   ();  break;
	case P_GENDER:		  str =          m_personal.getGender     ();  break;
	case P_BIRTHDATE:	  str = oneDate (m_personal.getBirthdate  ()); break;
	case P_ANNIVERSARY:	  str = oneDate (m_personal.getAnniversary()); break;
	case P_OPTIONSLIST:   str =          getOptionsList           ();  break;
	case P_NOTIFY:        str =          getNotifyList            ();  break;

	case D_SORTNAME:	  str =          getSortName              ();  break;
	case D_MAINPHONE:     str =          getMainPhone             ();  break;
	case D_MAINCELL:      str =          getMainCell              ();  break;
	case D_MAINFAX:       str =          getMainFAX               ();  break;
	case D_MAINPAGER:     str =          getMainPager             ();  break;
	case D_MAINEMAIL:     str =          getMainEmail             ();  break;
	case D_MAINWEBPAGE:   str =          getMainWebpage           ();  break;
	case D_MAINADDRESS:   str =          getMainAddress           ();  break;
	case D_MAINLOCALE:    str =          getMainLocale            ();  break;
	case B_LOCALE:		  str = m_office.getLocale                ();  break;
	case B_ADDRESS:       str = m_office.getFullAddress           ();  break;
	case P_LOCALE:		  str =   m_home.getLocale                ();  break;
	case P_ADDRESS:       str =   m_home.getFullAddress           ();  break;
	case D_DOWNLOADNAME:  str =          getFullName              ();  break;
	case D_DOWNLOADADDR:  str = m_office.getDownloadAddr          ();  break;
//	case D_ALLPHONES:     str =          EMPTY;                        break;
//	case D_ALLBUSINESS:   str =          EMPTY;                        break;
//	case D_ALLPERSONAL:   str =          EMPTY;                        break;
//	case D_ALLOTHER:      str =          EMPTY;                        break;
//	case D_ALLREVIEW:     str =          EMPTY;                        break;
	case D_CREATOR_STR:   str =          getCreatorString         ();  break;

	case U_NAME:		  str =          getFullName              ();  break;
	case U_USERID:		  str =          getUserID                ();  break;
	case U_PASSWORD:
	case U_CONFIRM:		  str =          getPassword              ();  break;
	case U_PERMISSION:	  str = asString(getPermission            (PRM_DEFAULT)); break;
	case U_PERM_SPEC:	  str = asString(getPermission            (PRM_SPECIAL)); break;
	case U_PASSHINT:	  str =          getPassHint              ();  break;
	case U_TRUSTED:		  str = asString(isTrusted                ()); break;
	case U_ACCESS:        str = asString(getAccess                ()); break;
	case U_DELETED:		  str = asString(isDeleted                ()); break;
	case U_PENDING:		  str = asString(getPendingPerm           ()); break;
	case U_CREATOR:		  str =          getCreator               ();  break;
	case U_CREATEDATE:	  str = oneDate (getCreateDate            ()); break;
	case U_LASTEDITOR:	  str =          getLastEditor            ();  break;
	case U_LASTCHANGE:	  str = oneDate (getLastEditDate          ()); break;
	case U_LASTLOGIN:	  str = oneDate (getLastLoginDate         ()); break;
	}

	switch (id)
	{
	case U_PREFIX:
	case U_SUFFIX:
	case B_REGION:
	case B_COUNTRY:
	case P_REGION:
	case P_COUNTRY:
		str = STRIP_DROP(str);
	}

	if (str.IsEmpty())
		str = def;

	return str;
}

//-------------------------------------------------------------------------------------
SFInt32 CPerson::sortByFieldID(const CPerson *p2, SFUint32 fieldID, SFBool reverse) const
{
	if (fieldID == (SFUint32)NOT_A_FIELD)
		return 0;

	SFString v1 = EMPTY, v2 = EMPTY;
	SFInt32  n1 = 0,     n2 = 0;

	SFInt32 type;
	switch (fieldID)
	{
	case C_DOH:
	case P_BIRTHDATE:
	case P_ANNIVERSARY:
	case U_LASTCHANGE: // returns sort dates anyway for some reason
	case U_CREATEDATE:
	case U_LASTLOGIN:
	default:
		type = T_TEXT;
		v1   =     getFieldValueByID(fieldID);
		v2   = p2->getFieldValueByID(fieldID);
		break;
	}

	if (reverse)
	{
		Swap(v1, v2);
		Swap(n1, n2);
	}

	switch (type)
	{
	case T_DATE:   ASSERT(0); break;
	case T_TEXT:   return strcasecmp(v1, v2);
	case T_NUMBER: return n1 - n2;
	}

	return 0;
}

//--------------------------------------------------------------------------------------------------------------
SFBool CPerson::isTrusted(void) const
{
	return m_trusted && (getPermission(PRM_DEFAULT) > PERMISSION_VIEW);
}

//--------------------------------------------------------------------------------------------------------------
SFInt32 CPerson::getFieldID(const SFString& fieldName) const
{
	ASSERT(getPeopleDB() && getPeopleDB()->getFieldList());
	const CFieldData *field = getPeopleDB()->getFieldList()->getFieldByName(fieldName);
	if (field) 
		return field->getFieldID();
	return NOT_A_FIELD;
}

//----------------------------------------------------------------------------------------------------
SFString CPerson::toAscii(const SFString& format) const
{
	ASSERT(getPeopleDB());
	SFString ret;

	SFInt32 iterator = 0;
	const CFieldData *field = CPerson::firstPersonField(iterator); ASSERT(field);
	while (field)
	{
		if (field->getFieldType() != T_PROMPT &&
			(field->getFieldID() & getPeopleDB()->m_FileFormat)) // THIS IS A BITWISE TEST
		{
			ASSERT(field);
			SFInt32  id     = field->getFieldID ();
			SFString prompt = field->getPrompt  ();
			SFString value  = getFieldValueByID (id, EMPTY);
			if (id == U_CONFIRM) // we don't store this
				value = EMPTY;
			SFBool   isPass = (id == U_PASSWORD || id == U_CONFIRM || id == U_PASSHINT);

			if (            id == U_USERID) value.ReplaceAll("&", EMPTY);
			if (format.IsEmpty())
			{
				value.ReplaceAll(SEP_SEMI, EMPTY);
				if (isPass) // encrypt passwordy things
					value = hard_encrypt(value, PW_KEY);
				ret += (value + SEP_SEMI);

			} else
			{
				if (isPass)
					value = "*****";
				if (format != "{FIELDNAME};" && id == U_PASSWORD && getPassword() == LoadStringGH(IDS_WRD_RESET))
					value += " (use password 'reset' on next login)";

				if (field->getFieldName() == "perm" || field->getFieldName() == "permspec")
				{
					SFInt32 val = atoi((const char *)value);
					value = CPermission::getPermissionStrLong(val);
				}

				SFBool showField = (id != U_CONFIRM && id != U_PENDING && id != U_DELETED);
				if (format == "{FIELDNAME};" || (!value.IsEmpty() && showField))
				{
					ret += format;
					if (!format.Contains("<tr")) // are we doing text or HTML?
						prompt = padRight(prompt, 30);
					if (id == U_TRUSTED)
						prompt = toProper(prompt);
					ret.Replace("{PROMPT}",    prompt);
					ret.Replace("{VALUE}",     value);
					ret.Replace("{FIELDNAME}", field->getFieldName());
				}
			}
		}

		field = CPerson::nextPersonField(iterator);
	}

	return ret;
}

//---------------------------------------------------------------------------------------
void CPerson::fromAscii(const SFString& lineIn, const CVersion& version)
{
	ASSERT(getPeopleDB());
	if (version.earlier(4,0,2))
	{
		fromAscii_4_0_1(lineIn, version);
		return;
	}

	SFInt32 fileFormat = getPeopleDB()->m_FileFormat;
	SFBool isUser    = (fileFormat & TYPE_USER);
	SFBool isContact = (fileFormat & TYPE_CONTACT);
	fileFormat = 0;
	if (isUser)
		fileFormat |= (version.eqEarlier(4,5,3) ? TYPE_USER_453 : TYPE_USER);
	if (isContact)
		fileFormat |= TYPE_CONTACT;

	SFString line = lineIn;
	SFInt32 iterator = 0;
	const CFieldData *field = CPerson::firstPersonField(iterator); ASSERT(field);
	while (field)
	{
		SFBool notPrompt   = field->getFieldType() != T_PROMPT;
		SFBool shouldWrite = (field->getFieldID() & fileFormat);
		if (notPrompt && shouldWrite)
		{
			SFString value = nextTokenClear(line, CH_SEMI);
			if (!value.IsEmpty())
			{
				SFInt32 id = field->getFieldID();
				setFieldValueByID(id, value);
			}
		}
		field = CPerson::nextPersonField(iterator);
	}
}

//---------------------------------------------------------------------------------------
SFString CPerson::getOption(const SFString& name)
{
	for (int i=0;i<m_nOptions;i++)
		if (m_options[i].getName() % name)
			return m_options[i].getValue();

	return EMPTY;
}

//---------------------------------------------------------------------------------------
SFBool CPerson::isOptionOn(const SFString& name)
{
	SFString val = getOption(name);
	return (val == "on" || val == "1");
}

//---------------------------------------------------------------------------------------
void CPerson::setOption(const SFString& name, const SFString& value)
{
	for (int i=0;i<m_nOptions;i++)
		if (m_options[i].getName() % name)
		{
			m_options[i].setValue(value);
			return;
		}

	if (m_nOptions<MAX_OPTIONS)
		m_options[m_nOptions++].set(name, value);
}

//---------------------------------------------------------------------------------------
void CPerson::setOptionsList(const SFString& list)
{
	m_nOptions    = 0;
	m_optionsList = list; // build it to tear it down...
	while (!m_optionsList.IsEmpty())
	{
		SFString value = nextTokenClear(m_optionsList, '|');
		SFString name  = nextTokenClear(value, '=');
		setOption(name, value);
	}
	// ... and then reset it for next time
	m_optionsList = list;
}

//---------------------------------------------------------------------------------------
SFString CPerson::getOptionsList(void) const
{
	CPerson *pThis = (CPerson*)this;
	pThis->m_optionsList = EMPTY;
	for (int i=0;i<m_nOptions;i++)
		pThis->m_optionsList += (m_options[i].getName()+"="+m_options[i].getValue()+"|");
	return m_optionsList;
}

//---------------------------------------------------------------------------------------
const char* IDS_WRD_REGISTRATION_USER =
"Registration_User";

