#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_

//---------------------------------------------------------------------------
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
// EXISTING_CODE

//--------------------------------------------------------------------------
class CTransaction : public CBaseNode
{
public:
	SFInt32 handle;
	SFString blockHash;
	SFString blockNumber;
	SFString confirmations;
	SFString contractAddress;
	SFString cumulativeGasUsed;
	SFString from;
	SFString gas;
	SFString gasPrice;
	SFString gasUsed;
	SFString hash;
	SFString input;
	SFString nonce;
	SFString timeStamp;
	SFString to;
	SFString transactionIndex;
	SFString value;

public:
					CTransaction  (void);
					CTransaction  (const CTransaction& tr);
				   ~CTransaction  (void);
	CTransaction&	operator= 		(const CTransaction& tr);

	DECLARE_NODE (CTransaction);

	// EXISTING_CODE
	SFInt32 parseJson(SFString& strIn);
	SFInt32 writeToFile(CSharedResource& file) const;
	SFBool  readFromFile(CSharedResource& file);
	// EXISTING_CODE

private:
	void			Clear      		(void);
	void			Init      		(void);
	void			Copy      		(const CTransaction& tr);

	// EXISTING_CODE
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
	blockNumber = EMPTY;
	confirmations = EMPTY;
	contractAddress = EMPTY;
	cumulativeGasUsed = EMPTY;
	from = EMPTY;
	gas = EMPTY;
	gasPrice = EMPTY;
	gasUsed = EMPTY;
	hash = EMPTY;
	input = EMPTY;
	nonce = EMPTY;
	timeStamp = EMPTY;
	to = EMPTY;
	transactionIndex = EMPTY;
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
inline SFArchive& operator<<(SFArchive& archive, CTransactionArray& array)
{
	SFInt32 count = array.getCount();
	if (!archive.writeDeleted())
	{
		for (int i=0;i<array.getCount();i++)
		{
			if (array[i].isDeleted())
				count--;
		}
	}
	archive << count;
	for (int i=0;i<array.getCount();i++)
		array[i].Serialize(archive);
	return archive;
}
inline SFArchive& operator>>(SFArchive& archive, CTransactionArray& array)
{
	SFInt32 count;
	archive >> count;
	for (int i=0;i<count;i++)
		array[i].Serialize(archive);
	return archive;
}

IMPLEMENT_ARCHIVE_LIST(CTransactionList);

//---------------------------------------------------------------------------
#include "transaction_custom.h"

// EXISTING_CODE
// EXISTING_CODE

#endif
