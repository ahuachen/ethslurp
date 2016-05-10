#ifndef _H_VISITORCLASS
#define _H_VISITORCLASS
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "paths.h"

//----------------------------------------------------------------------------------
class CVisitOptions
{
public:
	SFString prog;
	SFString cmd;
	SFString path;
	SFInt32  options;

	CVisitOptions(void);
	CVisitOptions(const CVisitOptions& opt);
	CVisitOptions& operator=(const CVisitOptions& opt);
};

//----------------------------------------------------------------------------------
class CVisitor;
typedef SFBool (*VISTORFUNC)(CVisitor& v, void *data);

//----------------------------------------------------------------------------------
class CVisitor
{
public:
	SFString            curRoot;
	SFString            mainRoot;

	SFFile              curFile;
	CVisitOptions       visitOpts;

	VISTORFUNC          enterFolder;
	VISTORFUNC          leaveFolder;
	VISTORFUNC          fileFunc;
	VISTORFUNC          fileCountFunc;
	VISTORFUNC          folderCountFunc;
	VISTORFUNC          statusFunc;

	SFInt32             nTouched;
	void               *helperPtr;
	SFString            retStr;

	         CVisitor   (const CVisitOptions& o);
	void     doVisit    (void *data=NULL);

private:
	void     setMainRoot(const SFString& root);
	void     setCurRoot (const SFString& root);
	void     doList     (void *data=NULL, SFBool folders=FALSE);

	CVisitor(const CVisitor& v) {}
	CVisitor& operator=(const CVisitor& v) { return *this; }
};

//-------------------------------------------------------------------------------
class CVisitCollector
{
public:
	SFInt32        nFiles;
	SFString      *files;
	SFString       retStr;
	CVisitOptions *opts;
	CVisitCollector(SFInt32 n, SFString *f, CVisitOptions *o)
	{
		opts   = o;
		nFiles = n;
		files  = f;
	}
};

//----------------------------------------------------------------------------------
#define F_INCLUDE_FOLDERS ( 1<<1 )
#define F_INCLUDE_FILES   ( 1<<2 )
#define F_FULL_PATHS      ( 1<<3 )
#define F_RECURSIVE       ( 1<<4 )
#define F_DEPTHFIRST      ( 1<<5 )
#define F_SORTED          ( 1<<6 )
#define F_INCLUDE_ALL     ( F_INCLUDE_FOLDERS | F_INCLUDE_FILES )
#define F_DEFAULT         ( F_INCLUDE_ALL     | F_RECURSIVE     | F_SORTED )
#define F_FILES_ONLY      ( F_INCLUDE_FILES   | F_RECURSIVE     | F_SORTED )
#define F_FOLDERS_ONLY    ( F_INCLUDE_FOLDERS | F_RECURSIVE     | F_SORTED )

//----------------------------------------------------------------------------------
inline void debugVisitOptions(SFInt32 opts)
{
	XX_B(opts&F_INCLUDE_FOLDERS);
	XX_B(opts&F_INCLUDE_FILES);
	XX_B(opts&F_FULL_PATHS);
	XX_B(opts&F_RECURSIVE);
	XX_B(opts&F_DEPTHFIRST);
	XX_B(opts&F_SORTED);
}

#endif
