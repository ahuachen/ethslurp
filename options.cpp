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
#include "ethslurp.h"

//---------------------------------------------------------------------------------------------------
CParams params[] =
{
	CParams("~addr",	"the address of the account or contract to slurp" ),
	CParams("-archive",	"filename of output (stdout otherwise)" ),
	CParams("-blocks",	"export records in block range (:0[:max])" ),
	CParams("-dates",	"export records in date range (:yyyymmdd[hhmmss][:yyyymmdd[hhmmss]])" ),
	CParams("-max",		"maximum transactions to slurp (:100000)" ),
	CParams("-rerun",	"re-run the most recent slurp" ),
	CParams("-slurp",	"force EthSlurp to take a slurp (ignore cached data)" ),
	CParams("-fmt",		"pretty print, optionally add ':txt,' ':csv,' or ':html'" ),
	CParams("-income",	"include income transactions only" ),
	CParams("-expense",	"include expenditures only" ),
	CParams("-open",	"open the configuration file for editing" ),
	CParams("-list",	"list previously slurped addresses in cache" ),
	CParams("-clear",	"clear all previously cached slurps" ),
	CParams( "",		"Fetches data off the Ethereum blockchain for an arbitrary account or smart contract. Optionally formats the output to your specification.\n" ),
};
SFInt32 nParams = sizeof(params) / sizeof(CParams);

