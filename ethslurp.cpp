/*--------------------------------------------------------------------------------
 * Copyright 2016 - Great Hill Corporation.
 --------------------------------------------------------------------------------*/
#include "ethslurp.h"

/* See options.cpp for a list of command line options */

//--------------------------------------------------------------------------------
int main(int argc, const char * argv[])
{
	// We handle a couple of the options early (verbose, testing, --file)
	// because the later code depends on them
	SFString cmdFile=EMPTY;
	for (int i=0;i<argc;i++)
	{
		SFString arg = argv[i];
		if (arg.startsWith("--file:"))
		{
			cmdFile = arg.Substitute("--file:",EMPTY);
			cmdFile.Replace("~/",getHomeFolder());
			if (!SFos::fileExists(cmdFile))
				return usage("--file: '" + cmdFile + "' not found. Quitting.");

		} else if (arg == "-h" || arg == "-help" || arg == "--help")
		{
			return usage();
		
		} else if (arg.startsWith("-v") || arg.startsWith("-verbose"))
		{
			verbose = TRUE;
			arg.Replace("-verbose",EMPTY);
			arg.Replace("-v",      EMPTY);
			arg.Replace(":",       EMPTY);
			if (!arg.IsEmpty())
				verbose = toLong(arg);
		
		} else if (arg=="-t" || arg=="-test" || arg=="-titles")
		{
			// During testing, send all output to the screen so it can be re-directed
			outErr = outScreen;
			isTesting = TRUE;
		}
	}

	// If we have a command file, we will use it, if not we will create
	// it and pretend it was always there. Makes following code easier.
	SFString commandList;
	if (cmdFile.IsEmpty())
	{
		for (int i=1;i<argc;i++) // we know the program's name
			commandList += (SFString(argv[i]) + " ");
		commandList += '\n';

	} else
	{
		commandList = asciiFileToString(cmdFile).Substitute("\t", " ").Substitute("  ", " ");
	}

	// We want to keep only one slurper so, if the user is using the --file option, and
	// they are reading repeatedly from the same contract, we only have to read the binary
	// file once. Note that it may get updated, but the whole thing will only be read once.
	CSlurperApp slurper;

	// Now we parse and process each command. In the case of --file there may be many. In the case of
	// a straight forward command line call there is only one.
	while (!commandList.IsEmpty())
	{
		SFString command = nextTokenClear(commandList, '\n');
		if (!command.startsWith(";"))
		{
			SFInt32 nArgs=0;
			SFString args[20];

			outErr << "Processing: " << command << "\n";
			while (!command.IsEmpty())
			{
				SFString arg = nextTokenClear(command, ' ');
				while (!arg.IsEmpty())
					args[nArgs++] = expandOption(arg);
			}

			COptions options;
			// Parse the command line
			if (options.parseArguments(nArgs, args) != RETURN_OK)
				return RETURN_FAIL;

			if (slurper.Init(options) != RETURN_OK)
				return RETURN_FAIL;
			// Slurp...
			SFString message;
			if (!slurper.Slurp(options, message))
				return usage(message);
			
			// Display...
			if (!slurper.Display(options, message))
				return usage(message);
		}
	}
	return RETURN_OK;
}

