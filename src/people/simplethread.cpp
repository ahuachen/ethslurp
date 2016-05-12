/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "peoplelib.h"
#include "simpleforum.h"

//--------------------------------------------------------------------------------------------
#define  MAX_THREADS 10000

//--------------------------------------------------------------------------------------------
CThread::CThread(SFBool initialize)
{
	index      = 0;
	parentID   = 0;
	sticky     = FALSE;
	closed     = FALSE;
	nViews     = 1;
	deleted    = FALSE;
	removed    = FALSE;

	next       = -1;
	prev       = -1;
	visited    = FALSE;

	if (initialize)
	{
		CPerson *user = getSimpleServer()->getUserDB()->getCurrentUser();
		ASSERT(user);
		subject    = user->isSuper() ? "Subject: " + Now().Format(FMT_SORTALL) : DEF_SUBJECT;
		body       = user->isSuper() ? "Default Body"                          : DEF_BODY;
		owner      = user->getUserID();
		lastEditor = user->getUserID();
		editDate   = Now();
		createDate = Now();
	}
}

//--------------------------------------------------------------------------------------------
CThread::~CThread(void)
{
}

//-----------------------------------------------------------------------------------------
SFBool CThread::isOwner(CPerson *user) const
{
	if (!user)           return FALSE;
	if (user->isSuper()) return TRUE;
	return (owner == user->getUserID());
}

//-----------------------------------------------------------------------------------------
SFString CThread::getModeLink(modeType type) const
{
	SFString idx = asString(type == MD_ADD ? 0 : index);

	CPerson *user = getSimpleServer()->getUserDB()->getCurrentUser();
	ASSERT(user);
	switch (type)
	{
		case MD_VIEW:   return (                              "`link:Forum~mode=View~thread="   + idx + "~expand=" + idx + "|View`" );
		case MD_ADD:    return (                              "`link:Forum~mode=Add~thread="    + idx                    + "|Add`"  );
		case MD_EDIT:   return ( user->isSuper()            ? "`link:Forum~mode=Edit~thread="   + idx                    + "|Edit`"   : EMPTY);
		case MD_DELETE: return ( user->isSuper()            ? "`link:Forum~mode=Delete~thread=" + idx                    + "|" + (deleted ? " Undelete" : "Delete") + "`" : EMPTY);
		case MD_REMOVE: return ( user->isSuper() && deleted ? "`link:Forum~mode=Remove~thread=" + idx                    + "|Remove`" : EMPTY);
		case MD_NONE:
			break;
		case MD_STALE:
			ASSERT(0);// should never happen
	}

	return EMPTY;
}

//--------------------------------------------------------------------------------------------
SFString CThread::getEditBody(SFBool forEdit) const
{
	return isComment() ? DEF_COMMENT : forEdit ? body.Substitute("<br>", "\n") : body;;
}

//--------------------------------------------------------------------------------------------
SFString CThread::getOwnerEmail(void) const
{
	CPerson *p = getSimpleServer()->getUserDB()->ReturnUserByID(owner);
	return (p ? p->getMainEmailNoLink() : owner);
}

//--------------------------------------------------------------------------------------------
SFString CThread::getLastEditorEmail(void) const
{
	CPerson *p = getSimpleServer()->getUserDB()->ReturnUserByID(lastEditor);
	return (p ? p->getMainEmailNoLink() : lastEditor);
}

//--------------------------------------------------------------------------------------------
typedef CNotifyClass<CThread*> CThreadNotify;

