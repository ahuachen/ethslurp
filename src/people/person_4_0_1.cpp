/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "peopledatabase.h"
#include "person.h"

// Format of ascii in older versions
static const SFInt32 ids_401[] =
{
	U_USERID,    U_PASSWORD,    U_CONFIRM, U_GIVENNAME, BAD_NUMBER,  U_FAMILYNAME, U_SUFFIX,       U_NAME,       U_CREATOR,   P_NICKNAME,
	U_DELETED,   U_PERMISSION,  U_PENDING, P_ADDRESS_1, P_ADDRESS_2, P_CITY,       P_REGION,       P_POSTALCODE, P_COUNTRY,   P_PHONE,
	P_FAX,       P_CELL,        P_PAGER,   P_WEBPAGE,   P_EMAIL,     B_ADDRESS_1,  B_ADDRESS_2,    B_CITY,       B_REGION,    B_POSTALCODE,
	B_COUNTRY,   B_PHONE,       B_FAX,     B_CELL,      B_PAGER,     B_WEBPAGE,    B_EMAIL,        B_COMPANY,    C_POSITION,  C_DEPT,
	C_OFFICE,    C_DOH,         P_SPOUSE,  P_CHILDREN,  P_GENDER,    P_BIRTHDATE,  P_ANNIVERSARY,  U_PERM_SPEC,  U_TRUSTED,   U_PASSHINT,    U_LASTCHANGE,
};
static const SFInt32 nIds_401 = sizeof(ids_401) / sizeof(SFInt32);

//---------------------------------------------------------------------------------------
static int searchByAttributeName(const void *rr1, const void *rr2)
{
	SFAttribute *a1 = (SFAttribute*)rr1;
	SFAttribute *a2 = (SFAttribute*)rr2;
	return (int)a1->getName().ICompare(a2->getName());
}

//---------------------------------------------------------------------------------------
static SFString convertRegion(const SFString& in, SFInt32 nVals, SFAttribute *attrs)
{
	SFAttribute key(in, in);
	SFAttribute *result = (SFAttribute*)bsearch(&in, attrs, nVals, sizeof(SFAttribute), searchByAttributeName);
	if (result)
		return result->getValue();
	return in;
}

#define MAX_REGIONS 500
SFAttribute regionsUp[MAX_REGIONS];
SFInt32     nUps = 0;
//---------------------------------------------------------------------------------------
void CPerson::fromAscii_4_0_1(const SFString& lineIn, const CVersion& version)
{
	ASSERT(version.earlier(4,0,2));

	ASSERT(getPeopleDB());
	if (version.eqEarlier(1,0,2))
	{
		SFString line         = Strip(lineIn, ' ');
		setUserID             (nextToken(line, ';'));
		setPassword           (nextToken(line, ';'));
		setFullName           (nextToken(line, ';'));
		getOffice().setEmail  (nextToken(line, ';'));
		getOffice().setPhone  (nextToken(line, ';'));
		getOffice().setFax    (nextToken(line, ';'));
		setDeleted            (atoi((const char *)nextToken(line, ';')));
		                       atoi((const char *)nextToken(line, ';')); //skip
		setPendingPerm        (atoi((const char *)nextToken(line, ';')));

	} else
	{
		SFString line = lineIn;
		ASSERT(getPeopleDB());
		for (int i=0;i<nIds_401;i++)
		{
			SFString value = Strip(nextTokenClear(line, CH_SEMI), ' ');
			if (ids_401[i] != BAD_NUMBER && !value.IsEmpty())
			{
				const CFieldData *field = getPeopleDB()->getFieldList()->getFieldByID(ids_401[i]); ASSERT(field);
				setFieldValueByID(field->getFieldID(), value);
			}
		}

		if (version.earlier(4,0,1))
		{
			// used to store operator/resource info here - OK to white it out
			// since passHint was introduced in later versions
			setPassHint(EMPTY);

			// Access Level didn't exist
			setAccess(0);

			// Prior to this version all ADD users behaved 
			// exactly like EDIT users so we bump them up 
			// here - and free up the ADD permission for future use.
			if (getPermission(PRM_DEFAULT) == PERMISSION_ADD)
				setPermission(PRM_DEFAULT, PERMISSION_EDIT);

			// Prior to this version pending users only stored
			// TRUE/FALSE and store the pending permission in the
			// actual permission making these users un-usable while in
			// a pending state.  After this version we simply store pending permission
			// and allow the user to continue to operate with their old
			// permission.  Pending users prior to this version were not useable
			// anyway so we convert them to new style penders but give them
			// a working permission of PERMISSION_NONE.  Old isPending calls
			// which would have made this user appear to be invisible will
			// now only make them invisible if they have pending perm and
			// their existing perms are NONE.
			if (getPendingPerm() == 0) // used to be TRUE/FALSE
			{
				setPendingPerm(PERMISSION_NOTSPEC);
			} else
			{
				setPendingPerm(getPermission(PRM_DEFAULT));
				setPermission(PRM_DEFAULT, PERMISSION_NONE);
			}

			// Stored in the wrong place before
			SFString suffix = getSuffix();
			if (suffix.ContainsI("Mr")   || suffix.ContainsI("Ms") || // Mr includes Mrs
				suffix.ContainsI("Miss") || suffix.ContainsI("Dr"))
			{
				setPrefix(suffix);
				setSuffix(EMPTY);
			}

			// These two fields were mistakenly switched in all versions prior to this version
			SFString realOwner = getNickname();
			setNickname(getCreator());
			setCreator (realOwner);
		}
	}

	// changed to using something more easy to understand
	if (getUserID().Contains("cw_user"))
	{
		SFString userid = getUserID();
		userid.Replace("cw_user", UPRE);
		setUserID(userid);
	}

	if (m_office.getCountry() % "usa") m_office.setCountry("United States");
	if (m_home.getCountry()   % "usa")   m_home.setCountry("United States");

	if (!nUps)
	{
		regionChoose(nUps, NULL, NULL);
		if (nUps)
		{
			regionChoose(nUps, regionsUp, NULL);
			qsort(regionsUp, nUps, sizeof(SFAttribute), sortByAttributeName);
		}
	}

	if (!m_office.getRegion().IsEmpty())
		m_office.setRegion(convertRegion(m_office.getRegion(), nUps, regionsUp));
	if (!  m_home.getRegion().IsEmpty())
		m_home.setRegion  (convertRegion(  m_home.getRegion(), nUps, regionsUp));

	// We never stored this so update it to the date of this
	// data upgrade and assume that the creator was the last
	// editor which is fine since only supers users used to be able to edit.
	setCreateDate  (Now());
	setLastEditor  (getCreator());
	setLastEditDate(Now());
}
