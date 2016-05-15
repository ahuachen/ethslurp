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
	CErrorExportContext theScreen;
	if (testOnly)
		outErr = outScreen;
	
	SFString addr;
    SFBool slurp = FALSE, prettyPrint=FALSE, readLast=FALSE, backup=FALSE, incomeOnly=FALSE, expenseOnly=FALSE, openFile=FALSE;
	SFString exportFormat="json";
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

		} else if (args[i] == "-f")
		{
			// -f by itself is json prettyPrint
			prettyPrint = TRUE;
			exportFormat = "json";
			
		} else if (args[i].startsWith("-f"))
		{
			// any other has the format attached  or is invalid
			prettyPrint = TRUE;
			exportFormat = args[i];
			SFString arg = nextTokenClear(exportFormat, ':');
			if (arg != "-f" && arg != "-fmt")
				return usage(args[0], "Unknown parameter: " + arg);
			outErr << "exportFormat: " << exportFormat << "\n";

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
			outErr << "Cached slurp files were cleared\n";
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
		theScreen
			<< conRed << "\n  ***Warning***" << conOff << "\n"
			<< "  " << conYellow << "ethslurp" << conOff << " needs an api_key from EtherScan in order to work. You may get one at\n"
			<< "  http://etherscan.io/apis. See our online help file for more information.\n"
			<< "  Please enter your key or 'exit'\n"
			<< "  > ";
		theScreen.Flush();
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
		outErr << "Cache does not exist or is empty....\nReading from block chain...\n";
		SFString url = "https://api.etherscan.io/api?module=account&action=txlist&address=" + addr + "&sort=asc&apikey=" + api_key;
		contents = urlToString(url);
		stringToAsciiFile(PATH_TO_SLURPS+addr+".json", contents);
	}

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
			if (!(nTrans%(5+cnt))) { outErr << "importing record " << nTrans+1 << " of " << of << (testOnly?"\n":"\r"); outErr.Flush(); }
			cnt--;
			if (cnt==0) cnt=5;
		}
	}
	nTrans--;
	outErr << "importing record " << of << " of " << of << "\n";  outErr.Flush();

	SFString header;
	SFString fmt = getDisplayString(prettyPrint, exportFormat, config, header);
	header.ReplaceAll("<td align=right","<th align=center");
	CStringExportContext records;
	for (int i=0;i<nTrans;i++)
		records << transactions[i].Format(fmt);

	SFString result = snagDisplayString(config, "fmt_"+exportFormat+"_file");
	result = result.Substitute("[{RECORDS}]", records.str);
	result = result.Substitute("[{HEADER}]",  header);
	result = result.Substitute("[{NOW}]",     Now().Format(FMT_DEFAULT));
	result = result.Substitute("[{ADDR}]",    addr);
	outScreen << result;
	
	delete [] transactions;
	
	return RETURN_OK;/* all done */
}

//---------------------------------------------------------------------------------------------------
SFString getDisplayString(SFBool prettyPrint, const SFString& exportFormat, const CConfig& config, SFString& header)
{
	SFString ret;
	SFString fieldString = snagDisplayString(config, "fmt_"+exportFormat+"_field");
	if (!exportFormat.IsEmpty() && !fieldString.IsEmpty())
	{
		SFString displayString;

		SFString fieldList    = ("|"+config.GetProfileStringGH("DISPLAY_STR", "fmt_fieldList",   EMPTY)+"|").Substitute("||","|");

		SFString recordString = snagDisplayString(config, "fmt_"+exportFormat+"_record");
		const CFieldList *fields = GETRUNTIME_CLASS(CTransaction)->m_FieldList;
		LISTPOS lPos = fields->GetFirstItem();
		while (lPos)
		{
			SFString field = fields->GetNextItem(lPos)->getFieldName();
			if (!isInternal(field) && fieldList.Contains("|"+field+"|"))
			{
				displayString += fieldString.Substitute("{FIELD}", "{"+toUpper(field)+"}").Substitute("{p:FIELD}", "{p:"+field+"}");
				header   += fieldString.Substitute("{FIELD}", field).Substitute("[",EMPTY).Substitute("]",EMPTY);
			}
		}
		ret = Strip(recordString.Substitute("[{FIELDS}]", displayString), '\t');
	}

	// one little hack for json
	if (exportFormat=="json")
	{
		ret.ReplaceReverse("}]\",","}]\"\n");
		if (prettyPrint)
		{
			ret.ReplaceAll("\"[{p:", "            \"[{p:");
			ret.ReplaceAll("}]\",",  "}]\",\n");
			ret.ReplaceAll("\":\"", "\": \"");
		}
	}

	return ret;
}

