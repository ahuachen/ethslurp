#ifndef _ACCOUNT_H_
#define _ACCOUNT_H_
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
#include "vote.h"
#include "proposal.h"
#include "transaction.h"

//--------------------------------------------------------------------------
class CAccount;
typedef SFArrayBase<CAccount>         CAccountArray;
typedef SFList<CAccount*>             CAccountList;
typedef CNotifyClass<const CAccount*> CAccountNotify;
typedef SFUniqueList<CAccount*>       CAccountListU;

//---------------------------------------------------------------------------
extern int sortAccount        (const SFString& f1, const SFString& f2, const void *rr1, const void *rr2);
extern int sortAccountByName  (const void *rr1, const void *rr2);
extern int sortAccountByID    (const void *rr1, const void *rr2);
extern int isDuplicateAccount (const void *rr1, const void *rr2);

// EXISTING_CODE
// EXISTING_CODE

//--------------------------------------------------------------------------
class CAccount : public CBaseNode
{
public:
	SFInt32 handle;
	SFString addr;
	SFInt32 nVotes;
	CVote *votes;
	SFInt32 nProposals;
	CProposal *proposals;
	SFInt32 nTransactions;
	CTransaction *transactions;

public:
					CAccount  (void);
					CAccount  (const CAccount& ac);
				   ~CAccount  (void);
	CAccount&	operator= 		(const CAccount& ac);

	DECLARE_NODE (CAccount);

	// EXISTING_CODE
	// EXISTING_CODE

private:
	void			Clear      		(void);
	void			Init      		(void);
	void			Copy      		(const CAccount& ac);

	// EXISTING_CODE
	// EXISTING_CODE
};

//--------------------------------------------------------------------------
inline CAccount::CAccount(void)
{
	Init();
	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline CAccount::CAccount(const CAccount& ac)
{
	// EXISTING_CODE
	// EXISTING_CODE
	Copy(ac);
}

// EXISTING_CODE
// EXISTING_CODE

//--------------------------------------------------------------------------
inline CAccount::~CAccount(void)
{
	Clear();
	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void CAccount::Clear(void)
{
	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void CAccount::Init(void)
{
	CBaseNode::Init();

	handle = 0;
	addr = EMPTY;
	nVotes = 0;
	votes = NULL;
	nProposals = 0;
	proposals = NULL;
	nTransactions = 0;
	transactions = NULL;

	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void CAccount::Copy(const CAccount& ac)
{
	Clear();

	CBaseNode::Copy(ac);
	handle = ac.handle;
	addr = ac.addr;
	nVotes = ac.nVotes;
	if (votes)
		*votes = *ac.votes;
	nProposals = ac.nProposals;
	if (proposals)
		*proposals = *ac.proposals;
	nTransactions = ac.nTransactions;
	if (transactions)
		*transactions = *ac.transactions;

	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline CAccount& CAccount::operator=(const CAccount& ac)
{
	Copy(ac);
	// EXISTING_CODE
	// EXISTING_CODE
	return *this;
}

//---------------------------------------------------------------------------
inline SFString CAccount::getValueByName(const SFString& fieldName) const
{
	// EXISTING_CODE
	// EXISTING_CODE
	return Format("[{"+toUpper(fieldName)+"}]");
}

//---------------------------------------------------------------------------
inline SFInt32 CAccount::getHandle(void) const
{
	// EXISTING_CODE
	// EXISTING_CODE
	return handle;
}

//---------------------------------------------------------------------------
extern SFString nextAccountChunk(const SFString& fieldIn, SFBool& force, const void *data);

//---------------------------------------------------------------------------
IMPLEMENT_ARCHIVE_ARRAY(CAccountArray);
IMPLEMENT_ARCHIVE_LIST(CAccountList);

//---------------------------------------------------------------------------
#include "account_custom.h"

// EXISTING_CODE
// EXISTING_CODE

#endif
