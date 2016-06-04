#ifndef manage_manage_h
#define manage_manage_h
/*--------------------------------------------------------------------------------
 * Copyright 2013 - Great Hill Corporation.
 --------------------------------------------------------------------------------*/

#include "utillib.h"
#include "ethlib.h"
#include "options.h"
#include "slurp.h"
#include "transaction.h"

//---------------------------------------------------------------------------------------------------
class CSlurperApp
{
private:
	CVersion  version;
	CConfig   config;
	CWebAPI   api;

public:
	CSlurp    theSlurp;
	
              CSlurperApp (void) : config(NULL) { };
             ~CSlurperApp (void) {  }
	
	int       Initialize  (COptions& options);
	SFBool    Slurp       (COptions& options, SFString& message);
	SFBool    Display     (COptions& options, SFString& message);

private:
	void      buildDisplayStrings (COptions& options);
	SFString  getFormatString     (COptions& options, const SFString& name);
};

//--------------------------------------------------------------------------------
extern SFString getHomeFolder(void);

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
