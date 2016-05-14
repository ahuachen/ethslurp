/*--------------------------------------------------------------------------------
 * Copyright 2016 - Great Hill Corporation.
 --------------------------------------------------------------------------------*/
#include "manage.h"
#include "ethslurp.h"

//---------------------------------------------------------------------------------------------------
CParams paramsEthSlurp[] =
{
	CParams("~addr",	"the address of the account or contract to slurp" ),
	CParams("-income",	"include income transactions only" ),
	CParams("-expense",	"include expenditures only" ),
	CParams("-date",	"specify date range as 'start-[end]' (yyyymmdd[:hhmmss])" ),
	CParams("-last",	"re-display most recent (last) slurp" ),
	CParams("-slurp",	"force EthSlurp to take a slurp (ignore cached data)" ),
	CParams("-fmt",		"pretty print, optionally add ':txt,' ':csv,' or ':html'" ),
	CParams("-open",	"open output in an associated editor" ),
	CParams("-backup",	"save a date stamped backup of the previous slurp" ),
	CParams("-clear",	"clear all previously cached slurps" ),
	CParams( "",		"Fetches data off the Ethereum block chain for an arbitrary account or smart contract. Optionally formats the output to your specification.\n" ),
};
SFInt32 nParamsEthSlurp = sizeof(paramsEthSlurp) / sizeof(CParams);

