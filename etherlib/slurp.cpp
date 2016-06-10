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
#include "slurp.h"

//---------------------------------------------------------------------------
IMPLEMENT_NODE(CSlurp, CBaseNode, NO_SCHEMA);

//---------------------------------------------------------------------------
void CSlurp::Format(CExportContext& ctx, const SFString& fmtIn, void *data) const
{
	if (!isShowing())
		return;

	SFString fmt = (fmtIn.IsEmpty() ? defaultFormat() : fmtIn);;
	if (handleCustomFormat(ctx, fmt, data))
		return;

	CSlurpNotify dn(this);
	while (!fmt.IsEmpty())
		ctx << getNextChunk(fmt, nextSlurpChunk, &dn);
}

//---------------------------------------------------------------------------
SFString nextSlurpChunk(const SFString& fieldIn, SFBool& force, const void *data)
{
	CSlurpNotify *sl = (CSlurpNotify*)data;
	const CSlurp *slu = sl->getDataPtr();

	// Give common (edit, delete, etc.) code a chance to override
	SFString ret = nextChunk_common(fieldIn, getString("cmd"), slu);
	if (!ret.IsEmpty())
		return ret;
	
	// Now give customized code a chance to override
	ret = nextSlurpChunk_custom(fieldIn, force, data);
	if (!ret.IsEmpty())
		return ret;
	
	switch (tolower(fieldIn[0]))
	{
		case 'a':
			if ( fieldIn % "addr" ) return slu->addr;
			break;
		case 'd':
			if ( fieldIn % "displayString" ) return slu->displayString;
			break;
		case 'f':
			if ( fieldIn % "functions" )
			{
				SFString ret = "\n";
				for (int i=0;i<slu->functions.getCount();i++)
					ret += slu->functions[i].Format();
				return ret;
			}
			break;
		case 'h':
			if ( fieldIn % "handle" ) return asString(slu->handle);
			if ( fieldIn % "header" ) return slu->header;
			break;
		case 'l':
			if ( fieldIn % "lastPage" ) return asString(slu->lastPage);
			if ( fieldIn % "lastBlock" ) return asString(slu->lastBlock);
			break;
		case 'n':
			if ( fieldIn % "nVisible" ) return asString(slu->nVisible);
			break;
		case 'p':
			if ( fieldIn % "pageSize" ) return asString(slu->pageSize);
			break;
		case 't':
			if ( fieldIn % "transactions" )
			{
				SFString ret = "\n";
				for (int i=0;i<slu->transactions.getCount();i++)
					ret += slu->transactions[i].Format();
				return ret;
			}
			break;
	}
	
	return "<span class=warning>Field not found: [{" + fieldIn + "}]</span>\n";
}

//---------------------------------------------------------------------------------------------------
SFBool CSlurp::setValueByName(const SFString& fieldName, const SFString& fieldValue)
{
	switch (tolower(fieldName[0]))
	{
		case 'a':
			if ( fieldName % "addr" ) { addr = fieldValue; return TRUE; }
			break;
		case 'd':
			if ( fieldName % "displayString" ) { displayString = fieldValue; return TRUE; }
			break;
		case 'f':
			if ( fieldName % "functions" ) return TRUE;
			break;
		case 'h':
			if ( fieldName % "handle" ) { handle = toLong(fieldValue); return TRUE; }
			if ( fieldName % "header" ) { header = fieldValue; return TRUE; }
			break;
		case 'l':
			if ( fieldName % "lastPage" ) { lastPage = toLong(fieldValue); return TRUE; }
			if ( fieldName % "lastBlock" ) { lastBlock = toLong(fieldValue); return TRUE; }
			break;
		case 'n':
			if ( fieldName % "nVisible" ) { nVisible = toLong(fieldValue); return TRUE; }
			break;
		case 'p':
			if ( fieldName % "pageSize" ) { pageSize = toBool(fieldValue); return TRUE; }
			break;
		case 't':
			if ( fieldName % "transactions" ) return TRUE;
			break;
		default:
			break;
	}
	return FALSE;
}

//---------------------------------------------------------------------------------------------------
void CSlurp::Serialize(SFArchive& archive)
{
	SERIALIZE_START();
	if (archive.isReading())
	{
		archive >> handle;
		archive >> addr;
		archive >> header;
		archive >> displayString;
		archive >> pageSize;
		archive >> lastPage;
		archive >> lastBlock;
		archive >> nVisible;
		archive >> transactions;
		archive >> functions;

	} else
	{
		archive << handle;
		archive << addr;
		archive << header;
		archive << displayString;
		archive << pageSize;
		archive << lastPage;
		archive << lastBlock;
		archive << nVisible;
		archive << transactions;
		archive << functions;

	}
	SERIALIZE_END();
}

//---------------------------------------------------------------------------
void CSlurp::registerClass(void)
{
	SFInt32 fieldNum=1000;
	ADD_FIELD(CSlurp, "schema",  T_NUMBER|TS_LABEL, ++fieldNum);
	ADD_FIELD(CSlurp, "deleted", T_RADIO|TS_LABEL,  ++fieldNum);
	ADD_FIELD(CSlurp, "handle", T_NUMBER|TS_LABEL,  ++fieldNum);
	ADD_FIELD(CSlurp, "addr", T_TEXT, ++fieldNum);
	ADD_FIELD(CSlurp, "header", T_TEXT, ++fieldNum);
	ADD_FIELD(CSlurp, "displayString", T_TEXT, ++fieldNum);
	ADD_FIELD(CSlurp, "pageSize", T_RADIO, ++fieldNum);
	ADD_FIELD(CSlurp, "lastPage", T_NUMBER, ++fieldNum);
	ADD_FIELD(CSlurp, "lastBlock", T_NUMBER, ++fieldNum);
	ADD_FIELD(CSlurp, "nVisible", T_NUMBER, ++fieldNum);
	ADD_FIELD(CSlurp, "transactions", T_TEXT|TS_ARRAY, ++fieldNum);
	ADD_FIELD(CSlurp, "functions", T_TEXT|TS_ARRAY, ++fieldNum);

	// Hide our internal fields, user can turn them on if they like
	HIDE_FIELD(CSlurp, "schema");
	HIDE_FIELD(CSlurp, "deleted");
	HIDE_FIELD(CSlurp, "handle");

	// EXISTING_CODE
	// EXISTING_CODE
}

//---------------------------------------------------------------------------
int sortSlurp(const SFString& f1, const SFString& f2, const void *rr1, const void *rr2)
{
	CSlurp *g1 = (CSlurp*)rr1;
	CSlurp *g2 = (CSlurp*)rr2;

	SFString v1 = g1->getValueByName(f1);
	SFString v2 = g2->getValueByName(f1);
	SFInt32 s = v1.Compare(v2);
	if (s || f2.IsEmpty())
		return (int)s;

	v1 = g1->getValueByName(f2);
	v2 = g2->getValueByName(f2);
	return (int)v1.Compare(v2);
}
int sortSlurpByName(const void *rr1, const void *rr2) { return sortSlurp("sl_Name", "", rr1, rr2); }
int sortSlurpByID  (const void *rr1, const void *rr2) { return sortSlurp("slurpID", "", rr1, rr2); }
