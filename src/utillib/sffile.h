#ifndef _H_FILECLASS
#define _H_FILECLASS
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "exportcontext.h"

//----------------------------------------------------------------------------------
class SFFile
{
	SFString path;
	SFString fileName;
	SFString input;
	SFInt32  type;
	SFInt32  size;

public:
	SFFile(void);
	SFFile(const SFString& fileName);
	SFFile(const SFFile& file);
	SFFile& operator=(const SFFile& file);
	virtual ~SFFile(void);
	SFString getPath(void) const;
	SFString getFilename(void) const;
	SFString getInput(void) const;
	SFInt32  getSize(void) const;
	SFInt32  getType(void) const;
	void setType(SFInt32 t);
	virtual void Report(CExportContext& ctx);

private:
	void Init(void);
	void Clear(void);
	void Copy(const SFFile& file);

	friend class SFFolder;
};

#endif
