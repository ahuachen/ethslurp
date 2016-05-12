/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "peopledatabase.h"
#include "protectedobject.h"
#include "person.h"

int sortByObjectName(const void *rr1, const void *rr2)
{
	CProtectedObject *e1 = (CProtectedObject*)rr1;
	CProtectedObject *e2 = (CProtectedObject*)rr2;

	int ret;
	ret = strcasecmp((const char*)(e1->getNameWithGrp()), (const char*)(e2->getNameWithGrp()));
	if (ret)
		return ret;

	ret = strcasecmp((const char*)e1->getName(),     (const char*)e2->getName());
	if (ret)
		return ret;

	ret = strcasecmp((const char*)e1->getObjectID(), (const char*)e2->getObjectID());
	return ret;
}

SFString CProtectedObject::getNameWithGrp(void) const
{
	if (!getGroup().IsEmpty())
		return getGroup() + " - " + getName();

	if (!getName().IsEmpty())
		return getName();

	return getObjectID();
}
