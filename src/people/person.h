#ifndef _PERSON_H
#define _PERSON_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "postalinfo.h"
#include "companyinfo.h"
#include "personalinfo.h"
#include "editableobject.h"
#include "peopledatabase.h"

#define MAX_OPTIONS 30

class CServer;

class CPerson : public CEditableObject
{
private:
		  char             m_userid[USERID_STRING_LEN];
		  SFString         m_password;

		  SFString         m_prefix;
		  SFString         m_given;
		  SFString         m_family;
		  SFString         m_suffix;
		  SFString         m_nickname;
		  SFString         m_initials;
		  SFString         m_usertext_1;
		  SFString         m_usertext_2;
		  SFString         m_optionsList;
		  SFString         m_notifylist;

		  SFBool           m_deleted;
		  SFInt32          m_defaultPerm;
		  SFInt32          m_apparentPerm;
		  SFInt32          m_specialPerm;
		  SFBool           m_pendingPerm;
		  SFBool           m_trusted;

		  SFInt32          m_access;
		  SFString         m_passhint;

		  CPostalInfo      m_home;
		  CPostalInfo      m_office;
		  CCompanyInfo     m_company;
		  CPersonalInfo    m_personal;

		  SFString         m_creator;
		  SFString         m_lastEditor;
		  SFTime           m_createDate;
		  SFTime           m_lastEditDate;
		  SFTime           m_lastLoginDate;

		  SFString         m_rotated; // not stored away
		  SFBool           m_isInternal;

	const CPeopleDatabase *m_peopleDB;

   	      // stored in file as optionsList
	      SFAttribute      m_options[MAX_OPTIONS];
	      SFInt32          m_nOptions;

public:
						  CPerson              (const CPeopleDatabase *db=NULL);
					      CPerson              (const CPerson& data) { Copy(data); }
					      CPerson& operator=   (const CPerson& data) { Copy(data); return *this; }

		  SFString        getUserID            (SFBool parse=TRUE) const;
		  SFString        getPassword          (void) const { return m_password; }
		  SFString        getGiven             (void) const { return m_given;    }
		  SFString        getPrefix            (void) const { return m_prefix;   }
		  SFString        getFamily            (void) const { return m_family;   }
		  SFString        getSuffix            (void) const { return m_suffix;   }
		  SFString        getNickname          (void) const { return m_nickname; }
		  SFString        getInitials          (void) const { return m_initials; }
		  SFString        getUserText1         (void) const { return m_usertext_1;}
		  SFString        getUserText2         (void) const { return m_usertext_2;}
		  SFString        getOptionsList       (void) const;
		  SFString        getNotifyList        (void) const { return m_notifylist; }
		  SFInt32         getAccess            (void) const { return m_access;   }
		  SFString        getPassHint          (void) const { return m_passhint; }

		  SFString        getCreator           (void) const { return m_creator;      }
		  SFString        getLastEditor        (void) const { return m_lastEditor;   }
		  SFTime          getCreateDate        (void) const { return m_createDate;   }
		  SFTime          getLastEditDate      (void) const { return m_lastEditDate; }
	      SFTime          getLastLoginDate     (void) const { return m_lastLoginDate;}

		  SFInt32         getPermission        (SFBool type) const;

		  SFBool          isTrusted            (void) const;
		  SFBool          isDeleted            (void) const { return m_deleted; }
		  SFBool          shouldRemove         (void) const { return (m_deleted == TRASHCAN_REMOVE); }
		  SFBool          getPendingPerm       (void) const { return m_pendingPerm; }
		  SFBool          isSuper              (void) const;
		  SFBool          isAdmin              (void) const;
		  SFBool          isEditor             (void) const;
		  SFBool          isAdder              (void) const;
		  SFBool          isJustAdder          (void) const;
		  SFBool          isViewer             (void) const;
		  SFBool          isTrustedEdit        (void) const;
		  SFBool          isTrustedAdmin	   (void) const;
		  SFBool          hasSamePWasUID       (void) const;
		  SFBool          isInternal           (void) const { return m_isInternal; }
		  SFBool          isPending            (void) const { return (getPendingPerm() != PERMISSION_NOTSPEC); }
		  SFBool          isPendingInvisible   (void) const { return (getPendingPerm() != PERMISSION_NOTSPEC && getPermission(PRM_DEFAULT) == PERMISSION_NONE); }
		  SFBool          isSpecial            (SFBool testPerm);
		  SFBool          isRawSuper           (void) const;
		  SFBool          isRawAdmin           (void) const;
		  SFBool          isRawEdit            (void) const;