//---------------------------------------------------------------------------------------------------
SFInt32 cmdEthSlurp(SFInt32 nArgs, const SFString *args)
{
	version.setVersion(0,0,9);

	// just so the test err output goes to stdin
	if (testOnly)
		outErr = outScreen;
	
	SFString addr;
    SFBool slurp = FALSE, prettyPrint=FALSE, readLast=FALSE, backup=FALSE, incomeOnly=FALSE, expenseOnly=FALSE, openFile=FALSE;
	SFString ppFmt="json";
    for (int i=1;i<nArgs;i++)
    {
		if (args[i] == "-i" || args[i] == "-income")
		{
			incomeOnly = TRUE;

		} else if (args[i] == "-e" || args[i] == "-expense")
		{
			expenseOnly = TRUE;

		} else if (args[i] == "-d" || args[i] == "-date")
		{
			// date stuff
			return usage(args[0], "date command not implemented");

		} else if (args[i].startsWith("-f"))
		{
			ppFmt = args[i];
			SFString arg = nextTokenClear(ppFmt, ':');
			if (arg != "-f" && arg != "-fmt")
				return usage(args[0], "Unknown parameter: " + arg);
			prettyPrint = TRUE;

		} else if (args[i] == "-l" || args[i] == "-last")
		{
			readLast = TRUE;

		} else if (args[i] == "-m" || args[i] == "-most")
		{
			// most command stuff
			return usage(args[0], "most command not implemented");

		} else if (args[i] == "-s" || args[i] == "-slurp")
		{
			slurp = TRUE;

		} else if (args[i] == "-t" || args[i] == "-totals")
		{
			// totals command stuff
			return usage(args[0], "totals commands not implemented");

		} else if (args[i] == "-o" || args[i] == "-open")
		{
			// open command stuff
			openFile = TRUE;
			return usage(args[0], "open command not implemented");

		} else if (args[i] == "-b" || args[i] == "-backup")
		{
			backup = TRUE;
			return usage(args[0], "backup command not implemented");

		} else if (args[i] == "-c" || args[i] == "-clear")
		{
			SFString unused1, unused2;
			SFos::removeFolder(PATH_TO_SLURPS, unused1, unused2, TRUE);
			outErr << "Cache files were cleared\n";
			exit(1);

		} else
		{
			if (args[i][0] == '-')
				return usage(args[0], "Invalid option " + args[i]);
			addr = args[i];
		}
    }

	CConfig config(NULL);
	config.setFilename(PATH_TO_ETH_SLURP+"config.dat");
	if (!establish_folders(&config))
		return usage(args[0], "Unable to create data folders at " + PATH_TO_SLURPS);

	SFString api_key = config.GetProfileStringGH("SETTINGS", "api_key", EMPTY);
	while (api_key.IsEmpty())
	{
		// Don't use outErr here since it might be redirected by testing
		CErrorExportContext errCtx;
		errCtx
			<< conYellow << "\n  ***Warning***" << conOff << "\n"
			<< "  EthSlurp needs an api_key in order to work. http://etherscan.io/apis in order to work.\n"
			<< "  See our online help file at http://ethslurp.com/documentation.html\n"
			<< "  Please go there now and enter your key or 'exit' to quit:\n"
			<< "  > ";
		errCtx.Flush();
		char buffer[256];
		cin >> buffer;
		api_key = buffer;
		if (api_key % "exit" || api_key % "quit")
			exit(0);
		config.SetProfileString("SETTINGS", "api_key", api_key);
		config.writeFile(version.toString());
	}
	
	if (addr.IsEmpty() && readLast)
		addr = asciiFileToString(PATH_TO_ETH_SLURP+"lastRead.dat").Substitute("\n",EMPTY);
	addr.MakeLower();
	if (addr.IsEmpty())
	{
		SFString msg = "You must supply an Ethereum account or contract address. ";
		if (!readLast)
			msg += "Use -l flag to read the most recently slurped address.";
		return usage(args[0], msg);
	}

	outErr << "Slurping " << addr << "\n";
	stringToAsciiFile(PATH_TO_ETH_SLURP+"lastRead.dat", addr);

	outErr << "Checking cache...may be empty\n";
	SFString contents = asciiFileToString(PATH_TO_SLURPS+addr+".json");
	if (slurp || contents.IsEmpty())
	{
		outErr << "Cache does not exist or is empty....\nReading from block chain using api_key: " << api_key << "...\n";
		SFString url = "https://api.etherscan.io/api?module=account&action=txlist&address=" + addr + "&sort=asc&apikey=" + api_key;
		outErr << "\twith url: " << url << "\n";
		contents = urlToString(url);
		stringToAsciiFile(PATH_TO_SLURPS+addr+".json", contents);
	}

	if (ppFmt.IsEmpty() && (!incomeOnly && !expenseOnly))
	{
		if (prettyPrint)
			system((const char*)"cat " + PATH_TO_SLURPS+addr+".json | python -m json.tool");
		else
			outScreen << contents;
		outScreen << "\n";

	} else
	{
		SFInt32 nTrans = countOf('}', contents);
		CTransaction *transactions = new CTransaction[nTrans];
		SFInt32 of = nTrans; nTrans=0;
		int cnt=5;
		while (!contents.IsEmpty())
		{
			SFString trans = nextTokenClear(contents, '}');
			CTransaction *cur = &transactions[nTrans];
			cur->parseJson(trans);
			if (incomeOnly && cur->to != addr)
			{
				if (verbose)
					outErr << cur->Format("skipping expenditure [{HASH}]\n");

			} else if (expenseOnly && cur->from != addr)
			{
				if (verbose)
					outErr << cur->Format("skipping inflow [{HASH}]\n");

			} else
			{
				nTrans++;
				if (!(nTrans%(5+cnt))) { outErr << "importing record " << nTrans+1 << " of " << of << "\r"; outErr.Flush(); }
				cnt--;
				if (cnt==0) cnt=5;
			}
		}
		nTrans--;
		outErr << "importing record " << of << " of " << of << "\n";  outErr.Flush();

		SFString fieldList = ("|"+config.GetProfileStringGH("DISPLAY_STR", "fmt_fieldList", EMPTY)+"|").Substitute("||","|");
		SFString surrFmt   = config.GetProfileStringGH("DISPLAY_STR", "fmt_"+ppFmt+"_surround", EMPTY).Substitute("\\n","\n").Substitute("\\t","\t");
		SFString recordFmt = config.GetProfileStringGH("DISPLAY_STR", "fmt_"+ppFmt+"_record",   EMPTY).Substitute("\\n","\n").Substitute("\\t","\t");
		SFString fieldFmt  = config.GetProfileStringGH("DISPLAY_STR", "fmt_"+ppFmt+"_field",    EMPTY).Substitute("\\n","\n").Substitute("\\t","\t");

		SFString fmt = getDisplayFormat(prettyPrint);
		if (!ppFmt.IsEmpty() && !fieldFmt.IsEmpty())
		{
			if (verbose && !fieldFmt.IsEmpty())
				outErr << "format: " << ppFmt << "\nsurround: " << surrFmt << "records: " << recordFmt << "field: " << fieldFmt << "\n";

			SFString fieldStr;
			const CFieldList *fields = GETRUNTIME_CLASS(CTransaction)->m_FieldList;
			LISTPOS lPos = fields->GetFirstItem();
			while (lPos)
			{
				SFString field = fields->GetNextItem(lPos)->getFieldName();
				if (!isInternal(field) && fieldList.Contains("|"+field+"|"))
					fieldStr += fieldFmt.Substitute("{FIELD}", "{"+toUpper(field)+"}");
			}
			fmt = Strip(recordFmt.Substitute("[FIELDS]", fieldStr), '\t');
		}
	
		CStringExportContext records;
		for (int i=0;i<nTrans;i++)
			records << transactions[i].Format(fmt);
		records << "\n";

		outScreen << (surrFmt.Contains("RECORDS") ? surrFmt.Substitute("[RECORDS]", records) : records);
		delete [] transactions;
	}

	return RETURN_OK;/* all done */
}

