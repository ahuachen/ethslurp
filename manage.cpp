/*--------------------------------------------------------------------------------
 * Copyright 2013 - Great Hill Corporation.
 --------------------------------------------------------------------------------*/

#include "manage.h"
#include "ethslurp.h"

//--------------------------------------------------------------------------------
CCmdFunction funcs[] =
{
	CCmdFunction( "ethslurp",  cmdEthSlurp,  CSlurperApp::params,  CSlurperApp::nParams ),
};
SFInt32 nFuncs = sizeof(funcs) / sizeof(CCmdFunction);

//--------------------------------------------------------------------------------
int main(int argc, const char * argv[])
{
	CTransaction::registerClass();

	//
	// Invoked with 'manage cmdName args'
	//
	// Shift the cmdName to arg[0] so called programs think they are called
	// from the command line. Shift each argument as well. Handle args
	// common to all commands (such as -h)

	CCmdFunction *cmdFunc = findCommand(argv[1]);
	if (!cmdFunc)
	{
		// The command was not found. Report and quit.
		outErr << "\n";
		outErr << " Usage: manage [ ";
		for (int i=0;i<nFuncs;i++)
			outErr << funcs[i].functionName << (i < nFuncs-1 ? " | " : "");
		outErr << " ]\nCompiled: " + SFString(__DATE__) + " : " + SFString(__TIME__) + "\n\n";
		return 0;
	}

	SFInt32   nArgs = 0;
	SFString *arguments = new SFString[argc+2]; // play it safe
	for (int i=0;i<argc;i++)
	{
		SFString arg = argv[i];
		if (arg=="-h" || arg=="-help" || arg=="--help")
		{
			usage(arguments[0]);
			delete [] arguments;
			return 0;

		} else if (arg.startsWith("-v") || arg.startsWith("-verbose"))
		{
			verbose = TRUE;
			arg.Replace("-verbose",EMPTY);
			arg.Replace("-v",      EMPTY);
			arg.Replace(":",       EMPTY);
			if (!arg.IsEmpty())
				verbose = atoi((const char*)arg);

		} else if (arg=="-t" || arg=="-test" || arg=="-titles")
		{
			isTesting = TRUE;

		} else
		{
			arguments[nArgs++] = arg;
		}
	}

	int ret = (int)(*cmdFunc->cFunc)(nArgs, arguments);
	delete [] arguments;
	return ret;
}

//--------------------------------------------------------------------------------
int usage(const SFString& cmd, const SFString& errMsg)
{
	CCmdFunction *cmdFunc = findCommand(cmd);
	if (cmdFunc)
	{
		qsort(cmdFunc->params, cmdFunc->nParams, sizeof(CParams), sortCommand);
		outErr << "\n" << (!errMsg.IsEmpty() ? "  " + errMsg + "\n\n" : "");
		outErr << "  Usage:   " + cmd + " " << cmdFunc->options() << "\n";
		outErr << cmdFunc->purpose();
		outErr << cmdFunc->descriptions() << "\n";
		outErr << "  Portions of this software are Powered by Etherscan.io APIs\n\n";
	} else
	{
		outErr << "Command '" << cmd << "' not found.\n";
	}

	return RETURN_FAIL;
}

//--------------------------------------------------------------------------------
CCmdFunction *findCommand(const SFString& cmd)
{
	return &funcs[0];
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
SFString CCmdFunction::options(void) const
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
SFString CCmdFunction::purpose(void) const
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
SFString CCmdFunction::descriptions(void) const
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
int sortCommand(const void *c1, const void *c2)
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
