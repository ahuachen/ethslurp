#ifndef _PARAMETER_H_
#define _PARAMETER_H_
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

//--------------------------------------------------------------------------
class CParameter;
typedef SFArrayBase<CParameter>         CParameterArray;
typedef SFList<CParameter*>             CParameterList;
typedef CNotifyClass<const CParameter*> CParameterNotify;
typedef SFUniqueList<CParameter*>       CParameterListU;

//---------------------------------------------------------------------------
extern int sortParameter        (const SFString& f1, const SFString& f2, const void *rr1, const void *rr2);
extern int sortParameterByName  (const void *rr1, const void *rr2);
extern int sortParameterByID    (const void *rr1, const void *rr2);
extern int isDuplicateParameter (const void *rr1, const void *rr2);

// EXISTING_CODE
class CFunction;
// EXISTING_CODE

//--------------------------------------------------------------------------
class CParameter : public CBaseNode
{
public:
	SFInt32 handle;
	SFString name;
	SFString type;
	CFunction *func;

public:
					CParameter  (void);
					CParameter  (const CParameter& pa);
				   ~CParameter  (void);
	CParameter&	operator= 		(const CParameter& pa);

	DECLARE_NODE (CParameter);

	// EXISTING_CODE
	// EXISTING_CODE

private:
	void			Clear      		(void);
	void			Init      		(void);
	void			Copy      		(const CParameter& pa);

	// EXISTING_CODE
	// EXISTING_CODE
};

//--------------------------------------------------------------------------
inline CParameter::CParameter(void)
{
	Init();
	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline CParameter::CParameter(const CParameter& pa)
{
	// EXISTING_CODE
	// EXISTING_CODE
	Copy(pa);
}

// EXISTING_CODE
// EXISTING_CODE

//--------------------------------------------------------------------------
inline CParameter::~CParameter(void)
{
	Clear();
	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void CParameter::Clear(void)
{
	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void CParameter::Init(void)
{
	CBaseNode::Init();

	handle = 0;
	name = EMPTY;
	type = EMPTY;
	func = NULL;

	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void CParameter::Copy(const CParameter& pa)
{
	Clear();

	CBaseNode::Copy(pa);
	handle = pa.handle;
	name = pa.name;
	type = pa.type;
//	if (func)
//		*func = *pa.func;

	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline CParameter& CParameter::operator=(const CParameter& pa)
{
	Copy(pa);
	// EXISTING_CODE
	// EXISTING_CODE
	return *this;
}

//---------------------------------------------------------------------------
inline SFString CParameter::getValueByName(const SFString& fieldName) const
{
	// EXISTING_CODE
	// EXISTING_CODE
	return Format("[{"+toUpper(fieldName)+"}]");
}

//---------------------------------------------------------------------------
inline SFInt32 CParameter::getHandle(void) const
{
	// EXISTING_CODE
	// EXISTING_CODE
	return handle;
}

//---------------------------------------------------------------------------
extern SFString nextParameterChunk(const SFString& fieldIn, SFBool& force, const void *data);

//---------------------------------------------------------------------------
IMPLEMENT_ARCHIVE_ARRAY(CParameterArray);
IMPLEMENT_ARCHIVE_LIST(CParameterList);

//---------------------------------------------------------------------------
#include "parameter_custom.h"

// EXISTING_CODE
// EXISTING_CODE

#endif
