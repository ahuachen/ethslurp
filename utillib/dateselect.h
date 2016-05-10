#ifndef _DATESELECT_H
#define _DATESELECT_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "html.h"
#include "dates.h"
#include "fielddata.h"

extern SFBool hourChoose12    (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool hourChoose24    (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool hourChoose24_24 (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool minChoose       (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool directionChoose (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool ampmChoose      (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);

//--------------------------------------------------------------------------------------
// Select the various components of the date selectors
#define DS_DAYDROP     (1<<4)
#define DS_MONTHDROP   (1<<5)
#define DS_YEARDROP    (1<<6)
#define DS_ALLDROPS (DS_DAYDROP|DS_MONTHDROP|DS_YEARDROP)

SFString getDateChooser   (const SFString& fieldName, const SFTime& value, const SFString& func, SFInt32 vt);
SFString getColorChooser  (const SFString& fieldName);
SFString getIconChooser   (const SFString& fieldName);

#endif