//--------------------------------------------------------------------------------------------
SFString nextThreadChunk(const SFString& fieldIn, SFBool& force, const void *data)
{
	CThreadNotify *tn     = (CThreadNotify*)data;
	const CServerBase *server = getSimpleServer();
	const CThread *thread = tn->getDataPtr();

	switch (tolower(fieldIn[0]))
	{
		case 'a':
			if ( fieldIn % "add"         ) return thread->getModeLink(MD_ADD);
			break;

		case 'b':
			if ( fieldIn % "body"        ) return thread->getEditBody(server->m_isEditing);
			break;

		case 'c':
			if ( fieldIn % "createdate"  ) return thread->createDate.Format(FMT_DEFAULT);
			if ( fieldIn % "closed"      ) return asString(thread->closed);
			if ( fieldIn % "children"    ) return thread->childList(server->m_helperStr);
			break;

		case 'd':
			if ( fieldIn % "delete"      ) return thread->getModeLink(MD_DELETE);
			if ( fieldIn % "deleted"     ) return asString(thread->deleted);
			break;

		case 'e':
			if ( fieldIn % "editdate"    ) return thread->editDate.Format(FMT_DEFAULT);
			if ( fieldIn % "edit"        ) return thread->getModeLink(MD_EDIT);
			break;

		case 'i':
			if ( fieldIn % "index"       ) return asString(thread->index);
			if ( fieldIn % "isdeleted"   ) return (thread->deleted ? "deleted" : (thread->isDeleted() ? "parent deleted" : EMPTY));
			break;

		case 'l':
			if ( fieldIn % "lasteditor"      ) return thread->lastEditor;
			if ( fieldIn % "lasteditor_email") return thread->getLastEditorEmail();
			break;

		case 'n':
			if ( fieldIn % "next"        ) return asString(thread->next);
			if ( fieldIn % "nChildren"   ) return asString(thread->GetCount());
			if ( fieldIn % "nreply"      ) return asString(thread->GetCount());
			if ( fieldIn % "nviews"      ) return asString(thread->nViews);
			break;

		case 'o':
			if ( fieldIn % "owner"       ) return thread->owner;
			if ( fieldIn % "owner_email" ) return thread->getOwnerEmail();
			break;

		case 'p':
			if ( fieldIn % "prev"        ) return asString(thread->prev);
			if ( fieldIn % "parentID"    ) return asString(thread->parentID);
			break;

		case 'r':
			if ( fieldIn % "remove"      ) return thread->getModeLink(MD_REMOVE);
			if ( fieldIn % "removed"     ) return asString(thread->removed);
			break;

		case 's':
			if ( fieldIn % "sticky"      ) return asString(thread->sticky);
			if ( fieldIn % "subject"     ) return server->m_isEditing ? thread->subject.Substitute("<br>", "\n") : thread->subject;
			break;

		case 'v':
			if ( fieldIn % "view"        ) return thread->getModeLink(MD_VIEW);
			break;
	}
	
	return EMPTY;
}

//--------------------------------------------------------------------------------------------
void CThread::fromXML(SFString& xml)
{
	index      = atoi((const char*)snagFieldClearStrip(xml, "index"      ));
	parentID   = atoi((const char*)snagFieldClearStrip(xml, "parentID"   ) );
	sticky     = atoi((const char*)snagFieldClearStrip(xml, "sticky"     ) );
	closed     = atoi((const char*)snagFieldClearStrip(xml, "closed"     ) );
	subject    =                   snagFieldClearStrip(xml, "subject"    );
	body       =                   snagFieldClearStrip(xml, "body"       ); body.ReplaceAny("\t\r", EMPTY); body.ReplaceAll("\n", "<br>");
	owner      =                   snagFieldClearStrip(xml, "owner"      );
	lastEditor =                   snagFieldClearStrip(xml, "lastEditor" );
	nViews     = atoi((const char*)snagFieldClearStrip(xml, "nViews"     ) );
	deleted    = atoi((const char*)snagFieldClearStrip(xml, "deleted"    ) );

	SetTimeAndDate(createDate, snagFieldClearStrip(xml, "createDate" ));
	SetTimeAndDate(editDate,   snagFieldClearStrip(xml, "editDate"   ));
	if (!editDate.IsValid())
		editDate = Now();
	if (!createDate.IsValid())
		createDate = editDate;
}

//--------------------------------------------------------------------------------------------
SFString CThread::Format(const SFString& fmtIn)
{
	SFString fmt = fmtIn;
	if (fmt.IsEmpty())
	{
		SFString fields = "parentID|index|sticky|closed|subject|body|createDate|owner|editDate|lastEditor|nViews|deleted";;
		while (!fields.IsEmpty())
		{
			SFString field = nextTokenClear(fields, '|');
			fmt += "\t\t<" + field + ">[{" + toUpper(field) + "}]</" + field + ">\n";
		}
		fmt = "\t<row>\n" + fmt + "\t</row>\n";
	}

	SFString ret;

	CThreadNotify tn(this);
	while (!fmt.IsEmpty())
		ret += getNextChunk(fmt, nextThreadChunk, &tn);

	return ret;
}

//-----------------------------------------------------------------------------------------
SFString CThread::childList(const SFString& fmt) const
{
	SFString ret;
	LISTPOS lPos = GetHeadPosition();
	while (lPos)
	{
		CThread *thread = GetNext(lPos);
		if (!thread->visited && (!thread->deleted || debugForum("savedel")))
		{
			thread->visited=TRUE;
			ret += thread->Format(fmt);
		}
	}
	return ret;
}