//---------------------------------------------------------------------------------------------------
int CSlurperApp::Init(COptions& options)
{
	CSlurp::registerClass();
	CTransaction::registerClass();

	version.setVersion(FILE_MAJOR_VERSION, FILE_MINOR_VERSION, FILE_BUILD_VERSION);
	config.setFilename(PATH_TO_ETH_SLURP+"config.dat");

	// If this is the first time we've ever run, build the config file
	if (!establish_folders(config, version.toString()))
		return usage("Unable to create data folders at " + PATH_TO_SLURPS);

	// The web APIs require an API key. You will have to get one of these yourself. The
	// program will ask for an api key until it gets one. You only need to provide it once.
	api_key      = config.GetProfileStringGH("SETTINGS", "api_key",      EMPTY);
	api_provider = config.GetProfileStringGH("SETTINGS", "api_provider", "EtherScan");
	api_url      = config.GetProfileStringGH("SETTINGS", "api_url",      "http://etherscan.io/apis");
	while (api_key.IsEmpty())
	{
		cerr
			<< conRed << "\n  ***Warning***" << conOff << "\n"
			<< "  " << conYellow << "ethslurp" << conOff << " needs an api_key from " + api_provider + " in order to work. You may get one at\n"
			<< "  " + api_url + ". See our online help file for more information.\n"
			<< "  Please provide an API key or type 'exit'\n"
			<< "  > ";
		cerr.flush();
		char buffer[256];
		cin >> buffer;
		api_key = buffer;
		if (api_key % "exit" || api_key % "quit")
			exit(0);
		
		// save the API key for later
		config.SetProfileString("SETTINGS", "api_key",      api_key);
		config.SetProfileString("SETTINGS", "api_provider", "EtherScan");
		config.SetProfileString("SETTINGS", "api_url",      "http://etherscan.io/apis");
		config.writeFile(version.toString());
	}
	
	// If we are told to get the address from the rerun address, do so...
	if (options.addr.IsEmpty() && options.rerun)
		options.addr = config.GetProfileStringGH("SETTINGS", "rerun", EMPTY);
	
	// Ethereum addresses are case insensative. Force all address to lower case to avoid mismatches.
	options.addr.MakeLower();
	
	// We can't run without an address...
	if (options.addr.IsEmpty())
	{
		SFString msg = "You must supply an Ethereum account or contract address. ";
		if (!options.rerun)
			msg += "Use -r flag to rerun the most recently slurped address.";
		return usage(msg);
	}
	
	// Save the address for the next run if needed
	config.SetProfileString("SETTINGS", "rerun", options.addr);
	config.writeFile(version.toString());

	// Load per address configurations
	if (SFos::fileExists(PATH_TO_ETH_SLURP+options.addr+".dat"))
	{
		CConfig perAddr(NULL);
		perAddr.loadFile(PATH_TO_ETH_SLURP+options.addr+".dat");
		config.mergeConfig(&perAddr);
	}

	// Ready to slurp
    if (theSlurp.addr == options.addr)
		options.rerun = TRUE;
	if (!options.rerun)
		theSlurp.transactions.Clear();
	theSlurp.addr = options.addr;
	outErr << "\tSlurping " << theSlurp.addr << "\n";

	// Finished
	return RETURN_OK;
}

//---------------------------------------------------------------------------------------------------
#define binaryFileName (PATH_TO_SLURPS+theSlurp.addr+".bin")

