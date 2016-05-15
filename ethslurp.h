#ifndef _ETHSLURP_H_
#define _ETHSLURP_H_
/*--------------------------------------------------------------------------------
 * Copyright 2016 - Great Hill Corporation.
 --------------------------------------------------------------------------------*/

#include "manage.h"

//---------------------------------------------------------------------------------------------------
#define PATH_TO_HOME       getHomeFolder()
#define PATH_TO_ETH_SLURP  SFString(getHomeFolder() + ".ethslurp/")
#define PATH_TO_SLURPS     SFString(PATH_TO_ETH_SLURP + "slurps/")

//--------------------------------------------------------------------------------
extern SFBool   establish_folders(CConfig *config);
extern SFString getHomeFolder(void);
extern SFString getDisplayString(SFBool prettyPrint, const SFString& exportFormat, const CConfig& config, SFString& header);
extern CVersion version;

//---------------------------------------------------------------------------------------------------
inline SFBool isInternal(const SFString& field)
{
	return field == "schema" || field == "deleted" || field == "handle" || (testOnly && field == "confirmations");
}

#endif