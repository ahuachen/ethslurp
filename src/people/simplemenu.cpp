/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "peoplelib.h"
#include "simpleserver.h"

//-----------------------------------------------------------------------------------------
SFString getMenu(SFBool light)
{
	SFString ret = (light ? "\n" : "\t<ul class=\"menuitems\">\n");

	SFBool   done = FALSE;
	SFString items = getSiteData("menu");
	SFString item;
	do
	{
		item = snagFieldClearStrip(items, "menuItem");
		if (theSite.m_homePage.IsEmpty())
			theSite.m_homePage = item; // first item in the menu is home page for the whole site
		if (!item.IsEmpty())
		{
			if (!item.startsWith(';'))
			{
				if (light)
				{
					SFString menuItem = "\t\t\t\t`link:" + item + "|<span id=bottom_row>" + item + "</span>`";
					if (!items.IsEmpty())
						menuItem += " |";
					menuItem += "\n";
					if (isCommand(item))
						menuItem.Replace("<span id=bottom_row>", "<span id=bottom_row style=font-weight:700;>");
					ret += menuItem;
				} else
				{
					SFString menuItem = "\t\t\t\t<li>`link:" + item + "|<span>" + item + "</span>`</li>";
					if (isCommand(item))
					{
						getSimpleServer()->setSessionCookieString("dest", item);
						menuItem.Replace("<span>", "<span class=\"selected\">");
					}
					ret += menuItem;
				}
			}
			ret += "\n";

		} else
		{
			done = TRUE;
		}

	} while (!done);

	if (isCommand("MyAccount")) // special case when user comes from an email
		getSimpleServer()->setSessionCookieString("dest", "MyAccount");

	ret += (light ? "" : "\t\t\t\t\t\t\t\t\t</ul>");
	ret.ReplaceReverse(" |", EMPTY);
	return finalize(ret,TRUE);
}

//-----------------------------------------------------------------------------------------
SFString getLogin(void)
{
	if (getSiteData("login") != "enabled")
		return EMPTY;

	if (getSimpleServer()->isLoggedIn())
		return "`link:MyAccount|<span id=top_row>My Account</span>` | `link:Login-Logout|<span id=top_row>Logout</span>` &nbsp;";

	return "`link:NewAccount|<span id=top_row>Create Account</span>` | `link:Login|<span id=top_row>Login</span>` &nbsp;";
}

//-----------------------------------------------------------------------------------------
SFString getSocial(void)
{
	SFString twitter  = getSiteData("twitterURL");
	SFString facebook = getSiteData("facebookURL");
	if (twitter.IsEmpty() && facebook.IsEmpty())
		return EMPTY;

	CStringExportContext ctx;
	ctx << "<div id=menuRight>";
	if (!twitter.IsEmpty())
	{
		ctx << "<a href=\"https://twitter.com/share?count=none\" class=\"twitter-share-button\" data-url=\"" + twitter + "\" data-text=\"I'm checking out \"></a>\n";
	}
	if (!facebook.IsEmpty())
	{
		ctx << "<a target=_blank href=\"" + facebook + "\">\n";
		ctx << "<img style=border:0px; title=\"Like us on facebook\" height=22px src=+IMAGES+/facebook.png />\n";
		ctx << "</a>\n";
	}
	ctx << "</div>";

	return ctx.str;
}
