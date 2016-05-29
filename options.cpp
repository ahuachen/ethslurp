/*--------------------------------------------------------------------------------
 * Copyright 2016 - Great Hill Corporation.
 --------------------------------------------------------------------------------*/
#include "manage.h"
#include "ethslurp.h"

//---------------------------------------------------------------------------------------------------
CParams CApplication::params[] =
{
	CParams("~addr",	"the address of the account or contract to slurp" ),
	CParams("-block",	"report only records on or after :num block (:0)" ),
	CParams("-income",	"include income transactions only" ),
	CParams("-expense",	"include expenditures only" ),
	CParams("-last",	"re-display most recent (last) slurp" ),
	CParams("-max",		"maximum transactions to slurp (:100000)" ),
	CParams("-slurp",	"force EthSlurp to take a slurp (ignore cached data)" ),
	CParams("-fmt",		"pretty print, optionally add ':txt,' ':csv,' or ':html'" ),
	CParams("-open",	"open the configuration file for editing" ),
	CParams("-clear",	"clear all previously cached slurps" ),
	CParams( "",		"Fetches data off the Ethereum block chain for an arbitrary account or smart contract. Optionally formats the output to your specification.\n" ),
};
SFInt32 CApplication::nParams = sizeof(CApplication::params) / sizeof(CParams);

//---------------------------------------------------------------------------------------------------
SFInt32 COptions::parseArguments(SFInt32 nArgs, const SFString *args)
{
	if (isTesting)
	{
		for (int i=0;i<nArgs;i++)
			outScreen << args[i] << " ";
		outScreen << "\n";
	}
	
	for (int i=1;i<nArgs;i++)
	{
		if (args[i] == "-i" || args[i] == "-income")
		{
			incomeOnly = TRUE;
			
		} else if (args[i] == "-e" || args[i] == "-expense")
		{
			expenseOnly = TRUE;
			
		} else if (args[i] == "-f")
		{
			// -f by itself is json prettyPrint
			prettyPrint = TRUE;
			exportFormat = "json";
			
		} else if (args[i].startsWith("-f"))
		{
			// any other -f has the format attached  or is invalid
			prettyPrint = TRUE;
			exportFormat = args[i];
			SFString arg = nextTokenClear(exportFormat, ':');
			if (arg != "-f" && arg != "-fmt")
				return usage(args[0], "Unknown parameter: " + arg);
			
		} else if (args[i] == "-l" || args[i] == "-last")
		{
			lastRead = TRUE;
			
		} else if (args[i].startsWith("-m"))
		{
			SFString val = args[i];
			SFString arg = nextTokenClear(val, ':');
			if (arg != "-m" && arg != "-max")
				return usage(args[0], "Unknown parameter: " + arg);
			maxTransactions = atoi((const char*)val);

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
			if (isTesting)
				outScreen << "Testing only for open command:\n" << asciiFileToString(PATH_TO_ETH_SLURP+"config.dat") << "\n";
			else
				system("open ~/.ethslurp/config.dat");
			exit(0);
			
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
	return RETURN_OK;
}

//---------------------------------------------------------------------------------------------------
COptions::COptions(void)
{
	slurp           = FALSE;
	prettyPrint     = FALSE;
	lastRead        = FALSE;
	backup          = FALSE;
	incomeOnly      = FALSE;
	expenseOnly     = FALSE;
	openFile        = FALSE;
	maxTransactions = 100000;
	pageSize        = 5000;
	exportFormat    = "json";
	//addr;
}

//--------------------------------------------------------------------------------
SFString getHomeFolder(void)
{
	struct passwd *pw = getpwuid(getuid());
	return SFString(pw->pw_dir)+"/";
}
