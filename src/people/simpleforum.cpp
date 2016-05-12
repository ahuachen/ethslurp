/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "peoplelib.h"
#include "simpleforum.h"

static SFString getScrollBar        (SFString& strings, SFInt32 curPage, SFInt32 maxPages);
static SFInt32  getPageNumber       (SFInt32 threadID, SFInt32 perPage);
static void     handleLinks         (SFString& table, SFString& strings, SFInt32 curPage, SFInt32 maxPages, SFInt32 perPage);
static void     handleSortIcons     (SFString& table);

//--------------------------------------------------------------------------------------------
void processForum(CSimplePage *page)
{
	loadThreads();

	modeType mode = getForumMode();
	switch (mode)
	{
		case MD_NONE:
		case MD_VIEW:   break;
		case MD_ADD:
		case MD_EDIT:   processThread      (page); return;
		case MD_DELETE: processThreadDelete(page); return;
		case MD_REMOVE: processThreadRemove(page); return;
		case MD_STALE:
			ASSERT(0); // should never happen
	}

	if (getInt32("expand",0)!=0)
	{
		expireCookie("details");
		CThread *thread = findThread(getInt32("expand", 0), TRUE);
		if (thread)
			thread->nViews++;
	}

	page->Initialize("Forum", FALSE);

	SFString headFmt    = snagFieldClearStrip(page->strings, "headFmt"  );
	SFString rowFmt     = page->rowFmt;
	if (!debugForum("showid")) { rowFmt  .ReplaceAll("[{INDEX}: ]", EMPTY);rowFmt.ReplaceAll("[{PARENTID}: ]",EMPTY);   }
	SFString detFmt     = snagFieldClearStrip(page->strings, "detFmt"   );                            { detFmt  .ReplaceAll("[EXPCOLOR]" , snagFieldClearStrip(page->strings, "expColor"));         }
	SFString childStr   = snagFieldClearStrip(page->strings, "childStr");  if (!debugForum("showid")) { childStr.ReplaceAll("[{INDEX}: ]", EMPTY);childStr.ReplaceAll("[{PARENTID}: ]",EMPTY); }
	SFString altColor   = snagFieldClearStrip(page->strings, "altColor" );
	SFString lastRow    = snagFieldClearStrip(page->strings, "emptyRow");
	page->innerBody.Replace("[HEADER]", headFmt);

	SFInt32  perPage    = getSetCookieInt32("perPage", 25, CK_SESSION);
	getSimpleServer()->setCookieInt32("page", getPageNumber(getSetCookieInt32("expand", 0, CK_SESSION), perPage));
	SFInt32  curPage    = getSetCookieInt32("page",     0, CK_SESSION);
	SFInt32  maxPages   = (nTopLevel-1)/perPage;
	SFInt32  start      = (curPage * perPage + 1); if (start > nTopLevel) start = MAX(0,start - perPage);
	SFInt32  end        = MIN(nTopLevel, (start   + perPage - 1));

	SFInt32  showDetail  = getSetCookieInt32("details",  FALSE, CK_SESSION);
	SFInt32  expanded    = getSetCookieInt32("expand",   FALSE, CK_SESSION);

	SFString rows, color = " bgcolor=" + altColor; SFInt32 cnt=0;
	getSimpleServer()->m_helperStr = "\n"+childStr+"\n\n\t";

	for (int i=0;i<nThreads;i++)
	{
		CThread *thread = &theThreads[i];
		if (thread->isTopLevel() && (!thread->isDeleted() || debugForum("savedel")))
		{
			++cnt;
			if (cnt >= start && cnt < (start+perPage))
			{
				SFString row = rowFmt;
				if (showDetail || expanded==thread->index)
					row += detFmt;

				color = (color.IsEmpty() ? " bgcolor=" + altColor : EMPTY);
				row.ReplaceAll("[COLOR]", color);

				SFString converted = thread->Format(row);
				converted.Replace("mode=Edit", "mode=Edit");
				converted.ReplaceAll("+FORM_NAME+", "cForm_" + asString(thread->index));
				if (expanded==thread->index)
				{
					converted.ReplaceAll("expand=", "expand=0&junk=");
					converted.ReplaceAll("|View", "|Hide");
				}
				rows += converted;
			}
		}
	}

	color = (color.IsEmpty() ? " bgcolor=" + altColor : EMPTY);

	lastRow.Replace("[COLOR]", color);
	rows += lastRow;
	page->innerBody.Replace("[THREADS]", rows);

	page->innerBody.Replace("[FORM_FOCUS]", getInt32("parentID", 0) ? "cForm_" + asString(getInt32("parentID", 0)) : "");
	page->innerBody += page->innerTail;

	handleLinks(page->innerBody, page->strings, curPage, maxPages, perPage);
	handleSortIcons(page->innerBody);

	page->innerBody.ReplaceAll("[DETAILS]",   asString(!showDetail));
	page->innerBody.ReplaceAll("[detStr]",    SFString(showDetail ? "collapse" : "expand"));
	page->innerBody.ReplaceAll("[COUNT]",     asString(nTopLevel));
	page->innerBody.ReplaceAll("[START]",     asString(start));
	page->innerBody.ReplaceAll("[END]",       asString(end));
	page->innerBody.ReplaceAll("|  |+", " |+"); //cleaning up
	page->innerBody.ReplaceAll(">+ |+<", "><");
	if (getInt32("expand",0)!=0)
		saveThreads(); // do not collaspe because save causes load which causes sort which will make the pointer stale

	return;
}

