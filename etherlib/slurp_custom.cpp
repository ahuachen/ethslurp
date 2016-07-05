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

// EXISTING_CODE
#include "/Users/jrush/src.GitHub/ethslurp/ethslurp.h"
// EXISTING_CODE

//---------------------------------------------------------------------------
SFString nextSlurpChunk_custom(const SFString& fieldIn, SFBool& force, const void *data)
{
	CSlurpNotify *sl = (CSlurpNotify*)data;
	const CSlurp *slu = sl->getDataPtr();
	switch (tolower(fieldIn[0]))
	{
		// EXISTING_CODE
		case 'n':
			if ( fieldIn % "now" ) return (isTesting ? "TESTING_TIME" : Now().Format(FMT_DEFAULT));
			break;
		case 'r':
			if ( fieldIn % "records" ) return (slu->transactions.getCount() == 0 ? "No records" : "");
			break;
		// EXISTING_CODE
		default:
			break;
	}
	
#pragma unused(sl)
#pragma unused(slu)

	return EMPTY;
}

//---------------------------------------------------------------------------
SFBool CSlurp::handleCustomFormat(CExportContext& ctx, const SFString& fmtIn, void *data) const
{
	// EXISTING_CODE
	// Split the format string into three parts: pre, post and records.
	// If no records, just process as normal. We do this because it's so slow
	// copying the records into a string, so we write it directly to the
	// export context. If there is no {RECORDS}, then just send handle it like normal
	if (!fmtIn.Contains("{RECORDS}") || transactions.getCount()==0)
	{
		SFString fmt = fmtIn;

		CSlurpNotify dn(this);
		while (!fmt.IsEmpty())
			ctx << getNextChunk(fmt, nextSlurpChunk, &dn);
	} else
	{
		SFString postFmt = fmtIn;
		postFmt.Replace("{RECORDS}","|");
		SFString preFmt = nextTokenClear(postFmt,'|');

		// We assume here that the token was properly formed. For the pre-text we
		// have to clear out the start '[', and for the post text we clear out the ']'
		preFmt.ReplaceReverse("[","");
		postFmt.Replace("]","");

		// We handle the display in three parts: pre, records, and post so as
		// to avoid building the entire record list into an ever-growing and
		// ever-slowing string
		CSlurpNotify dn(this);
		while (!preFmt.IsEmpty())
			ctx << getNextChunk(preFmt, nextSlurpChunk, &dn);
		SFInt32 cnt=0;
		for (int i=0;i<transactions.getCount();i++)
		{
			cnt += transactions[i].isShowing();
			if (cnt && !(cnt%REP_INFREQ))
			{
				outErr << "\tExporting record " << cnt << " of " << nVisible;
				outErr << (transactions.getCount()!=nVisible?" visible":"") << " records" << (isTesting?"\n":"\r"); outErr.Flush();
			}
			ctx << transactions[i].Format(displayString);
			if (cnt>=nVisible)
				break; // no need to keep spinning if we've shown them all
		}
		ctx << "\n";
		while (!postFmt.IsEmpty())
			ctx << getNextChunk(postFmt, nextSlurpChunk, &dn);
	}
	return TRUE;
	// EXISTING_CODE
	return FALSE;
}

// EXISTING_CODE
#define MAX_FUNCS 200
CFunction funcTable[MAX_FUNCS];
SFInt32 nFunctions=0;
//---------------------------------------------------------------------------
int sortFuncTable(const void *ob1, const void *ob2)
{
	CFunction *p1 = (CFunction*)ob1;
	CFunction *p2 = (CFunction*)ob2;
	return (int)p2->encoding.Compare(p1->encoding);
}

//---------------------------------------------------------------------------
void CSlurp::loadABI(void)
{
	// Already loaded?
	if (nFunctions)
		return;

extern SFString configPath(const SFString& part);
	SFString abiFilename = 	configPath("abis/"+addr+".json");
	if (!SFos::fileExists(abiFilename))
		return;
	
	outErr << "\tLoading abi file: " << abiFilename << "...\n";
	SFString contents = asciiFileToString(abiFilename);
	ASSERT(!contents.IsEmpty());

	char *p = cleanUpJson((char *)(const char*)contents);
	while (p && *p)
	{
		CFunction func;SFInt32 nFields=0;
		p = func.parseJson(p,nFields);
		if (nFields)
		{
			SFString ethabi = "/usr/local/bin/ethabi";
			if (SFos::fileExists(ethabi) && func.type == "function")
			{
				SFString cmd = ethabi + " encode function \"" + abiFilename + "\" " + func.name;
				SFString result = SFos::doCommand(cmd);
				func.encoding = result;
				funcTable[nFunctions++] = func;
			}
		}
	}
	qsort(funcTable, nFunctions, sizeof(CFunction), sortFuncTable);
	for (int i=0;i<nFunctions;i++)
		if (funcTable[i].type == "function" && verbose)
			outErr << funcTable[i].Format().Substitute("\n"," ") << "\n";
}
// EXISTING_CODE
