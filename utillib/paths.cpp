/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "paths.h"
#include "visitor.h"

//-------------------------------------------------------------------------------
static SFBool collectFiles(CVisitor& v, void *data)
{
	SFString fileName = v.curFile.getInput();
	if (!(v.visitOpts.options&F_FULL_PATHS))
		fileName.Replace(v.mainRoot, "./");
	if (v.helperPtr)
		((SFString*)v.helperPtr)[v.nTouched++] = fileName; //full;

	return TRUE;
}

//-------------------------------------------------------------------------------
static SFBool countFiles(CVisitor& v, void *data)
{
	v.nTouched++;
	return TRUE;
}

//-------------------------------------------------------------------------------
SFInt32 countFilesInFolder(const SFString& path, SFInt32 options)
{
	CVisitOptions opts;
	opts.path    = path;
	opts.options = options;

	CVisitor visitor(opts);
	visitor.fileFunc = countFiles;
	visitor.doVisit();

	return visitor.nTouched;
}

//-------------------------------------------------------------------------------
SFInt32 getFilesInFolder(const SFString& path, SFInt32& nFiles, SFString *files, SFInt32 options)
{
	CVisitOptions opts;
	opts.path    = path;
	opts.options = options;

	CVisitor visitor(opts);
	visitor.fileFunc  = collectFiles;
	visitor.nTouched  = 0;
	visitor.helperPtr = files;

	visitor.doVisit();

	nFiles = visitor.nTouched;
	return nFiles;
}

//----------------------------------------------------------------------------------
//forEveryFileInFolder
SFInt32 forAllItemsInFolder(const SFString& path, APPLYFUNC func, void *data, SFInt32 options)
{
	SFInt32 nProcessed = 0;
    SFInt32 nFiles     = countFilesInFolder(path, options);
	if (nFiles)
	{
		SFString *files = new SFString[nFiles];
		if (files)
		{
			getFilesInFolder(path, nFiles, files, options);
			for (int i=0;i<nFiles;i++)
				nProcessed += (*func)(files[i], data);
			delete [] files;
		}
	}
	return nProcessed;
}

//-------------------------------------------------------------------------------
static SFBool appendFilename(SFString& fileName, void *data)
{
	SFString *str = (SFString*)data;
	*str += (fileName+"|");
	return TRUE;
}

//----------------------------------------------------------------------------------
SFString listFilesInFolder(const SFString& path, SFInt32 options)
{
	SFString ret;
	forAllItemsInFolder(path, appendFilename, &ret, options);
	return ret;
}
