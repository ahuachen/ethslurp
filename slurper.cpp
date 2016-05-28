/*--------------------------------------------------------------------------------
 * Copyright 2016 - Great Hill Corporation.
 --------------------------------------------------------------------------------*/
#include "manage.h"
#include "ethslurp.h"

//---------------------------------------------------------------------------------------------------
#define binaryFileName (PATH_TO_SLURPS+theSlurp.addr+".bin")

//---------------------------------------------------------------------------------------------------
SFBool CSlurperApp::Display(SFString& message)
{
	double start = vrNow();
	theSlurp.Format_base(outScreen, getFormatString("file"));
	if (!isTesting)
	{
		double stop = vrNow();
		double timeSpent = stop-start;
		fprintf(stderr, "Displayed %ld records in %f seconds\n", theSlurp.transactions.getCount(), timeSpent);
		fflush(stderr);
	}
	return TRUE;
}

//--------------------------------------------------------------------------------
SFBool CSlurperApp::Slurp(SFString& message)
{
	double start = vrNow();
	theSlurp.transactions.Clear();

	// Do we have a binary file cached?
	if (SFos::fileExists(binaryFileName))
	{
		// Once a transaction is on the block chain, it will never change
		// therefore, we can store them in a binary cache
		CSharedResource file;
		if (!file.Lock(binaryFileName, binaryReadOnly, LOCK_NOWAIT))
		{
			message = "Could not open file: '" + binaryFileName + "'\n";
			return FALSE;
		}

		theSlurp.readFromFile(file);
		file.Close();
	}

	SFTime now = Now();
	SFTime fileTime = SFos::fileLastModifyDate(binaryFileName);

	// If the user tells us he/she wants to update the cache, or the cache
	// hasn't been updated in five minutes, then update it
	if (opt.slurp || (now - fileTime) > SFTimeSpan(0,5,0,0))
	{
		// This is how many records we currently have
		SFInt32 origCount = theSlurp.transactions.getCount();
		SFInt32 nextRecord = origCount;
		
		outErr << "Slurping new transactions from block chain...\n";

		SFString contents;
		SFInt32  nRequests = 0, nRead = 0;

		// We already have 'page' pages, so start there.
		SFInt32  page = MAX(theSlurp.lastPage,1);

		// Keep reading until we get less than a full page
		SFBool done = FALSE;
		while (!done)
		{
			SFString url = SFString("https://api.etherscan.io/api?module=account&action=txlist&sort=asc") +
										"&address=" + opt.addr +
										"&page="    + asString(page) +
										"&offset="  + asString(opt.pageSize) +
										"&apikey="  + api_key;

			// Grab a page of data
			SFString thisPage = urlToString(url);

			// See if it's good data, if not, bail
			message = nextTokenClear(thisPage, '[');
			if (!message.Contains("{\"status\":\"1\",\"message\":\"OK\""))
			{
				if (message.Contains("{\"status\":\"0\",\"message\":\"No transactions found\",\"result\":"))
					message = "No transactions were found for address '" + opt.addr + "'. Is it correct?";
				return RETURN_FAIL;
			}

			contents += thisPage;

			SFInt32 nRecords = countOf('}',thisPage)-1;
			nRead += nRecords;
			outErr << "\tDownloaded " << nRead << " potentially new transactions." << (isTesting?"\n":"\r");

			// If we got a full page, there are more to come
			done = (nRecords < opt.pageSize);
			if (!done)
				page++;

			// Etherscan.io doesn't want more than five pages per second, so sleep for a second
			if (++nRequests==4)
			{
				SFos::sleep(1.0);
				nRequests=0;
			}

			// Make sure we don't spin forever
			if (nRead >= opt.maxTransactions)
				done=TRUE;
		}
		if (contents.endsWith("}]}"))
			contents.SetAt(contents.GetLength()-2,'\0');
		stringToAsciiFile(binaryFileName.Substitute(".bin",".json"), contents);
		SFInt32 minBlock=0,maxBlock=0;
		findBlockRange(contents, minBlock, maxBlock);
		outErr << "\nDownload contains blocks from " << minBlock << " to " << maxBlock << "\n";
		
		// Keep track of which last full page we've read
		theSlurp.lastPage = page;
		theSlurp.pageSize = opt.pageSize;

		outErr << "Searching transactions";
		if (theSlurp.lastBlock>0)
			outErr << " after block " << theSlurp.lastBlock;
		outErr << "\n";

		SFInt32 lastBlock=0;
		while (!contents.IsEmpty())
		{
			CTransaction trans;

			SFString oneTransaction = nextTokenClear(contents, '}');
			if (trans.parseJson(oneTransaction))
			{
				static SFInt32 cnt=0;
				SFInt32 transBlock = atoi((const char*)trans.blockNumber);
				if (transBlock > theSlurp.lastBlock) // add the new transaction if it's in a new block
				{
					theSlurp.transactions[nextRecord++] = trans;
					lastBlock = transBlock;
				}
				if (lastBlock)
					if (!((cnt+1)%5)) { outErr << "Found new transaction at block " << lastBlock << ". Importing..." << (isTesting?"\n":"\r"); outErr.Flush(); }
				cnt++;
			}
		}
		if (lastBlock)
			outErr << "\n";
		theSlurp.lastBlock = lastBlock;
		
		// Write the data if we got new data
		SFInt32 newRecords = (theSlurp.transactions.getCount() - origCount);
		if (newRecords)
		{
			outErr << "Writing " << newRecords << " new records to cache\n";
			CSharedResource file;
			if (file.Lock(binaryFileName, binaryWriteCreate, LOCK_CREATE))
			{
				theSlurp.writeToFile(file);
				file.Close();
				
			} else
			{
				message = "Could not open file: '" + binaryFileName + "'\n";
				return FALSE;
			}
		}
	}
	
	// Apply filters if any (order matters) and find the last known block
	for (int i=0;i<theSlurp.transactions.getCount();i++)
	{
		if (opt.incomeOnly && theSlurp.transactions[i].to != opt.addr)
		{
			if (verbose)
				outErr << theSlurp.transactions[i].Format("skipping expenditure [{HASH}]\n");
			theSlurp.transactions[i].setShowing(FALSE);
			
		} else if (opt.expenseOnly && theSlurp.transactions[i].from != opt.addr)
		{
			if (verbose)
				outErr << theSlurp.transactions[i].Format("skipping inflow [{HASH}]\n");
			theSlurp.transactions[i].setShowing(FALSE);
		}
	}

	if (!isTesting)
	{
		double stop = vrNow();
		double timeSpent = stop-start;
		fprintf(stderr, "Loaded %ld total records in %f seconds\n", theSlurp.transactions.getCount(), timeSpent);
		fflush(stderr);
	}
	
	return (theSlurp.transactions.getCount()>0);
}

