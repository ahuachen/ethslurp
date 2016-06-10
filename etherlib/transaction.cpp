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
#include "transaction.h"

//---------------------------------------------------------------------------
IMPLEMENT_NODE(CTransaction, CBaseNode, NO_SCHEMA);

//---------------------------------------------------------------------------
void CTransaction::Format(CExportContext& ctx, const SFString& fmtIn, void *data) const
{
	if (!isShowing())
		return;

	SFString fmt = (fmtIn.IsEmpty() ? defaultFormat() : fmtIn);;
	if (handleCustomFormat(ctx, fmt, data))
		return;

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
	if (!ret.IsEmpty()||fieldIn=="function")
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
			if ( fieldIn % "isInternalTx" ) return asString(tra->isInternalTx);
			if ( fieldIn % "isError" ) return asString(tra->isError);
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
			if ( fieldName % "blockHash" ) { blockHash = fieldValue; return TRUE; }
			if ( fieldName % "blockNumber" ) { blockNumber = toLong(fieldValue); return TRUE; }
			break;
		case 'c':
			if ( fieldName % "confirmations" ) { confirmations = toLong(fieldValue); return TRUE; }
			if ( fieldName % "contractAddress" ) { contractAddress = fieldValue; return TRUE; }
			if ( fieldName % "cumulativeGasUsed" ) { cumulativeGasUsed = fieldValue; return TRUE; }
			break;
		case 'f':
			if ( fieldName % "from" ) { from = toLower(fieldValue);return TRUE; }
			break;
		case 'g':
			if ( fieldName % "gas" ) { gas = fieldValue; return TRUE; }
			if ( fieldName % "gasPrice" ) { gasPrice = fieldValue; return TRUE; }
			if ( fieldName % "gasUsed" ) { gasUsed = fieldValue; return TRUE; }
			break;
		case 'h':
			if ( fieldName % "handle" ) { handle = toLong(fieldValue); return TRUE; }
			if ( fieldName % "hash" ) { hash = fieldValue; return TRUE; }
			break;
		case 'i':
			if ( fieldName % "input" ) { input = fieldValue; return TRUE; }
			if ( fieldName % "isInternalTx" ) { isInternalTx = toBool(fieldValue); return TRUE; }
			if ( fieldName % "isError" ) { isError = toBool(fieldValue); return TRUE; }
			break;
		case 'n':
			if ( fieldName % "nonce" ) { nonce = fieldValue; return TRUE; }
			break;
		case 't':
			if ( fieldName % "timeStamp" ) { timeStamp = toLong(fieldValue); return TRUE; }
			if ( fieldName % "to" ) { to = toLower(fieldValue); return TRUE; }
			if ( fieldName % "transactionIndex" ) { transactionIndex = toLong(fieldValue); return TRUE; }
			break;
		case 'v':
			if ( fieldName % "value" ) { value = fieldValue; return TRUE; }
			break;
		default:
			break;
	}
	return FALSE;
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
		archive >> isInternalTx;
		archive >> isError;

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
		archive << isInternalTx;
		archive << isError;

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
	ADD_FIELD(CTransaction, "isInternalTx", T_RADIO, ++fieldNum);
	ADD_FIELD(CTransaction, "isError", T_RADIO, ++fieldNum);

	// Hide our internal fields, user can turn them on if they like
	HIDE_FIELD(CTransaction, "schema");
	HIDE_FIELD(CTransaction, "deleted");
	HIDE_FIELD(CTransaction, "handle");

	// EXISTING_CODE
	ADD_FIELD(CTransaction, "date", T_TEXT, ++fieldNum);
	ADD_FIELD(CTransaction, "ether", T_NUMBER, ++fieldNum);
	ADD_FIELD(CTransaction, "hitLimit", T_RADIO, ++fieldNum);
	ADD_FIELD(CTransaction, "inputLen", T_NUMBER, ++fieldNum);
	ADD_FIELD(CTransaction, "function", T_TEXT, ++fieldNum);

	// Hide fields we don't want to show by default
	HIDE_FIELD(CTransaction, "confirmations");
	//HIDE_FIELD(CTransaction, "hitLimit");
	//HIDE_FIELD(CTransaction, "inputLen");
	HIDE_FIELD(CTransaction, "function");
	//HIDE_FIELD(CTransaction, "isInternalTx");
	//HIDE_FIELD(CTransaction, "isError");
	// EXISTING_CODE
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
