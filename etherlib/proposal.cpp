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
#include "proposal.h"

//---------------------------------------------------------------------------
IMPLEMENT_NODE(CProposal, CBaseNode, NO_SCHEMA);

//---------------------------------------------------------------------------
void CProposal::Format(CExportContext& ctx, const SFString& fmtIn, void *data) const
{
	if (!isShowing())
		return;

	SFString fmt = (fmtIn.IsEmpty() ? defaultFormat() : fmtIn); //.Substitute("\n","\t");
	if (handleCustomFormat(ctx, fmt, data))
		return;

	CProposalNotify dn(this);
	while (!fmt.IsEmpty())
		ctx << getNextChunk(fmt, nextProposalChunk, &dn);
}

//---------------------------------------------------------------------------
SFString nextProposalChunk(const SFString& fieldIn, SFBool& force, const void *data)
{
	CProposalNotify *pr = (CProposalNotify*)data;
	const CProposal *pro = pr->getDataPtr();

	// Give common (edit, delete, etc.) code a chance to override
	SFString ret = nextChunk_common(fieldIn, getString("cmd"), pro);
	if (!ret.IsEmpty())
		return ret;
	
	// Now give customized code a chance to override
	ret = nextProposalChunk_custom(fieldIn, force, data);
	if (!ret.IsEmpty())
		return ret;
	
	switch (tolower(fieldIn[0]))
	{
		case 'c':
			if ( fieldIn % "closingDate" ) return pro->closingDate.Format(FMT_DATE);
			break;
		case 'h':
			if ( fieldIn % "handle" ) return asString(pro->handle);
			break;
		case 'n':
			if ( fieldIn % "nYes" ) return asString(pro->nYes);
			if ( fieldIn % "nNo" ) return asString(pro->nNo);
			break;
		case 'p':
			if ( fieldIn % "proposalID" ) return asString(pro->proposalID);
			break;
	}
	
	return "<span class=warning>Field not found: [{" + fieldIn + "}]</span>\n";
}

//---------------------------------------------------------------------------------------------------
SFBool CProposal::setValueByName(const SFString& fieldName, const SFString& fieldValue)
{
	switch (tolower(fieldName[0]))
	{
		case 'c':
			if ( fieldName % "closingDate" ) { closingDate = snagDate(fieldValue); return TRUE; }
			break;
		case 'h':
			if ( fieldName % "handle" ) { handle = toLong(fieldValue); return TRUE; }
			break;
		case 'n':
			if ( fieldName % "nYes" ) { nYes = toLong(fieldValue); return TRUE; }
			if ( fieldName % "nNo" ) { nNo = toLong(fieldValue); return TRUE; }
			break;
		case 'p':
			if ( fieldName % "proposalID" ) { proposalID = toLong(fieldValue); return TRUE; }
			break;
		default:
			break;
	}
	return FALSE;
}

//---------------------------------------------------------------------------------------------------
void CProposal::finishParse()
{
	// EXISTING_CODE
	// EXISTING_CODE
}

//---------------------------------------------------------------------------------------------------
void CProposal::Serialize(SFArchive& archive)
{
	if (!SerializeHeader(archive))
		return;
	
	if (archive.isReading())
	{
		archive >> handle;
		archive >> proposalID;
		archive >> closingDate;
		archive >> nYes;
		archive >> nNo;

	} else
	{
		archive << handle;
		archive << proposalID;
		archive << closingDate;
		archive << nYes;
		archive << nNo;

	}
}

//---------------------------------------------------------------------------
void CProposal::registerClass(void)
{
	SFInt32 fieldNum=1000;
	ADD_FIELD(CProposal, "schema",  T_NUMBER|TS_LABEL, ++fieldNum);
	ADD_FIELD(CProposal, "deleted", T_RADIO|TS_LABEL,  ++fieldNum);
	ADD_FIELD(CProposal, "handle", T_NUMBER|TS_LABEL,  ++fieldNum);
	ADD_FIELD(CProposal, "proposalID", T_NUMBER, ++fieldNum);
	ADD_FIELD(CProposal, "closingDate", T_DATE, ++fieldNum);
	ADD_FIELD(CProposal, "nYes", T_NUMBER, ++fieldNum);
	ADD_FIELD(CProposal, "nNo", T_NUMBER, ++fieldNum);

	// Hide our internal fields, user can turn them on if they like
	HIDE_FIELD(CProposal, "schema");
	HIDE_FIELD(CProposal, "deleted");
	HIDE_FIELD(CProposal, "handle");

	// EXISTING_CODE
	// EXISTING_CODE
}

//---------------------------------------------------------------------------
int sortProposal(const SFString& f1, const SFString& f2, const void *rr1, const void *rr2)
{
	CProposal *g1 = (CProposal*)rr1;
	CProposal *g2 = (CProposal*)rr2;

	SFString v1 = g1->getValueByName(f1);
	SFString v2 = g2->getValueByName(f1);
	SFInt32 s = v1.Compare(v2);
	if (s || f2.IsEmpty())
		return (int)s;

	v1 = g1->getValueByName(f2);
	v2 = g2->getValueByName(f2);
	return (int)v1.Compare(v2);
}
int sortProposalByName(const void *rr1, const void *rr2) { return sortProposal("pr_Name", "", rr1, rr2); }
int sortProposalByID  (const void *rr1, const void *rr2) { return sortProposal("proposalID", "", rr1, rr2); }