//---------------------------------------------------------------------------------------------------
const CFieldData *findField(const SFString& fieldName)
{
	const CFieldList *fields = GETRUNTIME_CLASS(CTransaction)->m_FieldList;
	LISTPOS lPos = fields->GetFirstItem();
	while (lPos)
	{
		const CFieldData *field = fields->GetNextItem(lPos);
		if (fieldName == field->getFieldName())
			return field;
	}
	return NULL;
}

//---------------------------------------------------------------------------------------------------
void CSlurperApp::buildDisplayStrings(void)
{
	// Set the default if it's not set
	if (opt.exportFormat.IsEmpty())
		opt.exportFormat = "json";
	
	const SFString fmtForFields  = getFormatString("field");  ASSERT(!fmtForFields.IsEmpty());
	
	// The user may have customized the field list, so get look config first
	SFString fieldList = config.GetProfileStringGH("DISPLAY_STR", "fmt_fieldList", EMPTY);
	if (fieldList.IsEmpty())
		fieldList = GETRUNTIME_CLASS(CTransaction)->listOfFields();

	while (!fieldList.IsEmpty())
	{
		SFString fieldName = nextTokenClear(fieldList, '|');
		const CFieldData *field = findField(fieldName);
		if (!field)
		{
			outErr << "Field '" << fieldName << "' not found. Quitting...\n";
			exit(0);
		}
		
		if (!isInternal(fieldName))
		{
			theSlurp.displayString += fmtForFields.Substitute("{FIELD}", "{" + toUpper(fieldName)+"}").Substitute("{p:FIELD}", "{p:"+fieldName+"}");
			theSlurp.header        += fmtForFields.Substitute("{FIELD}", fieldName).Substitute("[",EMPTY).Substitute("]",EMPTY).Substitute("<td ","<th ");
		}
	}

	// This is what we're really after
	const SFString fmtForRecords = getFormatString("record"); ASSERT(!fmtForRecords.IsEmpty());
	theSlurp.displayString = Strip(fmtForRecords.Substitute("[{FIELDS}]", theSlurp.displayString), '\t');
	if (opt.exportFormat=="json")
	{
		// one little hack to make raw json more readable
		theSlurp.displayString.ReplaceReverse("}]\",","}]\"\n");
		if (opt.prettyPrint)
		{
			theSlurp.displayString.ReplaceAll("\"[{p:", "            \"[{p:");
			theSlurp.displayString.ReplaceAll("}]\",",  "}]\",\n");
			theSlurp.displayString.ReplaceAll("\":\"", "\": \"");
		}
	}
}