		  SFString        getFullName          (SFBool asSort=FALSE) const;
		  SFString        getSortName          (void) const;
		  SFString        getMainPhone         (void) const;
		  SFString        getMainCell          (void) const;
		  SFString        getMainFAX           (void) const;
		  SFString        getMainPager         (void) const;
		  SFString        getMainEmail         (void) const;
		  SFString        getMainEmailNoLink   (void) const;
		  SFString        getMainWebpage       (void) const;
		  SFString        getMainWebpageNoLink (void) const;
		  SFString        getMainAddress       (void) const;
		  SFString        getMainLocale        (void) const;
		  SFString        getMainContact       (void) const;
		  SFString        getCreatorString     (void) const;

	const CPostalInfo&    getHome              (void) const { return m_home; }
	const CPostalInfo&    getOffice            (void) const { return m_office; }
	const CCompanyInfo&   getCompany           (void) const { return m_company; }
	const CPersonalInfo&  getPersonal          (void) const { return m_personal; }
	      CPostalInfo&    getHome              (void)       { return m_home; }
	      CPostalInfo&    getOffice            (void)       { return m_office; }
	      CCompanyInfo&   getCompany           (void)       { return m_company; }
	      CPersonalInfo&  getPersonal          (void)       { return m_personal; }

		  SFString        getOption            (const SFString& name);
	      SFInt32         getOptionInt32       (const SFString& name) { return atoi((const char*)getOption(name)); }
          void            setOption            (const SFString& name, const SFString& value);
	      void            setOptionInt32       (const SFString& name, SFInt32 value) { setOption(name, asString(value)); }
	      SFBool          isOptionOn           (const SFString& name);

		  void            setUserID            (const SFString& userid)   { SAFE_COPY(m_userid, (const char *)userid, USERID_STRING_LEN); }
		  void            setPassword          (const SFString& password) { m_password = hard_decrypt(password, PW_KEY); }
		  void            setPrefix            (const SFString& pre)      { m_prefix = pre; }
		  void            setGiven             (const SFString& given)    { m_given = given; }
		  void            setFamily            (const SFString& family)   { m_family = family; }
		  void            setFullName          (const SFString& fn)       { m_family = fn; }
		  void            setSuffix            (const SFString& suffix)   { m_suffix = suffix; }
		  void            setNickname          (const SFString& nick)     { m_nickname = nick; }
		  void            setInitials          (const SFString& init)     { m_initials = init; }
		  void            setUserText1         (const SFString& ut1)      { m_usertext_1 = ut1; }
		  void            setUserText2         (const SFString& ut2)      { m_usertext_2 = ut2; }
		  void            setOptionsList       (const SFString& ol);
		  void            setNotifyList        (const SFString& notif)    { m_notifylist = notif; }
		  void            setTrusted           (SFBool trusted)           { m_trusted = trusted; }
		  void            setAccess            (SFInt32 access)           { m_access = access; }
		  void            setPassHint          (const SFString& hint)     { m_passhint = hard_decrypt(hint, PW_KEY); }
		  void            setDeleted           (SFBool del)               { m_deleted = del; }
		  void            setPending           (SFBool pend)              { m_pendingPerm = pend; };
		  void            setPendingPerm       (SFBool pend)              { m_pendingPerm = pend; };
		  void            setPermission        (SFBool def, SFInt32 per);
		  void            setInternal          (SFBool inter)             { m_isInternal = inter; }

		  void            setCreator            (const SFString& creator) { m_creator       = creator;   }
		  void            setLastEditor         (const SFString& le)      { m_lastEditor    = le;     }
		  void            setCreateDate         (const SFTime& date)      { m_createDate    = date;   }
		  void            setLastEditDate       (const SFTime& date)      { m_lastEditDate  = date; }
	      void            setLastLoginDate      (const SFTime& date)      { m_lastLoginDate = date; }

