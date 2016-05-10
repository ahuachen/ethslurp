#ifndef _H_FOLDERCLASS
#define _H_FOLDERCLASS
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "sffile.h"
#include "list.h"

typedef SFList<SFFile*> SFFileList;

//----------------------------------------------------------------------------------
class SFFolder : public SFFile
{
private:
	SFFileList fileList;

public:
	SFFolder(const SFString& dir);
	SFFolder(const SFFolder& folder);
	SFFolder& operator=(const SFFolder& folder);
	~SFFolder(void);
	void AddFolder(const SFString& folderName);
	void AddFile(const SFString& fileName);
	void ListFiles(void);
	void Report(CExportContext &ctx);

private:
	void Init(void);
	void Clear(void);
	void Copy(const SFFolder& folder);
};

#endif