//---------------------------------------------------------------------------------------------------
SFInt32 COptions::parseArguments(SFInt32 nArgs, const SFString *args)
{
	outScreen.setOutput(stdout); // so we know where it is at the start of each run
	for (int i=0;i<nArgs;i++)
	{
		SFString arg = args[i];
		if (arg == "-i" || arg == "-income")
		{
			if (expenseOnly)
				return usage("Only one of -income or -expense may be specified.");
			incomeOnly = TRUE;

		} else if (arg == "-e" || arg == "-expense")
		{
			if (incomeOnly)
				return usage("Only one of -income or -expense may be specified.");
			expenseOnly = TRUE;

		} else if (arg == "-f")
		{
			// -f by itself is json prettyPrint
			prettyPrint = TRUE;
			exportFormat = "json";

		} else if (arg.startsWith("-f"))
		{
			// any other -f has the format attached  or is invalid
			prettyPrint = TRUE;
			exportFormat = arg;
			SFString arg = nextTokenClear(exportFormat, ':');
			if (arg != "-f" && arg != "-fmt")
				return usage("Unknown parameter: " + arg);

		} else if (arg == "-l" || arg == "-list")
		{
			SFInt32 nFiles=0;
			SFos::listFilesOrFolders(nFiles, NULL, cachePath("*.*"));
			if (nFiles)
			{
				SFString *files = new SFString[nFiles];
				SFos::listFilesOrFolders(nFiles, files, cachePath("*.*"));
				for (int i=0;i<nFiles;i++)
					outScreen << files[i] << "\n";
				delete [] files;
			} else
			{
				outScreen << "No files found in cache folder '" << cachePath() << "'\n";
			}
			exit(0);

		} else if (arg == "-b")
		{
			return usage("Invalid option -b. This option must include :firstBlock or :first:lastBlock range.");

		} else if (arg.Left(3) == "-b:" || arg.Left(8) == "-blocks:")
		{
			arg = arg.Substitute("-b:",EMPTY).Substitute("-blocks:",EMPTY);
			firstBlock2Read = MAX(0,toLong(arg));
			if (arg.Contains(":"))
			{
				nextTokenClear(arg,':');
				lastBlock2Read = MAX(firstBlock2Read, toLong(arg));
			}

		} else if (arg == "-d")
		{
			return usage("Invalid option -d. This option must include :firstDate or :first:lastDate range.");

		} else if (arg.Left(3) == "-d:" || arg.Left(8) == "-dates:")
		{
			SFString lateStr = arg.Substitute("-d:",EMPTY).Substitute("-dates:",EMPTY);
			SFString earlyStr = nextTokenClear(lateStr,':');

			earlyStr.ReplaceAll("-","");
			lateStr.ReplaceAll("-","");

			if (!earlyStr.IsEmpty() && earlyStr.GetLength() != 8 && earlyStr.GetLength() != 14)
				return usage("Option -d: Invalid date format for startDate. Format must be either yyyymmdd or yyyymmddhhmmss.");

			if (!lateStr.IsEmpty() && lateStr.GetLength () != 8 && lateStr.GetLength () != 14)
				return usage("Option -d: Invalid date format for endDate. Format must be either yyyymmdd or yyyymmddhhmmss.");

			firstDate = snagDate(earlyStr, earliestDate, -1);
			lastDate  = snagDate(lateStr,  latestDate,    1);

		} else if (arg == "-r" || arg == "-rerun")
		{
			rerun = TRUE;

		} else if (arg.startsWith("-m"))
		{
			SFString val = arg;
			SFString arg = nextTokenClear(val, ':');
			if (arg != "-m" && arg != "-max")
				return usage("Unknown parameter: " + arg);
			maxTransactions = toLong(val);

		} else if (arg == "-s" || arg == "-slurp")
		{
			slurp = TRUE;

		} else if (arg.startsWith("-a"))
		{
			SFString fileName = arg.Substitute("-a:",EMPTY).Substitute("-archive:",EMPTY);
			if (fileName.Contains("-a"))
				return usage("Invalid option: " + arg);
			SFFile file(fileName);
			if (!file.getPath().startsWith('/'))
				return usage("Archive file '" + arg + "' does not resolve to a full path. Use ./path/filename, ~/path/filename, or a fully qualified path.");
			ASSERT(output==NULL);
			output = fopen((const char*)file.getFullPath(), asciiWriteCreate);
			if (!output)
				return usage("file '" + file.getFullPath() + "' could not be opened. Quitting.");
			outScreen.setOutput(output);

		} else if (arg == "-o" || arg == "-open")
		{
			// open command stuff
			openFile = TRUE;
			if (isTesting)
				outScreen << "Testing only for open command:\n" << asciiFileToString(configPath("config.dat")) << "\n";
			else
				system("open ~/.ethslurp/config.dat");
			exit(0);

		} else if (arg == "-c" || arg == "-clear")
		{
			if (isTesting)
			{
				SFString unused1, unused2;
				SFos::removeFolder(cachePath(), unused1, unused2, TRUE);
				outErr << "Cached slurp files were cleared\n";
			} else
			{
				outErr << "Clearing the cache is not implemented. You may, if you wish, remove all\n";
				outErr << "files in ~/.ethslurp/slurps/ to acheive the same thing. Large contracts\n";
				outErr << "may take a very long time to re-download if you do.\n";
			}
			exit(1);

		} else
		{
			if (arg != "-v" && arg != "-t" && arg != "-h")
			{
				if (arg[0] == '-')
					return usage("Invalid option " + arg);
				addr = arg;
			}
		}
	}

	return TRUE;
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
	firstBlock2Read = 0;
	lastBlock2Read  = LONG_MAX;
	firstDate       = earliestDate;
	lastDate        = latestDate;
	maxTransactions = 100000;
	pageSize        = 5000;
	exportFormat    = "json";
	output          = NULL;
	//addr;
}

//--------------------------------------------------------------------------------
COptions::~COptions(void)
{
	outScreen.setOutput(stdout); // flushes and clears archive file if any
	output=NULL;
}

//--------------------------------------------------------------------------------
int usage(const SFString& errMsg)
{
	SFString cmd = "ethslurp";
	//		qsort(cmdFunc->params, cmdFunc->nParams, sizeof(CParams), sortParams);
	outErr << "\n" << (!errMsg.IsEmpty() ? "  " + errMsg + "\n\n" : "");
	outErr << "  Usage:   " + cmd + " " << options() << "\n";
	outErr << purpose();
	outErr << descriptions() << "\n";
	outErr << "  Portions of this software are Powered by Etherscan.io APIs\n\n";
	return FALSE;
}

