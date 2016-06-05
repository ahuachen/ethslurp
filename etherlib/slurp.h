#ifndef _SLURP_H_
#define _SLURP_H_
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
#include "transaction.h"

//--------------------------------------------------------------------------
class CSlurp;
typedef SFArrayBase<CSlurp>         CSlurpArray;
typedef SFList<CSlurp*>             CSlurpList;
typedef CNotifyClass<const CSlurp*> CSlurpNotify;
typedef SFUniqueList<CSlurp*>       CSlurpListU;

//---------------------------------------------------------------------------
extern int sortSlurp        (const SFString& f1, const SFString& f2, const void *rr1, const void *rr2);
extern int sortSlurpByName  (const void *rr1, const void *rr2);
extern int sortSlurpByID    (const void *rr1, const void *rr2);
extern int isDuplicateSlurp (const void *rr1, const void *rr2);

// EXISTING_CODE
// EXISTING_CODE

//--------------------------------------------------------------------------
class CSlurp : public CBaseNode
{
public:
	SFInt32 handle;
	SFString addr;
	SFString header;
	SFString displayString;
	SFBool pageSize;
	SFInt32 lastPage;
	SFInt32 lastBlock;
	SFInt32 nVisible;
	CTransactionArray transactions;

public:
					CSlurp  (void);
					CSlurp  (const CSlurp& sl);
				   ~CSlurp  (void);
	CSlurp&	operator= 		(const CSlurp& sl);

	DECLARE_NODE (CSlurp);

	// EXISTING_CODE
	SFInt32 writeToFile(CSharedResource& file) const;
	SFBool  readFromFile(CSharedResource& file);
	// EXISTING_CODE

private:
	void			Clear      		(void);
	void			Init      		(void);
	void			Copy      		(const CSlurp& sl);

	// EXISTING_CODE
	// EXISTING_CODE
};

//--------------------------------------------------------------------------
inline CSlurp::CSlurp(void)
{
	Init();
	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline CSlurp::CSlurp(const CSlurp& sl)
{
	// EXISTING_CODE
	// EXISTING_CODE
	Copy(sl);
}

// EXISTING_CODE
// EXISTING_CODE

//--------------------------------------------------------------------------
inline CSlurp::~CSlurp(void)
{
	Clear();
	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void CSlurp::Clear(void)
{
	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void CSlurp::Init(void)
{
	CBaseNode::Init();

	handle = 0;
	addr = EMPTY;
	header = EMPTY;
	displayString = EMPTY;
	pageSize = 0;
	lastPage = 0;
	lastBlock = 0;
	nVisible = 0;
//	transactions = ??; /* unknown type: CTransactionArray */

	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void CSlurp::Copy(const CSlurp& sl)
{
	Clear();

	CBaseNode::Copy(sl);
	handle = sl.handle;
	addr = sl.addr;
	header = sl.header;
	displayString = sl.displayString;
	pageSize = sl.pageSize;
	lastPage = sl.lastPage;
	lastBlock = sl.lastBlock;
	nVisible = sl.nVisible;
	transactions = sl.transactions;

	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline CSlurp& CSlurp::operator=(const CSlurp& sl)
{
	Copy(sl);
	// EXISTING_CODE
	// EXISTING_CODE
	return *this;
}

//---------------------------------------------------------------------------
inline SFString CSlurp::getValueByName(const SFString& fieldName) const
{
	// EXISTING_CODE
	// EXISTING_CODE
	return Format("[{"+toUpper(fieldName)+"}]");
}

//---------------------------------------------------------------------------
inline SFInt32 CSlurp::getHandle(void) const
{
	// EXISTING_CODE
	// EXISTING_CODE
	return handle;
}

//---------------------------------------------------------------------------
extern SFString nextSlurpChunk(const SFString& fieldIn, SFBool& force, const void *data);

//---------------------------------------------------------------------------
IMPLEMENT_ARCHIVE_ARRAY(CSlurpArray);
IMPLEMENT_ARCHIVE_LIST(CSlurpList);

//---------------------------------------------------------------------------
#include "slurp_custom.h"

// EXISTING_CODE
// EXISTING_CODE

#endif
