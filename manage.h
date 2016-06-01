#ifndef manage_manage_h
#define manage_manage_h
/*--------------------------------------------------------------------------------
 * Copyright 2013 - Great Hill Corporation.
 --------------------------------------------------------------------------------*/

#include "utillib.h"
#include "options.h"
#include "transaction.h"

//--------------------------------------------------------------------------------
class CApplication
{
public:
	static    CParams params[];
	static    SFInt32 nParams;
	COptions  opt;
	CVersion  version;
	CConfig   config;
	CApplication(void) : config(NULL) { nParams = 0; }
};

//--------------------------------------------------------------------------------
extern int           usage        (const SFString& cmd, const SFString& errMsg=nullString);
extern CCmdFunction *findCommand  (const SFString& cmd);
extern int           sortCommand  (const void *c1, const void *c2);
extern SFString      getHomeFolder(void);

//--------------------------------------------------------------------------------
extern SFBool verbose;
extern SFBool isTesting;
extern CFileExportContext  outScreen;
extern CFileExportContext& outErr;

//--------------------------------------------------------------------------------
// Return codes for functions
#define RETURN_OK   TRUE
#define RETURN_FAIL FALSE

//--------------------------------------------------------------------------------
extern SFInt32 cmdEthSlurp(SFInt32 nArgs,const SFString *args);

//--------------------------------------------------------------------------------
#define LARGEST_LONG 2147483647

//--------------------------------------------------------------------------------
#include <iostream>
using namespace std;

#endif
