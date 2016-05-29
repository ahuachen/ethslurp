#ifndef _EDITABLE_OBJECT_H
#define _EDITABLE_OBJECT_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

class CFieldList;
class CServerBase;

// See utillib/editableobjectbase.h file for the class definition.  This is done
// because we moved this file into the utillib tree at a late date.

//---------------------------------------------------------------------------------------------
class CNotifyUnionBase
{
public:
	CExportContext *ctx;
	
		                      CNotifyUnionBase (void) { ctx = NULL; }
	virtual                  ~CNotifyUnionBase (void) { }

	virtual       SFBool      hasDisplayItem   (void) const                                  { return TRUE; }
	virtual const CFieldData *getFieldData     (const SFString&   fieldName)                 { return NULL; }
	virtual       SFBool      shouldShow       (const CFieldData *fieldData)                 { return TRUE; }
	virtual       void        postProcess      (const SFString& fieldName, SFString& prompt) { }
};

//-----------------------------------------------------------------------------------------
template<class TYPE>
class CNotifyClass : public CNotifyUnionBase
{
private:
	TYPE m_record;

public:
	         CNotifyClass(const TYPE rec) { m_record = rec; }
	virtual ~CNotifyClass(void)  { }

	TYPE     getDataPtr  (void) const { return m_record; }
};

#endif
