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

// EXISTING_CODE
// EXISTING_CODE

//---------------------------------------------------------------------------
void finishParse(CTransaction *transaction)
{
	// EXISTING_CODE
	// EXISTING_CODE
}

//---------------------------------------------------------------------------
SFString nextTransactionChunk_custom(const SFString& fieldIn, SFBool& force, const void *data)
{
	CTransactionNotify *tr = (CTransactionNotify*)data;
	const CTransaction *tra = tr->getDataPtr();
	switch (tolower(fieldIn[0]))
	{
		// EXISTING_CODE
		case 'd':
			if ( fieldIn % "date" )
			{
				time_t utc = tra->timeStamp;
				struct tm *tm = gmtime(&utc);
				char retStr[40];
				strftime(retStr, sizeof(retStr), "%Y-%m-%d %H:%M:%S UTC", tm);
				return retStr;
			}
			break;
		case 'e':
			if ( fieldIn % "ether" )
			{
				// Make sure the wei number is at least 18 characters long. Once it is,
				// reverse it, put a decimal at the 18th position, reverse it back,
				// strip leading '0's except the tens digit.
				SFString ether = tra->value;
				if (ether.GetLength()<18)
					ether = padLeft(tra->value,18).Substitute(" ","0");
				ether.Reverse();
				ether = ether.Left(18) + "." + ether.Mid(18,1000);
				ether.Reverse();
				ether = StripLeading(ether, '0');
				if (ether.startsWith('.'))
					ether = "0" + ether;
				return ether;
			}
			break;
		case 'f':
			if ( fieldIn % "function" ) return tra->inputToFunction();
			break;
		case 'h':
			if ( fieldIn % "hitLimit" ) return ((tra->gas == tra->gasUsed) ? "TRUE" : "FALSE");
			break;
		case 'i':
			if ( fieldIn % "inputLen" ) return asString(tra->input.GetLength());
			break;
		// EXISTING_CODE
		default:
			break;
	}
	
#pragma unused(tr)
#pragma unused(tra)

	return EMPTY;
}

//---------------------------------------------------------------------------
SFBool CTransaction::handleCustomFormat(CExportContext& ctx, const SFString& fmtIn, void *data) const
{
	// EXISTING_CODE
	// EXISTING_CODE
	return FALSE;
}

// EXISTING_CODE
//---------------------------------------------------------------------------
SFInt32 CTransaction::readFromFile(CSharedResource& file)
{
	SFInt32 val=0; SFString str;
	file.Read( val ); setDeleted(val);
	file.Read( val ); setSchema(val);
	file.Read( val ); setShowing(val);
	file.Read( str ); ASSERT(str == ((CTransaction*)this)->getClassName() );
	file.Read( blockHash );
	file.Read( blockNumber );
	file.Read( confirmations );
	file.Read( contractAddress );
	file.Read( cumulativeGasUsed );
	file.Read( from );
	file.Read( gas );
	file.Read( gasPrice );
	file.Read( gasUsed );
	file.Read( hash );
	file.Read( input );
	file.Read( nonce );
	file.Read( timeStamp );
	file.Read( to );
	file.Read( transactionIndex );
	file.Read( value );
	return TRUE;
}

//---------------------------------------------------------------------------
SFInt32 CTransaction::writeToFile(CSharedResource& file) const
{
	file.Write( isDeleted() );
	file.Write( getSchema() );
	file.Write( isShowing() );
	file.Write( ((CTransaction*)this)->getClassName() );
	file.Write( blockHash );
	file.Write( blockNumber );
	file.Write( confirmations );
	file.Write( contractAddress );
	file.Write( cumulativeGasUsed );
	file.Write( from );
	file.Write( gas );
	file.Write( gasPrice );
	file.Write( gasUsed );
	file.Write( hash );
	file.Write( input );
	file.Write( nonce );
	file.Write( timeStamp );
	file.Write( to );
	file.Write( transactionIndex );
	file.Write( value );

	return TRUE;
}

//---------------------------------------------------------------------------
SFTime CTransaction::getDate(void) const
{
	return snagDate(Format("[{DATE}]").Substitute("-","").Substitute(":", "").Substitute(" ", ""));
}

#include "slurp.h"
//---------------------------------------------------------------------------
SFString CTransaction::inputToFunction(void) const
{
	for (int i=0;i<nFunctions;i++)
		if (funcTable[i].func)
			if (input.Mid(2,8) == funcTable[i].func->encoding)
				return funcTable[i].name;
	return "";
}
// EXISTING_CODE
