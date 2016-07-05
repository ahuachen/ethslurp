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
#include "account.h"

//---------------------------------------------------------------------------
IMPLEMENT_NODE(CAccount, CBaseNode, NO_SCHEMA);

//---------------------------------------------------------------------------
void CAccount::Format(CExportContext& ctx, const SFString& fmtIn, void *data) const
{
	if (!isShowing())
		return;

	SFString fmt = (fmtIn.IsEmpty() ? defaultFormat() : fmtIn); //.Substitute("\n","\t");
	if (handleCustomFormat(ctx, fmt, data))
		return;

	CAccountNotify dn(this);
	while (!fmt.IsEmpty())
		ctx << getNextChunk(fmt, nextAccountChunk, &dn);
}

//---------------------------------------------------------------------------
SFString nextAccountChunk(const SFString& fieldIn, SFBool& force, const void *data)
{
	CAccountNotify *ac = (CAccountNotify*)data;
	const CAccount *acc = ac->getDataPtr();

	// Give common (edit, delete, etc.) code a chance to override
	SFString ret = nextChunk_common(fieldIn, getString("cmd"), acc);
	if (!ret.IsEmpty())
		return ret;
	
	// Now give customized code a chance to override
	ret = nextAccountChunk_custom(fieldIn, force, data);
	if (!ret.IsEmpty())
		return ret;
	
	switch (tolower(fieldIn[0]))
	{
		case 'a':
			if ( fieldIn % "addr" ) return acc->addr;
			break;
		case 'h':
			if ( fieldIn % "handle" ) return asString(acc->handle);
			break;
		case 'n':
			if ( fieldIn % "nVotes" ) return asString(acc->nVotes);
			if ( fieldIn % "nProposals" ) return asString(acc->nProposals);
			if ( fieldIn % "nTransactions" ) return asString(acc->nTransactions);
			break;
		case 'p':
			return EMPTY;
//			if ( fieldIn % "proposals" ) return acc->proposals;
			break;
		case 't':
			return EMPTY;
//			if ( fieldIn % "transactions" ) return acc->transactions;
			break;
		case 'v':
			return EMPTY;
//			if ( fieldIn % "votes" ) return acc->votes;
			break;
	}
	
	return "<span class=warning>Field not found: [{" + fieldIn + "}]</span>\n";
}

//---------------------------------------------------------------------------------------------------
SFBool CAccount::setValueByName(const SFString& fieldName, const SFString& fieldValue)
{
	switch (tolower(fieldName[0]))
	{
		case 'a':
			if ( fieldName % "addr" ) { addr = fieldValue; return TRUE; }
			break;
		case 'h':
			if ( fieldName % "handle" ) { handle = toLong(fieldValue); return TRUE; }
			break;
		case 'n':
			if ( fieldName % "nVotes" ) { nVotes = toLong(fieldValue); return TRUE; }
			if ( fieldName % "nProposals" ) { nProposals = toLong(fieldValue); return TRUE; }
			if ( fieldName % "nTransactions" ) { nTransactions = toLong(fieldValue); return TRUE; }
			break;
		case 'p':
			return TRUE;
//			if ( fieldName % "proposals" ) { proposals = fieldValue; return TRUE; }
			break;
		case 't':
			return TRUE;
//			if ( fieldName % "transactions" ) { transactions = fieldValue; return TRUE; }
			break;
		case 'v':
			return TRUE;
//			if ( fieldName % "votes" ) { votes = fieldValue; return TRUE; }
			break;
		default:
			break;
	}
	return FALSE;
}

//---------------------------------------------------------------------------------------------------
void CAccount::finishParse()
{
	// EXISTING_CODE
	// EXISTING_CODE
}

//---------------------------------------------------------------------------------------------------
void CAccount::Serialize(SFArchive& archive)
{
	if (!SerializeHeader(archive))
		return;
	
	if (archive.isReading())
	{
		archive >> handle;
		archive >> addr;
		archive >> nVotes;
//		archive >> votes;
		archive >> nProposals;
//		archive >> proposals;
		archive >> nTransactions;
//		archive >> transactions;

	} else
	{
		archive << handle;
		archive << addr;
		archive << nVotes;
//		archive << votes;
		archive << nProposals;
//		archive << proposals;
		archive << nTransactions;
//		archive << transactions;

	}
}

//---------------------------------------------------------------------------
void CAccount::registerClass(void)
{
	SFInt32 fieldNum=1000;
	ADD_FIELD(CAccount, "schema",  T_NUMBER|TS_LABEL, ++fieldNum);
	ADD_FIELD(CAccount, "deleted", T_RADIO|TS_LABEL,  ++fieldNum);
	ADD_FIELD(CAccount, "handle", T_NUMBER|TS_LABEL,  ++fieldNum);
	ADD_FIELD(CAccount, "addr", T_TEXT, ++fieldNum);
	ADD_FIELD(CAccount, "nVotes", T_NUMBER, ++fieldNum);
	ADD_FIELD(CAccount, "votes", T_NONE, ++fieldNum);
	ADD_FIELD(CAccount, "nProposals", T_NUMBER, ++fieldNum);
	ADD_FIELD(CAccount, "proposals", T_NONE, ++fieldNum);
	ADD_FIELD(CAccount, "nTransactions", T_NUMBER, ++fieldNum);
	ADD_FIELD(CAccount, "transactions", T_NONE, ++fieldNum);

	// Hide our internal fields, user can turn them on if they like
	HIDE_FIELD(CAccount, "schema");
	HIDE_FIELD(CAccount, "deleted");
	HIDE_FIELD(CAccount, "handle");

	// EXISTING_CODE
	// EXISTING_CODE
}

//---------------------------------------------------------------------------
int sortAccount(const SFString& f1, const SFString& f2, const void *rr1, const void *rr2)
{
	CAccount *g1 = (CAccount*)rr1;
	CAccount *g2 = (CAccount*)rr2;

	SFString v1 = g1->getValueByName(f1);
	SFString v2 = g2->getValueByName(f1);
	SFInt32 s = v1.Compare(v2);
	if (s || f2.IsEmpty())
		return (int)s;

	v1 = g1->getValueByName(f2);
	v2 = g2->getValueByName(f2);
	return (int)v1.Compare(v2);
}
int sortAccountByName(const void *rr1, const void *rr2) { return sortAccount("ac_Name", "", rr1, rr2); }
int sortAccountByID  (const void *rr1, const void *rr2) { return sortAccount("accountID", "", rr1, rr2); }