		  SFInt32         getFieldID           (const SFString& fieldName) const;
		  SFInt32         daysSinceLastPW      (void) const;
	      SFInt32         daysSinceLastLogin   (void) const;
		  SFBool          needsReset           (SFInt32 age, SFInt32 notSame, const SFString& testVal) const;
		  SFInt32         sortByFieldID        (const CPerson *p2, SFUint32 fieldID, SFBool reverse) const;

		  const CPeopleDatabase *getPeopleDB   (void) const                { return m_peopleDB; }
		  void                   setPeopleDB   (const CPeopleDatabase *db) { m_peopleDB = db;   }

		  SFString               displayString (const CServer *server, const SFString& fmt) const;
		  SFString               asDisplayed   (const CServer *server, const SFString& fmt) const
			{ return displayString(server, fmt); }

		  //---------------------------------------------------------------------------------
		  SFString        getFieldValueByID    (SFInt32 id, const SFString& def=nullString) const;
		  void            setFieldValueByID    (SFInt32 id, const SFString& value);

		  SFString        toAscii              (const SFString& format) const;
		  void            fromAscii            (const SFString& in, const CVersion& version);
		  void            fromAscii_4_0_1      (const SFString& in, const CVersion& version);

	static       SFInt32     nPersonFields    (void);
	static const CFieldData *firstPersonField (SFInt32& iterator, SFInt32 start=0);
	static const CFieldData *peekPersonField  (SFInt32& iterator, SFInt32 item=-1);
	static const CFieldData *nextPersonField  (SFInt32& iterator, SFInt32 stop=-1);

private:
	void Init(void)
		{
			m_userid[0] = '\0';

//			m_prefix      = "";
//			m_given       = "";
//			m_family      = "";
//			m_suffix      = "";
//			m_suffix      = "";
//			m_nickname    = "";
//			m_initials    = "";
//			m_usertext_1  = "";
//			m_usertext_2  = "";
//			m_optionsList = "";
//			m_notifylist  = "";
//			m_creator     = "";
//			m_lastEditor  = "";
//			m_home        = ;
//			m_office      = ;
//			m_company     = ;
//			m_personal    = ;

			m_deleted      = FALSE;
			m_defaultPerm  = PERMISSION_EDIT;
			m_apparentPerm = PERMISSION_EDIT;
			m_specialPerm  = PERMISSION_NOTSPEC;
			m_pendingPerm  = PERMISSION_NOTSPEC;
			m_trusted      = FALSE;
//			m_passhint = "";
			m_access       = 0;
//			m_rotated = "";
			m_isInternal   = FALSE;
			m_createDate   = earliestDate;
			m_lastEditDate = earliestDate;
			m_lastLoginDate= earliestDate;
			m_nOptions     = 0;
			m_peopleDB     = NULL;
		}

	void Copy(const CPerson& data)
		{
			SAFE_COPY(m_userid, data.m_userid, USERID_STRING_LEN);
			m_password        = data.m_password;
			
			m_prefix          = data.m_prefix;
			m_given           = data.m_given;
			m_family          = data.m_family;
			m_suffix          = data.m_suffix;
			m_nickname        = data.m_nickname;
			m_initials        = data.m_initials;
			m_usertext_1      = data.m_usertext_1;
			m_usertext_2      = data.m_usertext_2;
			m_optionsList     = data.m_optionsList;
			m_notifylist      = data.m_notifylist;
			m_creator         = data.m_creator;
			m_lastEditor      = data.m_lastEditor;
			m_home            = data.m_home;
			m_office          = data.m_office;
			m_company         = data.m_company;
			m_personal        = data.m_personal;
			
			m_deleted         = data.m_deleted;
			m_defaultPerm     = data.m_defaultPerm;
			m_apparentPerm    = data.m_apparentPerm;
			m_specialPerm     = data.m_specialPerm;
			m_pendingPerm     = data.m_pendingPerm;
			m_trusted         = data.m_trusted;
			m_passhint        = data.m_passhint;
			m_access          = data.m_access;
			m_rotated         = data.m_rotated;
			m_isInternal      = data.m_isInternal;
			m_lastEditDate    = data.m_lastEditDate;
			m_lastLoginDate   = data.m_lastLoginDate;
			m_createDate      = data.m_createDate;
			if (data.m_peopleDB)
			m_peopleDB        = data.m_peopleDB;

			m_nOptions        = data.m_nOptions;
			for (int i=0;i<MAX_OPTIONS;i++)
				m_options[i]  = data.m_options[i];
		}
public:
	friend void dumpPerson(CExportContext& ctx, const CPerson *person);
};

