/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"
#include "mail.h"

#define repTo msg.getReplyTo()

//-----------------------------------------------------------------------------------------------
void CMailer::SendMail(const CMailMessage& msg, SFString& result, SFBool asHTML)
{
	SFString recipients, cc, bcc;
	if (!msg.parseAddresses(recipients, cc, bcc))
	{
		result = "Mail message contains no valid emails";
		return;
	}

	ASSERT(!recipients.IsEmpty());
	if (!Status(result))
		return;

#ifdef TESTING
	result = msg.ToString();
	
#else
	result = "Message sent";
	FILE *out = popen((const char *)(msg.getProgram() + " -t -f '" + msg.getSender() + "' "), "w");
	if (out)
	{
		SFString contentType = (asHTML ? "text/html" : "text/plain");
		SFString body        = msg.getSendBody(asHTML);
		
							  fprintf (out, "To: %s\n",           (const char*)recipients       );
							  fprintf (out, "From: %s\n",         (const char*)msg.getSender()  );
							  fprintf (out, "Subject: %s\n",      (const char*)msg.getSubject() );
		if (!repTo.IsEmpty()) fprintf (out, "Reply-To: %s\n",     (const char*)repTo            );
		if (!cc.IsEmpty())    fprintf (out, "CC: %s\n",           (const char*)cc               );
		if (!bcc.IsEmpty())   fprintf (out, "BCC: %s\n",          (const char*)bcc              );
							  fprintf (out, "Content-Type: %s\n", (const char*)contentType      );
							  fprintf (out, "\n"                                                ); // finishes header
							  fprintf (out, "%s\n",               (const char*)body             );

		pclose(out);

	} else
	{
		result = "Process (" + msg.getProgram() + ") failed";
	}

#endif

	return;
}

//-----------------------------------------------------------------------------------------------
SFBool CMailer::Status(SFString& result)
{
	return TRUE;
}

//-----------------------------------------------------------------------------------------------
static SFString convert2HTML(const SFString& str)
{
	SFString ret = str;
	ret.ReplaceAll("\n", "<br>");
	ret.ReplaceAll("\r", EMPTY);
	ret.ReplaceAll("  ", " &nbsp;");
	return ret;
}

//-----------------------------------------------------------------------------------------------
static SFString convert2Text(const SFString& str)
{
	SFString ret = str;
	ret.ReplaceAll("<br>",           "\n");
	ret.ReplaceAll("<br/>",          "\n");
	ret.ReplaceAll("<br />",         "\n");
	ret.ReplaceAll("&nbsp;",         " ");
	ret.ReplaceAll("<b>",            EMPTY);
	ret.ReplaceAll("</b>",           EMPTY);
	ret.ReplaceAll("<i>",            EMPTY);
	ret.ReplaceAll("</i>",           EMPTY);
	ret.ReplaceAll("<small>",        EMPTY);
	ret.ReplaceAll("</small>",       EMPTY);
	ret.ReplaceAll("<pre>",          EMPTY);
	ret.ReplaceAll("</pre>",         EMPTY);
	ret.ReplaceAll("<font size=-1>", EMPTY);
	ret.ReplaceAll("</font>",        EMPTY);
	ret.ReplaceAll("<p>",            "\n\n");
	ret.ReplaceAll("<hr>",           "\n");
	ret.ReplaceAll("&gt;",           ">");
	ret.ReplaceAll("&lt;",           "<");
	return ret;
}

//------------------------------------------------------------------------------
SFString CMailMessage::getSendBody(SFBool asHTML) const
{
	return (asHTML ? convert2HTML(getBody()) : convert2Text(getBody()));
}

