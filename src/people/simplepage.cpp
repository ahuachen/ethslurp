/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "peoplelib.h"
#include "simpleserver.h"
#include "simpleforum.h"

//-----------------------------------------------------------------------------------------
CSimplePage::CSimplePage(const SFString& cmdIn)
{
	// because there is no class with which to initialize globals
	clearThreads();
	Initialize(cmdIn, FALSE);
}

//-----------------------------------------------------------------------------------------
void CSimplePage::Initialize(const SFString& cmdIn, SFBool isConfirmed)
{
	cmd = cmdIn;
	cmd.Replace("-Logout", EMPTY); // Logout is not a real comment so wipe it out
	if (loginNeeded())
		cmd = "Login";

	readStrings(cmd);
	if (pageType == "SimpleRedirect")
		readStrings(snagFieldClearStrip(strings, "redirect"));
	SFString strData  = strings;
	title    = snagFieldClearStrip(strData, "title");
	subTitle = snagFieldClearStrip(strData, (isConfirmed ? "subTitle2" : "subTitle"));
	tailMsg  = snagFieldClearStrip(strData, (isConfirmed ? "tailMsg2"  : "tailMsg1"));
	tailMsg  = tailMsg + "<p>\n";

	nFields   = 0;
	fieldList = snagFieldClearStrip(strings, "fieldList");
	SFString fields = fieldList;
	while (!fields.IsEmpty())
	{
		SFString fieldID = "field" + asString(nFields++);
		SFString fieldName = nextTokenClear(fields, '|');
		strings.ReplaceAll("<"  + fieldName + ">",          "<"  + fieldID + ">");
		strings.ReplaceAll("</" + fieldName + ">",          "</" + fieldID + ">");
		strings.ReplaceAll("["  + toProper(fieldName) + "]",         toProper(fieldName));
		strings.ReplaceAll("["  + toUpper (fieldName) + "]", "["   + toUpper (fieldID) + "]");
	}

	rowFmt     = snagFieldClearStrip(strings, "rowFmt");
	rowData    = snagFieldClearStrip(strings, "rows");
	innerHead  = snagFieldClearStrip(strings, "innerHead");
	innerBody  = snagFieldClearStrip(strings, "innerBody");
	innerTail  = snagFieldClearStrip(strings, "innerTail");
	outerBody  = snagFieldClearStrip(strings, "outerBody");
	if (outerBody.IsEmpty())
		outerBody = "[BODY]";

	if (pageType == "SimpleTable")
	{
		innerHead = "\n<table class=\"insideTable\" cellpadding=7 cellspacing=0>\n" + innerHead;
		innerTail = innerTail + "\n</table>\n";

	} else if (pageType == "SimpleUList")
	{
		innerHead = "\n<ul>\n" + innerHead;
		innerTail = innerTail + "\n</ul>\n";
	}
	
	func = NULL;
	if (cmd.Contains( "Recover"     )) { func = processRecover;     } // order matters
	else if (cmd.Contains( "Login-Reset" )) { func = processLoginReset;  }
	else if (cmd.Contains( "Login"       )) { func = processLogin;       }
	else if (cmd.Contains( "MyAccount"   )) { func = processMyAccount;   }
	else if (cmd.Contains( "NewAccount"  )) { func = processNewAccount;  }
	else if (cmd.Contains( "Forum"       )) { func = processForum;       }
	else                                    { func = processSimple;      }
	ASSERT(func != NULL);

	callBk_display = NULL;
}

//-----------------------------------------------------------------------------------------
void CSimplePage::readStrings(const SFString& cmdIn)
{
	SFString contents = getAssetData(cmdIn + ".htm");
	strings = snagFieldClear     (contents, "strings");
	pageType = snagFieldClearStrip(strings, "type");
}

//--------------------------------------------------------------------------------------------
SFBool CSimplePage::loginNeeded(void)
{
	if (getSimpleServer()->isLoggedIn()) // we're already logged in
		return FALSE;

	static SFString loginStr;
	if (loginStr.IsEmpty())
		loginStr = getSiteData("needsLogin");
	if (!loginStr.Contains("|"+cmd+"|")) // do we need to login?
		return FALSE;

	// We need to login and we don't have one
	return TRUE;
}

