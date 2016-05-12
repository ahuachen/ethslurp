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
		// EXISTING_CODE
		default:
			break;
	}
	
#pragma unused(tr)
#pragma unused(tra)

	return EMPTY;
}

// EXISTING_CODE
void CTransaction::parseJson(SFString& strIn)
{
	strIn.ReplaceAll("\"",EMPTY);
	if (strIn.Contains("[")) nextTokenClear(strIn,'[');//skip to beginning
	nextTokenClear(strIn,'{');//skip to beginning
	while (!strIn.IsEmpty())
	{
		SFString value = nextTokenClear(strIn, ',');
		SFString field = nextTokenClear(value, ':');
		setValueByName(field,value);
	}
}
// EXISTING_CODE