//-----------------------------------------------------------------------------------------------
SFString CMailMessage::ToString(SFInt32 fmt) const
{
	SFString ret;

	ret += ("MailServer: "   + m_MailServer   + "<br>");
	ret += ("Subject: "      + m_Subject      + "<br>");
	ret += ("Sender: "       + m_Sender       + "<br>");
	ret += ("Program: "      + m_Program      + "<br>");
	ret += ("Body: "         + m_Body         + "<br>");
	ret += ("Recipient: "    + m_Recipient    + "<br>");
	ret += ("FriendlyName: " + m_FriendlyName + "<br>");
	ret += ("ReplyTo: "      + m_ReplyTo      + "<br>");
	ret += ("CC: "           + m_CC           + "<br>");
	ret += ("BCC: "          + m_BCC          + "<br>");
	ret += ("LogFile: "      + m_LogFile      + "<br>");

	return ret;
}

//-----------------------------------------------------------------------------------------------
// we must protect against malicious input so we trim the email list to only valid email 
// addresses assuming that any part of the thing that is not a valid  address may be malicous
static SFString cleanEmailList(const SFString& inList)
{
    SFString in = inList;	
	in.ReplaceAll (" ",  EMPTY);		  
	in.ReplaceAny (";",  CH_COMMA);		  
	in.ReplaceAny (":",  CH_COMMA);
    in.ReplaceAll (", ", CH_COMMA);

    SFString ret;
    while (!in.IsEmpty())
    {
        SFString email = nextTokenClear(in, CH_COMMA);
        if (IsValidEmail(email))
        {
            if (!ret.IsEmpty())
                ret += ",";
            ret += email;
        }
    }
    return ret;
}

//-----------------------------------------------------------------------------------------------
SFBool CMailMessage::parseAddresses(SFString& recipients, SFString& cc, SFString& bcc) const
{
	recipients = cleanEmailList(getRecipient());
	cc         = cleanEmailList(getCC       ());
	bcc        = cleanEmailList(getBCC      ());

	// make sure at least one email address is in recipient field
	// otherwise the mail component won't send anything
	while (recipients.IsEmpty() && (!cc.IsEmpty() || !bcc.IsEmpty()))
	{
		if (cc.IsEmpty()) recipients = nextTokenClear(bcc, CH_COMMA);
		else              recipients = nextTokenClear(cc,  CH_COMMA);
	}

	return !recipients.IsEmpty();
}

//---------------------------------------------------------------------------------------
SFBool IsValidEmail(const SFString& email)
{
	if (             email.IsEmpty   ()     ) return FALSE;
	if (             email.GetLength () < 5 ) return FALSE;                      // x@x.x smallest possible email
	if (            !email.Contains  ("@")  ) return FALSE;                      // '@' symbol is missing
	if (            !email.Contains  (".")  ) return FALSE;                      // '.' is missing
	if (countOf('@', email)             > 1 ) return FALSE;                      // no more than one '@' symbol
	if (             email.startsWith('@') || email.endsWith('@')) return FALSE; // may not start or end with '@'
	if (             email.startsWith('.') || email.endsWith('.')) return FALSE; // may not start or end with '.'
	
	// Check for invalid characters
	SFString copy = email;
	SFString repStr = CHR_VALID_NAME; repStr.ReplaceAny(CHR_ALLOW_EMAIL, EMPTY);
	copy.ReplaceAny(repStr, EMPTY);
	if (email != copy)
		return FALSE;
	
	// Check length of tld (2 or 3 chars)
	SFString tld = email.Mid(email.ReverseFind('.')+1);
	if (tld.GetLength() < 2 && tld.GetLength() > 3)
		return FALSE;
	
	return TRUE;
}

