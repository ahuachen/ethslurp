/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "permission.h"

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFString CPermission::getPermissionStr(void) const
{
	return CPermission::getPermissionStrShort(m_permission);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFString CPermission::getPermissionStrShort(SFBool perm)
{
	if (perm == PERMISSION_NOTSPEC)
		return "Not Specified";
	ASSERT(perm >= PERMISSION_NONE || perm <= PERMISSION_SUPER)
	return perms[perm];
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFString CPermission::getPermissionStrLong(SFBool perm)
{
	if (perm == PERMISSION_NOTSPEC)
		return "Not Specified";
	ASSERT(perm >= PERMISSION_NONE || perm <= PERMISSION_SUPER)
	return permsPrompts[perm];
}

