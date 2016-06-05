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
#include "fielddata.h"
#include "editableobject.h"

//---------------------------------------------------------------------------------------------
CServerBase::CServerBase(void)
{
	m_theCookie = "not-set";
	// m_finalScript;
	// m_helperStr;
	m_isEditing = FALSE;
}

//---------------------------------------------------------------------------
void CServerBase::establishCookie(void)
{
//	if (m_theCookie == "not-set")
//		m_theCookie = cgi_uCookie();
}

//---------------------------------------------------------------------------
SFString CServerBase::getCookieString(const SFString& name, const SFString& defVal) const
{
	((CServerBase*)this)->establishCookie();
	return parseCookie(name, defVal);
}

//-------------------------------------------------------------------
void CServerBase::setCookieString(const SFString& name, const SFString& value, SFInt32 nMonths)
{
	SFString cookie = formatCookie(name, value);

	SFString newCookie = "<script>WriteCookie(" + asciiQuote(name) + ", " + asciiQuote(cookie) + ", " + asString(nMonths) + ");</script>";
	if (!m_finalScript.Contains(newCookie)) // if it already contains the exact same cookie, don't set it again
		m_finalScript += newCookie;

#ifdef TESTING
	extern SFString g_unitTesting_cookie;
	snagFieldClear(g_unitTesting_cookie, name, EMPTY);
	g_unitTesting_cookie += formatCookie(name, value);
#endif

	updateCookie(name, value);
}

//---------------------------------------------------------------------------
void CServerBase::updateCookie(const SFString& name, const SFString& newVal)
{
	SFString curVal = getCookieString(name);

	establishCookie();
	SFString& ref = m_theCookie;

#ifdef TESTING
	extern SFString g_unitTesting_cookie;
	if (g_unitTesting)
		ref = g_unitTesting_cookie;
#endif

	if (!curVal.IsEmpty())
	{
		ref.Replace(formatCookie(name, curVal), EMPTY);
		ref.Replace(name + "=", EMPTY);
	}

	ref += name + "=" + formatCookie(name, newVal) + ";";
}

//---------------------------------------------------------------------------------------------
SFString getNextChunk(SFString& fmtOut, NEXTCHUNKFUNC func, const void *data)
{
	SFString chunk = fmtOut;
	if (!fmtOut.Contains("["))
	{
		// There are no more tokens.  Return the last chunk and empty out the format
		fmtOut = EMPTY;
		return chunk;
	}

	if (!fmtOut.startsWith('['))
	{
		// We've encountered plain text outside of a token. There is more to process so grab
		// the next chunk and then prepare the remaining chunk by prepending the token.
		// The next time through we will hit the token.
		chunk  = nextToken(fmtOut, '[');
		fmtOut = "[" + fmtOut;
		return chunk;
	}

	// We've hit a token, toss the start token, look for a field and toss the last token
	// leaving the remainder of the format in fmtOut.  A field is found if we find a pair
	// of squigglies).  Save text inside the start token and outside the field in pre and post
	ASSERT(fmtOut.startsWith('['));

	SFString pre, fieldName, post;
	nextToken(fmtOut, '['); // toss the start token
	if (chunk.Contains("{"))
	{
		// we've encountered a field
		pre       = nextToken(fmtOut, '{');
		fieldName = nextToken(fmtOut, '}');
		post      = nextToken(fmtOut, ']');
	}	else
	{
		// we've encountered a token with no field inside of it.  Just pull off
		// the entire contents into post.  It will be returned just below.
		post = nextToken(fmtOut, ']');
	}

	// Either no squigglies were found or an empty pair of squigglies were found.  In either
	// case return surrounding text (text inside the token and outside the squiggles)
	if (fieldName.IsEmpty())
		return pre + post;

	// We have a field so lets process it.
	ASSERT(fieldName.GetLength()>=2);
	SFBool  isPrompt = FALSE;
	SFInt32 maxWidth = BAD_NUMBER;

	// The fieldname may contain p: or w:width: or both.  If it contains either it
	// must contain them at the beginning of the string (before the fieldName).  Anything
	// left after the last ':' is considered the fieldName
	SFString promptName = fieldName;
	if (fieldName.ContainsI("p:"))
	{
		isPrompt = TRUE;
		fieldName.ReplaceI("p:", EMPTY);
		promptName = fieldName;
	}

	if (fieldName.ContainsI("w:"))
	{
		ASSERT(fieldName.Left(2) % "w:");         // must be first thing in the string
		fieldName.ReplaceI("w:", EMPTY);          // get rid of the 'w:'
		maxWidth = toLong(fieldName); // grab the width
		nextTokenClear(fieldName, ':');           // skip to the start of the fieldname
	}

	if (fieldName.ContainsI("f:"))
	{
		ASSERT(fieldName.Left(2) % "f:");         // must be first thing in the string
		fieldName.ReplaceI("f:", EMPTY);          // get rid of the 'w:'
		pre = pre + "<input type=text size=80 value=\"";
		post = "\">" + post;
	}

	// Get the value of the field.  If the value of the field is empty we return empty for the entire token.
	SFBool forceShow = FALSE;
	SFString fieldValue = (func)(fieldName, forceShow, data);
	forceShow = (isPrompt?TRUE:forceShow);
	if (!forceShow && fieldValue.IsEmpty())
		return EMPTY;
	fieldValue = truncPad(fieldValue, maxWidth); // pad or truncate

	// The notify union will contain an event for event processing, a contact for
	// contact processing and be NULL for other uses.
	CNotifyUnionBase *notUnion = (CNotifyUnionBase*)data;
	ASSERT(!notUnion || notUnion->hasDisplayItem());

	// Try to find the fieldData for this field to see if the field is hidden.
	// Pick the customization up from the event's database (which may be different than
	// the server's database in a merged in calendar for example)
	const CFieldData *fieldData = (notUnion ? notUnion->getFieldData(fieldName) : NULL);

	// If the field is hidden the entire token is empty
	if (notUnion && fieldData && !notUnion->shouldShow(fieldData))
		return EMPTY;

	// The field is not hidden, the value of the field is not empty, we are not working
	// on a prompt, so we toss back the token referencing the value of the field.
	if (!isPrompt)
		return pre + fieldValue + post;

	// We are working on a prompt.  Pick up customizations if any
	SFString prompt = promptName; //toProper(fieldName);
	if (fieldData)
		prompt = fieldData->getPrompt();

	if (notUnion)
		notUnion->postProcess(fieldName, prompt);
	prompt = truncPad(prompt, maxWidth); // pad or truncate
	return pre + prompt + post;
}

//---------------------------------------------------------------------------
SFString CServerBase::getStringBase(const SFString& fieldName, const SFString& defVal) const
{
    return ""; //getString_unSafe(fieldName, defVal);
}
