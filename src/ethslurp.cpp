/*--------------------------------------------------------------------------------
 * Copyright 2016 - Great Hill Corporation.
 --------------------------------------------------------------------------------*/
#include "manage.h"
#include "ethslurp.h"

//---------------------------------------------------------------------------------------------------
CParams paramsEthSlurp[] =
{
	CParams("~addr",		"the ethereum address of the account or contract you wish to read"                                      ),
	CParams( "-nobackup",	"do not save a timestamped backup file of the previous data file (data will be backed up by default)"   ),
	CParams( "-pretty",		"output data to screen in human readable format and quit"                                               ),
	CParams( "-slurp",      "pull fresh data from the blockchain"                                                                   ),
	CParams( "-last",		"read from the last (most recently read) address"                                                       ),
//	CParams( "-write:fmt",	"write the data in various formats (.txt only for now) with optional (on by default) backup"            ),
	CParams( "",			"Fetches data off the Ethereum block chain for an arbitrary account or smart contract.\nOptionally prettyPrints or stores the data in various formats.\n" ),
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
SFInt32 cmdEthSlurp(SFInt32 nArgs, const SFString *args)
{
	SFString addr;
    SFBool slurp = FALSE, prettyPrint=FALSE, readLast=FALSE, backup=TRUE;
    for (int i=1;i<nArgs;i++)
    {
		if (args[i] == "-l" || args[i] == "-last")
		{
			readLast = TRUE;
			
		} else if (args[i] == "-n" || args[i] == "-nobackup")
		{
			backup = FALSE;

		} else if (args[i] == "-p" || args[i] == "-pretty"  || args[i] == "-prettyPrint")
		{
			prettyPrint = TRUE;

		} else if (args[i] == "-s" || args[i] == "-save")
		{
			slurp = TRUE;

		} else
		{
			addr = args[i];
		}
    }

	if (addr.IsEmpty() && readLast)
		addr = asciiFileToString(PATH_TO_ETH_SLURP+"lastRead.dat").Substitute("\n",EMPTY);

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
	if (prettyPrint)
		system((const char*)"cat " + PATH_TO_SLURPS+addr+".txt | python -m json.tool");
	else
		outScreen << contents;
	outScreen << "\n";
	/*
	if (!visitQueue.Peek())
		usage(args[0],"Specify a URL to get results");

	SFString path = "/Users/jrush/source/EthSlurp/";
	while (visitQueue.Peek())
	{
		CWebPage *item = visitQueue.Pop();
		if (verbose)
			outErr << "popping: " << unencode(item->url) << SFString(' ', 100 - item->url.GetLength()) << "\r";
		if (item->level+1 < maxLevel)
		{
			SFString contents = urlToString(unencode(item->url));
			if (toFile)
				stringToAsciiFile(path + makeValidName_noDash(item->url)+".htm", contents);
			SFString start    = item->url;
			start.Replace("http://", EMPTY);
			if (start.startsWith("/"))
				start = start.Mid(1,start.Find("/"));
			start = nextToken(start, '/');
			if (stayOnSite && CWebPage::root.IsEmpty())
				CWebPage::root = encode("http://" + start + "/");
			contents.ReplaceAll("href", "\nhref");
			while (!contents.IsEmpty())
			{
				SFString line = nextTokenClear(contents, '\n');
				if (!extract || line.Contains("href="))
				{
					if (extract)
					{
						line = Strip(line.Mid(line.Find("href=")+5),'\"');
						if (line.startsWith("/"))
							line = "http://" + start + line;
						line = nextTokenClear(line, '>');
						line = nextTokenClear(line, '\"');
						line = nextTokenClear(line, '\t');
						line = nextTokenClear(line, '\n');
						line = encode(line);
					}

					if (!extract || line.Contains(start))
					{
						if (noParams)
							line = nextTokenClear(line, '?');
						SFBool ret = pushOnList(item->level+1, line);
						if (ret && verbose)
							outErr << "pushing: " << line << SFString(' ', 100 - line.GetLength()) << "\r";
					}
				}
			}
		}
    }
	outScreen << "found " << visitedList.GetCount() << " items" << SFString(' ', 90) << "\n";

	LISTPOS lPos = visitedList.GetHeadPosition();
	while (lPos)
	{
		CWebPage *item = visitedList.GetNext(lPos);
		outScreen << unencode(item->url) << "\n";
		delete item;
	}
*/
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
		SFos::mkdir(PATH_TO_ETH_SLURP);
		SFos::mkdir(PATH_TO_SLURPS);
		return SFos::folderExists(PATH_TO_SLURPS);
	}
	return TRUE;
}