//--------------------------------------------------------------------------------------------------------------
#define NAMES             (1000 | TYPE_CONTACT)
#define U_PREFIX          (1005 | TYPE_CONTACT)
#define U_GIVENNAME       (1010 | TYPE_CONTACT)
#define U_FAMILYNAME      (1020 | TYPE_CONTACT)
#define U_SUFFIX          (1025 | TYPE_CONTACT)
#define U_INITIALS        (1035 | TYPE_CONTACT)
#define U_USERTEXT1       (1040 | TYPE_CONTACT)

#define BUSINESS_PHONES   (1100 | TYPE_CONTACT)
#define B_PHONE           (1105 | TYPE_CONTACT | TYPE_USER | TYPE_USER_453)
#define B_CELL            (1110 | TYPE_CONTACT | TYPE_USER | TYPE_USER_453)
#define B_FAX             (1115 | TYPE_CONTACT | TYPE_USER | TYPE_USER_453)
#define B_PAGER           (1120 | TYPE_CONTACT)

#define PERSONAL_PHONES   (1200 | TYPE_CONTACT | TYPE_USER)
#define P_PHONE           (1205 | TYPE_CONTACT | TYPE_USER)
#define P_CELL            (1210 | TYPE_CONTACT | TYPE_USER)
#define P_FAX             (1215 | TYPE_CONTACT | TYPE_USER)
#define P_PAGER           (1220 | TYPE_CONTACT | TYPE_USER)

#define BUSINESS_ADDRESS  (1300 | TYPE_CONTACT | TYPE_USER)
#define B_COMPANY         (1305 | TYPE_CONTACT | TYPE_USER)
#define B_ADDRESS_1       (1310 | TYPE_CONTACT | TYPE_USER)
#define B_ADDRESS_2       (1315 | TYPE_CONTACT | TYPE_USER)
#define B_CITY            (1320 | TYPE_CONTACT | TYPE_USER)
#define B_REGION          (1325 | TYPE_CONTACT | TYPE_USER)
#define B_POSTALCODE      (1330 | TYPE_CONTACT | TYPE_USER)
#define B_COUNTRY         (1335 | TYPE_CONTACT | TYPE_USER)

#define PERSONAL_ADDRESS  (1400 | TYPE_CONTACT | TYPE_USER)
#define P_ADDRESS_1       (1405 | TYPE_CONTACT | TYPE_USER)
#define P_ADDRESS_2       (1410 | TYPE_CONTACT | TYPE_USER)
#define P_CITY            (1415 | TYPE_CONTACT | TYPE_USER)
#define P_REGION          (1420 | TYPE_CONTACT | TYPE_USER)
#define P_POSTALCODE      (1425 | TYPE_CONTACT | TYPE_USER)
#define P_COUNTRY         (1430 | TYPE_CONTACT | TYPE_USER)

#define BUSINESS_INTERNET (1500 | TYPE_CONTACT | TYPE_USER)
#define B_EMAIL           (1505 | TYPE_CONTACT | TYPE_USER | TYPE_USER_453)
#define B_WEBPAGE         (1510 | TYPE_CONTACT | TYPE_USER)

#define PERSONAL_INTERNET (1600 | TYPE_CONTACT | TYPE_USER)
#define P_EMAIL           (1605 | TYPE_CONTACT | TYPE_USER)
#define P_WEBPAGE         (1610 | TYPE_CONTACT | TYPE_USER)

