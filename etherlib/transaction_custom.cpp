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
SFTime CTransaction::getDate(void) const
{
	return snagDate(Format("[{DATE}]").Substitute("-","").Substitute(":", "").Substitute(" ", ""));
}

#include "slurp.h"
SFBool CTransaction::isFunction(const SFString& func) const
{
	if (func.IsEmpty())
		return FALSE;
	
	switch (func[0])
	{
		case 'a':
			if ( func == "approve" ) return input.startsWith( "0x095ea7b3" );
			else return FALSE;
			break;
			
		case 'c':
			if ( func == "checkProposalCode" ) return input.startsWith( "0xeceb2945" );
			else if ( func == "createTokenProxy" ) return input.startsWith( "0xbaac5300" );
			else return FALSE;
			break;
			
		case 'e':
			if ( func == "executeProposal" ) return input.startsWith( "0x237e9492" );
			else return FALSE;
			break;
			
		case 'g':
			if ( func == "getMyReward" ) return input.startsWith( "0xcc9ae3f6" );
			else return FALSE;
			break;
			
		case 'h':
			if ( func == "halveMinQuorum" ) return input.startsWith( "0x78524b2e" );
			else return FALSE;
			break;
			
		case 'n':
			if ( func == "newProposal" ) return input.startsWith( "0x612e45a3" );
			else if ( func == "none" )
			{
				SFString ret = inputToFunction();
				if (ret.ContainsAny("acghrstuv"))
					return FALSE;
				return (ret == " ");
			}
			else return FALSE;
			break;
			
		case 'r':
			if ( func == "receiveEther" ) return input.startsWith( "0xa3912ec8" );
			else if ( func == "refund" ) return input.startsWith( "0x590e1ae3" );
			else if ( func == "retrieveDAOReward" ) return input.startsWith( "0xa1da2fb9" );
			else return FALSE;
			break;
			
		case 's':
			if ( func == "splitDAO" ) return input.startsWith( "0x82661dc4" );
			else return FALSE;
			break;
			
		case 't':
			if ( func == "transfer" ) return input.startsWith( "0xa9059cbb" );
			else if ( func == "transferFrom" ) return input.startsWith( "0x23b872dd" );
			else if ( func == "transferFromWithoutReward" ) return input.startsWith( "0xdbde1988" );
			else if ( func == "transferWithoutReward" ) return input.startsWith( "0x4e10c3ee" );
			else return FALSE;
			break;
			
		case 'u':
			if ( func == "unblockMe" ) return input.startsWith( "0x2632bf20" );
			else return FALSE;
			break;
			
		case 'v':
			if ( func == "vote" ) return input.startsWith( "0xc9d27afe" );
			else return FALSE;
			break;
	}
	
	return FALSE;
}

//---------------------------------------------------------------------------
SFString parseParams(const SFString& which, const SFString& params)
{
	if (which=="vote")
	{
		//function vote(uint _proposalID, bool _supportsProposal)
		SFString part1 = "0x"+Strip(params.Left(64), '0');
		SFString part2 = Strip(params.Right(64),'0');
		return which + "|" + padNum3(strtoul((const char*)part1, NULL, 16)) + "|" + (part2.IsEmpty()?"Nay":"Yea");
	
	} else if (which=="approve")
	{
		//function approve(address _spender, uint256 _amount) returns (bool success) {  discuss
		SFString part1 = "0x"+Strip(params.Left(64), '0');
		SFString part2 = Strip(params.Right(64),'0');
		return which + "|" + part1 + "|" + asString(strtoul((const char*)part2,NULL,16));

	} else if (which=="newProposal")
	{
		//function newProposal(address _recipient, uint _amount, string _description, bytes _transactionData, uint _debatingPeriod, bool _newCurator)
		SFString part1 = "0x"+Strip(params.Left(64), '0');
		SFString part2 = Strip(params.Mid(64,64),'0');
		SFString part4 = Strip(params.Right(128).Left(64), '0');
		SFString part5 = Strip(params.Right(64),'0');
		SFString part3 = params;
		part3.Replace(part1,"");
		part3.Replace(part2,"");
		part3.ReplaceReverse(part5,"");
		part3.ReplaceReverse(part4,"");
		part3 = Strip(part3,'0');
		return which + "|" + part1 + "|" + part2 + "|" + part3 + "|" + part4 + "|" + part5;

	} else if (which=="splitDAO")
	{
		//function splitDAO(uint _proposalID, address _newCurator)
		SFString part1 = "0x"+Strip(params.Left(64), '0');
		SFString part2 = Strip(params.Right(64),'0');
		return which + "|" + asString(strtoul((const char*)part1, NULL, 16)) + "|" + part2;

	} else if (which=="createTokenProxy")
	{
		return which + "|" + params;

	} else if (which=="transferFrom")
	{
		//function transferFrom(address _from, address _to, uint256 _amount)
		SFString part1 = "0x"+Strip(params.Left(64), '0');
		SFString part2 = "0x"+Strip(params.Mid(64,64),'0');
		SFString part3 = Strip(params.Right(64),'0');
		return which + "|" + part1 + "|" + part2 + "|" + asString(strtoul((const char*)part3,NULL,16));

	} else if (which=="transfer")
	{
		//function transfer(address _to, uint256 _amount)
		SFString part1 = "0x"+Strip(params.Left(64), '0');
		SFString part2 = Strip(params.Right(64),'0');
		return which + "|" + part1 + "|" + asString(strtoul((const char*)part2,NULL,16));

	}

	return which;
}

