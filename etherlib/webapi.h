#ifndef _ETH_WEB_API_
#define _ETH_WEB_API_
/*--------------------------------------------------------------------------------
 * Copyright 2016 - Great Hill Corporation.
 --------------------------------------------------------------------------------*/

#include "utillib.h"

//---------------------------------------------------------------------------------------------------
class CWebAPI
{
private:
	SFString key;
	SFString provider;
	SFString url;

public:
              CWebAPI (void);
             ~CWebAPI (void);

	SFBool    checkKey (CConfig& config);
	SFString  getKey   (void) const;
};

#endif
