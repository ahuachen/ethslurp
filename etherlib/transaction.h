#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_
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
class CTransaction;
typedef SFArrayBase<CTransaction>         CTransactionArray;
typedef SFList<CTransaction*>             CTransactionList;
typedef CNotifyClass<const CTransaction*> CTransactionNotify;
typedef SFUniqueList<CTransaction*>       CTransactionListU;

//---------------------------------------------------------------------------
extern int sortTransaction        (const SFString& f1, const SFString& f2, const void *rr1, const void *rr2);
extern int sortTransactionByName  (const void *rr1, const void *rr2);
extern int sortTransactionByID    (const void *rr1, const void *rr2);
extern int isDuplicateTransaction (const void *rr1, const void *rr2);

// EXISTING_CODE
extern int sortTransactionsForWrite(const void *rr1, const void *rr2);
class CSlurp;
// EXISTING_CODE

//--------------------------------------------------------------------------
class CTransaction : public CBaseNode
{
public:
	SFInt32 handle;
	SFString blockHash;
	SFInt32 blockNumber;
	SFInt32 confirmations;
	SFString contractAddress;
	SFString cumulativeGasUsed;
	SFString from;
	SFString gas;
	SFString gasPrice;
	SFString gasUsed;
	SFString hash;
	SFString input;
	SFBool isError;
	SFBool isInternalTx;
	SFString nonce;
	SFInt32 timeStamp;
	SFString to;
	SFInt32 transactionIndex;
	SFString value;

public:
					CTransaction  (void);
					CTransaction  (const CTransaction& tr);
				   ~CTransaction  (void);
	CTransaction&	operator= 		(const CTransaction& tr);

	DECLARE_NODE (CTransaction);

	// EXISTING_CODE
	SFTime  getDate(void) const;
	SFString inputToFunction(void) const;
	CSlurp *slurp;
	SFBool isFunction(const SFString& func) const;
	// EXISTING_CODE

private:
	void			Clear      		(void);
	void			Init      		(void);
	void			Copy      		(const CTransaction& tr);

	// EXISTING_CODE
	friend int sortTransactionsForWrite(const void *rr1, const void *rr2);
	// EXISTING_CODE
};

//--------------------------------------------------------------------------
inline CTransaction::CTransaction(void)
{
	Init();
	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline CTransaction::CTransaction(const CTransaction& tr)
{
	// EXISTING_CODE
	// EXISTING_CODE
	Copy(tr);
}

// EXISTING_CODE
// EXISTING_CODE

//--------------------------------------------------------------------------
inline CTransaction::~CTransaction(void)
{
	Clear();
	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void CTransaction::Clear(void)
{
	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void CTransaction::Init(void)
{
	CBaseNode::Init();

	handle = 0;
	blockHash = EMPTY;
	blockNumber = 0;
	confirmations = 0;
	contractAddress = EMPTY;
	cumulativeGasUsed = EMPTY;
	from = EMPTY;
	gas = EMPTY;
	gasPrice = EMPTY;
	gasUsed = EMPTY;
	hash = EMPTY;
	input = EMPTY;
	isError = 0;
	isInternalTx = 0;
	nonce = EMPTY;
	timeStamp = 0;
	to = EMPTY;
	transactionIndex = 0;
	value = EMPTY;

	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void CTransaction::Copy(const CTransaction& tr)
{
	Clear();

	CBaseNode::Copy(tr);
	handle = tr.handle;
	blockHash = tr.blockHash;
	blockNumber = tr.blockNumber;
	confirmations = tr.confirmations;
	contractAddress = tr.contractAddress;
	cumulativeGasUsed = tr.cumulativeGasUsed;
	from = tr.from;
	gas = tr.gas;
	gasPrice = tr.gasPrice;
	gasUsed = tr.gasUsed;
	hash = tr.hash;
	input = tr.input;
	isError = tr.isError;
	isInternalTx = tr.isInternalTx;
	nonce = tr.nonce;
	timeStamp = tr.timeStamp;
	to = tr.to;
	transactionIndex = tr.transactionIndex;
	value = tr.value;

	// EXISTING_CODE
	// EXISTING_CODE
}

//--------------------------------------------------------------------------
inline CTransaction& CTransaction::operator=(const CTransaction& tr)
{
	Copy(tr);
	// EXISTING_CODE
	// EXISTING_CODE
	return *this;
}

//---------------------------------------------------------------------------
inline SFString CTransaction::getValueByName(const SFString& fieldName) const
{
	// EXISTING_CODE
	// EXISTING_CODE
	return Format("[{"+toUpper(fieldName)+"}]");
}

//---------------------------------------------------------------------------
inline SFInt32 CTransaction::getHandle(void) const
{
	// EXISTING_CODE
	// EXISTING_CODE
	return handle;
}

//---------------------------------------------------------------------------
extern SFString nextTransactionChunk(const SFString& fieldIn, SFBool& force, const void *data);

//---------------------------------------------------------------------------
IMPLEMENT_ARCHIVE_ARRAY(CTransactionArray);
IMPLEMENT_ARCHIVE_LIST(CTransactionList);

//---------------------------------------------------------------------------
#include "transaction_custom.h"

// EXISTING_CODE
// EXISTING_CODE

#endif