//---------------------------------------------------------------------------
SFString CTransaction::inputToFunction(void) const
{
	if (input.GetLength()<10)
		return " ";

	switch (input[2])
	{
		case '0': if (input.startsWith( "0x095ea7b3" )) return parseParams("approve",                   input.Mid(10,input.GetLength())); break;

		case '2': if (input.startsWith( "0x237e9492" )) return parseParams("executeProposal",           input.Mid(10,input.GetLength()));
		     else if (input.startsWith( "0x23b872dd" )) return parseParams("transferFrom",              input.Mid(10,input.GetLength()));
		     else if (input.startsWith( "0x2632bf20" )) return parseParams("unblockMe",                 input.Mid(10,input.GetLength())); break;

		case '4': if (input.startsWith( "0x4e10c3ee" )) return parseParams("transferWithoutReward",     input.Mid(10,input.GetLength())); break;

		case '5': if (input.startsWith( "0x590e1ae3" )) return parseParams("refund",                    input.Mid(10,input.GetLength())); break;

		case '6': if (input.startsWith( "0x612e45a3" )) return parseParams("newProposal",               input.Mid(10,input.GetLength())); break;

		case '7': if (input.startsWith( "0x78524b2e" )) return parseParams("halveMinQuorum",            input.Mid(10,input.GetLength())); break;

		case '8': if (input.startsWith( "0x82661dc4" )) return parseParams("splitDAO",                  input.Mid(10,input.GetLength())); break;

		case 'a': if (input.startsWith( "0xa1da2fb9" )) return parseParams("retrieveDAOReward",         input.Mid(10,input.GetLength()));
		     else if (input.startsWith( "0xa3912ec8" )) return parseParams("receiveEther",              input.Mid(10,input.GetLength()));
		     else if (input.startsWith( "0xa9059cbb" )) return parseParams("transfer",                  input.Mid(10,input.GetLength())); break;

		case 'b': if (input.startsWith( "0xbaac5300" )) return parseParams("createTokenProxy",          input.Mid(10,input.GetLength())); break;

		case 'c': if (input.startsWith( "0xc9d27afe" )) return parseParams("vote",                      input.Mid(10,input.GetLength()));
		     else if (input.startsWith( "0xcc9ae3f6" )) return parseParams("getMyReward",               input.Mid(10,input.GetLength())); break;

		case 'd': if (input.startsWith( "0xdbde1988" )) return parseParams("transferFromWithoutReward", input.Mid(10,input.GetLength())); break;

		case 'e': if (input.startsWith( "0xeceb2945" )) return parseParams("checkProposalCode",         input.Mid(10,input.GetLength())); break;
	}

	for (int i=0;i<nFunctions;i++)
		if (input.Mid(2,8) == funcTable[i].encoding)
			return funcTable[i].Format("[{NAME}]");

	return " ";
}

//---------------------------------------------------------------------------
int sortTransactionsForWrite(const void *rr1, const void *rr2)
{
	CTransaction *tr1 = (CTransaction*)rr1;
	CTransaction *tr2 = (CTransaction*)rr2;

	SFInt32 ret;
	ret = tr1->timeStamp - tr2->timeStamp;         if (ret!=0) return (int)ret;
	ret = tr1->from.Compare(tr2->from);            if (ret!=0) return (int)ret;
	ret = toLong(tr1->nonce) - toLong(tr2->nonce); if (ret!=0) return (int)ret;

	return (int)tr1->hash.Compare(tr2->hash);
}
// EXISTING_CODE