//-----------------------------------------------------------------------------------------
SFString CSimplePage::status(SFInt32 isGet, pageStatus stType, const SFString& newStatus)
{
	if (stType == ST_UPPER)
	{
		static SFString upperStatus;
		upperStatus = (isGet ? upperStatus : (upperStatus.IsEmpty() ? newStatus : upperStatus + "<br>" + newStatus));
		return upperStatus;
        
	}
    
	static SFString lowerStatus;
	lowerStatus = (isGet ? lowerStatus : (lowerStatus.IsEmpty() ? newStatus : lowerStatus + "<br>" + newStatus));
	return lowerStatus;
}

//-----------------------------------------------------------------------------------------
SFString CSimplePage::statusMsg(pageStatus stType)
{
	SFString msg = status();
	if (stType == ST_UPPER)
	{
		msg = getSiteData("headMsg");
		if (msg.IsEmpty())
			return EMPTY;
	}

	return "<span class=warning>" + msg + "</span>";
}

//-----------------------------------------------------------------------------------------
void CSimplePage::setStatusMsg(pageStatus stType, const SFString& msg)
{
	if (stType == ST_UPPER)
		return;
	status(FALSE, ST_LOWER, msg);
}

//-----------------------------------------------------------------------------------------
void CSimplePage::closeForm(const SFString& newStat)
{
	setStatusMsg   (ST_LOWER, newStat);
    
	innerBody.ReplaceAll("type=submit", "type=remove");
	innerBody.ReplaceAll("<msg>",  "<!--");
	innerBody.ReplaceAll("</msg>", "-->" );
}

//-----------------------------------------------------------------------------------------
SFString CSimplePage::getCode(void)
{
	if (!pageType.Contains("Simple"))
		return "Unknown form type: " + pageType + "<br>";

	ASSERT(func);
	(*func)(this);

	title     = title   .IsEmpty() ? EMPTY : "<h2>" + title + "</h2><p>\n";
	subTitle  = subTitle.IsEmpty() ? EMPTY : "<h3>" + subTitle + "</h3><p>\n";

    SFString ret = title + subTitle + outerBody;
    ret.ReplaceAll("[BODY]", innerHead + innerBody + innerTail);
	return ret;
}

//-----------------------------------------------------------------------------------------
SFString CSimplePage::displayPage(void)
{
	SFString html;

	// we want to have a chance to produce a page with no template--for reporting to PDF files for example
	if (callBk_display && !((*callBk_display)(this, html)))
	{
		return html;
	}

	html = getTemplateData("shape", "all");

	html.ReplaceAll("[PAL_STYLE]",    getTemplateData  ("colors", "pal_styles"));
	html.ReplaceAll("[MENU]",         getMenu          ( FALSE    ) );
	html.ReplaceAll("[MENU_LIGHT]",   getMenu          ( TRUE     ) );
	html.ReplaceAll("[CONTENT]",      getCode          (          ) ); // order matter here, do content first because the account might change
	html.ReplaceAll("[TITLE]",        getSiteData      ( "title"  ) );
	html.ReplaceAll("[PAGE_TITLE]",   stripHTML        ( getSiteData("title") ) );
	html.ReplaceAll("[U_STATUS]",     statusMsg        ( ST_UPPER ) );
	html.ReplaceAll("[L_STATUS]",     statusMsg        ( ST_LOWER ) );
	html.ReplaceAll("[SOCIAL]",       getSocial        (          ) );
	html.ReplaceAll("[FOOTER]",       getSiteData      ( "footer" ) );
	html.ReplaceAll("[LOGIN]",        getLogin         (          ) );
	html.ReplaceAll("[CONSTRUCTION]", getSiteData      ( "construction" ));
	html.ReplaceAll("[DATE]",         Now              (          ).Format(FMT_DEFAULT));
	html.ReplaceAll("[DEBUG]",        getDebug         (DB_BASIC|DB_FORUM|DB_USERS|DB_OTHER));
	html.Replace   ("[BUILD_DATE]",   getSimpleServer  ()->getUserDB()->getCurrentUser()->isSuper() ? "<br>Built: " + SFString(__DATE__) + " " + SFString(__TIME__) : EMPTY);
	return finalize(html, TRUE);
}

