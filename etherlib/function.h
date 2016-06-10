#ifndef _FUNCTION_H_
#define _FUNCTION_H_
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
#include "utillib.h"
#include "parameter.h"

//--------------------------------------------------------------------------
class CFunction;
typedef SFArrayBase<CFunction>         CFunctionArray;
typedef SFList<CFunction*>             CFunctionList;
typedef CNotifyClass<const CFunction*> CFunctionNotify;
typedef SFUniqueList<CFunction*>       CFunctionListU;

//---------------------------------------------------------------------------
extern int sortFunction        (const SFString& f1, const SFString& f2, const void *rr1, const void *rr2);
extern int sortFunctionByName  (const void *rr1, const void *rr2);
extern int sortFunctionByID    (const void *rr1, const void *rr2);
extern int isDuplicateFunction (const void *rr1, const void *rr2);

// EXISTING_CODE
// EXISTING_CODE

//--------------------------------------------------------------------------
class CFunction : public CBaseNode
{
public:
	SFInt32 handle;
	SFString name;
	SFString type;
	SFBool indexed;
	SFBool anonymous;
	SFBool constant;
	SFString encoding;
	CParameterArray inputs;
	CParameterArray outputs;

public:
					CFunction  (void);
					CFunction  (const CFunction& fu);
				   ~CFunction  (void);
	CFunction&	operator= 		(const CFunction& fu);

	DECLARE_NODE (CFunction);

	// EXISTING_CODE
	void parseParams(SFBool input, const SFString& value);
	// EXISTING_CODE

private:
	void			Clear      		(void);
	void			Init      		(void);
	void			Copy      		(const CFunction& fu);

	// EXISTING_CODE
	// EXISTING_CODE
};

//--------------------------------------------------------------------------
inline CFunction::CFunction(void)
{
	Init();
	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline CFunction::CFunction(const CFunction& fu)
{
	// EXISTING_CODE
	// EXISTING_CODE
	Copy(fu);
}

// EXISTING_CODE
// EXISTING_CODE

//--------------------------------------------------------------------------
inline CFunction::~CFunction(void)
{
	Clear();
	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void CFunction::Clear(void)
{
	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void CFunction::Init(void)
{
	CBaseNode::Init();

	handle = 0;
	name = EMPTY;
	type = EMPTY;
	indexed = 0;
	anonymous = 0;
	constant = 0;
	encoding = EMPTY;
//	inputs = ??; /* unknown type: CParameterArray */
//	outputs = ??; /* unknown type: CParameterArray */

	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void CFunction::Copy(const CFunction& fu)
{
	Clear();

	CBaseNode::Copy(fu);
	handle = fu.handle;
	name = fu.name;
	type = fu.type;
	indexed = fu.indexed;
	anonymous = fu.anonymous;
	constant = fu.constant;
	encoding = fu.encoding;
	inputs = fu.inputs;
	outputs = fu.outputs;

	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline CFunction& CFunction::operator=(const CFunction& fu)
{
	Copy(fu);
	// EXISTING_CODE
	// EXISTING_CODE
	return *this;
}

//---------------------------------------------------------------------------
inline SFString CFunction::getValueByName(const SFString& fieldName) const
{
	// EXISTING_CODE
	// EXISTING_CODE
	return Format("[{"+toUpper(fieldName)+"}]");
}

//---------------------------------------------------------------------------
inline SFInt32 CFunction::getHandle(void) const
{
	// EXISTING_CODE
	// EXISTING_CODE
	return handle;
}

//---------------------------------------------------------------------------
extern SFString nextFunctionChunk(const SFString& fieldIn, SFBool& force, const void *data);

//---------------------------------------------------------------------------
IMPLEMENT_ARCHIVE_ARRAY(CFunctionArray);
IMPLEMENT_ARCHIVE_LIST(CFunctionList);

//---------------------------------------------------------------------------
#include "function_custom.h"

// EXISTING_CODE
// EXISTING_CODE

#endif
