/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "attribute.h"

SFAttribute::SFAttribute(void)
{
	//m_Name;
	//m_Value;
}

SFAttribute::SFAttribute(const SFAttribute& attr)
{
	m_Name  = attr.m_Name;
	m_Value = attr.m_Value;
}

SFAttribute::SFAttribute(const SFString& n, const SFString& v)
{
	m_Name  = n;
	m_Value = v;
}

SFAttribute& SFAttribute::operator=(const SFAttribute& attr)
{
	m_Name  = attr.m_Name;
	m_Value = attr.m_Value;
	return *this;
}

static int sortAttr(const SFString& v1, const SFString& v2, const SFString& av1, const SFString& av2)
{
	int result = (int)v1.ICompare(v2);
	if (result != 0)
		return result;
	return (int)av1.ICompare(av2);
}

int sortByAttributeName(const void *rr1, const void *rr2)
{
	SFAttribute *a1 = (SFAttribute*)rr1;
	SFAttribute *a2 = (SFAttribute*)rr2;
	return sortAttr(a1->getName(), a2->getName(), a1->getValue(), a2->getValue());
}

int sortByAttributeValue(const void *rr1, const void *rr2)
{
	SFAttribute *a1 = (SFAttribute*)rr1;
	SFAttribute *a2 = (SFAttribute*)rr2;
	return sortAttr(a1->getValue(), a2->getValue(), a1->getName(), a2->getName());
}

int sortByAttributeNameReverse(const void *rr1, const void *rr2)
{
	SFAttribute *a1 = (SFAttribute*)rr1;
	SFAttribute *a2 = (SFAttribute*)rr2;
	return sortAttr(a2->getName(), a1->getName(), a2->getValue(), a1->getValue());
}

int sortByAttributeValueReverse(const void *rr1, const void *rr2)
{
	SFAttribute *a1 = (SFAttribute*)rr1;
	SFAttribute *a2 = (SFAttribute*)rr2;
	return sortAttr(a2->getValue(), a1->getValue(), a2->getName(), a1->getName());
}

int sortByAttributeIntReverse(const void *rr1, const void *rr2)
{
	SFInt32 v1 = atoi((const char *)((SFAttribute*)rr1)->getValue());
	SFInt32 v2 = atoi((const char *)((SFAttribute*)rr2)->getValue());
	if (v2 == v1)
		return sortByAttributeName(rr1, rr2);
	return (int)(v2 - v1);
}