//--------------------------------------------------------------------------------
CParams::CParams( const SFString& nameIn, const SFString& descr )
{
	SFString name = nameIn;

	description = descr;
	if (!name.IsEmpty())
	{
		shortName   = name.Left(2);
		if (name.GetLength()>2)
			longName  = name;
		if (name.Contains("{"))
		{
			name.Replace("{","|{");
			nextTokenClear(name,'|');
			shortName += name;

		} else if (name.Contains(":"))
		{
			nextTokenClear(name,':');
			shortName += name[0];
			longName = "-" + name;
		}
	}
}

//--------------------------------------------------------------------------------
SFString options(void)
{
	SFString required;

	CStringExportContext ctx;
	ctx << "[";
	for (int i=0;i<nParams;i++)
	{
		if (params[i].shortName.startsWith('~'))
		{
			required += (" " + params[i].longName.Mid(1));

		} else if (!params[i].shortName.IsEmpty())
		{
			ctx << params[i].shortName << "|";
		}
	}
	ctx << "-t|-v|-h]";
	ctx << required;

	return ctx.str;
}

//--------------------------------------------------------------------------------
SFString purpose(void)
{
	SFString purpose;
	for (int i=0;i<nParams;i++)
		if (params[i].shortName.IsEmpty())
			purpose += ("\n           " + params[i].description);

	CStringExportContext ctx;
	if (!purpose.IsEmpty())
	{
		purpose.Replace("\n           ",EMPTY);
		ctx << "  Purpose: " << purpose.Substitute("\n","\n           ") << "\n";
	}
	ctx << "  Where:\n";
	return ctx.str;
}

//--------------------------------------------------------------------------------
SFString descriptions(void)
{
	SFString required;

	SFBool usesT = FALSE;
	CStringExportContext ctx;
	for (int i=0;i<nParams;i++)
	{
		if (params[i].shortName.startsWith('~'))
		{
			required += ("\t" + padRight(params[i].longName,22) + params[i].description).Substitute("~",EMPTY) + " (required)\n";

		} else if (!params[i].shortName.IsEmpty())
		{
			ctx << "\t" << padRight(params[i].shortName,3) << padRight((params[i].longName.IsEmpty() ? "" : " (or "+params[i].longName+")"),18) << params[i].description << "\n";
		}
		if (params[i].shortName.startsWith("-t"))
			usesT = TRUE;
	}
	ctx.str = (required + ctx.str);
	if (!usesT)
		ctx << "\t" << "-t  (or -test)       generate intermediary files but do not execute the commands\n";
	ctx << "\t" << "-v  (or -verbose)    set verbose level. Follow with a number to set level (-v0 for silent)\n";
	ctx << "\t" << "-h  (or -help)       display this help screen\n";
	return ctx.str;
}

//--------------------------------------------------------------------------------
SFString expandOption(SFString& arg)
{
	SFString ret = arg;

	// Not an option
	if (!arg.startsWith('-'))
	{
		arg=EMPTY;
		return ret;
	}

	// Single option
	if (arg.GetLength()==2)
	{
		arg=EMPTY;
		return ret;
	}

	// One of the range commands. These must be alone on
	// the line (this is a bug for -rf:txt for example)
	if (arg.Contains(":"))
	{
		arg=EMPTY;
		return ret;
	}

	// This is a ganged-up option. We need to pull it apart by returning
	// the first two chars, and saving the rest for later.
	ret = arg.Left(2);
	arg = "-"+arg.Mid(2,1000);
	return ret;
}

//--------------------------------------------------------------------------------
int sortParams(const void *c1, const void *c2)
{
	CParams *p1 = (CParams*)c1;
	CParams *p2 = (CParams*)c2;
	if (p1->shortName=="-h")
		return 1;
	else if (p2->shortName=="-h")
		return -1;
	return (int)p1->shortName.Compare(p2->shortName);
}

//--------------------------------------------------------------------------------
SFBool verbose  = FALSE;
SFBool isTesting = FALSE;

//--------------------------------------------------------------------------------
CFileExportContext   outScreen;
CErrorExportContext  outErr_internal;
CFileExportContext&  outErr = outErr_internal;
