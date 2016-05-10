#ifndef _MAILHEADER_H
#define _MAILHEADER_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "sfos.h"

//-------------------------------------------------------------------------
// An email message
//-------------------------------------------------------------------------
class CMailMessage
{
	SFString m_MailServer;
	SFString m_Subject;
	SFString m_Sender;
	SFBool   m_VerboseLog;
	SFString m_LogFile;
	SFString m_Program;
	SFString m_Body;
	SFString m_Recipient;
	SFString m_FriendlyName;
	SFString m_ReplyTo;
	SFString m_CC;
	SFString m_BCC;

public:
	              CMailMessage(void)                         { Init();                   }
	              CMailMessage(const CMailMessage& data)     { Copy(data);               }
	CMailMessage& operator=   (const CMailMessage& data)     { Copy(data); return *this; }

	SFString getMailServer    (void) const                   { return m_MailServer;      }
	SFString getSubject       (void) const                   { return m_Subject;         }
	SFString getSender        (void) const                   { return m_Sender;          }
	SFBool   getVerbose       (void) const                   { return m_VerboseLog;      }
	SFString getLogFile       (void) const                   { return m_LogFile;         }
	SFString getProgram       (void) const                   { return m_Program;         }
	SFString getBody          (void) const                   { return m_Body;            }
	SFString getRecipient     (void) const                   { return m_Recipient;       }
	SFString getFriendlyName  (void) const                   { return m_FriendlyName;    }
	SFString getReplyTo       (void) const                   { return m_ReplyTo;         }
	SFString getCC            (void) const                   { return m_CC;              }
	SFString getBCC           (void) const                   { return m_BCC;             }

	void setServer            (const SFString& mailserver)   { m_MailServer   = mailserver;   }
	void setSubject           (const SFString& subject)      { m_Subject      = subject;      }
	void setSender            (const SFString& sender)       { m_Sender       = sender;       }
	void setVerbose           (SFBool on)                    { m_VerboseLog   = on;           }
	void setLogFile           (const SFString& logFile)      { m_LogFile      = logFile;      }
	void setProgram           (const SFString& mailer)       { m_Program      = mailer;       }
	void setBody              (const SFString& body)         { m_Body         = body;         }
	void setRecipient         (const SFString& recipient)    { m_Recipient    = recipient;    }
	void setReplyTo           (const SFString& reply)        { m_ReplyTo      = reply;        }
	void setFriendlyName      (const SFString& friendlyname) { m_FriendlyName = friendlyname; }
	void setCC                (const SFString& cc)           { m_CC           = cc;           }
	void setBCC               (const SFString& bcc)          { m_BCC          = bcc;          }

	SFBool hasEmails          (void) const                   { return !(getRecipient() + getCC() + getBCC()).IsEmpty(); }
	SFBool hasOnlyCopies      (void) const                   { return hasEmails() && getRecipient().IsEmpty();          }

	SFBool parseAddresses     (SFString& recipients, SFString& cc, SFString& bcc) const;

	SFString ToString         (SFInt32 fmt=0) const;
	SFString getSendBody      (SFBool asHTML) const;

private:
	void Init(void)
		{
//			m_MailServer;
//			m_Subject;
//			m_Sender;
			m_VerboseLog = FALSE;
//			m_LogFile;
			m_Program = "/usr/sbin/sendmail";
//			m_FriendlyName;
//			m_ReplyTo;
//			m_Body;
//			m_Recipient;
//			m_CC;
//			m_BCC;
		}
	void Copy(const CMailMessage& data)
		{
			m_MailServer   = data.m_MailServer;
			m_Subject      = data.m_Subject;
			m_Sender       = data.m_Sender;
			m_VerboseLog   = data.m_VerboseLog;
			m_LogFile	   = data.m_LogFile;
			m_Program      = data.m_Program;
			m_FriendlyName = data.m_FriendlyName;
			m_ReplyTo      = data.m_ReplyTo;
			m_Body         = data.m_Body;
			m_Recipient    = data.m_Recipient;
			m_CC           = data.m_CC;
			m_BCC          = data.m_BCC;
		}
};

//-------------------------------------------------------------------------
// An email mailer (to promote portability)
//-------------------------------------------------------------------------
class CMailer
{
public:
	              CMailer       (void) { }
	static SFBool Status        (SFString& result);
	       void   SendMail      (const CMailMessage& msg, SFString& result, SFBool asHTML);
};

#endif
