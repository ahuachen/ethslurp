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
#include "parameter.h"

//---------------------------------------------------------------------------
IMPLEMENT_NODE(CParameter, CBaseNode, NO_SCHEMA);

//---------------------------------------------------------------------------
void CParameter::Format(CExportContext& ctx, const SFString& fmtIn, void *data) const
{
	if (!isShowing())
		return;

	SFString fmt = (fmtIn.IsEmpty() ? defaultFormat() : fmtIn).Substitute("\n","\t");
	if (handleCustomFormat(ctx, fmt, data))
		return;

	CParameterNotify dn(this);
	while (!fmt.IsEmpty())
		ctx << getNextChunk(fmt, nextParameterChunk, &dn);
}

//---------------------------------------------------------------------------
SFString nextParameterChunk(const SFString& fieldIn, SFBool& force, const void *data)
{
	CParameterNotify *pa = (CParameterNotify*)data;
	const CParameter *par = pa->getDataPtr();

	// Give common (edit, delete, etc.) code a chance to override
	SFString ret = nextChunk_common(fieldIn, getString("cmd"), par);
	if (!ret.IsEmpty())
		return ret;
	
	// Now give customized code a chance to override
	ret = nextParameterChunk_custom(fieldIn, force, data);
	if (!ret.IsEmpty())
		return ret;
	
	switch (tolower(fieldIn[0]))
	{
		case 'f':
			return EMPTY;
//			if ( fieldIn % "func" ) return par->func;
			break;
		case 'h':
			if ( fieldIn % "handle" ) return asString(par->handle);
			break;
		case 'n':
			if ( fieldIn % "name" ) return par->name;
			break;
		case 't':
			if ( fieldIn % "type" ) return par->type;
			break;
	}
	
	return "<span class=warning>Field not found: [{" + fieldIn + "}]</span>\n";
}

//---------------------------------------------------------------------------------------------------
SFBool CParameter::setValueByName(const SFString& fieldName, const SFString& fieldValue)
{
	switch (tolower(fieldName[0]))
	{
		case 'f':
			return TRUE;
//			if ( fieldName % "func" ) { func = fieldValue; return TRUE; }
			break;
		case 'h':
			if ( fieldName % "handle" ) { handle = toLong(fieldValue); return TRUE; }
			break;
		case 'n':
			if ( fieldName % "name" ) { name = fieldValue; return TRUE; }
			break;
		case 't':
			if ( fieldName % "type" ) { type = fieldValue; return TRUE; }
			break;
		default:
			break;
	}
	return FALSE;
}

//---------------------------------------------------------------------------------------------------
void CParameter::Serialize(SFArchive& archive)
{
	SERIALIZE_START();
	if (archive.isReading())
	{
		archive >> handle;
		archive >> name;
		archive >> type;
//		archive >> func;

	} else
	{
		archive << handle;
		archive << name;
		archive << type;
//		archive << func;

	}
	SERIALIZE_END();
}

//---------------------------------------------------------------------------
void CParameter::registerClass(void)
{
	SFInt32 fieldNum=1000;
	ADD_FIELD(CParameter, "schema",  T_NUMBER|TS_LABEL, ++fieldNum);
	ADD_FIELD(CParameter, "deleted", T_RADIO|TS_LABEL,  ++fieldNum);
	ADD_FIELD(CParameter, "handle", T_NUMBER|TS_LABEL,  ++fieldNum);
	ADD_FIELD(CParameter, "name", T_TEXT, ++fieldNum);
	ADD_FIELD(CParameter, "type", T_TEXT, ++fieldNum);
	ADD_FIELD(CParameter, "func", T_NONE, ++fieldNum);

	// Hide our internal fields, user can turn them on if they like
	HIDE_FIELD(CParameter, "schema");
	HIDE_FIELD(CParameter, "deleted");
	HIDE_FIELD(CParameter, "handle");

	// EXISTING_CODE
	// EXISTING_CODE
}

//---------------------------------------------------------------------------
int sortParameter(const SFString& f1, const SFString& f2, const void *rr1, const void *rr2)
{
	CParameter *g1 = (CParameter*)rr1;
	CParameter *g2 = (CParameter*)rr2;

	SFString v1 = g1->getValueByName(f1);
	SFString v2 = g2->getValueByName(f1);
	SFInt32 s = v1.Compare(v2);
	if (s || f2.IsEmpty())
		return (int)s;

	v1 = g1->getValueByName(f2);
	v2 = g2->getValueByName(f2);
	return (int)v1.Compare(v2);
}
int sortParameterByName(const void *rr1, const void *rr2) { return sortParameter("pa_Name", "", rr1, rr2); }
int sortParameterByID  (const void *rr1, const void *rr2) { return sortParameter("parameterID", "", rr1, rr2); }
