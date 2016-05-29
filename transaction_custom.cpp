/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2015 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

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
				time_t utc = strtol((const char*)tra->timeStamp, NULL, 10);
				struct tm *tm = gmtime(&utc);
				char retStr[40];
				strftime(retStr, sizeof(retStr), "%Y-%m-%d %H:%M:%S UTC", tm);
				return retStr;
			}
			break;
		case 'e':
			if ( fieldIn % "ether" )
			{
				// because wei is a string representing 1/(10^18) ether, and because
				// we want five decimal points, we simply remove 12 digits from the end.
				// if the thirteeth is '5' or greater we should round, but we truncate
				SFString wei = tra->value;
				wei.Reverse();
				SFString remove = wei.Left(13);
				wei.Replace(remove,EMPTY);
				wei.Reverse();
				SFString ether = wei;
				ether = SFString(ether.GetAt(0)) + "." + ether.Mid(1,1000);
				if (ether.IsEmpty())
					ether = "0.00000";
				ether = padRight(ether,5).Substitute(" ","0");
				return ether;
			}
			break;
		// EXISTING_CODE
		default:
			break;
	}
	
#pragma unused(tr)
#pragma unused(tra)

	return EMPTY;
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
	file.Write( TRUE ); // don't store this--let the filters decide );
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
SFInt32 CTransaction::parseJson(SFString& strIn)
{
#define SEARCHING     0
#define IN_FIELDNAME  1
#define IN_FIELDVALUE 2

	SFInt32 nFields=0;

	SFInt32 state=SEARCHING;
	char *s = (char*)(const char*)strIn;
	char *fieldName=NULL;
	char *fieldVal=NULL;
	while (*s)
	{
		switch (state)
		{
		case SEARCHING:
			if (*s == '\"' && !fieldName)
			{
				fieldName = s+1;
				state=IN_FIELDNAME;
			} else if (*s == '\"')
			{
				fieldVal = s+1;
				state=IN_FIELDVALUE;
			}
			break;
		case IN_FIELDNAME:
			if (*s == '\"')
			{
				*s = '\0';
				state=SEARCHING;
			}
			break;
		case IN_FIELDVALUE:
			if (*s == '\"')
			{
				*s = '\0';
				setValueByName(fieldName, fieldVal);
				fieldName = fieldVal = NULL;
				state=SEARCHING;
				nFields++;
			}
			break;
		}
		s++;
	}
	return nFields;
}
// EXISTING_CODE
