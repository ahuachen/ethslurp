#ifndef _PROPOSAL_H_
#define _PROPOSAL_H_
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
class CProposal;
typedef SFArrayBase<CProposal>         CProposalArray;
typedef SFList<CProposal*>             CProposalList;
typedef CNotifyClass<const CProposal*> CProposalNotify;
typedef SFUniqueList<CProposal*>       CProposalListU;

//---------------------------------------------------------------------------
extern int sortProposal        (const SFString& f1, const SFString& f2, const void *rr1, const void *rr2);
extern int sortProposalByName  (const void *rr1, const void *rr2);
extern int sortProposalByID    (const void *rr1, const void *rr2);
extern int isDuplicateProposal (const void *rr1, const void *rr2);

// EXISTING_CODE
// EXISTING_CODE

//--------------------------------------------------------------------------
class CProposal : public CBaseNode
{
public:
	SFInt32 handle;
	SFInt32 proposalID;
	SFTime closingDate;
	SFInt32 nYes;
	SFInt32 nNo;

public:
					CProposal  (void);
					CProposal  (const CProposal& pr);
				   ~CProposal  (void);
	CProposal&	operator= 		(const CProposal& pr);

	DECLARE_NODE (CProposal);

	// EXISTING_CODE
	// EXISTING_CODE

private:
	void			Clear      		(void);
	void			Init      		(void);
	void			Copy      		(const CProposal& pr);

	// EXISTING_CODE
	// EXISTING_CODE
};

//--------------------------------------------------------------------------
inline CProposal::CProposal(void)
{
	Init();
	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline CProposal::CProposal(const CProposal& pr)
{
	// EXISTING_CODE
	// EXISTING_CODE
	Copy(pr);
}

// EXISTING_CODE
// EXISTING_CODE

//--------------------------------------------------------------------------
inline CProposal::~CProposal(void)
{
	Clear();
	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void CProposal::Clear(void)
{
	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void CProposal::Init(void)
{
	CBaseNode::Init();

	handle = 0;
	proposalID = 0;
	closingDate = earliestDate;
	nYes = 0;
	nNo = 0;

	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void CProposal::Copy(const CProposal& pr)
{
	Clear();

	CBaseNode::Copy(pr);
	handle = pr.handle;
	proposalID = pr.proposalID;
	closingDate = pr.closingDate;
	nYes = pr.nYes;
	nNo = pr.nNo;

	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline CProposal& CProposal::operator=(const CProposal& pr)
{
	Copy(pr);
	// EXISTING_CODE
	// EXISTING_CODE
	return *this;
}

//---------------------------------------------------------------------------
inline SFString CProposal::getValueByName(const SFString& fieldName) const
{
	// EXISTING_CODE
	// EXISTING_CODE
	return Format("[{"+toUpper(fieldName)+"}]");
}

//---------------------------------------------------------------------------
inline SFInt32 CProposal::getHandle(void) const
{
	// EXISTING_CODE
	// EXISTING_CODE
	return handle;
}

//---------------------------------------------------------------------------
extern SFString nextProposalChunk(const SFString& fieldIn, SFBool& force, const void *data);

//---------------------------------------------------------------------------
IMPLEMENT_ARCHIVE_ARRAY(CProposalArray);
IMPLEMENT_ARCHIVE_LIST(CProposalList);

//---------------------------------------------------------------------------
#include "proposal_custom.h"

// EXISTING_CODE
// EXISTING_CODE

#endif
