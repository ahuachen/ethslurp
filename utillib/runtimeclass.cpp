/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "runtimeclass.h"

//-------------------------------------------------------------------------
ghRuntimeClass::~ghRuntimeClass(void)
{
	ClearFieldList();
	ASSERT(!m_FieldList);

	if (m_Schema==0xFFFA)
	{
		// This will only happen when the PROTO is read from
		// a file which is not yet supported.
		if (m_ClassName)
			free(m_ClassName);
	}
}

//-------------------------------------------------------------------------
char *ghRuntimeClass::getClassNamePtr(void) const
{
	return m_ClassName;
}

//-------------------------------------------------------------------------
SFBool ghRuntimeClass::IsDerivedFrom(const ghRuntimeClass* pBaseClass) const
{
	const ghRuntimeClass* pClassThis = this;
	while (pClassThis != NULL)
	{
		if (pClassThis == pBaseClass)
			return TRUE;
		pClassThis = pClassThis->m_BaseClass;
	}
	return FALSE;
}

//-------------------------------------------------------------------------
void ghRuntimeClass::Initialize(const SFString& protoName)
{
	SFString copy = protoName;
	if (!copy.IsEmpty())
		m_ClassName = strdup(copy.GetBuffer());

	m_ObjectSize    = 0;
	// Signifies that we were created with Initialize so we can cleanup (i.e. the class is not builtin)
	m_Schema        = 0xFFFA;
	m_BaseClass     = NULL;
	m_FieldList     = NULL;
	m_RefCount      = 0;
	m_CreateFunc    = NULL;
}

//-------------------------------------------------------------------------
void ghRuntimeClass::ClearFieldList(void)
{
	if (m_FieldList)
	{
		LISTPOS p = m_FieldList->GetFirstItem();
		while (p)
		{
			CFieldData *field = m_FieldList->GetNextItem(p);
			delete field;
		}
		m_FieldList->RemoveAll();
		delete m_FieldList;
		m_FieldList = NULL;
	}
}

//-------------------------------------------------------------------------
void ghRuntimeClass::AddField(const SFString& fieldName, SFInt32 dataType, SFInt32 fieldID)
{
	if (!m_FieldList)
		m_FieldList = new CFieldList;
	ASSERT(m_FieldList);

	m_FieldList->AddTail(new CFieldData("", fieldName, "", "", dataType, FALSE, fieldID));
}

//-------------------------------------------------------------------------
SFInt32 ghRuntimeClass::Reference(void)
{
	++m_RefCount;
	return m_RefCount;
}

//-------------------------------------------------------------------------
SFInt32 ghRuntimeClass::Dereference(void)
{
	m_RefCount--; 
	ASSERT(m_RefCount>=0); 

	if (!m_RefCount || (m_CreateFunc && m_RefCount==1))
	{
		// Clear the field list for proper PROTO's when last
		// referenced or BuiltIns when only the builtin is still referenced.
		ClearFieldList();
	}

	return (m_RefCount==0);
}

//-------------------------------------------------------------------------
CBuiltIn::CBuiltIn(ghRuntimeClass *pClass, const SFString& className, SFInt32 size, PFNV createFunc, ghRuntimeClass *pBase, SFInt32 schema)
{
	m_pClass = pClass;
	SFString copy = className;
	if (!copy.IsEmpty())
		pClass->m_ClassName = strdup(copy.GetBuffer());
	
	pClass->m_ObjectSize    = size;
	// Signifies that we were created with Initialize so we can cleanup (i.e. the class is not builtin)
	pClass->m_Schema        = schema;
	pClass->m_BaseClass     = pBase;
	pClass->m_FieldList     = NULL;
	pClass->m_RefCount      = 0;
	pClass->m_CreateFunc    = createFunc;
}

//-----------------------------------------------------------------------------------------
SFString ghRuntimeClass::getHTML(const SFString& subCmd, SFBool isLoggedIn) const
{
	CStringExportContext ctx;
	{CTable table(ctx, "width=100% cellspacing=5");
		{CRow row(ctx, " bgcolor=tan");
			{CColumn col(ctx, "colspan=2");
				ctx << getClassNamePtr();
			}
		}
		LISTPOS lPos = GetFieldList()->GetFirstItem();
		while (lPos)
		{
			CFieldData *field = GetFieldList()->GetNextItem(lPos);
			SFString fn = toUpper(field->getFieldName());
			ctx << "[";
			{CRow row(ctx, "");
				{CColumn col(ctx);
					ctx << "{p:" << fn << "}:";
				}
				ctx << "][";
				{CColumn col(ctx, "align=right valign=top");
					if (field->getFieldType() & TS_ARRAY)
					{
						if (!(subCmd%"add" || subCmd%"edit"))
							ctx << "{" << fn << "}";
						else
							ctx << "{NULL}Not editable";
						
					} else if (field->getFieldType() & TS_LABEL)
					{
						ctx << "{" << fn << "}";
						
					} else
					{
						ctx << "{";
						if (subCmd%"add" || subCmd%"edit")
							ctx << "f:";
						ctx << fn << "}";
					}
				}
			}
			ctx << "]";
		}
	}

	if (isLoggedIn)
		ctx.str.Replace("{HANDLE}", "{HANDLE}][ {EDIT}][ {DELETE}");

	return ctx.str;
}