//-----------------------------------------------------------------------------------------
SFInt32 CThread::Delete(void)
{
	deleted = !deleted;
	return 1;
}

//-----------------------------------------------------------------------------------------
SFInt32 CThread::Remove(void)
{
	removed = TRUE;
	return 1;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
CThread  theThreads[MAX_THREADS];
SFInt32  nThreads  = 0;
SFInt32  nTopLevel = 0;

//--------------------------------------------------------------------------------------------
static CThread *theIndex  [MAX_THREADS];
static SFInt32  nextID    = 1;

//--------------------------------------------------------------------------------------------
SFBool CThread::isDeleted(void) const
{
	// A thread is deleted if it is deleted...
	if (deleted)
		return TRUE;

	// ...it is not deleted if it has no parent...
	if (isTopLevel())
		return FALSE;

	// ..or if it's parent does not exist...
	if (!theIndex[parentID])
		return FALSE;

	// ...but, it is deleted if one of its ancestors is deleted (this climbs to the root)
	return theIndex[parentID]->isDeleted();
}

//--------------------------------------------------------------------------------------------
SFBool CThread::isRemoved(void) const
{
	// A thread is removed if it is removed...
	if (removed)
		return TRUE;

	// ...it is not removed if it has no parent...
	if (isTopLevel())
		return FALSE;

	// ..or if it's parent does not exist...
	if (!theIndex[parentID])
		return FALSE;

	// ...but, it is removed if one of its ancestors is removed (this climbs to the root)
	return theIndex[parentID]->isRemoved();
}

static modeType forumMode = MD_STALE;
//-----------------------------------------------------------------------------------------
modeType getForumMode(void)
{
	if (forumMode==MD_STALE)
	{
		SFString mode = getString("mode");
        if (mode % "view"  ) forumMode = MD_VIEW;
		else if (mode % "add"   ) forumMode = MD_ADD;
		else if (mode % "edit"  ) forumMode = MD_EDIT;
		else if (mode % "delete") forumMode = MD_DELETE;
		else if (mode % "remove") forumMode = MD_REMOVE;
		else                      forumMode = MD_NONE;
	}
    
	return forumMode;
}

//-----------------------------------------------------------------------------------------
void setForumMode(modeType newMode)
{
	forumMode = newMode;
}

//--------------------------------------------------------------------------------------------
void clearThreads(void)
{
	// Reset the array as if it's empty
	for (int i=0;i<MAX_THREADS;i++)
		theThreads[i] = CThread();
	nThreads=0;
	getForumData("clear");
	bzero(theIndex, sizeof(theIndex));
}

//--------------------------------------------------------------------------------------------
CThread *findThread(SFInt32 id, SFBool returnNew)
{
	if (!nThreads)
		loadThreads();

	if (theIndex[id])
		return theIndex[id];

	if (!returnNew)
		return NULL;

	// Because this is static we have to re-initiate it each time it's used.
	static CThread newThread;
	newThread = CThread(TRUE);
	return &newThread;
}

//--------------------------------------------------------------------------------------------
void saveThreads(void)
{
	SFString threads;

	qsort(theThreads, nThreads, sizeof(CThread), getSortFunc("index"));

	const CSimpleUserDB *userDB    = getSimpleServer()->getUserDB();
	SFString             curUserid = ((CSimpleUserDB*)userDB)->getCurrentUser()->getUserID();
	for (int i=0;i<nThreads;i++)
	{
		CThread *thread = &theThreads[i];
		if (thread->owner     .IsEmpty()      ) thread->owner      = curUserid;
		if (thread->lastEditor.IsEmpty()      ) thread->lastEditor = curUserid;
		if (thread->editDate   == earliestDate) thread->editDate   = Now();
		if (thread->createDate == earliestDate) thread->createDate = Now();
		// fix for old data that used to store email as owner, probably no longer needed
		if (thread->owner.Contains("@"))
		{
			CPerson *p = getSimpleServer()->getUserDB()->ReturnUserByEmail(thread->owner);
			thread->owner = (p ? p->getUserID() : curUserid);
		}
		if (thread->lastEditor.Contains("@"))
		{
			CPerson *p = getSimpleServer()->getUserDB()->ReturnUserByEmail(thread->lastEditor);
			thread->lastEditor = (p ? p->getUserID() : curUserid);
		}
	}

    // NOTE: In each of the following loops, do not use isDeleted or isRemoved. Use the actual raw data. isDeleted and isRemoved check parents'

	threads = "\n\t<!-- top level -->\n";
	for (int i=0;i<nThreads;i++)
	{
		if (!theThreads[i].removed && !theThreads[i].deleted && theThreads[i].isTopLevel())
			threads += theThreads[i].Format(EMPTY);
	}

	threads += "\n\t<!-- comments -->\n";
	for (int i=0;i<nThreads;i++)
	{
		if (!theThreads[i].removed && !theThreads[i].deleted && theThreads[i].isComment())
		{
			SFString str = theThreads[i].Format(EMPTY);
			str.Replace(DEF_COMMENT, EMPTY);
			threads += str;
		}
	}

	if ( debugForum("savedel") )
	{
		threads += "\n\t<!-- deleted -->\n";
		for (int i=0;i<nThreads;i++)
		{
			if (!theThreads[i].removed && theThreads[i].deleted)
				threads += theThreads[i].Format(EMPTY);
		}
	}

	CSharedResource file;
    if (file.Lock(pathToDatabase("forums.xml"), asciiWriteCreate, LOCK_WAIT))
	{
        file.WriteLine("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		file.WriteLine("<rows>\n");
		file.WriteLine("\t<nextID>"+asString(nextID)+"</nextID>\n");
		file.WriteLine(threads);
		file.WriteLine("</rows>\n");
		file.Close();
    }

	clearThreads();
	loadThreads();
}

//--------------------------------------------------------------------------------------------
SFString getForumData(const SFString& valName)
{
	static SFString forumData;
	if (valName == "clear")
		forumData = EMPTY;
	if (forumData.IsEmpty())
		forumData = getDatabaseData("forums.xml");
	return snagField(forumData, valName);
}

//--------------------------------------------------------------------------------------------
SFInt32 getNextID(void)
{
//#error - this is not correct. It needs to allocate the next id and store that value back into the file.
    return ++nextID;
}

//--------------------------------------------------------------------------------------------
SFInt32 loadThreads(void)
{
	// handle sorting
	if (!getString("sort").IsEmpty())
	{
		expireCookie("page");
		SFString currentSort = toLower(getCookieString("sort"));
		SFString newSort     = getString("sort");
		if (newSort % currentSort)
			toggleCookie("reverse", CK_SESSION);
		else
			expireCookie("reverse");
	}

	// reset visits
	for (int i=0;i<nThreads;i++)
		theThreads[i].visited=FALSE;

	// we only reload the threads if we've never read them or they've been cleared (during save for example)
	if (nThreads)
		return nThreads;

	clearThreads();

	SFString threadData = getForumData("rows");
	if (threadData.Contains("<nextID"))
		nextID = atoi((const char*)snagFieldClearStrip(threadData, "nextID"));

	while (threadData.Contains("<row"))
	{
		SFString theString = snagFieldClearStrip(threadData, "row");

		CThread *thread = &theThreads[nThreads];
		thread->fromXML(theString);
		nThreads++;
	}

	qsort(theThreads, nThreads, sizeof(CThread), getSortFunc());

	for (int i=0;i<nThreads;i++)
	{
		theThreads[i].prev = (i==0          ? theThreads[nThreads-1].index : theThreads[i-1].index);
		theThreads[i].next = (i==nThreads-1 ? theThreads[0].index          : theThreads[i+1].index);
	}

	for (int i=0;i<nThreads;i++)
	{
		CThread *thread = &theThreads[i];
		theIndex[thread->index] = thread;
	}

	nTopLevel=0;
	for (int i=0;i<nThreads;i++)
	{
		CThread *thread = &theThreads[i];
		if (thread->isComment())
		{
			if (theIndex[thread->parentID])
				theIndex[thread->parentID]->AddTail(thread);

		} else
		{
			nTopLevel++;
		}
	}

	return nThreads;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// Sorting....
//--------------------------------------------------------------------------------------------
#define REVERSE(a) \
{ \
	if (getCookieInt32("reverse", FALSE)) \
		return int(-1*(a)); \
	return int(a); \
}

//--------------------------------------------------------------------------------------------
#define CHECK_COMMENT(t1, t2) \
	if (t1->isComment() && t2->isComment()) \
		return sortByComment(t1, t2); \
	else if (t1->isComment()) \
		return 1; \
	else if (t2->isComment()) \
		return -1;
		
//--------------------------------------------------------------------------------------------
extern int sortByIndex          (const void *r1, const void *r2);
extern int sortByComment        (const void *r1, const void *r2);
extern int sortBySubject        (const void *r1, const void *r2);
extern int sortByLastEditDate   (const void *r1, const void *r2);
extern int sortBynViews         (const void *r1, const void *r2);
extern int sortBynReplies       (const void *r1, const void *r2);
extern int sortByOwnerEmail     (const void *r1, const void *r2);
extern int sortByLastEditorEmail(const void *r1, const void *r2);

//--------------------------------------------------------------------------------------------
int sortByComment(const void *r1, const void *r2)
{
	const CThread *t1 = (CThread*)r1;
	const CThread *t2 = (CThread*)r2;
	return (int)(t1->index - t2->index);
}

//--------------------------------------------------------------------------------------------
int sortByIndex(const void *r1, const void *r2)
{
	const CThread *t1 = (CThread*)r1;
	const CThread *t2 = (CThread*)r2;
	CHECK_COMMENT(t1, t2);
	REVERSE(t1->index - t2->index);
}

//--------------------------------------------------------------------------------------------
int sortBySubject(const void *r1, const void *r2)
{
	const CThread *t1 = (CThread*)r1;
	const CThread *t2 = (CThread*)r2;
	CHECK_COMMENT(t1, t2);

	int ret = (int)t1->subject.Compare(t2->subject);
	if (ret)
		REVERSE(ret);
	return sortByIndex(r1, r2);
}

//--------------------------------------------------------------------------------------------
int sortByLastEditDate(const void *r1, const void *r2)
{
	const CThread *t1 = (CThread*)r1;
	const CThread *t2 = (CThread*)r2;
	CHECK_COMMENT(t1, t2);

	if (t1->editDate > t2->editDate) REVERSE(-1);
	if (t2->editDate > t1->editDate) REVERSE( 1);
	return sortBySubject(r1, r2);
}

//--------------------------------------------------------------------------------------------
int sortBynViews(const void *r1, const void *r2)
{
	const CThread *t1 = (CThread*)r1;
	const CThread *t2 = (CThread*)r2;
	CHECK_COMMENT(t1, t2);

	int ret = int(t2->nViews - t1->nViews);
	if (ret)
		REVERSE(ret);
	return sortBySubject(r1, r2);
}

//--------------------------------------------------------------------------------------------
int sortBynReplies(const void *r1, const void *r2)
{
	const CThread *t1 = (CThread*)r1;
	const CThread *t2 = (CThread*)r2;
	CHECK_COMMENT(t1, t2);

	int ret = int(t2->GetCount() - t1->GetCount());
	if (ret)
		REVERSE(ret);
	return sortBySubject(r1, r2);
}

//--------------------------------------------------------------------------------------------
int sortByOwnerEmail(const void *r1, const void *r2)
{
	const CThread *t1 = (CThread*)r1;
	const CThread *t2 = (CThread*)r2;
	CHECK_COMMENT(t1, t2);

	int ret = int(t1->getOwnerEmail().Compare(t2->getOwnerEmail()));
	if (ret)
		REVERSE(ret);
	return sortBySubject(r1, r2);
}

//--------------------------------------------------------------------------------------------
int sortByLastEditorEmail(const void *r1, const void *r2)
{
	const CThread *t1 = (CThread*)r1;
	const CThread *t2 = (CThread*)r2;
	CHECK_COMMENT(t1, t2);

	int ret = int(t1->getLastEditorEmail().Compare(t2->getLastEditorEmail()));
	if (ret)
		REVERSE(ret);
	return sortBySubject(r1, r2);
}

//--------------------------------------------------------------------------------------------
SORTINGFUNC getSortFunc(const SFString& force)
	{
	SFString sortBy = toLower(force.IsEmpty() ? getSetCookieString("sort", "Date", CK_SESSION) : force);
    switch (sortBy[0])
	{
        case 'd':
            if (sortBy % "date")       return sortByLastEditDate;
            break;
        case 'e':
            if (sortBy % "editor")     return sortByLastEditorEmail;
            break;
        case 'i':
            if (sortBy % "index")      return sortByIndex;
            break;
        case 'n':
            if (sortBy % "nviews")     return sortBynViews;
            if (sortBy % "nreply")     return sortBynReplies;
            break;
        case 'o':
            if (sortBy % "originator") return sortByOwnerEmail;
            break;
        case 's':
            if (sortBy % "subject")    return sortBySubject;
            break;
	}

	// default to index
	return sortByIndex;
}
