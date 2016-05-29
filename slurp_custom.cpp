/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2015 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "manage.h"
#include "slurp.h"

// EXISTING_CODE
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
		if (!(++nRead%5)) { outErr << "Reading from binary cache...record " << nRead << " of " << nRecords << (isTesting?"\n":"\r"); outErr.Flush(); }
		SFInt32 curBlock = atoi((const char*)transactions[i].blockNumber);
		maxBlock = MAX(maxBlock,curBlock);
	}
	outErr << "\n";
	if (maxBlock != lastBlock)
	{ outErr << "Previously stored lastBlock '" << lastBlock << "' not equal to maxBlock '" << maxBlock << "'\n"; }
	
	return TRUE;
}

//---------------------------------------------------------------------------
SFInt32 CSlurp::writeToFile(CSharedResource& file) const
{
	file.Write( isDeleted() );
	file.Write( getSchema() );
	file.Write( TRUE ); // don't show this--let the filters decide );
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
