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

//--------------------------------------------------------------------------------
SFString CBaseNode::defaultFormat(void) const
{
	SFString ret;
	CFieldList *fieldList = getRuntimeClass()->GetFieldList();
	if (fieldList)
	{
		LISTPOS lPos = fieldList->SFList<CFieldData *>::GetHeadPosition();
		while (lPos)
		{
			CFieldData *field = fieldList->GetNextItem(lPos);
			if (!field->isHidden())
				ret += "["+toProper(field->getFieldName())+": {"+toUpper(field->getFieldName())+"}\n]";
		}
	} else
	{
		fprintf(stderr,"No fieldList. Did you register the class? Quitting...\n");
		exit(0);
	}
	return ret;
}

//--------------------------------------------------------------------------------
char *CBaseNode::parseCSV(char *s, SFInt32& nFields, const SFString *fields)
{
	nFields = 0;

	typedef enum { OUTSIDE=0, INSIDE } parseState;
	parseState state = OUTSIDE;
	
	char *fieldVal=NULL;
	while (*s)
	{
		switch (state)
		{
			case OUTSIDE:
				if (*s=='\"')
				{
					state = INSIDE;
					fieldVal=s+1;
					
				} else if (*s=='\n')
				{
					return (s+1);
				}
				s++;
				break;
				
			case INSIDE:
				if (*s == '\"')
				{
					*s = '\0';
					if (!this->setValueByName(fields[nFields++], fieldVal))
					{
					//	fprintf(stderr,"Bad field name %s. Quitting...", (const char*)fields[nFields-1]);
					//	return NULL;
					}
					fieldVal = NULL;
					state = OUTSIDE;
					
				}
				s++;
				break;
		}
	}
	return NULL;
}

//--------------------------------------------------------------------------------
char *CBaseNode::parseJson(char *s, SFInt32& nFields)
{
	typedef enum { OUTSIDE=0, IN_NAME, IN_VALUE } parseState;
	parseState state = OUTSIDE;

	char *fieldName=NULL;
	char *fieldVal=NULL;
	while (*s)
	{
		switch (state)
		{
		case OUTSIDE:
			if (*s=='{')
				state = IN_NAME;
			s++;
			break;
		
		case IN_NAME:
			if (!fieldName)
			{
				fieldName = s;

			} else if (*s == ':')
			{
				state = IN_VALUE;
				*s = '\0';
				//			printf("fn: %-10.10s fv: %-40.40s ---> %-60.60s\n" , fieldName, fieldVal, (s+1));
			}
			s++;
			break;

		case IN_VALUE:
			fieldVal = s;
			if (*s=='[') // array skip to end of array
			{
				fieldVal++;
				while (*s && *s!=']')
					s++;
			} else
			{
				while (*s && *s!=',' && *s!='}')
					s++;
			}
			*s = '\0';
//			printf("fn: %-10.10s fv: %-40.40s ---> %-60.60s\n" , fieldName, fieldVal, (s+1));
			nFields += this->setValueByName(fieldName, fieldVal);
			fieldName = NULL;
			fieldVal = NULL;
			state = IN_NAME;
			s++;
			if (*s && *s==',')
				s++;
			if (*s && (*s=='{'||*s==']'))
			{
				finishParse();
				return s;
			}
			break;
		}
	}
	finishParse();
	return NULL;
}

//--------------------------------------------------------------------------------
char *cleanUpJson(char *s)
{
	if (!s)
		return s;

	char *l = s, *start = s;
	while (*s)
	{
		if (!isWhiteSpace(*s) && *s != '\"') // zap all the white space and quotes
		{
			*l = *s;
			l++;
		}
		s++;
	}
	*l = '\0';
	return start;
}

#include "sfarchive.h"
//---------------------------------------------------------------------------
SFBool CBaseNode::SerializeHeader(SFArchive& archive)
{
	if (archive.m_isReading)
	{
		SFString str;
		archive >> m_deleted;
		archive >> m_schema;
		archive >> m_showing;
		archive >> str; ASSERT(str == getRuntimeClass()->getClassNamePtr());
	} else
	{
		if (m_deleted && !archive.writeDeleted())
			return FALSE;
		archive << m_deleted;
		archive << m_schema;
		archive << m_showing;
		archive << getRuntimeClass()->getClassNamePtr();
	}
	return TRUE;
}

