/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "peopledatabase.h"
#include "person.h"
#include "usergroup.h"

static const SFInt32 ids[] =
{ GR_NAME, GR_DESCR, BAD_NUMBER, GR_MEMBERS, GR_DELETED, GR_EXPANDED, GR_PERMISSION, GR_SUBSCRIBE, GROUP_DATA };
static const SFInt32 nIds  = sizeof(ids) / sizeof(SFInt32);

//---------------------------------------------------------------------------------------
void CUserGroup::fromAscii_4_0_1(const SFString& in, const CVersion& version)
{
	ASSERT(version.eqEarlier(4,0,2))
	SFString email, line = in;
	for (int i=0;i<nIds;i++)
	{
		SFString value = nextTokenClear(line, CH_SEMI);
		if (ids[i] == BAD_NUMBER)
			email = value;
		else
			setFieldValueByID(ids[i], value);
	}

	// Eliminated email only groups in later versions - just copy the value into members list
	if (!email.IsEmpty())
		setMembers( getMembers() + ", " + email );

	// Prior to this version user groups did not have permissions so set every one to view permission
	if (version.earlier(4,0,1))
		setPermission(PERMISSION_VIEW);
}