//--------------------------------------------------------------------------------
SFString CSlurperApp::getFormatString(const SFString& which)
{
	if (which == "file")
		buildDisplayStrings();

	SFString errMsg;
	
	SFString formatName = "fmt_" + opt.exportFormat + "_" + which;
	SFString ret = config.GetProfileStringGH("DISPLAY_STR", formatName, EMPTY);
	if (ret.Contains("file:"))
	{
		SFString file = ret.Substitute("file:",EMPTY);
		if (!SFos::fileExists(file))
			errMsg = SFString("Formatting file '") + file + "' for display string '" + formatName + "' not found. Quiting...\n";
		else
			ret = asciiFileToString(file);
		
	} else if (ret.Contains("fmt_")) // it's referring to another format string...
	{
		SFString newName = ret;
		ret = config.GetProfileStringGH("DISPLAY_STR", newName, EMPTY);
		formatName += ":"+newName;
	}
	ret = ret.Substitute("\\n","\n").Substitute("\\t","\t");
	
	// some sanity checks
	if (countOf('{',ret) != countOf('}',ret) ||
		countOf('[',ret) != countOf(']',ret))
	{
		errMsg = SFString("Mismatched brackets in display string '") + formatName + "': '" + ret + "'. Quiting...\n";

	} else if (ret.IsEmpty())
	{
		errMsg = SFString("Empty display string '") + formatName + "'. Quiting...\n";
	}

	if (!errMsg.IsEmpty())
	{
		outErr << errMsg;
		exit(0);
	}

	return ret;
}

//--------------------------------------------------------------------------------
void findBlockRange(const SFString& contents, SFInt32& minBlock, SFInt32& maxBlock)
{
	SFString search = "\"blockNumber\":\"";
	SFInt32  len = search.GetLength();

	minBlock = 0;
	SFInt32 first = contents.Find(search);
	if (first!=-1)
	{
		SFString str = contents.Mid(first+len,100);
		minBlock = atoi((const char*)str);
//		outScreen << first << " : " << str << " : " << minBlock << "\n";
	}

	SFString end = contents.Mid(contents.ReverseFind('{'),10000);//pull off the last transaction
	SFInt32 last = end.Find(search);
	if (last!=-1)
	{
		SFString str = end.Mid(last+len,100);
		maxBlock = atoi((const char*)str);
//		outScreen << last << " : " << str << " : " << maxBlock << "\n";
	}
}