//--------------------------------------------------------------------------------
SFBool CSlurperApp::Slurp(COptions& options, SFString& message)
{
	double start = vrNow();
	
	// Do we have a binary file cached?
	SFBool needToRead = SFos::fileExists(binaryFileName);
	if (options.rerun && theSlurp.transactions.getCount())
		needToRead=FALSE;
	if (needToRead)
	{
		// Once a transaction is on the blockchain, it will never change
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
	SFInt32 nSeconds = MAX(60,config.GetProfileIntGH("SETTINGS", "update_freq", 300));
	if (options.slurp || (now - fileTime) > SFTimeSpan(0,0,0,nSeconds))
	{
		// This is how many records we currently have
		SFInt32 origCount = theSlurp.transactions.getCount();
		SFInt32 nextRecord = origCount;
		
		outErr << "\tSlurping new transactions from blockchain...\n";
		
		SFString contents;
		SFInt32  nRequests = 0, nRead = 0;
		
		// We already have 'page' pages, so start there.
		SFInt32  page = MAX(theSlurp.lastPage,1);
		
		// Keep reading until we get less than a full page
		SFBool done = FALSE;
		while (!done)
		{
			SFString url = SFString("https://api.etherscan.io/api?module=account&action=txlist&sort=asc") +
			"&address=" + options.addr +
			"&page="    + asString(page) +
			"&offset="  + asString(options.pageSize) +
			"&apikey="  + api_key;
			
			// Grab a page of data
			SFString thisPage = urlToString(url);
			
			// See if it's good data, if not, bail
			message = nextTokenClear(thisPage, '[');
			if (!message.Contains("{\"status\":\"1\",\"message\":\"OK\""))
			{
				if (message.Contains("{\"status\":\"0\",\"message\":\"No transactions found\",\"result\":"))
					message = "No transactions were found for address '" + options.addr + "'. Is it correct?";
				return RETURN_FAIL;
			}
			
			contents += thisPage;
			
			SFInt32 nRecords = countOf('}',thisPage)-1;
			nRead += nRecords;
			outErr << "\tDownloaded " << nRead << " potentially new transactions." << (isTesting?"\n":"\r");
			
			// If we got a full page, there are more to come
			done = (nRecords < options.pageSize);
			if (!done)
				page++;
			
			// Etherscan.io doesn't want more than five pages per second, so sleep for a second
			if (++nRequests==4)
			{
				SFos::sleep(1.0);
				nRequests=0;
			}
			
			// Make sure we don't spin forever
			if (nRead >= options.maxTransactions)
				done=TRUE;
		}
		if (contents.endsWith("}]}"))
			contents.SetAt(contents.GetLength()-2,'\0');

		SFInt32 minBlock=0,maxBlock=0;
		findBlockRange(contents, minBlock, maxBlock);
		outErr << "\n\tDownload contains blocks from " << minBlock << " to " << maxBlock << "\n";
		
		// Keep track of which last full page we've read
		theSlurp.lastPage = page;
		theSlurp.pageSize = options.pageSize;
		
		outErr << "\tSearching transactions";
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
				SFInt32 transBlock = trans.blockNumber;
				if (transBlock > theSlurp.lastBlock) // add the new transaction if it's in a new block
				{
					theSlurp.transactions[nextRecord++] = trans;
					lastBlock = transBlock;
				}
				if (lastBlock)
					if (!((cnt+1)%5)) { outErr << "\tFound new transaction at block " << lastBlock << ". Importing..." << (isTesting?"\n":"\r"); outErr.Flush(); }
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
			outErr << "\tWriting " << newRecords << " new records to cache\n";
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
	theSlurp.nVisible=0;
	for (int i=0;i<theSlurp.transactions.getCount();i++)
	{
		// reset every transaction since we're going to use them again possible if we're in --file mode
		theSlurp.transactions[i].setShowing(TRUE);
		if (options.firstDate != earliestDate || options.lastDate != latestDate)
		{
			SFTime date = theSlurp.transactions[i].getDate();
			SFBool isVisible = (date >= options.firstDate && date <= options.lastDate);
			theSlurp.transactions[i].setShowing(isVisible);
			
		} else if (options.firstBlock!=0||options.lastBlock!=LONG_MAX)
		{
			SFInt32 bN = theSlurp.transactions[i].blockNumber;
			SFBool isVisible = (bN >= options.firstBlock && bN <= options.lastBlock);
			theSlurp.transactions[i].setShowing(isVisible);
		}
		if (options.incomeOnly && theSlurp.transactions[i].to != options.addr)
		{
			if (verbose)
				outErr << theSlurp.transactions[i].Format("\tskipping expenditure [{HASH}]\n");
			theSlurp.transactions[i].setShowing(FALSE);
			
		} else if (options.expenseOnly && theSlurp.transactions[i].from != options.addr)
		{
			if (verbose)
				outErr << theSlurp.transactions[i].Format("\tskipping inflow [{HASH}]\n");
			theSlurp.transactions[i].setShowing(FALSE);
		}
		theSlurp.nVisible+=theSlurp.transactions[i].isShowing();
	}
	
	if (!isTesting)
	{
		double stop = vrNow();
		double timeSpent = stop-start;
		fprintf(stderr, "\tLoaded %ld total records (%ld visible) in %f seconds\n", theSlurp.transactions.getCount(), theSlurp.nVisible, timeSpent);
		fflush(stderr);
	}
	
	return (theSlurp.transactions.getCount()>0);
}

//---------------------------------------------------------------------------------------------------
SFBool CSlurperApp::Display(COptions& options, SFString& message)
{
	double start = vrNow();
	
	theSlurp.Format_base(outScreen, getFormatString(options, "file"));
	
	if (!isTesting)
	{
		double stop = vrNow();
		double timeSpent = stop-start;
		fprintf(stderr, "\tExported %ld records in %f seconds             \n\n", theSlurp.nVisible, timeSpent);
		fflush(stderr);
	}
	return TRUE;
}

//---------------------------------------------------------------------------------------------------
inline SFBool isInternal(const SFString& field)
{
	return field == "schema" || field == "deleted" || field == "handle" || (isTesting && field == "confirmations");
}

//---------------------------------------------------------------------------------------------------
void CSlurperApp::buildDisplayStrings(COptions& options)
{
	// Set the default if it's not set
	if (options.exportFormat.IsEmpty())
		options.exportFormat = "json";
	
	const SFString fmtForFields  = getFormatString(options, "field");  ASSERT(!fmtForFields.IsEmpty());
	
	// The user may have customized the field list, so get look config first
	SFString fieldList = config.GetProfileStringGH("DISPLAY_STR", "fmt_fieldList", EMPTY);
	if (fieldList.IsEmpty())
		fieldList = GETRUNTIME_CLASS(CTransaction)->listOfFields();
	
	theSlurp.displayString=theSlurp.header=EMPTY;
	while (!fieldList.IsEmpty())
	{
		SFString fieldName = nextTokenClear(fieldList, '|');
		const CFieldData *field = GETRUNTIME_CLASS(CTransaction)->FindField(fieldName);
		if (!field)
		{
			outErr << "Field '" << fieldName << "' not found. Quitting...\n";
			exit(0);
		}

		if (!field->isHidden())
		{
			SFString resolved = fieldName;
			if (options.exportFormat!="json")
				resolved = config.GetProfileStringGH("FIELD_STR", fieldName, fieldName);
			theSlurp.displayString += fmtForFields.Substitute("{FIELD}", "{" + toUpper(resolved)+"}").Substitute("{p:FIELD}", "{p:"+resolved+"}");
			theSlurp.header        += fmtForFields.Substitute("{FIELD}", resolved).Substitute("[",EMPTY).Substitute("]",EMPTY).Substitute("<td ","<th ");
		}
	}
	
	// This is what we're really after
	const SFString fmtForRecords = getFormatString(options, "record"); ASSERT(!fmtForRecords.IsEmpty());
	theSlurp.displayString = Strip(fmtForRecords.Substitute("[{FIELDS}]", theSlurp.displayString), '\t');
	if (options.exportFormat=="json")
	{
		// one little hack to make raw json more readable
		theSlurp.displayString.ReplaceReverse("}]\",","}]\"\n");
		if (options.prettyPrint)
		{
			theSlurp.displayString.ReplaceAll("\"[{p:", "            \"[{p:");
			theSlurp.displayString.ReplaceAll("}]\",",  "}]\",\n");
			theSlurp.displayString.ReplaceAll("\":\"", "\": \"");
		}
	}
}

//--------------------------------------------------------------------------------
SFString CSlurperApp::getFormatString(COptions& options, const SFString& which)
{
	if (which == "file")
		buildDisplayStrings(options);
	
	SFString errMsg;
	
	SFString formatName = "fmt_" + options.exportFormat + "_" + which;
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
		minBlock = toLong(str);
		//		outScreen << first << " : " << str << " : " << minBlock << "\n";
	}
	
	SFString end = contents.Mid(contents.ReverseFind('{'),10000);//pull off the last transaction
	SFInt32 last = end.Find(search);
	if (last!=-1)
	{
		SFString str = end.Mid(last+len,100);
		maxBlock = toLong(str);
		//		outScreen << last << " : " << str << " : " << maxBlock << "\n";
	}
}

//--------------------------------------------------------------------------------
// Make sure our data folder exist, if not establish it
SFBool establish_folders(CConfig& config, const SFString& vers)
{
	if (SFos::folderExists(PATH_TO_SLURPS) && SFos::fileExists(PATH_TO_ETH_SLURP+"config.dat"))
	{
		config.loadFile(PATH_TO_ETH_SLURP+"config.dat");
		return TRUE;
	}
	
	// create the main folder
	SFos::mkdir(PATH_TO_ETH_SLURP);
	if (!SFos::folderExists(PATH_TO_ETH_SLURP))
		return FALSE;
	
	// create the folder for the slurps
	SFos::mkdir(PATH_TO_SLURPS);
	if (!SFos::folderExists(PATH_TO_SLURPS))
		return FALSE;
	
	config.SetProfileString("SETTINGS",     "datapath",          PATH_TO_SLURPS);
	config.SetProfileString("SETTINGS",     "api_key",           EMPTY);
	config.SetProfileString("SETTINGS",     "focus",             "189");
	
	config.SetProfileString("DISPLAY_STR",  "fmt_fieldList",     EMPTY);
	
	config.SetProfileString("DISPLAY_STR",  "fmt_txt_file",      "[{HEADER}]\\n[{RECORDS}]");
	config.SetProfileString("DISPLAY_STR",  "fmt_txt_record",    "[{FIELDS}]\\n");
	config.SetProfileString("DISPLAY_STR",  "fmt_txt_field",     "\\t[{FIELD}]");
	
	config.SetProfileString("DISPLAY_STR",  "fmt_csv_file",      "[{HEADER}]\\n[{RECORDS}]");
	config.SetProfileString("DISPLAY_STR",  "fmt_csv_record",    "[{FIELDS}]\\n");
	config.SetProfileString("DISPLAY_STR",  "fmt_csv_field",     "[\"{FIELD}\"],");
	
	config.SetProfileString("DISPLAY_STR",  "fmt_html_file",     "<table>\\n[{HEADER}]\\n[{RECORDS}]</table>\\n");
	config.SetProfileString("DISPLAY_STR",  "fmt_html_record",   "\\t<tr>\\n[{FIELDS}]</tr>\\n");
	config.SetProfileString("DISPLAY_STR",  "fmt_html_field",    "\\t\\t<td>[{FIELD}]</td>\\n");
	
	config.SetProfileString("DISPLAY_STR",  "fmt_json_file",     "[{RECORDS}]\\n");
	config.SetProfileString("DISPLAY_STR",  "fmt_json_record",   "\\n        {\\n[{FIELDS}]        },");
	config.SetProfileString("DISPLAY_STR",  "fmt_json_field",    "\"[{p:FIELD}]\":\"[{FIELD}]\",");
	
	config.SetProfileString("DISPLAY_STR",  "fmt_custom_file",	 "file:custom_format_file.html");
	config.SetProfileString("DISPLAY_STR",  "fmt_custom_record", "fmt_html_record");
	config.SetProfileString("DISPLAY_STR",  "fmt_custom_field",	 "fmt_html_field");
	
	config.writeFile(vers);
	return SFos::fileExists(config.getFilename());
}

