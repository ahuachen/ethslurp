/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "peoplelib.h"
#include "simpleserver.h"

CSimpleSite theSite;

//-----------------------------------------------------------------------------------------
CSimpleServer *getSimpleServer(void)
{
	static CSimpleServer theServer;
	return &theServer;
}

//-----------------------------------------------------------------------------------------
CSimpleUserDB *CSimpleServer::getUserDB(void) const
{
	CSimpleUserDB *userDB = &((CSimpleServer*)getSimpleServer())->theUserDB;
	if (userDB->getFilename().IsEmpty())
		userDB->setFilename(pathToDatabase("accounts.txt"));
	userDB->ReadDatabase(EMPTY, EMPTY);
	return userDB;
}

//---------------------------------------------------------------------------
SFString CSimpleServer::parseCookie(const SFString& name, const SFString& defVal) const
{
	SFString cookie = m_theCookie;
	return snagField(cookie, name, defVal);
}

//---------------------------------------------------------------------------
SFString CSimpleServer::formatCookie(const SFString& name, const SFString& value) const
{
	return ("<" + name + ">" + value + "</" + name + ">");
}

//--------------------------------------------------------------------------------------------
SFBool CSimpleServer::isLoggedIn(void)
{
	return !getUserDB()->getCurrentUser()->getUserID().IsEmpty();
}

//--------------------------------------------------------------------------------------------
void CSimpleServer::log(void)
{
	if (!getSiteData("log").Contains("enabled"))
		return;

	if (getSiteData("log") != "enabled-local" && theSite.getFullURL().Contains("localhost"))
	return;

	FILE *fp = fopen((const char*)pathToDatabase("access_log.txt"), asciiWriteAppend);
	if (fp)
	{
		SFString logFmt = getSiteData("logFmt");
		SFTime now = Now();
		logFmt.ReplaceAll("[IP]",   cgiRemoteAddr);
		logFmt.ReplaceAll("[DATE]", now.Format(FMT_DEFAULT_EXPORT).Substitute(" pm", "_pm").Substitute(" am", "_am").Substitute(" ", ";").Substitute("_", " "));
		logFmt.ReplaceAll("[CMD]",  getString("cmd", "Welcome"));
		fprintf(fp, "%s\n", (const char*)logFmt);
		fclose(fp);
	}
}

//--------------------------------------------------------------------------------------------
void CSimpleServer::login(void)
{
	if (!isCommand("Login-Logout"))
	{
		getUserDB()->setCurrentUser(getUserDB()->ReturnUserByID(hard_decrypt(getCookieString("format"), PW_KEY)));
        
	} else
	{
		getSimpleServer()->expireCookie("format");
	}
}

//--------------------------------------------------------------------------------------------
SFString getSetCookieString(const SFString& name, const SFString& defVal, cookieType persist)
{
	SFString cookieVal = getSimpleServer()->getCookieString(name, defVal);
	SFString curVal    = getString(name, cookieVal);

	if (curVal == defVal)
	{
		getSimpleServer()->expireCookie(name);
		
	} else
	{
		switch (persist)
		{
			case CK_NOSET:   break;
			case CK_SESSION: getSimpleServer()->setSessionCookieString(name, curVal); break;
			case CK_PERSIST: getSimpleServer()->setCookieString       (name, curVal); break;
		}
	}

	return curVal;
}

//--------------------------------------------------------------------------------------------
SFString getCookieString(const SFString& name)
{
	return getSimpleServer()->getCookieString(name);
}

//--------------------------------------------------------------------------------------------
SFInt32 getSetCookieInt32(const SFString& name, SFInt32 defVal, cookieType persist)
{
	return atoi((const char*)getSetCookieString(name, asString(defVal), persist));
}

//--------------------------------------------------------------------------------------------
SFInt32 getCookieInt32(const SFString& name, SFInt32 defVal)
{
	return getSimpleServer()->getCookieInt32(name, defVal);
}

//-----------------------------------------------------------------------------------------
void toggleCookie(const SFString& name, cookieType persist)
{
	SFInt32 value = getSimpleServer()->getCookieInt32(name, FALSE);
	if (persist == CK_PERSIST)
		getSimpleServer()->setCookieInt32(name, (value ? FALSE : TRUE));
	else
		getSimpleServer()->setSessionCookieInt32(name, (value ? FALSE : TRUE));
}

//-----------------------------------------------------------------------------------------
void expireCookie(const SFString& name)
{
	getSimpleServer()->expireCookie(name);
}

//-----------------------------------------------------------------------------------------
extern const char* STR_FORM_ROW_CHECKBOX;
extern const char* STR_FORM_ROW_TEXT;
extern const char* STR_FORM_ROW_MULTILINE;

