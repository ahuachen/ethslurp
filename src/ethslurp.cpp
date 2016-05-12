/*--------------------------------------------------------------------------------
 * Copyright 2016 - Great Hill Corporation.
 --------------------------------------------------------------------------------*/
#include "manage.h"
#include "ethslurp.h"

//---------------------------------------------------------------------------------------------------
CParams paramsEthSlurp[] =
{
	CParams("~addr",		"the address of the account or contract to slurp"                      ),
	CParams("-income",		"include income transactions only"                                     ),
	CParams("-expense",		"include expenditures transaction only"                                ),
	CParams("-date",		"specify a start date or date range"                                   ),
//	CParams("-feed",		"specify a price feed (for example US dollars)"                        ),
	CParams("-last",		"re-display the last slurp without providing address"                  ),
//	CParams("-most",		"export only the most interesting data fields"                         ),
	CParams("-slurp",		"force a slurp from the blockchain (ignore cache)"                     ),
	CParams("-prettyPrint",	"pretty print the output optionally ':txt', ':csv', otherwise ':json'" ),
//	CParams("-open",		"open the results in its associated editor"                            ),
//	CParams("-backup",		"save a date stamped backup of the previous slurp"                     ),
	CParams("-clear",		"clear all cached slurps"                                              ),
	CParams( "",			"Fetches data off the Ethereum block chain for an arbitrary account or smart contract. Optionally formats the output to your specification.\n" ),
};
SFInt32 nParamsEthSlurp = sizeof(paramsEthSlurp) / sizeof(CParams);

//---------------------------------------------------------------------------------------------------
#define PATH_TO_HOME       homeFolder()
#define PATH_TO_ETH_SLURP  SFString(homeFolder() + "/.ethslurp/")
#define PATH_TO_SLURPS     SFString(PATH_TO_ETH_SLURP + "slurps/")

//--------------------------------------------------------------------------------
extern SFBool   establish_folders(void);
extern SFString homeFolder(void);

//---------------------------------------------------------------------------------------------------
SFString getDisplayFormat(SFBool prettyPrint)
{
	const CFieldList *fields = GETRUNTIME_CLASS(CTransaction)->m_FieldList;

	SFString ret;
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

//---------------------------------------------------------------------------------------------------
SFInt32 cmdEthSlurp(SFInt32 nArgs, const SFString *args)
{
	SFString addr;
    SFBool slurp = FALSE, prettyPrint=FALSE, readLast=FALSE, backup=FALSE, incomeOnly=FALSE, expenseOnly=FALSE, openFile=FALSE;
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
			
		} else if (args[i] == "-f" || args[i] == "-feed")
		{
			// feed stuff
			return usage(args[0], "price feed command not implemented");
			
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
			
		} else if (args[i] == "-p" || args[i] == "-pretty"  || args[i] == "-prettyPrint")
		{
			prettyPrint = TRUE;

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
			establish_folders();
			outScreen << "Cache files were cleared\n";
			exit(1);

		} else
		{
			if (args[i][0] == '-')
				return usage(args[0], "Invalid option " + args[i]);
			addr = args[i];
		}
    }

	if (addr.IsEmpty() && readLast)
		addr = asciiFileToString(PATH_TO_ETH_SLURP+"lastRead.dat").Substitute("\n",EMPTY);
	addr.MakeLower();
	if (addr.IsEmpty())
	{
		SFString msg = "You must supply an Ethereum account or contract address. ";
		if (!readLast)
			msg += "Use -l flag to read the most recently read address.";
		return usage(args[0], msg);
	}

	outScreen << "Slurping " << addr << "\n";
	stringToAsciiFile(PATH_TO_ETH_SLURP+"lastRead.dat", addr);

	if (!establish_folders())
	{
		return usage(args[0], "Unable to create data folders at " + PATH_TO_SLURPS);
	}
	
	SFString contents = asciiFileToString(PATH_TO_SLURPS+addr+".txt");
	if (slurp || contents.IsEmpty())
	{
		SFString url = "https://api.etherscan.io/api?module=account&action=txlist&address=" + addr + "&sort=asc&apikey=68E1BQYW85ETVNHKWV27B8N5HYICEHEPH1";
		contents = urlToString(url);
		stringToAsciiFile(PATH_TO_SLURPS+addr+".txt", contents);
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
				outScreen << cur->Format("rejecting expentature transaction [{HASH}]\n");

		} else if (expenseOnly && cur->from != addr)
		{
			if (verbose)
				outScreen << cur->Format("rejecting income transaction [{HASH}]\n");

		} else
		{
			nTrans++;
			if (!(nTrans%(5+cnt))) { outScreen << "importing record " << nTrans+1 << " of " << of << "\r"; outScreen.Flush(); }
			cnt--;
			if (cnt==0) cnt=5;
		}
	}
	nTrans--;
	outScreen << "importing record " << of << " of " << of << "\n";  outScreen.Flush(); // trans #" << transactions[nTrans-100].hash << "\n";

	SFBool json = TRUE;
	if (prettyPrint)
	{
		if (json)
			outScreen << "{\n    \"message\": \"OK\",\n    \"result\": [";
		SFString fmt = getDisplayFormat(prettyPrint);
		for (int i=0;i<nTrans;i++)
			outScreen << transactions[i].Format(fmt);
		if (json)
			outScreen << "\n    ],\n    \"status\": \"1\"\n}\n";
	} else
	{
		SFString fmt = getDisplayFormat(FALSE);
		for (int i=0;i<nTrans;i++)
			outScreen << transactions[i].Format(fmt);
//		outScreen << asciiFileToString(PATH_TO_SLURPS+addr+".txt");
	}
	outScreen << "\n";

	delete [] transactions;

	return RETURN_OK;/* all done */
}

//--------------------------------------------------------------------------------
SFString homeFolder(void)
{
	system("whoami > shit.txt");
	SFString user = asciiFileToString("shit.txt").Substitute("\n",EMPTY).Substitute("\r",EMPTY);
//if (verbose>1) outErr << "homeFolder: found user '" << user << "'\n";
	system("pwd > shit.txt");
	SFString folder = asciiFileToString("shit.txt").Substitute("\n",EMPTY).Substitute("\r",EMPTY);
//if (verbose>1) outErr << "homeFolder: found folder '" << folder << "'\n";
	SFString ret = folder.Left(folder.Find(user)+user.GetLength()+1);
//if (verbose) outErr << "homeFolder: returning '" << ret << "'\n";
	SFos::removeFile("shit.txt");
	return ret;
}

//--------------------------------------------------------------------------------
SFBool establish_folders(void)
{
	SFBool exists = SFos::folderExists(PATH_TO_SLURPS);
	if (!exists)
	{
		if (!SFos::folderExists(PATH_TO_ETH_SLURP))
			SFos::mkdir(PATH_TO_ETH_SLURP);
		SFos::mkdir(PATH_TO_SLURPS);
		return SFos::folderExists(PATH_TO_SLURPS);
	}
	return TRUE;
}
