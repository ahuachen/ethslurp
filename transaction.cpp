/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2015 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "manage.h"
#include "transaction.h"

//---------------------------------------------------------------------------
IMPLEMENT_NODE(CTransaction, CBaseNode, NO_SCHEMA);

//---------------------------------------------------------------------------
void CTransaction::Format_base(CExportContext& ctx, const SFString& fmtIn, void *data) const
{
	if (!isShowing())
		return;

	if (handleCustomFormat(ctx, fmtIn, data))
		return;
	
	SFString fmt = fmtIn;

	CTransactionNotify dn(this);
	while (!fmt.IsEmpty())
		ctx << getNextChunk(fmt, nextTransactionChunk, &dn);
}

//---------------------------------------------------------------------------
SFString nextTransactionChunk(const SFString& fieldIn, SFBool& force, const void *data)
{
	CTransactionNotify *tr = (CTransactionNotify*)data;
	const CTransaction *tra = tr->getDataPtr();

	// Give common (edit, delete, etc.) code a chance to override
	SFString ret = nextChunk_common(fieldIn, getString("cmd"), tra);
	if (!ret.IsEmpty())
		return ret;
	
	// Now give customized code a chance to override
	ret = nextTransactionChunk_custom(fieldIn, force, data);
	if (!ret.IsEmpty())
		return ret;
	
	switch (tolower(fieldIn[0]))
	{
		case 'b':
			if ( fieldIn % "blockHash" ) return tra->blockHash;
			if ( fieldIn % "blockNumber" ) return asString(tra->blockNumber);
			break;
		case 'c':
			if ( fieldIn % "confirmations" ) return asString(tra->confirmations);
			if ( fieldIn % "contractAddress" ) return tra->contractAddress;
			if ( fieldIn % "cumulativeGasUsed" ) return tra->cumulativeGasUsed;
			break;
		case 'f':
			if ( fieldIn % "from" ) return tra->from;
			break;
		case 'g':
			if ( fieldIn % "gas" ) return tra->gas;
			if ( fieldIn % "gasPrice" ) return tra->gasPrice;
			if ( fieldIn % "gasUsed" ) return tra->gasUsed;
			break;
		case 'h':
			if ( fieldIn % "handle" ) return asString(tra->handle);
			if ( fieldIn % "hash" ) return tra->hash;
			break;
		case 'i':
			if ( fieldIn % "input" ) return tra->input;
			break;
		case 'n':
			if ( fieldIn % "nonce" ) return tra->nonce;
			break;
		case 't':
			if ( fieldIn % "timeStamp" ) return asString(tra->timeStamp);
			if ( fieldIn % "to" ) return tra->to;
			if ( fieldIn % "transactionIndex" ) return asString(tra->transactionIndex);
			break;
		case 'v':
			if ( fieldIn % "value" ) return tra->value;
			break;
	}
	
	return "<span class=warning>Field not found: [{" + fieldIn + "}]</span>\n";
}

//---------------------------------------------------------------------------------------------------
SFBool CTransaction::setValueByName(const SFString& fieldName, const SFString& fieldValue)
{
	switch (tolower(fieldName[0]))
	{
		case 'b':
			if ( fieldName % "blockHash" ) blockHash = fieldValue;
			if ( fieldName % "blockNumber" ) blockNumber = toLong(fieldValue);
			break;
		case 'c':
			if ( fieldName % "confirmations" ) confirmations = toLong(fieldValue);
			if ( fieldName % "contractAddress" ) contractAddress = fieldValue;
			if ( fieldName % "cumulativeGasUsed" ) cumulativeGasUsed = fieldValue;
			break;
		case 'f':
			if ( fieldName % "from" ) from = toLower(fieldValue);
			break;
		case 'g':
			if ( fieldName % "gas" ) gas  = fieldValue;
			if ( fieldName % "gasPrice" ) gasPrice = fieldValue;
			if ( fieldName % "gasUsed" ) gasUsed = fieldValue;
			break;
		case 'h':
			if ( fieldName % "handle" ) return handle = toLong(fieldValue);
			if ( fieldName % "hash" ) hash = fieldValue;
			break;
		case 'i':
			if ( fieldName % "input" ) input = fieldValue;
			break;
		case 'n':
			if ( fieldName % "nonce" ) nonce = fieldValue;
			break;
		case 't':
			if ( fieldName % "timeStamp" ) timeStamp = toLong(fieldValue);
			if ( fieldName % "to" ) to = toLower(fieldValue);
			if ( fieldName % "transactionIndex" ) transactionIndex = toLong(fieldValue);
			break;
		case 'v':
			if ( fieldName % "value" ) value = fieldValue;
			break;
	}
	return TRUE;
}

