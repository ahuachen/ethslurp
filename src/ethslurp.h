#ifndef _ETHSLURP_H_
#define _ETHSLURP_H_
/*--------------------------------------------------------------------------------
 * Copyright 2016 - Great Hill Corporation.
 --------------------------------------------------------------------------------*/

#include "manage.h"

//---------------------------------------------------------------------------------------------------
inline SFBool isInternal(const SFString& field)
{
	return field == "schema" || field == "deleted" || field == "handle" || (testOnly && field == "confirmations");
}

#endif