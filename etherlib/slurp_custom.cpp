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
#include "ethslurp.h"
// EXISTING_CODE

//---------------------------------------------------------------------------
void finishParse(CSlurp *slurp)
{
	// EXISTING_CODE
	// EXISTING_CODE
}

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
			if (cnt && !(cnt%5))
			{
				outErr << "\tExporting record " << cnt << " of " << nVisible;
				outErr << (transactions.getCount()!=nVisible?" visible":"") << " records" << (isTesting?"\n":"\r"); outErr.Flush();
			}
			ctx << transactions[i].Format(displayString);
			if (cnt>=nVisible)
				continue; // no need to keep spinning if we've shown them all
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
//---------------------------------------------------------------------------
SFInt32 CSlurp::readFromFile(CSharedResource& file)
{
	SFInt32 val=0;SFString str;
	file.Read( val ); setDeleted(val);
	file.Read( val ); setSchema(val);
	file.Read( val ); setShowing(val);
	file.Read( str ); ASSERT(str == ((CSlurp*)this )->getClassName());
	file.Read( handle );
	file.Read( addr );
	file.Read( header );
	file.Read( displayString );
	file.Read( pageSize );
	file.Read( lastPage );
	file.Read( lastBlock );

	// Now read the array
	SFInt32 nRecords;
	file.Read( nRecords );
	transactions.Grow(nRecords+1); // resets size of array

	SFInt32 nRead=0, maxBlock=0;
	for (int i=0;i<nRecords;i++)
	{
		transactions[i].readFromFile(file);
		if (!(++nRead%5)) { outErr << "\tReading from cache...record " << nRead << " of " << nRecords << (isTesting?"\n":"\r"); outErr.Flush(); }
		SFInt32 curBlock = transactions[i].blockNumber;
		maxBlock = MAX(maxBlock,curBlock);
	}

	if (!isTesting)
		outErr << "\tReading from cache...record " << nRecords << " of " << nRecords << "\n";
	if (maxBlock != lastBlock)
		outErr << "Previously stored lastBlock '" << lastBlock << "' not equal to maxBlock '" << maxBlock << "\n";
	outErr.Flush();

	return TRUE;
}

//---------------------------------------------------------------------------
SFInt32 CSlurp::writeToFile(CSharedResource& file) const
{
	file.Write( isDeleted() );
	file.Write( getSchema() );
	file.Write( TRUE ); // don't show this--let the filters decide during display;
	file.Write( ((CSlurp*)this )->getClassName());
	file.Write( handle );
	file.Write( addr );
	file.Write( header );
	file.Write( displayString );
	file.Write( pageSize );
	file.Write( lastPage );
	file.Write( lastBlock );
	file.Write( transactions.getCount() );
	for (int i=0;i<transactions.getCount();i++)
		transactions[i].writeToFile(file);

	return TRUE;
}
// EXISTING_CODE
