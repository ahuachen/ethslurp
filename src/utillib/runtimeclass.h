#ifndef __RUNTIME_H3D
#define __RUNTIME_H3D
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

class   CBaseNode;
typedef CBaseNode* (*PFNV)(void);

#include "fielddata.h"

//----------------------------------------------------------------------------
#define NO_SCHEMA -1

//----------------------------------------------------------------------------
class ghRuntimeClass;
class CBuiltIn
{
public:
	const ghRuntimeClass *m_pClass;
	CBuiltIn(ghRuntimeClass *pClass, const SFString& className, SFInt32 size, PFNV createFunc, ghRuntimeClass *pBase, SFInt32 schema);
};

//----------------------------------------------------------------------------
class ghRuntimeClass
{
public:
	char             *m_ClassName;
	SFInt32           m_ObjectSize;
	SFInt32           m_Schema;
	PFNV              m_CreateFunc;
	ghRuntimeClass   *m_BaseClass;
	CFieldList       *m_FieldList;
	SFInt32           m_RefCount;

public:
	virtual      ~ghRuntimeClass  (void);

	char         *getClassNamePtr       (void) const;
	SFBool        IsDerivedFrom   (const ghRuntimeClass* pBaseClass) const;
	void          AddField        (const SFString& fieldName, SFInt32 dataType, SFInt32 fieldID);
	void          ClearFieldList  (void);
	CFieldList   *GetFieldList    (void) const { return m_FieldList; }
	SFInt32       Reference       (void);
	SFInt32       Dereference     (void);
	void          Initialize    (const SFString& protoName);

	SFString getHTML(const SFString& subCmd, SFBool isLoggedIn) const;
	CBaseNode *CreateObject(void)
		{
			if (m_CreateFunc)
				return (*m_CreateFunc)();
			return NULL;
		}
	
//	virtual SFBool SetFieldValue(const SFString& fieldName, void *val);
	virtual CFieldData *FindField(const SFString& fieldName)
		{
			if (m_FieldList)
			{
				LISTPOS pos = m_FieldList->GetFirstItem();
				while (pos)
				{
					CFieldData *field = m_FieldList->GetNextItem(pos);
					if (field->getFieldName() == fieldName)
						return field;
				}
			}
			return NULL;
		}
};

#define asHTML() getRuntimeClass()->getHTML(getString("subCmd"), FALSE); //getSimpleServer()->isLoggedIn())

//------------------------------------------------------------
#define GETRUNTIME_CLASS(CLASS_NAME) \
	(&CLASS_NAME::class##CLASS_NAME)

//------------------------------------------------------------
#define DECLARE_NODE_BASE(CLASS_NAME) \
public: \
	static  ghRuntimeClass  class##CLASS_NAME; \
	static  CBaseNode      *CreateObject   (void); \
	virtual ghRuntimeClass *getRuntimeClass(void) const;


//------------------------------------------------------------
#define DECLARE_NODE(CLASS_NAME) \
	        DECLARE_NODE_BASE      (CLASS_NAME) \
	virtual SFInt32  getHandle     (void) const; \
	virtual SFString getValueByName(const SFString& fieldName) const; \
	virtual SFBool   setValueByName(const SFString& fieldName, const SFString& fieldValue); \
	virtual void     Serialize     (SFArchive& archive); \
	virtual SFString Format        (const SFString& fmtIn) const; \
	        SFString getClassName  (void); \
	static  void     registerClass (void);

//---------------------------------------------------------------------------
#define SERIALIZE_START() \
	SFInt32 fileVersion = -1; \
	if (archive.isReading()) \
	{ \
		archive.m_readFile->refreshBuffer(); \
		SFBool del; archive >> del; setDeleted(del); \
		archive >> fileVersion; \
		if (fileVersion != archive.getSchema()) \
			fprintf(stderr, "'%ld' data version found. Expected '%ld.'\n", fileVersion, archive.getSchema()); \
		setSchema(fileVersion); \
		SFString className; archive >> className; \
		if (className != getClassName()) \
			fprintf(stderr, "'%s' data type found. Expected '%s.'\n", (const char*)className, (const char*)getClassName()); \
	} else \
	{ \
		if (isDeleted() && !archive.writeDeleted()) \
			return; \
		archive << isDeleted(); \
		archive << getSchema(); \
		archive << getClassName(); \
	}

//---------------------------------------------------------------------------
#define SERIALIZE_END() \
	if (archive.isReading()) { \
		char wasteRecordEnd; \
		archive >> wasteRecordEnd; \
		finishParse(this); \
	} else { \
		archive << archive.m_recordEnd; \
	}

//------------------------------------------------------------
#define IMPLEMENT_NODE(CLASS_NAME, BASECLASS_NAME, SCHEMA) \
	ghRuntimeClass  CLASS_NAME::class##CLASS_NAME; \
	ghRuntimeClass *CLASS_NAME::getRuntimeClass(void) const { return &CLASS_NAME::class##CLASS_NAME; } \
	SFString        CLASS_NAME::getClassName   (void)       { return CLASS_NAME::class##CLASS_NAME.getClassNamePtr(); } \
	CBaseNode*      CLASS_NAME::CreateObject   (void)       { return new CLASS_NAME; } \
	static CBuiltIn _bi##CLASS_NAME(&CLASS_NAME::class##CLASS_NAME, #CLASS_NAME, sizeof(CLASS_NAME), CLASS_NAME::CreateObject, GETRUNTIME_CLASS(BASECLASS_NAME), SCHEMA);

//------------------------------------------------------------
#define ADD_FIELD(CLASS_NAME, FIELD_NAME, FIELD_TYPE, FIELD_ID) \
	GETRUNTIME_CLASS(CLASS_NAME)->AddField(FIELD_NAME, FIELD_TYPE, FIELD_ID);

//------------------------------------------------------------
// Archive array containers
#define IMPLEMENT_ARCHIVE_ARRAY(ARRAY_CLASS) \
inline SFArchive& operator<<(SFArchive& archive, ARRAY_CLASS& array) \
{ \
	SFInt32 count = array.getCount(); \
	if (!archive.writeDeleted()) \
	{ \
		for (int i=0;i<array.getCount();i++) \
		{ \
			if (array[i].isDeleted()) \
				count--; \
		} \
	} \
	archive << count; \
	for (int i=0;i<array.getCount();i++) \
		array[i].Serialize(archive); \
	return archive; \
} \
inline SFArchive& operator>>(SFArchive& archive, ARRAY_CLASS& array) \
{ \
	SFInt32 count; \
	archive >> count; \
	for (int i=0;i<count;i++) \
		array[i].Serialize(archive); \
	return archive; \
}

//------------------------------------------------------------
// Archive list containers
#define IMPLEMENT_ARCHIVE_LIST(LIST_CLASS) \
inline SFArchive& operator<<(SFArchive& archive, LIST_CLASS& array) \
{ \
	return archive; \
} \
inline SFArchive& operator>>(SFArchive& archive, LIST_CLASS& array) \
{ \
	return archive; \
}

//---------------------------------------------------------------------------
extern SFString nextChunk_common(const SFString& fieldIn, const SFString& cmd, const CBaseNode *node);
extern SFString addIcon(ghRuntimeClass *pClass);

#endif