#define COMPANY_INFO      (1700 | TYPE_CONTACT | TYPE_USER)
#define C_POSITION        (1705 | TYPE_CONTACT | TYPE_USER)
#define C_DEPT            (1710 | TYPE_CONTACT | TYPE_USER)
#define C_OFFICE          (1715 | TYPE_CONTACT | TYPE_USER)
#define C_DOH             (1720 | TYPE_CONTACT | TYPE_USER)
#define C_USERTEXT2       (1725 | TYPE_CONTACT | TYPE_USER)

#define PERSONAL_INFO     (1800 | TYPE_CONTACT | TYPE_USER)
#define P_NICKNAME	      (1805 | TYPE_CONTACT | TYPE_USER)
#define P_SPOUSE          (1810 | TYPE_CONTACT | TYPE_USER)
#define P_CHILDREN        (1815 | TYPE_CONTACT | TYPE_USER)
#define P_GENDER          (1820 | TYPE_CONTACT | TYPE_USER)
#define P_BIRTHDATE       (1825 | TYPE_CONTACT | TYPE_USER)
#define P_ANNIVERSARY     (1830 | TYPE_CONTACT | TYPE_USER)
#define P_OPTIONSLIST     (1835 | TYPE_CONTACT | TYPE_USER)
#define P_NOTIFY          (1840 | TYPE_CONTACT | TYPE_USER)

// Display only stuff
#define D_SORTNAME        (1900)
#define D_MAINPHONE       (1905)
#define D_MAINCELL        (1910)
#define D_MAINFAX         (1915)
#define D_MAINPAGER       (1920)
#define D_MAINEMAIL       (1925)
#define D_MAINWEBPAGE     (1930)
#define D_MAINADDRESS     (1935)
#define D_MAINLOCALE      (1940)
#define B_LOCALE          (1945)
#define B_ADDRESS         (1950)
#define P_LOCALE          (1955)
#define P_ADDRESS         (1960)
#define D_DOWNLOADNAME    (1965)
#define D_DOWNLOADADDR    (1970)
#define D_ALLPHONES       (1973)
#define D_ALLBUSINESS     (1975)
#define D_ALLPERSONAL     (1980)
#define D_ALLOTHER        (1985)
#define D_ALLREVIEW       (1990)
#define D_CREATOR_STR     (1995)

#define USERDB_DATA       (2000 | TYPE_CONTACT)
#define U_NAME            (2005 | TYPE_USER | TYPE_USER_453)
#define U_USERID          (2010 | TYPE_USER | TYPE_USER_453 | TYPE_CONTACT | E_NO_EDIT)
#define U_PASSWORD        (2015 | TYPE_USER | TYPE_USER_453)
#define U_CONFIRM         (2020 | TYPE_USER | TYPE_USER_453)
#define U_PERMISSION      (2025 | TYPE_USER | TYPE_USER_453)
#define U_PERM_SPEC       (2030 | TYPE_USER | TYPE_USER_453)
#define U_PASSHINT        (2035 | TYPE_USER | TYPE_USER_453)
#define U_TRUSTED         (2040 | TYPE_USER | TYPE_USER_453)
#define U_ACCESS          (2045 | TYPE_USER | TYPE_USER_453 | TYPE_CONTACT | E_NO_EDIT)
#define U_DELETED         (2050 | TYPE_USER | TYPE_USER_453 | TYPE_CONTACT | E_NO_EDIT)
#define U_PENDING         (2055 | TYPE_USER | TYPE_USER_453 | TYPE_CONTACT | E_NO_EDIT)
#define U_CREATOR         (2060 | TYPE_USER | TYPE_USER_453 | TYPE_CONTACT)
#define U_CREATEDATE      (2065 | TYPE_USER | TYPE_USER_453 | TYPE_CONTACT | E_NO_EDIT)
#define U_LASTEDITOR      (2070 | TYPE_USER | TYPE_USER_453 | TYPE_CONTACT | E_NO_EDIT)
#define U_LASTCHANGE      (2075 | TYPE_USER | TYPE_USER_453 | TYPE_CONTACT | E_NO_EDIT)
#define U_LASTLOGIN       (2076 | TYPE_USER | E_NO_EDIT)

