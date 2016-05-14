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
extern SFString getDisplayFormat(SFBool prettyPrint);
extern CVersion version;

//---------------------------------------------------------------------------------------------------
inline SFBool isInternal(const SFString& field)
{
	return field == "schema" || field == "deleted" || field == "handle" || (testOnly && field == "confirmations");
}

#endif