//---------------------------------------------------------------------------------------------------
void CTransaction::Serialize(SFArchive& archive)
{
	SERIALIZE_START();
	if (archive.isReading())
	{
		archive >> handle;
		archive >> blockHash;
		archive >> blockNumber;
		archive >> confirmations;
		archive >> contractAddress;
		archive >> cumulativeGasUsed;
		archive >> from;
		archive >> gas;
		archive >> gasPrice;
		archive >> gasUsed;
		archive >> hash;
		archive >> input;
		archive >> nonce;
		archive >> timeStamp;
		archive >> to;
		archive >> transactionIndex;
		archive >> value;

	} else
	{
		archive << handle;
		archive << blockHash;
		archive << blockNumber;
		archive << confirmations;
		archive << contractAddress;
		archive << cumulativeGasUsed;
		archive << from;
		archive << gas;
		archive << gasPrice;
		archive << gasUsed;
		archive << hash;
		archive << input;
		archive << nonce;
		archive << timeStamp;
		archive << to;
		archive << transactionIndex;
		archive << value;

	}
	SERIALIZE_END();
}

//---------------------------------------------------------------------------
void CTransaction::registerClass(void)
{
	SFInt32 fieldNum=1000;
	ADD_FIELD(CTransaction, "schema",  T_NUMBER|TS_LABEL, ++fieldNum);
	ADD_FIELD(CTransaction, "deleted", T_RADIO|TS_LABEL,  ++fieldNum);
	ADD_FIELD(CTransaction, "handle", T_NUMBER|TS_LABEL,  ++fieldNum);
	ADD_FIELD(CTransaction, "blockHash", T_TEXT, ++fieldNum);
	ADD_FIELD(CTransaction, "blockNumber", T_NUMBER, ++fieldNum);
	ADD_FIELD(CTransaction, "confirmations", T_NUMBER, ++fieldNum);
	ADD_FIELD(CTransaction, "contractAddress", T_TEXT, ++fieldNum);
	ADD_FIELD(CTransaction, "cumulativeGasUsed", T_TEXT, ++fieldNum);
	ADD_FIELD(CTransaction, "from", T_TEXT, ++fieldNum);
	ADD_FIELD(CTransaction, "gas", T_TEXT, ++fieldNum);
	ADD_FIELD(CTransaction, "gasPrice", T_TEXT, ++fieldNum);
	ADD_FIELD(CTransaction, "gasUsed", T_TEXT, ++fieldNum);
	ADD_FIELD(CTransaction, "hash", T_TEXT, ++fieldNum);
	ADD_FIELD(CTransaction, "input", T_TEXT, ++fieldNum);
	ADD_FIELD(CTransaction, "nonce", T_TEXT, ++fieldNum);
	ADD_FIELD(CTransaction, "timeStamp", T_NUMBER, ++fieldNum);
	ADD_FIELD(CTransaction, "to", T_TEXT, ++fieldNum);
	ADD_FIELD(CTransaction, "transactionIndex", T_NUMBER, ++fieldNum);
	ADD_FIELD(CTransaction, "value", T_TEXT, ++fieldNum);
}

//---------------------------------------------------------------------------
int sortTransaction(const SFString& f1, const SFString& f2, const void *rr1, const void *rr2)
{
	CTransaction *g1 = (CTransaction*)rr1;
	CTransaction *g2 = (CTransaction*)rr2;

	SFString v1 = g1->getValueByName(f1);
	SFString v2 = g2->getValueByName(f1);
	SFInt32 s = v1.Compare(v2);
	if (s || f2.IsEmpty())
		return (int)s;

	v1 = g1->getValueByName(f2);
	v2 = g2->getValueByName(f2);
	return (int)v1.Compare(v2);
}
int sortTransactionByName(const void *rr1, const void *rr2) { return sortTransaction("tr_Name", "", rr1, rr2); }
int sortTransactionByID  (const void *rr1, const void *rr2) { return sortTransaction("transactionID", "", rr1, rr2); }
