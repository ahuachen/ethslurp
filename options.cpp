/*--------------------------------------------------------------------------------
 * Copyright 2016 - Great Hill Corporation.
 --------------------------------------------------------------------------------*/
#include "manage.h"
#include "ethslurp.h"

//---------------------------------------------------------------------------------------------------
CParams CApplication::params[] =
{
	CParams("~addr",	"the address of the account or contract to slurp" ),
	CParams("-blocks",	"export records in block range (:0[:max])" ),
	CParams("-income",	"include income transactions only" ),
	CParams("-expense",	"include expenditures only" ),
	CParams("-dates",	"export records in date range (:yyyymmdd[hhmmss][:yyyymmdd[hhmmss]])" ),
	CParams("-max",		"maximum transactions to slurp (:100000)" ),
	CParams("-list",	"list previously slurped addresses in cache" ),
	CParams("-slurp",	"force EthSlurp to take a slurp (ignore cached data)" ),
	CParams("-rerun",	"re-run the most recent slurp" ),
	CParams("-fmt",		"pretty print, optionally add ':txt,' ':csv,' or ':html'" ),
	CParams("-open",	"open the configuration file for editing" ),
	CParams("-clear",	"clear all previously cached slurps" ),
	CParams( "",		"Fetches data off the Ethereum blockchain for an arbitrary account or smart contract. Optionally formats the output to your specification.\n" ),
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

		} else if (args[i] == "-l" || args[i] == "-list")
		{
			SFInt32 nFiles=0;
			SFos::listFilesOrFolders(nFiles, NULL, PATH_TO_SLURPS+"*.*");
			if (nFiles)
			{
				SFString *files = new SFString[nFiles];
				SFos::listFilesOrFolders(nFiles, files, PATH_TO_SLURPS+"*.*");
				for (int i=0;i<nFiles;i++)
					outScreen << files[i] << "\n";
				delete [] files;
			} else
			{
				outScreen << "No files found in cache folder '" << PATH_TO_SLURPS << "'\n";
			}
			exit(0);

		} else if (args[i] == "-b")
		{
			return usage(args[0], "Invalid option -b. This option must include a :firstBlock or :first:lastBlock range.");

		} else if (args[i].Left(3) == "-b:" || args[i].Left(8) == "-blocks:")
		{
			SFString arg = args[i].Substitute("-b:",EMPTY).Substitute("-blocks:",EMPTY);
			firstBlock = MAX(0,toLong(arg));
			if (arg.Contains(":"))
			{
				nextTokenClear(arg,':');
				lastBlock = MAX(firstBlock, toLong(arg));
			}

		} else if (args[i] == "-d")
		{
			return usage(args[0], "Invalid option -d. This option must include a :firstDate or :first:lastDate range.");

		} else if (args[i].Left(3) == "-d:" || args[i].Left(8) == "-dates:")
		{
			SFString lateStr = args[i].Substitute("-d:",EMPTY).Substitute("-dates:",EMPTY);
			SFString earlyStr = nextTokenClear(lateStr,':');

			earlyStr.ReplaceAll("-","");
			lateStr.ReplaceAll("-","");

			if (!earlyStr.IsEmpty() && earlyStr.GetLength() != 8 && earlyStr.GetLength() != 14)
				return usage(args[0], "Option -d: Invalid date format for startDate. Format must be either yyyymmdd or yyyymmddhhmmss.");

			if (!lateStr.IsEmpty() && lateStr.GetLength () != 8 && lateStr.GetLength () != 14)
				return usage(args[0], "Option -d: Invalid date format for endDate. Format must be either yyyymmdd or yyyymmddhhmmss.");

			firstDate = snagDate(earlyStr, earliestDate, -1);
			lastDate  = snagDate(lateStr,  latestDate,    1);

		} else if (args[i] == "-r" || args[i] == "-rerun")
		{
			rerun = TRUE;

		} else if (args[i].startsWith("-m"))
		{
			SFString val = args[i];
			SFString arg = nextTokenClear(val, ':');
			if (arg != "-m" && arg != "-max")
				return usage(args[0], "Unknown parameter: " + arg);
			maxTransactions = toLong(val);

		} else if (args[i] == "-s" || args[i] == "-slurp")
		{
			slurp = TRUE;

		} else if (args[i] == "-o" || args[i] == "-open")
		{
			// open command stuff
			openFile = TRUE;
			if (isTesting)
				outScreen << "Testing only for open command:\n" << asciiFileToString(PATH_TO_ETH_SLURP+"config.dat") << "\n";
			else
				system("open ~/.ethslurp/config.dat");
			exit(0);

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
	rerun           = FALSE;
	incomeOnly      = FALSE;
	expenseOnly     = FALSE;
	openFile        = FALSE;
	firstBlock      = 0;
	lastBlock       = LARGEST_LONG;
	firstDate       = earliestDate;
	lastDate        = latestDate;
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