/*
THIS CODE USED TO WORK IN WINDOWS - I SAVED IT IF IT IS EVER NEEDED
#include <windows.h>
#include "basetypes.h"
#include "mail.h"
 
//-----------------------------------------------------------------------------------------------
void CMailer::SendMail(const CMailMessage& msg, SFString& result, SFBool asHTML)
{
	SFString recipients, cc, bcc;
	if (!msg.parseAddresses(recipients, cc, bcc))
	{
		result = "Mail message contains no valid emails";
		return;
	}
 
	ASSERT(!recipients.IsEmpty());
	if (!Status(result))
		return;

	CkEmail email;

	email.put_From       (msg.getSender   ()      );
	email.put_Subject    (msg.getSubject  ()      );
	email.put_ReplyTo    (msg.getReplyTo  ()      );
	if (asHTML) email.SetHtmlBody(msg.getSendBody(asHTML)); else email.SetTextBody(msg.getSendBody(asHTML), "text/plain");

	setRecipients(&email, recipients, cc, bcc);

	CkMailMan mailman;
	mailman.put_SmtpHost           (msg.getMailServer());
	//	mailman.put_SmtpPort           (25); // leave default alone - may need this later
	if (msg.getVerbose())
	mailman.put_LogMailSentFilename(msg.getLogFile   ());
	mailman.put_ConnectTimeout     (5);

	if (mailman.SendEmail(&email))
		result = "Message sent";
	else
	{
		CkString err;
		mailman.LastErrorHtml(err);
		result = getStatusMessage(mailman.get_LastSmtpStatus()) + "<br>" + err.getAnsi();
	}

	// Clean it so it doesn't build up - we just ignore bad emails.
	mailman.ClearBadEmailAddresses();

#ifdef TESTING
	result = "Unit Testing";
#endif

	return;
}
 
//-----------------------------------------------------------------------------------------------
SFBool CMailer::Status(SFString& result)
{
	CkMailMan mailman;
	if (mailman.IsUnlocked())
		return TRUE;

	// To unlock a different component - search for 'UNLOCK' in *.txt
	// to find a list of all license unlock keys
	if (mailman.UnlockComponent("STRUSHMAILQ_FfnAKQLU6R9Z"))
		return TRUE;

	result = "Failed to unlock email component";
	return FALSE;
}

//-----------------------------------------------------------------------------------------------
void CMailer::setRecipients(void *item, SFString& recipients, SFString& cc, SFString& bcc)
{
#define ADD_TO(rIn)  ((CkEmail*)item)->AddTo (EMPTY, nextTokenClear (rIn, CH_COMMA))
#define ADD_CC(cIn)  ((CkEmail*)item)->AddCC (EMPTY, nextTokenClear (cIn, CH_COMMA))
#define ADD_BCC(bIn) ((CkEmail*)item)->AddBcc(EMPTY, nextTokenClear (bIn, CH_COMMA))
	while (!recipients.IsEmpty()) ADD_TO(recipients);
	while (!cc.IsEmpty())         ADD_CC(cc);
	while (!bcc.IsEmpty())        ADD_BCC(bcc);
 }
 
//-----------------------------------------------------------------------------------------------
// These are chilkat error messages
 //-----------------------------------------------------------------------------------------------
SFString getStatusMessage(long status)
{
	SFString ret = "SMTP Status Message: ";
 	switch (status)
	{
		case 211: ret += "System status, or system help reply";
		case 214: ret += "Help message";
		case 220: ret += "<domain> Service ready";
		case 221: ret += "<domain> Service closing transmission channel";
		case 250: ret += "Requested mail action okay, completed";
		case 251: ret += "User not local; will forward to <forward-path>";
		case 354: ret += "Start mail input; end with <CRLF>.<CRLF>";
		case 421: ret += "<domain> Service not available, closing transmission channel";
		case 450: ret += "Requested mail action not taken: mailbox unavailable";
		case 451: ret += "Requested action aborted: local error in processing";
		case 452: ret += "Requested action not taken: insufficient system storage";
		case 500: ret += "Syntax error, command unrecognized";
		case 501: ret += "Syntax error in parameters or arguments";
		case 502: ret += "Command not implemented";
		case 503: ret += "Bad sequence of commands";
		case 504: ret += "Command parameter not implemented";
		case 550: ret += "Requested action not taken: mailbox unavailable";
		case 551: ret += "User not local; please try <forward-path>";
		case 552: ret += "Requested mail action aborted: exceeded storage allocation";
		case 553: ret += "Requested action not taken: mailbox name not allowed";
		case 554: ret += "Transaction failed";
		default:
		ret = EMPTY;
	}
	return ret;
}

//-----------------------------------------------------------------------------------------------
// Put this in the header file
#include "../chilkat/include/CkMailMan.h"
 
*/