//---------------------------------------------------------------------------------------------------
SFString getDisplayFormat(SFBool prettyPrint)
{
	SFString ret;
	const CFieldList *fields = GETRUNTIME_CLASS(CTransaction)->m_FieldList;
	LISTPOS lPos = fields->GetFirstItem();
	while (lPos)
	{
		SFString field = fields->GetNextItem(lPos)->getFieldName();
		if (!isInternal(field))
		{
			if (!ret.IsEmpty())
				ret += ",";
			ret += "\"" + field + "\":\"[{" + toUpper(field) + "}]\"";
		}
	}
	if (prettyPrint)
	{
		ret.Replace("\"","            \"");
		ret.ReplaceAll("\",\"","\",            \"");
		ret.ReplaceAll("}]\",", "}]\",\n");
		ret.ReplaceAll(":",": ");
		ret += "\n";
	}
	return "\n        {\n" + ret + "        },";
}

//--------------------------------------------------------------------------------
// Make sure our data folder exist, if not establish it
SFBool establish_folders(CConfig *config)
{
	if (SFos::folderExists(PATH_TO_SLURPS))
	{
		config->loadFile(PATH_TO_ETH_SLURP+"config.dat");
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

const char* STR_DATA_FIELDS=
"blockHash|"
"blockNumber|"
"confirmations|"
"contractAddress|"
"cumulativeGasUsed|"
"from|"
"gas|"
"gasPrice|"
"gasUsed|"
"hash|"
"input|"
"nonce|"
"timeStamp|"
"to|"
"transactionIndex|"
"value";
	
	config->SetProfileString("SETTINGS",     "datapath",          PATH_TO_SLURPS);
	config->SetProfileString("SETTINGS",     "api_key",           EMPTY);
	config->SetProfileString("SETTINGS",     "focus",             "189");
	
	config->SetProfileString("DISPLAY_STR",  "fmt_fieldList",     STR_DATA_FIELDS);
	
	config->SetProfileString("DISPLAY_STR",  "fmt_txt_surround",  "[RECORDS]\\n");
	config->SetProfileString("DISPLAY_STR",  "fmt_txt_record",    "[FIELDS]\\n");
	config->SetProfileString("DISPLAY_STR",  "fmt_txt_field",     "\\t[{FIELD}]");
	
	config->SetProfileString("DISPLAY_STR",  "fmt_csv_surround",  "[RECORDS]\\n");
	config->SetProfileString("DISPLAY_STR",  "fmt_csv_record",    "[FIELDS]\\n");
	config->SetProfileString("DISPLAY_STR",  "fmt_csv_field",     "[\"{FIELD}\"],");
	
	config->SetProfileString("DISPLAY_STR",  "fmt_html_surround", "<table>\\n[RECORDS]</table>\\n");
	config->SetProfileString("DISPLAY_STR",  "fmt_html_record",   "\\t<tr>\\n[FIELDS]</tr>\\n");
	config->SetProfileString("DISPLAY_STR",  "fmt_html_field",    "\\t\\t<td>[{FIELD}]</td>\\n");

	config->writeFile(version.toString());
	return SFos::fileExists(config->getFilename());
}

//--------------------------------------------------------------------------------
SFString getHomeFolder(void)
{
	struct passwd *pw = getpwuid(getuid());
	return SFString(pw->pw_dir)+"/";
}

//---------------------------------------------------------------------------------------------------
CVersion version;
