#ifndef _AFFIL_H
#define _AFFIL_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------------
// Do not ever change these.  Installed base depends on this.  These are released in the wild
#define AFF_SERVER      1 // Destinations
#define AFF_FEATURES    2
#define AFF_PRICES      3
#define AFF_DOWNLOAD    4
#define AFF_FAQ         5
#define AFF_UPDATES     6
#define AFF_STORE       7
#define AFF_HOSTING     8
#define AFF_STORE_H     9
#define AFF_SAMPLES    15
#define AFF_SAMPLES_H  16
#define AFF_PROGRAM    17
#define AFF_VRANIML    18
#define AFF_SERVER2    20
// Do not ever change these.
#define AFF_HEADER      7 // Sources
#define AFF_FOOTER      8
#define AFF_ERROR       9
#define AFF_OVERFLOW   10
#define AFF_INSTALL    11
#define AFF_POWER_BY   12
#define AFF_REGISTER   13
#define AFF_ABOUT      14
#define AFF_EMAIL      15
#define AFF_SIGHT_LIC  18
#define AFF_HELP       19

#define AFF_SPIDER_D   25 // dest
#define AFF_SPIDER_S   26 // source

// Do not ever change these.  Installed base depends on this.  These are released in the wild
#define AFF_UNKNOWN    100
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
inline SFString getAffiliateLink(SFInt32 dest, SFInt32 from, SFBool isDemo)
{
	return
		SFString("http://www.calendarhost.com/cgi-bin/affiliate.cgi")         +
		SFString("?dest=" + asString( dest   ) + "&from=" + asString( from )) +
		SFString("&demo=" + asString( isDemo ));
}

#endif
