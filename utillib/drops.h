#ifndef _DROPDOWNS_H_
#define _DROPDOWNS_H_
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

// The defines are used as array indicies so do not change them
#define EDBS_NONE       ((SFInt32)-1)
#define EDBS_CAT        ((SFInt32)0)
#define EDBS_PRIO       ((SFInt32)1)
#define EDBS_LOC        ((SFInt32)2)
#define EDBS_USERDROP1  ((SFInt32)3)
#define EDBS_USERDROP2  ((SFInt32)4)
#define EDBS_USERDROP3  ((SFInt32)5)
#define EDBS_USERDROP4  ((SFInt32)6)
#define EDBS_STATUS     ((SFInt32)7)
// even the order

#define N_DROPDOWNS     ((SFInt32)8)
#define OLD_N_DROPDOWNS ((SFInt32)5)

// A shorthand assert
#ifdef _DEBUG

#define ASSERT_DROPS(idx) \
ASSERT(EDBS_CAT == 0 && EDBS_STATUS == (N_DROPDOWNS-1)); \
ASSERT(idx >= EDBS_CAT && idx <= EDBS_STATUS);

#else

#define ASSERT_DROPS(idx)

#endif

inline SFInt32 getDropFieldToken(const SFString& which)
{
	if (which % "category")  return EDBS_CAT;
	if (which % "priority")  return EDBS_PRIO;
	if (which % "location")  return EDBS_LOC;
	if (which % "status")    return EDBS_STATUS;
	if (which % "userdrop1") return EDBS_USERDROP1;
	if (which % "userdrop2") return EDBS_USERDROP2;
	if (which % "userdrop3") return EDBS_USERDROP3;
	if (which % "userdrop4") return EDBS_USERDROP4;
	return -1;
}

inline SFString getDropFieldName(SFInt32 which)
{
	if (which == EDBS_CAT)       return "category";
	if (which == EDBS_PRIO)      return "priority";
	if (which == EDBS_LOC)       return "location";
	if (which == EDBS_STATUS)    return "status";
	if (which == EDBS_USERDROP1) return "userdrop1";
	if (which == EDBS_USERDROP2) return "userdrop2";
	if (which == EDBS_USERDROP3) return "userdrop3";
	if (which == EDBS_USERDROP4) return "userdrop4";
	return EMPTY;
}

#endif