//-----------------------------------------------------------------------------------------
SFString finalize(const SFString& in, SFBool final)
{
	SFString ret;
	SFString inStr = in;
	inStr.ReplaceAll("~", "&");
	inStr.ReplaceAll("[YEAR]",   asString(Now().GetYear()));
	SFString save = inStr;

	while (!inStr.IsEmpty())
	{
		ret += nextTokenClear(inStr, '`');
		if (inStr.startsWith("form:") || inStr.startsWith("link:") || inStr.startsWith("formRow:") || inStr.startsWith("formEnd:"))
		{
			SFString result;

			SFString token = nextTokenClear(inStr, '`');
			if (token.startsWith("form:"))
			{
				token.Replace("form:", EMPTY);
				result += "<table width=70%>\n";
				result += "\t<!-- formStart:" + token + " -->\n";
				result += "\t<form method=\"post\" action=\"+WEBSITE+\" name=\""  + token + "\" />\n";
				result += "\t\t<input type=\"hidden\" name=\"cmd\"      value=\"" + token + "\" />\n";
				result += "\t\t<input type=\"hidden\" name=\"formName\" value=\"" + token + "\" />\n";

			} else if (token.startsWith("link:"))
			{
				token.Replace("link:", EMPTY);
				SFString cmd = nextTokenClear(token, '|');
				if (cmd.startsWith("http") || cmd.startsWith("/") || cmd.startsWith("_top_:"))
				{
					cmd.Replace("_top_:", "+WEBSITE+?cmd=");
					result += "<a class=\"external\" target=\"_blank\" href=\"" + cmd + "\">" + token + "</a>";
				} else if (cmd.startsWith("mailto:"))
				{
					result += "<a class=\"external\" href=\"" + cmd + "\">" + token + "</a>";
				} else
				{
					result += "<a href=\"+WEBSITE+?cmd=" + cmd + "\">" + token + "</a>";
				}

			} else if (token.startsWith("formEnd:"))
			{
				result += "</form>\n<!-- " + token + " -->\n";
				result += "\t</table>\n";

			} else if (token.startsWith("formRow:"))
			{
#define data token
				data.Replace("formRow:", EMPTY);

				SFString rowType = nextTokenClear(data, '|');
				if (rowType == "header")
				{
					result += "<tr><td colspan=\"3\">" + data + "<p>&nbsp;</p></td></tr>";

				} else if (rowType == "submit")
				{
					SFString msg2 = data;
					data = nextTokenClear(msg2, '|');
					result += "<submitRow>\n";
					result += "\t\t<tr><td colspan=\"3\">&nbsp;</td></tr>\n";
					result += "\t\t<tr><td width=\"2%\" colspan=\"2\">&nbsp;</td><td><input type=submit name=\"button\" value=\"" + data + "\" /></td></tr>\n";
					result += "\t</submitRow>\n";
					result += "\t<tr><td align=\"middle\" colspan=\"3\">&nbsp;</td></tr>";
					if (!msg2.IsEmpty())
					{
						result += "<tr><td align=\"left\" colspan=\"3\">" + msg2 + "</td></tr>";
					}

				} else if (rowType == "hidden")
				{
					SFString name = nextTokenClear(data, '|');
					result = "<input type=\"hidden\" name=\"" + name + "\" value=\"" + data + "\" />";

				} else if (rowType == "spacer")
				{
					result = "<tr><td colspan=\"3\">&nbsp;</td></tr>";

				} else if (rowType == "checkbox")
				{
					SFString name   = nextTokenClear(data, '|');
					SFString name_l = toLower (name); name_l.ReplaceAll(" ", "_");

					result = STR_FORM_ROW_CHECKBOX;
					result.ReplaceAll("[NAME_L]", name_l);
					result.ReplaceAll("[LABEL]",  data);

				} else if (rowType == "message")
				{
					SFString str = nextTokenClear(data, '|');
					SFString msg = snagField(save, str); // pick up the message we're being told to use
					if (msg.IsEmpty())
						msg = str;
					msg = "<msg>"+msg+"</msg>";
					snagFieldClear(inStr,str);

					result = STR_FORM_ROW_TEXT;
					result.Replace("[PROMPT]:", EMPTY);
					result.Replace("<input [D] type=\"[TYPE]\" name=\"[NAME_L]\" value=\"[[NAME_U]]\" size=\"[SIZE]\" /> <span class=\"warning\">[REQ][[NAME_U]_ERR]<span>", msg);

				} else if (rowType == "text" || rowType == "password" || rowType == "disabled" || rowType == "message" || rowType == "multiline")
				{
					SFBool disabled = (rowType=="disabled");
					if (disabled)
						rowType = "text";

					SFString name   = nextTokenClear(data, '|');
					SFBool   req    = SFBool(data=="required");
					SFString name_l = toLower (name); name_l.ReplaceAll(" ", "_");
					SFString name_u = toUpper (name); name_u.ReplaceAll(" ", "_");
					SFString prompt = toProper(name);

					result = STR_FORM_ROW_TEXT;
					if (rowType=="multiline")
						result = STR_FORM_ROW_MULTILINE;
					result.ReplaceAll("[PROMPT]", prompt); if (prompt.IsEmpty()) result.ReplaceAll(">:<", "><");
					result.ReplaceAll("[NAME_L]", name_l);
					result.ReplaceAll("[NAME_U]", name_u);
					result.ReplaceAll("[TYPE]",   rowType);
					result.ReplaceAll("[REQ]",    (req?"*":""));
					result.ReplaceAll("[SIZE]",   rowType == "password" ? "32" : "70");
					if (disabled)
						result.ReplaceAll("[D]", "disabled");
				}
			}
			ret += result;
		}
	}

	if (final)
	{
		if (ret.Contains("type=remove"))
			snagFieldClearStrip(ret, "submitRow");
		snagFieldClearStrip(ret, "strings");

		ret.ReplaceAll("[FOCUS]",   EMPTY);
		convertWebsite(ret, getSiteData("mainEmail"));

		if (!getSimpleServer()->m_finalScript.IsEmpty())
			ret += "\n" + getSimpleServer()->m_finalScript;

	}

	return ret;
}

