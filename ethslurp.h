#ifndef manage_manage_h
#define manage_manage_h
/*--------------------------------------------------------------------------------
 * Copyright 2013 - Great Hill Corporation.
 --------------------------------------------------------------------------------*/

#include <iostream>
#include "utillib.h"
#include "options.h"
#include "slurp.h"
#include "transaction.h"
#include "limits.h"

using namespace std;

//---------------------------------------------------------------------------------------------------
class CSlurperApp
{
public:
	CVersion  version;
	CConfig   config;

	static    CParams params[];
	static    SFInt32 nParams;
	
	SFString  api_key;
	SFString  api_provider;
	SFString  api_url;
	CSlurp    theSlurp;
	
	CSlurperApp(void) : config(NULL) { };
	~CSlurperApp(void) {  }
	
	void     buildDisplayStrings (COptions& options);
	SFString getFormatString     (COptions& options, const SFString& name);
	
	int    Init   (COptions& options);
	SFBool Slurp  (COptions& options, SFString& message);
	SFBool Display(COptions& options, SFString& message);
};

//--------------------------------------------------------------------------------
extern int           usage        (const SFString& errMsg=nullString);
extern int           sortCommand  (const void *c1, const void *c2);
extern SFString      getHomeFolder(void);

//--------------------------------------------------------------------------------
// Return codes for functions
#define RETURN_OK   TRUE
#define RETURN_FAIL FALSE

//---------------------------------------------------------------------------------------------------
extern SFBool establish_folders(CConfig& config, const SFString& vers);
extern void   findBlockRange   (const SFString& contents, SFInt32& minBlock, SFInt32& maxBlock);

//---------------------------------------------------------------------------------------------------
#define PATH_TO_ETH_SLURP  SFString(getHomeFolder() + ".ethslurp" + (isTesting?".test":EMPTY) + "/")
#define PATH_TO_SLURPS     SFString(PATH_TO_ETH_SLURP + "slurps/")

//--------------------------------------------------------------------------------
extern SFBool verbose;
extern SFBool isTesting;
extern CFileExportContext  outScreen;
extern CFileExportContext& outErr;

#endif
