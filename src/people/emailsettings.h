#ifndef _EMAILSETTINGS_H
#define _EMAILSETTINGS_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

//-------------------------------------------------------------------------
#define EMAIL_NEVER   0
#define EMAIL_CONFIRM 1
#define EMAIL_AUTO    2
#define EMAIL_DEBUG   3

#define BCC_ES  (1<<1)  // event, super
#define BCC_EA  (1<<2)  // admin
#define BCC_EC  (1<<3)  // creator
#define BCC_EL  (1<<4)  // last edit
#define BCC_US  (1<<5)  // user, super
#define BCC_UA  (1<<6)  // admin
#define BCC_UU  (1<<7)  // user
#define BCC_UX  (1<<8)  // unused
#define BCC_GS  (1<<9)  // group, super
#define BCC_GA  (1<<10) // admin
#define BCC_GG  (1<<11) // group
#define BCC_GX  (1<<12) // unused
#define BCC_ALL (BCC_ES|BCC_EA|BCC_EC|BCC_EL|BCC_US|BCC_UA|BCC_UU|BCC_UX|BCC_GS|BCC_GA|BCC_GG|BCC_GX)

//-------------------------------------------------------------------------
class CEmailSettings
{
public:
	SFInt32  m_Method;
	SFString m_Server;
	SFString m_FromAddr;
	SFString m_Program;

	SFInt32  m_Bcc; // bitmap
	SFString m_Notify;

	SFBool   m_Comments;
	SFBool   m_AllFields;

	SFBool	 m_HTML;
	SFBool	 m_Logging;

	void     Init (void);
	void     Copy (const CEmailSettings& settings);

public:
	                CEmailSettings  (void);
	                CEmailSettings  (const CEmailSettings& settings);
	CEmailSettings& operator=       (const CEmailSettings& settings);

	void            setValueByField (CFieldData *field);

	SFString        toAscii         (void) const;
	void            fromAscii       (const SFString& str, const CVersion& version);

	SFBool          getBCC          (SFInt32 mask) const;
	void            setBCC          (SFInt32 mask, SFBool val);
};

#endif
