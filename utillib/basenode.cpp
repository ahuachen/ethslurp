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
#include "basenode.h"

//--------------------------------------------------------------------------------
ghRuntimeClass CBaseNode::classCBaseNode;
static CBuiltIn _biBaseNode(&CBaseNode::classCBaseNode, "CBaseNode", sizeof(CBaseNode), NULL, NULL, NO_SCHEMA);

//--------------------------------------------------------------------------------
CBaseNode::CBaseNode(void)
{
	Init();
}

//--------------------------------------------------------------------------------
CBaseNode::~CBaseNode(void)
{
	ASSERT(m_refCount==0);
}

//--------------------------------------------------------------------------------
void CBaseNode::Init(void)
{
	m_refCount = 0;
	m_deleted  = FALSE;
	m_schema = NO_SCHEMA;
	m_showing = TRUE;
}

//--------------------------------------------------------------------------------
void CBaseNode::Copy(const CBaseNode& bn)
{
	m_refCount = bn.m_refCount;
	m_deleted  = bn.m_deleted;
	m_schema = bn.m_schema;
	m_showing = bn.m_showing;
}

//--------------------------------------------------------------------------------
void CBaseNode::Reference(void)
{
	m_refCount++;
}

//--------------------------------------------------------------------------------
SFInt32 CBaseNode::Dereference(void)
{
	ASSERT(m_RefCount>0);
	m_refCount--;
	return (m_refCount==0);
}

//--------------------------------------------------------------------------------
SFBool CBaseNode::isReferenced(void) const
{
	return (m_refCount);
}

//--------------------------------------------------------------------------------
SFBool CBaseNode::isDeleted(void) const
{
	return (m_deleted);
}

//--------------------------------------------------------------------------------
void CBaseNode::setDeleted(SFBool del)
{
	m_deleted = del;
}

//--------------------------------------------------------------------------------
SFInt32 CBaseNode::getSchema(void) const
{
	return (m_schema);
}

//--------------------------------------------------------------------------------
SFBool CBaseNode::isShowing(void) const
{
	return (m_showing);
}

//--------------------------------------------------------------------------------
void CBaseNode::setSchema(SFBool val)
{
	m_schema = val;
}

//--------------------------------------------------------------------------------
void CBaseNode::setShowing(SFBool val)
{
	m_showing = val;
}

//--------------------------------------------------------------------------------
SFBool CBaseNode::isKindOf(const ghRuntimeClass* pClass) const
{
	ghRuntimeClass* pClassThis = getRuntimeClass();
	return pClassThis->IsDerivedFrom(pClass);
}

//--------------------------------------------------------------------------------
ghRuntimeClass* CBaseNode::getRuntimeClass() const
{
	return &CBaseNode::classCBaseNode;
}

//--------------------------------------------------------------------------------
SFString addIcon(ghRuntimeClass *pClass)
{
	CBaseNode *node = pClass->CreateObject();
	SFString fmt = SFString("[<table><td><td>Add ") + toLower(SFString(pClass->getClassNamePtr()).Mid(1,100)) + ": {ADD}</td></tr></table>]";
	SFString ret = node->Format(fmt);
	delete node;
	return ret;
}
