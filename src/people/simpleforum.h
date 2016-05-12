#ifndef library_threads_h
#define library_threads_h
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "simpleserver.h"
#include "simplepage.h"

//-----------------------------------------------------------------------------------------
enum modeType {
	MD_STALE    = -1,
	MD_NONE     =  0,
	MD_VIEW     =  1,
	MD_ADD      =  2,
	MD_EDIT     =  3,
	MD_DELETE   =  4,
	MD_REMOVE   =  5
};

//--------------------------------------------------------------------------------------------
class CThread;
typedef SFList<CThread*> CThreadList;

//--------------------------------------------------------------------------------------------
#define DEF_SUBJECT SFString( "<Your subject>" )
#define DEF_BODY    SFString( "<Your body>"    )
#define DEF_COMMENT SFString( "<Comment body may not be edited>" )

//--------------------------------------------------------------------------------------------
class CThread : public CThreadList
{
public:
	SFInt32     index;
	SFInt32     parentID;
	SFBool      sticky;
	SFBool      closed;
	SFString    subject;
	SFString    body;
	SFTime      createDate;
	SFString    owner;
	SFTime      editDate;
	SFString    lastEditor;
	SFInt32     nViews;
	SFBool      deleted;
	SFBool      removed;

	// These are transient and not stored in the database
	SFInt32     next;
	SFInt32     prev;
	SFBool      visited;

	         CThread      (SFBool initialize=FALSE);
	         CThread      (SFString& inStr) { fromXML(inStr); };
	        ~CThread      (void);

	SFString getEditBody  (SFBool forEdit) const;
	SFString getModeLink  (modeType type) const;
	SFString childList    (const SFString& fmt=nullString) const;

	SFInt32  Delete       (void);
	SFInt32  Remove       (void);

	SFBool   isOwner      (CPerson *user) const;
	SFBool   isDeleted    (void) const;
	SFBool   isRemoved    (void) const;
	SFBool   isComment    (void) const { return (parentID != 0); }
	SFBool   isTopLevel   (void) const { return !isComment();    }

	SFString getOwnerEmail(void) const;
	SFString getLastEditorEmail(void) const;
	SFString Format       (const SFString& fmt);
	void     fromXML      (SFString& xml);
};

//--------------------------------------------------------------------------------------------
extern SORTINGFUNC getSortFunc       (const SFString& force=nullString);

//--------------------------------------------------------------------------------------------
extern SFString    nextThreadChunk   (const SFString& fieldIn, SFBool& force, const void *data);
extern CThread    *findThread        (SFInt32 id, SFBool returnNew);

//--------------------------------------------------------------------------------------------
extern SFInt32     loadThreads       (void);
extern void        saveThreads       (void);
extern void        clearThreads      (void);

//--------------------------------------------------------------------------------------------
extern SFInt32     nTopLevel;
extern SFInt32     nThreads;
extern CThread     theThreads[];

//--------------------------------------------------------------------------------------------
extern modeType    getForumMode       (void);
extern void        setForumMode       (modeType newMode);

//--------------------------------------------------------------------------------------------
extern SFString    getForumData       (const SFString& valName);
extern void        processForum       (CSimplePage *page);
extern void        processThread      (CSimplePage *page);
extern void        processThreadDelete(CSimplePage *page);
extern void        processThreadRemove(CSimplePage *page);

//--------------------------------------------------------------------------------------------
extern SFBool      threadEditSubmit   (CSimplePage *page, CThread *thread, CThread *parent, SFString& focus, SFInt32& expandID);
extern SFInt32     threadAdd          (const SFString& subject, const SFString& body, SFInt32 parentID);

//--------------------------------------------------------------------------------------------
extern SFBool      debugForum         (const SFString& check);
extern SFInt32     getNextID          (void);

#endif