//-----------------------------------------------------------------------------------------
void sendAnEmail(const SFString& emailIn, const SFString& subjectIn, const SFString& bodyIn)
{
	SFString subject = subjectIn; convertWebsite(subject, getSiteData("mainEmail"));
	SFString body    = bodyIn;    convertWebsite(body,    getSiteData("mainEmail"));

#ifdef LINUX
	CMailMessage mail;
	mail.setRecipient (emailIn);
	mail.setSubject   (subject);
	mail.setBody	  (body);
	mail.setReplyTo   (getSiteData("mainEmail"));
	mail.setServer    (getSiteData("mailServer"));
	mail.setSender    (getSiteData("mainEmail")); //"library AT gladwyne.net");
	mail.setVerbose   (1);
	mail.setProgram   ("/usr/sbin/sendmail");

	CMailer mailer;SFString result;
	mailer.SendMail(mail, result, FALSE);
#endif

	if (getSiteData("emailLog")=="on")
	{
		SFString add;
		add += SFString('~', 80) + "\n";
		add += "Email sent: " + Now().Format(FMT_DEFAULT) + "\n";
		add += "To:         " + emailIn + "\n";
		add += "Subject:    " + subject + "\n";
		add += "\n"           + body + "\n";

		SFString contents = add + getDatabaseData("email.log");
		stringToAsciiFile(pathToDatabase("email.log"), contents);
	}
}

static SFString debugCookieList;
//-----------------------------------------------------------------------------------------
void setDebugCookieList(const SFString& list, SFBool append)
{
	if (append)
		debugCookieList += list;
	else
		debugCookieList = list;
}

//-----------------------------------------------------------------------------------------
SFString getDebug(SFInt32 types)
{
	return EMPTY;
	
	if (!(types&DB_FORCE))
	{
		if (!getSimpleServer()->getUserDB()->getCurrentUser()->isSuper())
			return EMPTY;
    
		if (!getSetCookieInt32("debug", FALSE, CK_PERSIST))
			return "`link:"+getString("cmd")+"~debug=1|debug on`";
	}

	SFString ret = "<table width=100%>";
    if (types & DB_BASIC)
    {
        ret += SS_DEBUG_ROW    ( "<hr>","<hr>"    );
        ret += SS_DEBUG_ROW    ( "<b>Login cookies","" );
        ret += SS_DEBUG_STRING ( "cmd"            );
        ret += SS_DEBUG_COOKIE ( "dest"           );
        ret += SS_DEBUG_COOKIE ( "lastLogin"      );
        ret += SS_DEBUG_COOKIE ( "format"         );
		SFString ll = debugCookieList;
		while (!ll.IsEmpty())
		{
			SFString item = nextTokenClear(ll, '|');
			ret += SS_DEBUG_COOKIE ( item );
		}
        ret += SS_DEBUG_ROW    ( "<hr>","<hr>"    );
        ret += SS_DEBUG_ROW    ( "<b>Site data",  "" );
        ret += SS_DEBUG_ROW    ( "site root",     getSiteRoot   ());
        ret += SS_DEBUG_ROW    ( "asset path",    pathToAsset   (""));
        ret += SS_DEBUG_ROW    ( "template path", pathToTemplate(""));
        ret += SS_DEBUG_ROW    ( "db path",       pathToDatabase(""));
        ret += SS_DEBUG_ROW    ( "siteEmail",     getSiteData   ("mainEmail"));
    }
    
	if (types & DB_FORUM)
    {
        ret += SS_DEBUG_ROW    ( "<hr>","<hr>"    );
        ret += SS_DEBUG_ROW    ( "<b>Forum data","" );
        ret += SS_DEBUG_STRING ( "thread"         );
        ret += SS_DEBUG_STRING ( "parentID"       );
        ret += SS_DEBUG_STRING ( "mode"           );
        ret += SS_DEBUG_COOKIE ( "sort"           );
        ret += SS_DEBUG_COOKIE ( "reverse"        );
        ret += SS_DEBUG_COOKIE ( "details"        );
        ret += SS_DEBUG_COOKIE ( "expand"         );
        ret += SS_DEBUG_COOKIE ( "page"           );
        ret += SS_DEBUG_COOKIE ( "perPage"        );
        ret += SS_DEBUG_ROW    ( "<hr>","<hr>"    );
    }
	ret += "</table>";
    
	ret += ("Whoami: " + SFos::doCommand("whoami") + "<br>");
	ret += ("pwd: "    + SFos::doCommand("pwd")    + "<br>");
	if (types & DB_USERS)
    {
		ret += "<h3>Users</h3>";
        const CSimpleUserDB *userDB = getSimpleServer()->getUserDB();
        for (int i=0;i<userDB->getUserCount();i++)
        {
            const CPerson *p = userDB->getUserAt(i);
            ret += "`link:Login~formForm=1~email="+p->getMainEmailNoLink()+"~password="+p->getPassword()+"|"+p->getMainEmailNoLink()+"`<br>";
            
        }
    }
    
	ret += "`link:"+getString("cmd")+"~debug=0|debug off`";
	return ret;
}

