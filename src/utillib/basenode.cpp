/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
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
}

//--------------------------------------------------------------------------------
void CBaseNode::Copy(const CBaseNode& bn)
{
	m_refCount = bn.m_refCount;
	m_deleted  = bn.m_deleted;
	m_schema = bn.m_schema;
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
void CBaseNode::setSchema(SFBool val)
{
	m_schema = val;
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