//-----------------------------------------------------------------------------------------
void processSimple(CSimplePage *page)
{
	SFString theData = page->rowData;
	SFBool done = FALSE;
	do
	{
		SFString row = snagFieldClearStrip(theData, "row");
		if (row.Contains("<field0>"))
		{
			SFString line = page->rowFmt;
			for (int i=0;i<page->nFields;i++)
			{
				SFString upper  = "[FIELD" + asString(i) + "]";
				SFString field  = "field"  + asString(i);
				line.Replace(upper, snagFieldClearStrip(row, field));
			}
			page->innerBody += (line+"\n");
            
		} else
		{
			done = TRUE;
		}
	} while (!done);
    
	page->innerBody = page->innerHead + page->innerBody + page->innerTail;
	page->innerHead = page->innerTail = EMPTY;
    
	return;
}

//-----------------------------------------------------------------------------------------
#define TEST_FILE(a) { XX_B(SFos::fileExists((a))); if (!SFos::fileExists((a))) { printf("File not found: %s. Quitting...", (const char*)(a)); fflush(stdout); exit(0); } }

//-----------------------------------------------------------------------------------------
SFString freshenTemplate(void)
{
	SFString styleFile    = pathToAsset   ( "styles.css"                    );
	SFString siteFile     = pathToDatabase( theSite.getShortName() + ".xml" );
	SFString masterFile   = pathToTemplate("template.css"                   );
	SFString templateFile = pathToTemplate( getSiteData("colors")           );

#if 0
SFString dbStyleFile = styleFile+":"+SFos::fileLastModifyDate( styleFile ).Format(FMT_DEFAULT_EXPORT);
XX_A(dbStyleFile);
SFString dbSiteFile = siteFile +":"+SFos::fileLastModifyDate( siteFile ).Format(FMT_DEFAULT_EXPORT);
XX_A(dbSiteFile);
SFString dbMasterFile = masterFile+":"+SFos::fileLastModifyDate( masterFile ).Format(FMT_DEFAULT_EXPORT);
XX_A(dbMasterFile);
SFString dbTemplateFile = templateFile+":"+SFos::fileLastModifyDate( templateFile ).Format(FMT_DEFAULT_EXPORT);
XX_A(dbTemplateFile);
XX_B(isFileNewer(templateFile, styleFile));
XX_B(isFileNewer(masterFile, styleFile));
XX_B(isFileNewer(siteFile, styleFile));
TEST_FILE(styleFile);
TEST_FILE(siteFile);
TEST_FILE(masterFile);
TEST_FILE(templateFile);
#endif

	// if any file is newer than the style file, regenerate the style file
	if (isFileNewer(templateFile, styleFile) || isFileNewer(masterFile, styleFile) || isFileNewer(siteFile, styleFile))
	{
		getTemplateData("clear", EMPTY);

		SFString contents = asciiFileToString(masterFile);
		SFString reps[] = {
			"bodyColor",       "contentColor",  "outerColor",      "innerColor",     "mainBodyColor",
			"mainHeaderColor", "mainMenuColor", "mainFooterColor", "statusBarColor", "htmlback",
			"darkBorder",      "lightBorder",   "outerText:u",     "outerText:l",    "textColor",
			"headerText",      "hiliteText",    "colInput",        "colInputBdr",
		};
		SFInt32 nReps = sizeof(reps) / sizeof(SFString);
		for (int i=0;i<nReps;i++)
		{
			SFString color = reps[i];
			SFString rep   = "[" + color + "]";
			contents.ReplaceAll(rep, getTemplateData("colors", color).Substitute("\t",EMPTY));
		}

		SFInt32 width = atoi((const char*)getSiteData("pgWidth"));
		contents.Replace("[pgWidth]",  asString(width)+"%");
		contents.Replace("[pgMargin]", asString((100-width)/2)+"%");
		SFString headHgt = getSiteData("headHgt");
		if (headHgt.IsEmpty())
			headHgt = "2.65em";
		contents.Replace("[headHgt]", headHgt);

		contents.ReplaceAll("background-color:  ; ", EMPTY);
		stringToAsciiFile(styleFile, contents);
		printf("The styles.css file was updated. Press reload");
//		exit(0);
	}
	return EMPTY;
}
