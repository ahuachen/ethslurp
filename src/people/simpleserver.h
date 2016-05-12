#ifndef _SIMPLESERVER_H_
#define _SIMPLESERVER_H_
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------------------
#include "simpleuserdb.h"
#include "simplepage.h"

//--------------------------------------------------------------------------------------------
enum cookieType
{
	CK_SESSION=0,
	CK_PERSIST=1,
	CK_NOSET=2,
};

//--------------------------------------------------------------------------------------------
enum
{
	DB_BASIC=(1<<1),
	DB_FORUM=(1<<2),
	DB_USERS=(1<<3),
	DB_OTHER=(1<<4),
	DB_FORCE=(1<<5),
};

//-----------------------------------------------------------------------------------------
class CSimpleServer : public CServerBase
{
private:
	CSimpleUserDB theUserDB;

public:
	CSimpleUserDB *getUserDB    (void) const;

	SFString       parseCookie  (const SFString& name, const SFString& value) const;
	SFString       formatCookie (const SFString& name, const SFString& value) const;

    SFBool         isLoggedIn   (void);
    void           login        (void);
    void           log          (void);
};

//-----------------------------------------------------------------------------------------
extern CSimpleServer *getSimpleServer (void);

//--------------------------------------------------------------------------------------------
// Reads from environment (getString) and also stores the new value (if any)
extern SFString getSetCookieString (const SFString& name, const SFString& defVal, cookieType persist);
extern SFInt32  getSetCookieInt32  (const SFString& name, SFInt32 defVal,         cookieType persist);

// Does not read from environment nor store the value from getString
extern SFString getCookieString    (const SFString& name);
extern SFInt32  getCookieInt32     (const SFString& name, SFInt32 defVal);

extern void     toggleCookie       (const SFString& name, cookieType persist);
extern void     expireCookie       (const SFString& name);

//-----------------------------------------------------------------------------------------
extern void     processSimple      (CSimplePage *page);
extern void     processLogin       (CSimplePage *page);
extern void     processLoginReset  (CSimplePage *page);
extern void     processRecover     (CSimplePage *page);
extern void     processMyAccount   (CSimplePage *page);
extern void     processNewAccount  (CSimplePage *page);
extern void     processForum       (CSimplePage *page);

//-----------------------------------------------------------------------------------------
extern SFString getMenu            (SFBool light);
extern SFString getSocial          (void);
extern SFString getLogin           (void);
extern SFString finalize           (const SFString& in, SFBool final);
extern void     sendAnEmail        (const SFString& to, const SFString& subject, const SFString& body);
extern void     convertWebsite     (SFString& strIn, const SFString& mainEmail);
extern SFString getDebug           (SFInt32 types);
extern void     setDebugCookieList (const SFString& list, SFBool append=TRUE);

//-----------------------------------------------------------------------------------------
class CSimpleSite
{
private:
	SFString fullURL;
	SFString fullPath;
	SFString shortName;
public:
	SFString m_homePage;

	void     setValues   (const SFString fu, const SFString& fp, const SFString& sn);
	SFString getFullURL  (void) const { return fullURL; }
	SFString getFullPath (void) const { return fullPath; }
	SFString getShortName(void) const { return shortName; }
};
extern CSimpleSite theSite;
extern SFString freshenTemplate(void);

//-----------------------------------------------------------------------------------------
inline void CSimpleSite::setValues(const SFString fu, const SFString& fp, const SFString& sn)
{
	fullURL   = fu;
	fullPath  = fp;
	shortName = sn;
	freshenTemplate();
}

//-----------------------------------------------------------------------------------------
inline SFString getSiteRoot(void)
{
	ASSERT(!theSite.getFullPath().IsEmpty() && theSite.getFullPath().endsWith('/'));
	return theSite.getFullPath();
}

//-----------------------------------------------------------------------------------------
inline SFString getSiteURL(void)
{
	ASSERT(!theSite.getFullURL().IsEmpty() && theSite.getFullURL().endsWith('/'));
	return theSite.getFullURL();
}

//--------------------------------------------------------------------------------
extern SFString getIconLink(const SFString& subCmd, const SFString& className,	SFInt32 handle);

