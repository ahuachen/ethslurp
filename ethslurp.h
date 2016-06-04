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
	CSlurp    theAccount;

              CSlurperApp (void) : config(NULL) { };
             ~CSlurperApp (void) {  }

	SFBool    Initialize  (COptions& options, SFString& message);
	SFBool    Slurp       (COptions& options, SFString& message);
	SFBool    Filter      (COptions& options, SFString& message);
	SFBool    Display     (COptions& options, SFString& message);

private:
	void      buildDisplayStrings (COptions& options);
	SFString  getFormatString     (COptions& options, const SFString& name);
};

//--------------------------------------------------------------------------------
extern SFString getHomeFolder(void);

//---------------------------------------------------------------------------------------------------
extern SFBool establishFolders(CConfig& config, const SFString& vers);
extern void   findBlockRange  (const SFString& contents, SFInt32& minBlock, SFInt32& maxBlock);

//---------------------------------------------------------------------------------------------------
extern SFString configPath(const SFString& part=EMPTY);
extern SFString cachePath (const SFString& part=EMPTY);

//--------------------------------------------------------------------------------
extern SFBool verbose;
extern SFBool isTesting;
extern CFileExportContext  outScreen;
extern CFileExportContext& outErr;

#endif
