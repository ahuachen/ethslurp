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
	SFInt32 v1 = toLong(((SFAttribute*)rr1)->getValue());
	SFInt32 v2 = toLong(((SFAttribute*)rr2)->getValue());
	if (v2 == v1)
		return sortByAttributeName(rr1, rr2);
	return (int)(v2 - v1);
}