//-----------------------------------------------------------------------------------------
const char* STR_FORM_ROW_TEXT=
"<tr>\n\t\t<td width=\"20px\">&nbsp;</td>\n\t\t<td nowrap width=\"7%\">[PROMPT]:</td>\n\t\t"
"<td><input [D] type=\"[TYPE]\" name=\"[NAME_L]\" value=\"[[NAME_U]]\" size=\"[SIZE]\" /> <span class=\"warning\">[REQ][[NAME_U]_ERR]<span></td>\n\t"
"</tr>";

//-----------------------------------------------------------------------------------------
const char* STR_FORM_ROW_MULTILINE=
"<tr valign=top>\n\t\t<td width=\"20px\">&nbsp;</td>\n\t\t<td nowrap width=\"7%\">[PROMPT]:</td>\n\t\t"
"<td><textarea [D] type=\"[TYPE]\" name=\"[NAME_L]\" cols=\"[SIZE]\">[[NAME_U]]</textarea><span class=\"warning\">[REQ][[NAME_U]_ERR]<span></td>\n\t"
"</tr>";

//-----------------------------------------------------------------------------------------
const char* STR_FORM_ROW_CHECKBOX=
"<tr>\n\t\t\t<td width=\"5%\"></td>\n\t\t\t<td nowrap width=\"7%\"></td>\n\t\t\t"
"<td><input [D] type=\"checkbox\" name=\"[NAME_L]\" [CHECKED]> <label for=\"[NAME_L]\" />[LABEL]</label></td>\n\t\t"
"</tr>";

//-----------------------------------------------------------------------------------------
SFString getIconLink(const SFString& subCmd, const SFString& className, SFInt32 handle)
{
       	CSimpleServer *server = getSimpleServer();
       	if (!server->isLoggedIn())
               	return "<x>";

        return "`link:" + server->getStringBase("cmd",EMPTY) +
                               	"&subCmd=" + subCmd +
                               	"&type="   + className +
                               	"&handle=" + asString(handle) +
                               	"|<img style=\"border:0px;\" src=../images/" + toLower(subCmd) + ".gif>`";
}

//--------------------------------------------------------------------------------
SFString nextChunk_common(const SFString& fieldIn, const SFString& cmd, const CBaseNode *node)
{
	SFString className = node->getRuntimeClass()->getClassNamePtr();
	switch (tolower(fieldIn[0]))
	{
		case 'a':
			if ( fieldIn % "add" ) return getIconLink(fieldIn, className, node->getHandle());
			break;
		case 'e':
			if ( fieldIn % "edit" ) return node->isDeleted() ? "<x>" : getIconLink(fieldIn, className, node->getHandle());
			break;
		case 'd':
			if ( fieldIn % "delete" ) return getIconLink((node->isDeleted() ? "UN" : EMPTY) + fieldIn, className, 
node->getHandle());
			if ( fieldIn % "deleted" ) return asString(node->isDeleted());;
			break;
		case 'h':
			if ( fieldIn % "handle" ) return asString(node->getHandle());
			break;
		case 'n':
			if ( fieldIn % "null" ) return "<x>";
			break;
		case 's':
			if ( fieldIn % "schema" ) return asString(node->getSchema());
			break;
		default:
			break;
	}
	
	return EMPTY;
}
