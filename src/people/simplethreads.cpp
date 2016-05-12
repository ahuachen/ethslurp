/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "peoplelib.h"
#include "simpleforum.h"

//--------------------------------------------------------------------------------------------
void processThread(CSimplePage *page)
{
	modeType mode = getForumMode();
	ASSERT(mode == MD_ADD || mode == MD_EDIT);

	CThread *thread   = findThread(getInt32("thread", 0), TRUE);
	CThread *parent   = findThread(getInt32("parentID",   thread->parentID), FALSE);
	thread->parentID  = (parent ? parent->index : 0);

	SFString formName  =  getString("formName");
	SFBool   fromForm  = (formName == "ThreadEdit");
	SFBool   isComment =  getInt32("isComment", FALSE);
	SFString focus     =  "subject";

	if (fromForm)
	{
		SFInt32 expandID=-1;
		if (!threadEditSubmit(page, thread, parent, focus, expandID))
		{
			mode = MD_EDIT;

		} else
		{
			SFString eSubject = snagFieldClearStrip(page->strings, "eSubject");
			SFString eBody    = snagFieldClearStrip(page->strings, "eBody"   );
			sendAnEmail(getSiteData("mainEmail"), eSubject, thread->Format(eBody));
			getSimpleServer()->setSessionCookieInt32("expand", expandID);
			clearThreads();
			loadThreads();

			setForumMode(MD_NONE);
			page->Initialize("Forum", FALSE);
			processForum(page);
			return;
		}
	}

	page->Initialize("Thread", FALSE);

	SFString headFmt  = snagFieldClearStrip(page->strings, "editHead");
	SFString rowFmt   = page->rowFmt; rowFmt.ReplaceAll("\t", EMPTY);

	SFString ll1      = snagFieldClearStrip(page->strings, "leftLink1"  );
	SFString ll2      = snagFieldClearStrip(page->strings, "leftLink2"  );
	SFString cl1      = snagFieldClearStrip(page->strings, "centerLink1");
	SFString cl2      = snagFieldClearStrip(page->strings, "centerLink2");
	SFString rl1      = snagFieldClearStrip(page->strings, "rightLink1" );
	SFString rl2      = snagFieldClearStrip(page->strings, "rightLink2" );

	getSimpleServer()->m_helperStr = "[{JUNK}]";
	headFmt.Replace("[ISCOMMENT]", asString(isComment));

	page->innerBody.Replace("[T_LINK1_L]", ll1.IsEmpty() ? EMPTY : thread->Format(ll1));
	page->innerBody.Replace("[T_LINK2_L]", ll2.IsEmpty() ? EMPTY : thread->Format(ll2));
	page->innerBody.Replace("[T_LINK1_C]", cl1.IsEmpty() ? EMPTY : thread->Format(cl1));
	page->innerBody.Replace("[T_LINK2_C]", cl2.IsEmpty() ? EMPTY : thread->Format(cl2));
	page->innerBody.Replace("[T_LINK1_R]", rl1.IsEmpty() ? EMPTY : thread->Format(rl1));
	page->innerBody.Replace("[T_LINK2_R]", rl2.IsEmpty() ? EMPTY : thread->Format(rl2));

	// Do the header
	SFString header = headFmt;
	header     .ReplaceAll("[MODE]", getString("mode"));
	page->title.ReplaceAll("[MODE]", getString("mode"));

	// Do the body of the form
	SFString row = rowFmt;
	getSimpleServer()->m_isEditing = TRUE;

	SFString editSubject = snagFieldClearStrip(page->strings, "editSubject");
	header.Replace("[{SUBJECT}]",  editSubject);
	row   .Replace("[{SUBJECT}]",  editSubject);

	SFString editBody = isComment ? EMPTY : snagFieldClearStrip(page->strings, "editBody");
	editBody.Replace("[D]", thread && thread->isComment() ? "disabled" : "");
	header.Replace("[{BODY}]",     editBody);
	row   .Replace("[{BODY}]",     editBody);

	page->innerBody.ReplaceAll("[HEADER]",  thread->Format(header));
	page->innerBody.ReplaceAll("[THREADS]", thread->Format(row));
	page->innerBody.ReplaceAll("[FOCUS]",   focus);

	return;
}

