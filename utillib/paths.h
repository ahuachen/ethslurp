#ifndef _PATHS_H_
#define _PATHS_H_
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "sffile.h"
#include "sffolder.h"
#include "visitor.h"

//-----------------------------------------------------------------------------------------------
extern SFInt32  countFilesInFolder (const SFString& path,                                   SFInt32 options=F_DEFAULT);
extern SFInt32  getFilesInFolder   (const SFString& path, SFInt32& nFiles, SFString *files, SFInt32 options=F_DEFAULT);
extern SFString listFilesInFolder  (const SFString& path,                                   SFInt32 options=F_DEFAULT);
extern SFInt32  forAllItemsInFolder(const SFString& path, APPLYFUNC func, void *data, SFInt32 options);

#endif
