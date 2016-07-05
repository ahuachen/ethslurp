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
#include "vote.h"

//---------------------------------------------------------------------------
IMPLEMENT_NODE(CVote, CBaseNode, NO_SCHEMA);

//---------------------------------------------------------------------------
void CVote::Format(CExportContext& ctx, const SFString& fmtIn, void *data) const
{
	if (!isShowing())
		return;

	SFString fmt = (fmtIn.IsEmpty() ? defaultFormat() : fmtIn);;
	if (handleCustomFormat(ctx, fmt, data))
		return;

	CVoteNotify dn(this);
	while (!fmt.IsEmpty())
		ctx << getNextChunk(fmt, nextVoteChunk, &dn);
}

//---------------------------------------------------------------------------
SFString nextVoteChunk(const SFString& fieldIn, SFBool& force, const void *data)
{
	CVoteNotify *vo = (CVoteNotify*)data;
	const CVote *vot = vo->getDataPtr();

	// Give common (edit, delete, etc.) code a chance to override
	SFString ret = nextChunk_common(fieldIn, getString("cmd"), vot);
	if (!ret.IsEmpty())
		return ret;
	
	// Now give customized code a chance to override
	ret = nextVoteChunk_custom(fieldIn, force, data);
	if (!ret.IsEmpty())
		return ret;
	
	switch (tolower(fieldIn[0]))
	{
		case 'f':
			if ( fieldIn % "from" ) return vot->from;
			break;
		case 'h':
			if ( fieldIn % "handle" ) return asString(vot->handle);
			break;
		case 'p':
			if ( fieldIn % "proposalID" ) return padNum3(vot->proposalID);
			break;
		case 'v':
			if ( fieldIn % "votedYes" ) return asString(vot->votedYes);
			break;
	}
	
	return "<span class=warning>Field not found: [{" + fieldIn + "}]</span>\n";
}

//---------------------------------------------------------------------------------------------------
SFBool CVote::setValueByName(const SFString& fieldName, const SFString& fieldValue)
{
	switch (tolower(fieldName[0]))
	{
		case 'f':
			if ( fieldName % "from" ) { from = fieldValue; return TRUE; }
			else if ( fieldName % "function" ) {
				SFString val = fieldValue;
				SFString func = nextTokenClear(val,'|'); // skip
				SFString prop = nextTokenClear(val,'|');
				SFString vote = nextTokenClear(val,'|');
				setValueByName("proposalID", prop);
				setValueByName("votedYes",   vote == "Yea" ? "1" : "0");
				return TRUE;
			}
			break;
		case 'h':
			if ( fieldName % "handle" ) { handle = toLong(fieldValue); return TRUE; }
			break;
		case 'p':
			if ( fieldName % "proposalID" ) { proposalID = toLong(fieldValue); return TRUE; }
			break;
		case 'v':
			if ( fieldName % "votedYes" ) { votedYes = toBool(fieldValue); return TRUE; }
			break;
		default:
			break;
	}
	return FALSE;
}

//---------------------------------------------------------------------------------------------------
void CVote::finishParse()
{
	// EXISTING_CODE
	// EXISTING_CODE
}

//---------------------------------------------------------------------------------------------------
void CVote::Serialize(SFArchive& archive)
{
	if (!SerializeHeader(archive))
		return;
	
	if (archive.isReading())
	{
		archive >> handle;
		archive >> from;
		archive >> proposalID;
		archive >> votedYes;

	} else
	{
		archive << handle;
		archive << from;
		archive << proposalID;
		archive << votedYes;

	}
}

//---------------------------------------------------------------------------
void CVote::registerClass(void)
{
	SFInt32 fieldNum=1000;
	ADD_FIELD(CVote, "schema",  T_NUMBER|TS_LABEL, ++fieldNum);
	ADD_FIELD(CVote, "deleted", T_RADIO|TS_LABEL,  ++fieldNum);
	ADD_FIELD(CVote, "handle", T_NUMBER|TS_LABEL,  ++fieldNum);
	ADD_FIELD(CVote, "from", T_TEXT, ++fieldNum);
	ADD_FIELD(CVote, "proposalID", T_NUMBER, ++fieldNum);
	ADD_FIELD(CVote, "votedYes", T_RADIO, ++fieldNum);

	// Hide our internal fields, user can turn them on if they like
	HIDE_FIELD(CVote, "schema");
	HIDE_FIELD(CVote, "deleted");
	HIDE_FIELD(CVote, "handle");

	// EXISTING_CODE
	// EXISTING_CODE
}

//---------------------------------------------------------------------------
int sortVote(const SFString& f1, const SFString& f2, const void *rr1, const void *rr2)
{
	CVote *g1 = (CVote*)rr1;
	CVote *g2 = (CVote*)rr2;

	SFString v1 = g1->getValueByName(f1);
	SFString v2 = g2->getValueByName(f1);
	SFInt32 s = v1.Compare(v2);
	if (s || f2.IsEmpty())
		return (int)s;

	v1 = g1->getValueByName(f2);
	v2 = g2->getValueByName(f2);
	return (int)v1.Compare(v2);
}
int sortVoteByName(const void *rr1, const void *rr2) { return sortVote("vo_Name", "", rr1, rr2); }
int sortVoteByID  (const void *rr1, const void *rr2) { return sortVote("voteID", "", rr1, rr2); }