//--------------------------------------------------------------------------------------------
SFBool threadEditSubmit(CSimplePage *page, CThread *thread, CThread *parent, SFString& focus, SFInt32& expandID)
{
	SFString subject   = getString("subject");
	SFString body      = getString("body");
	SFBool   isComment = getInt32("isComment", FALSE);

	SFString subjectError, bodyError;
	if ( !debugForum("allow_html")  && body   .Contains("<") ) { focus = "body";    bodyError    = "<br>Invalid character: '<'. You may not enter HTML code into this form."; }
	if (                               body    == DEF_BODY   ) { focus = "body";    bodyError    = "<br>Please enter a valid body.";                                          }
	if ( !thread->isComment()       && body   .IsEmpty()     ) { focus = "body";    bodyError    = "<br>Body field is required.";                                             }

	if ( !debugForum("allow_html")  && subject.Contains("<") ) { focus = "subject"; subjectError = "<br>Invalid character: '<'. You may not enter HTML code into this form."; }
	if (                               subject == DEF_SUBJECT) { focus = "subject"; subjectError = "<br>Please enter a valid subject.";                                       }
	if ( !thread->isComment()       && subject.IsEmpty ()    ) { focus = "subject"; subjectError = "<br>Subject field is required.";                                          }

	body.ReplaceAll("\n", "<br>");
	thread->subject  = subject;

	if (isComment && thread->subject.IsEmpty() && debugForum("autoenter"))
	{
		if (parent)
		{
			thread->subject += " comment " + asString(parent->GetCount());
		} else
		{
			thread->subject += " comment " + Now().Format(FMT_SORTALL);
		}
	}

	thread->body = body;
	thread->body.ReplaceAll(DEF_COMMENT, EMPTY);

	SFBool okay = (subjectError+bodyError).IsEmpty();
	if (!okay)
	{
		subjectError.Replace("<br>", EMPTY);
		bodyError.Replace("<br>", EMPTY);
		if (!subjectError.IsEmpty())
		{
			page->setStatusMsg(ST_LOWER, subjectError + (!bodyError.IsEmpty() ? "<br>"+bodyError : EMPTY));

		} else
		{
			ASSERT(!bodyError.IsEmpty());
			page->setStatusMsg(ST_LOWER, bodyError);
		}

		expandID = thread->isComment() ? thread->parentID : thread->index;
		return FALSE;
	}

	if (thread->index == 0)
	{
		if (isComment && thread->subject.IsEmpty())
		{
			page->setStatusMsg(ST_LOWER, "Please enter a comment.");
			expandID = thread->isComment() ? getInt32("parentID", thread->parentID) : thread->index;

		} else
		{
			SFInt32 parentID = 0;
			if (parent)
			{
				parent->lastEditor = getSimpleServer()->getUserDB()->getCurrentUser()->getUserID();
				parent->editDate   = Now();
				parentID = parent->index;
			}

			SFInt32 retID = threadAdd(thread->subject, thread->body, parentID);
			page->setStatusMsg(ST_LOWER, "Thread '" + asString(retID) + "' was added.");
			expandID = (parent ? parentID : retID);
		}

	} else
	{
		if (parent)
		{
			parent->lastEditor = getSimpleServer()->getUserDB()->getCurrentUser()->getUserID();
			parent->editDate   = Now();
		}
		thread->lastEditor = getSimpleServer()->getUserDB()->getCurrentUser()->getUserID();
		thread->editDate   = Now();
		expandID = (parent ? parent->index : thread->index);
		saveThreads();
		page->setStatusMsg(ST_LOWER, "Thread '" + asString(thread->index) + "' was saved.");
	}

	return TRUE;
}

//--------------------------------------------------------------------------------------------
SFInt32 threadAdd(const SFString& subject, const SFString& body, SFInt32 parentID)
{
	CThread *thread = &theThreads[nThreads++];
	*thread = CThread(TRUE);

	SFInt32 newID = getNextID();
	thread->index    = (newID);
	thread->parentID = parentID;
	thread->subject  = subject;
	thread->body     = body;

	saveThreads();
	return newID;
}

//--------------------------------------------------------------------------------------------
void processThreadDelete(CSimplePage *page)
{
	CThread *thread = findThread(getInt32("thread", 0), TRUE);
	SFInt32 curID  = thread->index; // pick these values up before saving otherwise they are stale.
	SFInt32 curDel = !thread->deleted;
	thread->Delete();
	saveThreads();

	SFString verb = (curDel ? "deleted" : "undeleted");
	page->setStatusMsg(ST_LOWER, "Thread '" + asString(curID) + "' was " + verb + ".");
	setForumMode(MD_NONE);
	processForum(page);
	return;
}

//--------------------------------------------------------------------------------------------
void processThreadRemove(CSimplePage *page)
{
	CThread *thread = findThread(getInt32("thread", 0), TRUE);
	SFInt32 curID  = thread->index; // pick these values up before saving otherwise they are stale.
	thread->Remove();
	saveThreads();

	page->setStatusMsg(ST_LOWER, "Thread '" + asString(curID) + "' was removed.");
	setForumMode(MD_NONE);
	processForum(page);
	return;
}
