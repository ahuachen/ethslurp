#ifndef _ETHSLURP_H_
#define _ETHSLURP_H_
/*--------------------------------------------------------------------------------
 * Copyright 2016 - Great Hill Corporation.
 --------------------------------------------------------------------------------*/
#include "manage.h"
#include "slurp.h"

//---------------------------------------------------------------------------------------------------
class CSlurperApp : public CApplication
{
public:
	SFString  api_key;
	SFString  api_provider;
	SFString  api_url;
	CSlurp    theSlurp;

	         CSlurperApp(void) : CApplication() { };
        	~CSlurperApp(void) {  }

	void     buildDisplayStrings (void);
	SFString getFormatString     (const SFString& name);
	
	SFBool Slurp  (SFString& message);
	SFBool Display(SFString& message);
};

//---------------------------------------------------------------------------------------------------
extern SFBool establish_folders(CConfig& config, const SFString& vers);
extern void   findBlockRange   (const SFString& contents, SFInt32& minBlock, SFInt32& maxBlock);

//---------------------------------------------------------------------------------------------------
#define PATH_TO_ETH_SLURP  SFString(getHomeFolder() + ".ethslurp" + (isTesting?".test":EMPTY) + "/")
#define PATH_TO_SLURPS     SFString(PATH_TO_ETH_SLURP + "slurps/")

//---------------------------------------------------------------------------------------------------
inline SFBool isInternal(const SFString& field)
{
	return field == "schema" || field == "deleted" || field == "handle" || (isTesting && field == "confirmations");
}

#endif
