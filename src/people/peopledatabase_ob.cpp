/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "peopledatabase.h"
#include "protectedobject.h"
#include "person.h"

//----------------------------------------------------------------------------------------
static SFBool isFrontPageFile(const SFString& filename)
{
	return ((filename.Left(5) % "_vti_"   ) || 
		(filename.Left(8) % "_derived") || 
		(filename.Left(8) % "_overlay") ||
		(filename         % "_notes"  ));
}

//----------------------------------------------------------------------------------------
static void getInternalObjectData(SFString& obName, SFString& obGroup, SFInt32& defPerm, SFInt32& minPerm, SFInt32& fallPerm, const SFString& base, const SFString& dirName, const SFString& fn)
{
	if (base.IsEmpty())
		return;

	ASSERT(base.Find("\\") == -1);
	ASSERT(base.endsWith('/'));
	
	SFString filename = base + fn;
	if (dirName != "default")
	{
		ASSERT(dirName.Find("\\") == -1);
		ASSERT(!dirName.endsWith('/'));
		filename = base + dirName + "/" + fn;
	}
	
	CConfigReadOnly config;
	config.loadFile(filename); // if it fails it reverts to default
	obName   = config.GetProfileStringGH("PAGE", "calendar_name",  dirName);
	obGroup  = config.GetProfileStringGH("PAGE", "calendar_group", EMPTY);
	defPerm  = config.GetProfileIntGH   ("PAGE", "default_perms",  PERMISSION_VIEW);
	minPerm  = config.GetProfileIntGH   ("PAGE", "min_perms",      PERMISSION_NONE);
	fallPerm = config.GetProfileIntGH   ("PAGE", "min_fallback",   PERMISSION_VIEW);
	config.Release();

	return;
}

//-------------------------------------------------------------------------
void CUserDatabase::LoadObjectTable(void)
{
//	ASSERT(!m_curObject.IsEmpty());
//	ASSERT(!m_dataPath.IsEmpty());

	// Rebuild each time
	m_nObjects   = 0;
	if (m_objects)
	{
		delete [] m_objects;
		m_objects = NULL;
	}

	SFos::listFolders(m_nObjects, NULL, m_dataPath + "*");
	m_nObjects++; // the default object

	ASSERT(m_nObjects);
	m_objects = new CProtectedObject[m_nObjects];
	ASSERT(m_objects);
	
	m_objects[0].setObjectID("default");
				
	// Get the folder names and copy them over
	SFInt32 nDirs=m_nObjects-1;
	if (nDirs>0)
	{
		SFString *folders = new SFString[nDirs+2]; // leave room just in case
		if (folders)
		{
			SFos::listFolders(nDirs, folders, m_dataPath + "*");
			for (int i=0;i<nDirs;i++)
				m_objects[i+1].setObjectID(folders[i]);
			delete [] folders;
		}
	}

	for (int i=0;i<m_nObjects;i++)
	{
		CProtectedObject *ob = &m_objects[i];
		
		// Get the full name of the object
		SFString obName;
		SFString obGroup;
		SFInt32  defPerm, minPerm, fallPerm;
		getInternalObjectData(obName, obGroup, defPerm, minPerm, fallPerm, m_dataPath, m_objects[i].getObjectID(), "calweb.ini");

		obName = Strip(obName, '\n');
		ob->setName       (obName  );
		ob->setGroup      (obGroup );
		ob->setDefaultPerm(defPerm );
		ob->setMinPerm    (minPerm );
		ob->setFallback   (fallPerm);

		SFString deletedFile  = m_dataPath + ob->getObjectID() + "/" + DELETEDFILENAME;
		SFString archivedFile = m_dataPath + ob->getObjectID() + "/" + ARCHIVEFILENAME;
		SFString hiddenFile   = m_dataPath + ob->getObjectID() + "/" + HIDEFILENAME;
		SFString compactFile  = m_dataPath + ob->getObjectID() + "/" + COMPACTFILENAME;

		ob->setDeleted(SFos::fileExists(deletedFile));
		if (!ob->isDeleted())
			ob->setHidden(SFos::fileExists(hiddenFile));

		// As of 4.6.2 we removed archive folders. So we disappear them here entirely
		if (SFos::fileExists(archivedFile))
		{
			CSharedResource lock;
			if (lock.Lock(compactFile, asciiWriteCreate, LOCK_CREATE))
			{
				lock.WriteLine(LoadStringGH("You may remove this folder. It is no longer used."));
				lock.Release();
			}
		}
	}

	// Trow away any FrontPage files or folders that have been deleted but not removed
	SFInt32 cnt = 0;
	for (int k=0;k<m_nObjects;k++)
	{
		CProtectedObject *ob = &m_objects[k];
		SFString compactFile = m_dataPath + ob->getObjectID() + "/" + COMPACTFILENAME;

		if (!isFrontPageFile(ob->getObjectID()) && !SFos::fileExists((const char *)compactFile))
			m_objects[cnt++] = *ob;
	}
	m_nObjects = cnt;

	// Sort them, leaving the default one where it is (don't change this - the calendar manager depends on it being this way)
	if (m_nObjects > 2)
		qsort(&m_objects[1], m_nObjects-1, sizeof(CProtectedObject), sortByObjectName);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFBool CUserDatabase::FindObjectByID(CProtectedObject& object, const SFString& objectid, SFBool type) const
{
	// Note: This linear search would be better as a binary search, particularly as nObjects grows
	int i=0;
	for (i=0;i<m_nObjects;i++)
	{
		CProtectedObject *ob = &m_objects[i];
		if (ob->getObjectID() % objectid)
		{
			SFBool ok = TRUE;
			if (ob->isDeleted())
				ok = (type & DELETED_ALSO);
			if (ob->isHidden())
				ok = (type & HIDDEN_ALSO);

			if (ok)
				object = *ob;

			return ok;
		}
	}

	// Should never happen
	return FALSE;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFInt32 CUserDatabase::nDeletedObjects(void) const
{
	ASSERT(m_nObjects==0 || m_objects);
	
	SFInt32 cnt = 0;
	int i=0;
	for (i=0;i<m_nObjects;i++)
		if (m_objects[i].isDeleted())
			cnt++;

	return cnt;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFInt32 CUserDatabase::nHiddenObjects(void) const
{
	ASSERT(m_nObjects==0 || m_objects);
	
	SFInt32 cnt = 0;
	int i=0;
	for (i=0;i<m_nObjects;i++)
		if (m_objects[i].isHidden())
			cnt++;

	return cnt;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
SFInt32 CUserDatabase::nLicensesLeft(void) const
{
	ASSERT(m_nObjects==0 || m_objects);
	return m_registration.nRegistered() - nAccessibleObjects();
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
CProtectedObject *CUserDatabase::getObjectAt(SFInt32 index) const
{
	if (m_objects && index >=0 && index < m_nObjects)
		return &m_objects[index];
	
	// should never happen
	ASSERT(0);
	return NULL;
}