//--------------------------------------------------------------------------------
SFString snagDisplayString(const CConfig& config, const SFString& name)
{
	SFString ret = config.GetProfileStringGH("DISPLAY_STR", name, EMPTY);
	if (ret.Contains("file:"))
	{
		SFString file = ret.Substitute("file:",EMPTY);
		if (!SFos::fileExists(file))
		{
			SFString cmd="ethslurp";
			outErr << SFString("Formatting file '") + file + "' not found. Quiting...\n";
			exit(0);
		}
		ret = asciiFileToString(file);
		
	} else if (ret.Contains("fmt_"))
	{
		ret = config.GetProfileStringGH("DISPLAY_STR", ret, EMPTY);
	}
	ret = ret.Substitute("\\n","\n").Substitute("\\t","\t");
	if (verbose)
		outErr << "snagDisplayString(" << name << "): " << ret << "\n";
	return ret;
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
"blockNumber|"
"timeStamp|"
"hash|"
"nonce|"
"blockHash|"
"transactionIndex|"
"from|"
"to|"
"value|"
"gas|"
"gasPrice|"
"input|"
"contractAddress|"
"cumulativeGasUsed|"
"gasUsed|"
"confirmations";
	
	config->SetProfileString("SETTINGS",     "datapath",          PATH_TO_SLURPS);
	config->SetProfileString("SETTINGS",     "api_key",           EMPTY);
	config->SetProfileString("SETTINGS",     "focus",             "189");
	
	config->SetProfileString("DISPLAY_STR",  "fmt_fieldList",     STR_DATA_FIELDS);
	
	config->SetProfileString("DISPLAY_STR",  "fmt_txt_file",      "[{HEADER}]\\n[{RECORDS}]");
	config->SetProfileString("DISPLAY_STR",  "fmt_txt_record",    "[{FIELDS}]\\n");
	config->SetProfileString("DISPLAY_STR",  "fmt_txt_field",     "\\t[{FIELD}]");
	
	config->SetProfileString("DISPLAY_STR",  "fmt_csv_file",      "[{HEADER}]\\n[{RECORDS}]");
	config->SetProfileString("DISPLAY_STR",  "fmt_csv_record",    "[{FIELDS}]\\n");
	config->SetProfileString("DISPLAY_STR",  "fmt_csv_field",     "[\"{FIELD}\"],");
	
	config->SetProfileString("DISPLAY_STR",  "fmt_html_file",     "<table>\\n[{HEADER}]\\n[{RECORDS}]</table>\\n");
	config->SetProfileString("DISPLAY_STR",  "fmt_html_record",   "\\t<tr>\\n[{FIELDS}]</tr>\\n");
	config->SetProfileString("DISPLAY_STR",  "fmt_html_field",    "\\t\\t<td>[{FIELD}]</td>\\n");

	config->SetProfileString("DISPLAY_STR",  "fmt_json_file",     "[{RECORDS}]\\n");
	config->SetProfileString("DISPLAY_STR",  "fmt_json_record",   "\\n        {\\n[{FIELDS}]        },");
	config->SetProfileString("DISPLAY_STR",  "fmt_json_field",    "\"[{p:FIELD}]\":\"[{FIELD}]\",");

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