//--------------------------------------------------------------------------------------------
static SFString getScrollBar(SFString& strings, SFInt32 curPage, SFInt32 maxPages)
{
	SFString stats = snagFieldClearStrip(strings, "rightLink2" );
	if (curPage<2)
	{
		if (curPage<2) stats.ReplaceAll("&page=[frstPage]\">first</a>", "\"></a>first");
		if (curPage<1)
			stats.ReplaceAll("&page=[prevPage]\">prev</a>",       "\"></a>prev");
		else
			stats.ReplaceAll("[prevPage]", asString(MAX(0,curPage-1)));
	} else
	{
		stats.ReplaceAll("[frstPage]", asString(0));
		stats.ReplaceAll("[prevPage]", asString(MAX(0,curPage-1)));
	}

	if (curPage>maxPages-2)
	{
		if (curPage>maxPages-1)
			stats.ReplaceAll("&page=[nextPage]\">next</a>",      "\"></a>next");
		else
			stats.ReplaceAll("[nextPage]", asString(MIN(maxPages,curPage+1)));
		if (curPage>maxPages-2)
			stats.ReplaceAll("&page=[lastPage]\">last</a>", "\"></a>last");
	} else
	{
		stats.ReplaceAll("[nextPage]", asString(MIN(maxPages,curPage+1)));
		stats.ReplaceAll("[lastPage]", asString(maxPages));
	}
	return stats;
}

//--------------------------------------------------------------------------------------------
static SFInt32 getPageNumber(SFInt32 threadID, SFInt32 perPage)
{
	SFInt32 index=0;
	for (int i=0;i<nThreads;i++)
		if (theThreads[i].index == threadID)
			index = i;
	return index / perPage;
}

//--------------------------------------------------------------------------------------------
void handleLinks(SFString& table, SFString& strings, SFInt32 curPage, SFInt32 maxPages, SFInt32 perPage)
{
	SFString ll1        = snagFieldClearStrip(strings, "leftLink1"  );
	SFString ll2        = snagFieldClearStrip(strings, "leftLink2"  );
	SFString cl1        = snagFieldClearStrip(strings, "centerLink1");
	SFString cl2        = snagFieldClearStrip(strings, "centerLink2"); if (nTopLevel<2) cl2 = EMPTY;
	SFString rl1        = snagFieldClearStrip(strings, "rightLink1" );
	SFString rl2        = getScrollBar(strings, curPage, maxPages);
	if (nTopLevel <= perPage) { rl2 = rl1; rl1 = EMPTY; }
	if (nThreads==0) { ll1 = ll2 = cl1 = cl2 = rl1 = EMPTY; rl2 = SPACER; }

	table.Replace("[T_LINK1_L]", ll1); table.Replace("[T_LINK2_L]", ll2);
	table.Replace("[T_LINK1_C]", cl1); table.Replace("[T_LINK2_C]", cl2);
	table.Replace("[T_LINK1_R]", rl1); table.Replace("[T_LINK2_R]", rl2);
}

//--------------------------------------------------------------------------------------------
void handleSortIcons(SFString& table)
{
	SFBool   reverse = getSetCookieInt32("reverse", FALSE, CK_SESSION);
	SFString sort    = toLower(getSetCookieString("sort", "Date", CK_SESSION));
	SFString notrev  = " <img class=\"icon\" style=\"width:12px;\" src=\"+IMAGES+/down.png\">";
	SFString rev     = " <img class=\"icon\" style=\"width:12px;\" src=\"+IMAGES+/up.png\">";

	table.Replace("[s1]", (sort == "subject"    ? (reverse==1 ? rev : notrev) : EMPTY));
	table.Replace("[s2]", (sort == "nviews"     ? (reverse==1 ? rev : notrev) : EMPTY));
	table.Replace("[s3]", (sort == "nreply"     ? (reverse==1 ? rev : notrev) : EMPTY));
	table.Replace("[s4]", (sort == "date"       ? (reverse==1 ? rev : notrev) : EMPTY));
	table.Replace("[s5]", (sort == "editor"     ? (reverse==1 ? rev : notrev) : EMPTY));
	table.Replace("[s6]", (sort == "originator" ? (reverse==1 ? rev : notrev) : EMPTY));
}

//-----------------------------------------------------------------------------------------
SFBool debugForum(const SFString& check)
{
	if (!getSimpleServer()->getUserDB()->getCurrentUser()->isSuper())
		return FALSE;

	static SFString debugStr;
	if (debugStr.IsEmpty())
	{
		SFString all = getAssetData("Forum.htm");
		debugStr = snagFieldClear(all, "debugForum");
	}

	return debugStr.Contains(check);
}
