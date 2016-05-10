#ifndef manage_manage_h
#define manage_manage_h
/*--------------------------------------------------------------------------------
 * Copyright 2013 - Great Hill Corporation.
 --------------------------------------------------------------------------------*/

#include "utillib/utillib.h"

//--------------------------------------------------------------------------------
typedef SFInt32 (*CMDFUNC)  (SFInt32 nArgs, const SFString *args);

//--------------------------------------------------------------------------------
typedef SFInt32 (*USAGEFUNC)(const SFString& errMsg);

//--------------------------------------------------------------------------------
class CParams
{
public:
	SFString  shortName;
	SFString  longName;
	SFString  description;
	CParams( const SFString& name, const SFString& descr );
};

//--------------------------------------------------------------------------------
class CCmdFunction
{
	CCmdFunction( void ) {}
public:
	SFString    functionName;
	CMDFUNC     cFunc;
	CParams    *params;
	SFInt32     nParams;
	CCmdFunction (const SFString& functionNameIn, CMDFUNC cFuncIn, CParams *paramsIn, SFInt32 nParamsIn)
	{
		functionName = functionNameIn;
		cFunc        = cFuncIn;
		params       = paramsIn;
		nParams      = nParamsIn;
	}
	SFString options     (void) const;
	SFString descriptions(void) const;
	SFString purpose     (void) const;
};

//--------------------------------------------------------------------------------
#define DEFINE_COMMAND(cmdName) extern SFInt32 cmd##cmdName(SFInt32 nArgs,const SFString *args); extern CParams params##cmdName[]; extern SFInt32 nParams##cmdName;

//--------------------------------------------------------------------------------
DEFINE_COMMAND(EthSlurp);

//--------------------------------------------------------------------------------
int           usage       (const SFString& cmd, const SFString& errMsg=nullString);
CCmdFunction *findCommand (const SFString& cmd);
int           sortCommand (const void *c1, const void *c2);

//--------------------------------------------------------------------------------
extern SFBool verbose;
extern SFBool testOnly;
extern CFileExportContext  outScreen;
extern CErrorExportContext outErr;

//--------------------------------------------------------------------------------
// Return codes for functions
#define RETURN_OK   TRUE
#define RETURN_FAIL FALSE

#endif
