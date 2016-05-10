/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "paths.h"

//----------------------------------------------------------------------------------
CVisitOptions::CVisitOptions(void)
{
	options = F_DEFAULT;
	// SFString prog;
	// SFString cmd;
	// SFString path;
}

//----------------------------------------------------------------------------------
CVisitOptions::CVisitOptions(const CVisitOptions& opt)
{
	options = opt.options;
	prog    = opt.prog;
	cmd     = opt.cmd;
	path    = opt.path;
}

//----------------------------------------------------------------------------------
CVisitOptions& CVisitOptions::operator=(const CVisitOptions& opt)
{
	options = opt.options;
	prog    = opt.prog;
	cmd     = opt.cmd;
	path    = opt.path;

	return *this;
}

//----------------------------------------------------------------------------------
CVisitor::CVisitor(const CVisitOptions& o)
{
	nTouched    = 0;
	helperPtr   = NULL;
	visitOpts   = o;

	setMainRoot (o.path);
	setCurRoot  (o.path);

	enterFolder = leaveFolder = fileFunc = fileCountFunc = folderCountFunc = statusFunc = NULL;
}

//----------------------------------------------------------------------------------
void CVisitor::setMainRoot(const SFString& root)
{
	mainRoot = root;
	mainRoot.ReplaceAll("\\", "/");
	if (!mainRoot.endsWith('/'))
		mainRoot += "/";
}

//----------------------------------------------------------------------------------
void CVisitor::setCurRoot(const SFString& root)
{
	curRoot = root;
	curRoot.ReplaceAll("\\", "/");
	if (!curRoot.endsWith('/'))
		curRoot += "/";
}

//----------------------------------------------------------------------------------
void CVisitor::doList(void *data, SFBool folders)
{
	SFString *items = NULL;

	SFString mask = curRoot + "*";

	SFInt32 nItems = 0;
	listItems(nItems, NULL, mask, folders);

	curFile = SFFile(mask);

	// Reporting
	if (folders && folderCountFunc && nItems)
		(folderCountFunc)(*this, &nItems);

	else if (fileCountFunc && nItems > 2)
	{
		SFInt32 n = MAX(0, nItems - 2);
		(fileCountFunc)(*this, &n); // don't count '.' and '..'
	}

	if (nItems)
	{
		items = new SFString[nItems];

		listItems(nItems, items, mask, folders);

		if (visitOpts.options & F_SORTED)
			qsort(items, nItems, sizeof(SFString), sortByStringValue);

		for (int i=0;i<nItems;i++)
		{
			if (items[i] != "." && items[i] != "..") // always ignore these
			{
				SFString curFilename = curRoot + items[i];
				curFile = SFFile(curFilename);
				curFile.setType((folders) ? 2 : 1);

				if (folders && (visitOpts.options & F_DEPTHFIRST) && (visitOpts.options & F_RECURSIVE))
				{
					SFString r = curRoot;
					setCurRoot(curFilename);
					doVisit(data);
					setCurRoot(r);

				} else
				{
					if (fileFunc && (!(SFos::isFolder(curFilename)) || (visitOpts.options&F_INCLUDE_FOLDERS)))
						(fileFunc)(*this, data);
				}
			}
		}

		if (folders && !(visitOpts.options & F_DEPTHFIRST) && (visitOpts.options & F_RECURSIVE))
		{
			for (int i=0;i<nItems;i++)
			{
				if (items[i] != "." && items[i] != "..") // always ignore these
				{
					SFString curFilename = curRoot + items[i];
					curFile = SFFile(curFilename);

					SFString r = curRoot;
					setCurRoot(curFilename);
					doVisit(data);
					setCurRoot(r);
				}
			}
		}

		if (items)
			delete [] items;
	}
}

//----------------------------------------------------------------------------------
void CVisitor::doVisit(void *data)
{
	//debugVisitOptions(visitOpts.options);

	curFile = SFFile(curRoot);

	if (enterFolder) (enterFolder)(*this, data);

	if (visitOpts.options & F_INCLUDE_FILES)
		doList(data, FALSE); // Files first

	if (visitOpts.options & F_RECURSIVE)
		doList(data, TRUE);  // Then folders

	if (leaveFolder) (leaveFolder)(*this, data);

	if (statusFunc)  (statusFunc)(*this, data);
}