//-----------------------------------------------------------------------------------------
inline void convertWebsite(SFString& strIn, const SFString& mainEmail)
{
	SFString root = getSiteURL();
	strIn.ReplaceAll("+WEBROOT+",    root);
	strIn.ReplaceAll("+WEBSITE+",    root + "cgi-bin/site.cgi");
	strIn.ReplaceAll("+ASSETS+",     root + "assets");
	strIn.ReplaceAll("+IMAGES+",     root + "images");
	strIn.ReplaceAll("+SITE_EMAIL+", mainEmail);
	strIn.ReplaceAll("+|", "[");
	strIn.ReplaceAll("|+", "]");
}

//-----------------------------------------------------------------------------------------
inline SFString getString(const SFString& fieldName, const SFString& defVal)
{
    return getSimpleServer()->getStringBase(fieldName, defVal);
}

//-----------------------------------------------------------------------------------------
inline SFString getString(const SFString& fieldName)
{
    return getString(fieldName, EMPTY);
}

//-----------------------------------------------------------------------------------------
inline SFString pathToTemplate(const SFString& assetIn)
{
#ifdef LINUX
	return "/home/jrush/public_html/templates/" + assetIn;
#else
	return "/Users/jrush/src/websiteCode/templates/" + assetIn;
#endif
}

//-----------------------------------------------------------------------------------------
inline SFString pathToAsset(const SFString& assetIn)
{
	return getSiteRoot() + "assets/" + assetIn;
}

//-----------------------------------------------------------------------------------------
inline SFString pathToDatabase(const SFString& dbIn)
{
	return getSiteRoot() + "databases/" + dbIn;
}

//-----------------------------------------------------------------------------------------
inline SFString getAssetData(const SFString& fileName)
{
	return finalize(asciiFileToString(pathToAsset(fileName)), FALSE);
}

//-----------------------------------------------------------------------------------------
inline SFString getDatabaseData(const SFString& fileName)
{
	return StripAny(asciiFileToString(pathToDatabase(fileName)), "\t\n\r ");
}

//-----------------------------------------------------------------------------------------
inline SFString getDatabaseField(const SFString& fileName, const SFString& valName)
{
	SFString data = getDatabaseData(fileName);
	return StripAny(snagField(data, valName), "\t\n\r ");
}

//-----------------------------------------------------------------------------------------
inline SFString getSiteData(const SFString& valName)
{
	static SFString siteData;
	if (siteData.IsEmpty())
		siteData = getDatabaseData(theSite.getShortName()+".xml");
	return StripAny(snagField(siteData, valName), "\t\n\r ");
}

//-----------------------------------------------------------------------------------------
inline SFString getTemplateData(const SFString& type, const SFString& valName)
{
	static SFString shapeData, colorData;
	SFString ret;
	if (type == "shape")
	{
		if (shapeData.IsEmpty())
			shapeData = asciiFileToString(pathToTemplate(getSiteData(type)));
		ret = StripAny(snagField(shapeData, valName), "\t\n\r ");

	} else if (type == "colors")
	{
		if (colorData.IsEmpty())
			colorData = asciiFileToString(pathToTemplate(getSiteData(type)));
		ret = StripAny(snagField(colorData, valName), "\t\n\r ");

	} else if (type == "clear")
	{
		shapeData = colorData = EMPTY;

	} else
	{
		fprintf(stderr, "%s\n", (const char*)("Unknown template data type: " + type));
	}
#if 0
	XX_A(type)
	XX_A(valName)
	XX_A(getSiteData(type));
	XX_A(pathToTemplate(getSiteData(type)));
	XX_A(shapeData);
	XX_A(colorData);
	XX_A(ret);
#endif
    if (!SFos::fileExists(pathToTemplate(getSiteData(type))))
    {
        printf("File not found: %s<br>", (const char*)pathToTemplate(getSiteData(type)));
        exit(0);
    }
	return ret;
}

//-----------------------------------------------------------------------------------------
#define SS_DEBUG_ROW(a,b)  ("<tr><td width=25%>"+SFString(a)+"</td><td>"+(b)+"</td></tr>\n")
#define SS_DEBUG_COOKIE(a) SS_DEBUG_ROW(SFString(a)+":", getCookieString((a)))
#define SS_DEBUG_STRING(a) SS_DEBUG_ROW(SFString(a)+":", getString((a)))

#endif
