#ifndef _STRING_TABLE_H_
#define _STRING_TABLE_H_
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

inline SFString LoadStringGH     (const char *in) { return SFString(in); }
extern SFString LoadStringGH     (SFInt32 id);
extern void     clearStrings     (void);

#endif
