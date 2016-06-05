#ifndef __RUNTIME_H3D
#define __RUNTIME_H3D
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
class   CBaseNode;
typedef CBaseNode* (*PFNV)(void);

#include "fielddata.h"

//----------------------------------------------------------------------------
#define NO_SCHEMA 501

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
	SFString      listOfFields    (char sep='|') const;
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
	        DECLARE_NODE_BASE          (CLASS_NAME) \
	virtual SFInt32  getHandle         (void) const; \
	virtual SFString getValueByName    (const SFString& fieldName) const; \
	virtual SFBool   setValueByName    (const SFString& fieldName, const SFString& fieldValue); \
	virtual void     Serialize         (SFArchive& archive); \
    virtual SFBool   handleCustomFormat(CExportContext& ctx, const SFString& fmtIn, void *data=NULL) const; \
    virtual void     Format_base       (CExportContext& ctx, const SFString& fmtIn, void *data=NULL) const; \
    virtual SFString Format            (const SFString& fmtIn) const { CStringExportContext ctx;Format_base(ctx, fmtIn, NULL);return ctx.str;} \
	        SFString getClassName      (void) const; \
	static  void     registerClass     (void);

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
		SFBool show; archive >> show; setShowing(show); \
		SFString className; archive >> className; \
		if (className != getClassName()) \
			fprintf(stderr, "'%s' data type found. Expected '%s.'\n", (const char*)className, (const char*)getClassName()); \
	} else \
	{ \
		if (isDeleted() && !archive.writeDeleted()) \
			return; \
		archive << isDeleted(); \
		archive << getSchema(); \
		archive << isShowing(); \
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
	SFString        CLASS_NAME::getClassName   (void) const { return CLASS_NAME::class##CLASS_NAME.getClassNamePtr(); } \
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
