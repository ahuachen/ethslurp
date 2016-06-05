#ifndef _EDITABLE_OBJECT_H
#define _EDITABLE_OBJECT_H
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