#define GROUP_DATA        (2100 | TYPE_GROUP)
#define GR_NAME           (2105 | TYPE_GROUP)
#define GR_DESCR          (2110 | TYPE_GROUP)
#define GR_MEMBERS        (2115 | TYPE_GROUP)
#define GR_PERMISSION     (2120 | TYPE_GROUP)
#define GR_SUBSCRIBE      (2125 | TYPE_GROUP)
#define GR_DELETED        (2130 | TYPE_GROUP)
#define GR_EXPANDED       (2135 | TYPE_GROUP)
#define GR_PERMISSION_STR (2140 | TYPE_GROUP)

//--------------------------------------------------------------------------------------------------------------
#define ACTIVE_ITEMS   (1<<1)
#define DELETED_ITEMS  (1<<2)
#define PENDING_ITEMS  (1<<3)
#define ALL_ITEMS      (ACTIVE_ITEMS | DELETED_ITEMS | PENDING_ITEMS)

// sugar
#define HIDDEN_ITEMS   PENDING_ITEMS

//--------------------------------------------------------------------------------------------------------------
inline SFBool shouldInclude(SFBool isDeleted, SFBool isPending, SFInt32 which)
{
	if (which == ALL_ITEMS)
		return TRUE;

	if (isDeleted) return (which & DELETED_ITEMS);
	if (isPending) return (which & PENDING_ITEMS);
	return (which & ACTIVE_ITEMS);
}

//--------------------------------------------------------------------------------------------------------------
inline SFBool isNoRow(SFInt32 fieldID)
{
	return (fieldID & E_NO_ROW);
}

//----------------------------------------------------------------------------------------------------
inline SFBool   CPerson::isSuper       (void) const { return getPermission(PRM_APPARENT) >= PERMISSION_SUPER; }
inline SFBool   CPerson::isAdmin       (void) const { return getPermission(PRM_APPARENT) >= PERMISSION_ADMIN; }
inline SFBool   CPerson::isEditor      (void) const { return getPermission(PRM_APPARENT) >= PERMISSION_EDIT;  }
inline SFBool   CPerson::isAdder       (void) const { return getPermission(PRM_APPARENT) >= PERMISSION_ADD;   }
inline SFBool   CPerson::isViewer      (void) const { return getPermission(PRM_APPARENT) >= PERMISSION_VIEW;  }
inline SFBool   CPerson::isJustAdder   (void) const { return isAdder() && !isEditor(); }

inline SFBool   CPerson::hasSamePWasUID(void) const { return (getUserID() % getPassword()); }

inline SFBool   CPerson::isRawSuper    (void) const { return (getPermission(PRM_DEFAULT) == PERMISSION_SUPER); }
inline SFBool   CPerson::isRawAdmin    (void) const { return (getPermission(PRM_DEFAULT) == PERMISSION_ADMIN); }
inline SFBool   CPerson::isRawEdit     (void) const { return (getPermission(PRM_DEFAULT) == PERMISSION_EDIT ); }

inline SFBool   CPerson::isTrustedAdmin(void) const { return (isSuper() || (isRawAdmin() && isTrusted())); }
inline SFBool   CPerson::isTrustedEdit (void) const { return (isAdmin() || (isRawEdit () && isTrusted())); }

inline SFBool   CPerson::isSpecial     (SFBool testPerm)    { return (getPermission(PRM_APPARENT) == testPerm && getPermission(PRM_APPARENT) > getPermission(PRM_DEFAULT)); }
inline SFString CPerson::getUserID     (SFBool parse) const { return SFString(m_userid); }

//----------------------------------------------------------------------------------------------------
inline int sortByUserID(const void *rr1, const void *rr2)
{
	SFString n1 = ((CPerson *)rr1)->getUserID(FALSE);
	SFString n2 = ((CPerson *)rr2)->getUserID(FALSE);
	return (int)n1.ICompare(n2); // unique so compare ignoring case
}

extern const char* IDS_WRD_REGISTRATION_USER;

#endif
