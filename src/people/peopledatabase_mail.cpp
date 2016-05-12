/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "peopledatabase.h"

//---------------------------------------------------------------------------------------
void CUserDatabase::LoadMailTable(SFString& contents)
{
	if (m_version.eqEarlier(1,0,2))
		return;

	SFInt32 linesRead = 0;
	while (!contents.IsEmpty())
	{
		SFString line = nextTokenClearDecrypt(contents, '\n');
		if (!isComment(line)) // is this line a comment?
		{
			if (m_version.eqLater(3,5,3))
			{
				m_emailSettings.fromAscii(line, m_version);
				linesRead++;
				if (linesRead==1)
					break;

			} else if (m_version.eqLater(2,0,1))
			{
				// for version on or after 2.0.1 (added logging option)
				switch (linesRead)
				{
				case 0:
					m_emailSettings.m_Method = atoi((const char *)line);
					break;
				case 1:
					m_emailSettings.m_HTML = atoi((const char *)line);
					break;
				case 2:
					m_emailSettings.m_Logging = atoi((const char *)line);
					break;
				case 3:
					m_emailSettings.m_Server = line;
					break;
				case 4:
					m_emailSettings.m_FromAddr = line;
					break;
				case 5:
					m_emailSettings.m_Program = line;
					break;
				default:
					// should not happen
					ASSERT(0);
					return;
				}
				linesRead++;
				if (linesRead==6)
					break;

			} else
			{
				switch (linesRead)
				{
				case 0:
					m_emailSettings.m_Method = atoi((const char *)line);
					break;
				case 1:
					m_emailSettings.m_HTML = atoi((const char *)line);
					break;
				case 2:
					m_emailSettings.m_Server = line;
					break;
				case 3:
					m_emailSettings.m_FromAddr = line;
					break;
				case 4:
					m_emailSettings.m_Program = line;
					break;
				default:
					// should not happen
					ASSERT(0);
					return;
				}
				linesRead++;
				if (linesRead==5)
					break;
			}
		} else
		{
			// this table has at least 4 lines - if 4 and then a comment
			// then this is old style and we should quit.
			if (linesRead==4)
				break;
		}
	}

	// Didn't have blind carbons in the past so set to
	// off for all blind carbons
	if (!m_version.eqLater(4,0,1) && m_emailSettings.m_Method != EMAIL_NEVER)
		m_emailSettings.setBCC(BCC_ALL, FALSE);

	// 4 or more lines means we read it correctly
	return;
}

//---------------------------------------------------------------------------------------
static void parseLine(const SFString& input, SFInt32 cnt, SFString *strs)
{
	SFString line = input;
	for (int i=0;i<cnt;i++)
		strs[i] = nextTokenClear(line, ';');
}

//---------------------------------------------------------------------------------------
void CUserDatabase::LoadDateSettings(SFString& contents)
{
	if (m_version.eqEarlier(1,0,2))
		return;

	SFInt32 linesRead = 0;
	SFString strings[4][12];

	SFBool   euro = FALSE, mil  = FALSE;
	SFString dSep = "/",   tSep = ":";
	SFInt32  fDay = SUNDAY;
	SFInt32  sTZ  = 0;

	while (!contents.IsEmpty())
	{
		SFString line = nextTokenClearDecrypt(contents, '\n');
		if (!isComment(line)) // is this line a comment?
		{
			if (linesRead<4)
				parseLine(line, (linesRead<2 ? 12 : 7), &strings[linesRead][0]);
			else
			{
				euro = atoi(nextTokenClear(line, ';'));
				mil  = atoi(nextTokenClear(line, ';'));
				dSep = nextTokenClear(line, ';');
				dSep.ReplaceAll("$", ";");
				tSep = nextTokenClear(line, ';');
				tSep.ReplaceAll("$", ";");
				fDay = atoi(nextTokenClear(line, ';'));
				if (fDay == 0)
					fDay = SUNDAY;
				sTZ  = atoi(nextTokenClear(line, ';'));
			}
			linesRead++;
			if (linesRead==5)
				break;
		}
	}

	if (linesRead>3)
	{
		if (m_version.eqEarlier(3,8,0) && strings[3][3] == "Thur")
			strings[3][3] = "Thu";
		SFos::resetDateNames(strings[0], strings[1], strings[2], strings[3], euro, mil, dSep, tSep, fDay, sTZ);
	}
		
	return;
}

//---------------------------------------------------------------------------------------
void CUserDatabase::LoadLoginSettings(SFString& contents)
{
	if (m_version.eqEarlier(1,2,0))
		return;

	SFInt32  nLines    = 3;
	SFInt32  linesRead = 0;
	while (!contents.IsEmpty())
	{
		SFString line = nextTokenClearDecrypt(contents, '\n');
		if (!isComment(line)) // is this line a comment?
		{
			if (m_version.eqLater(4,0,1))
			{
				nLines = 1;
				if (linesRead == 0)
					m_loginSettings.fromAscii(line);
				else
					ASSERT(0); // should not happen

			} else
			{
				// Parsing for older versions
				switch (linesRead)
				{
				case 0:
					setAllowRemote(atoi((const char *)line));
					break;

				case 1:
					{
						// ADD permission in older versions is converted to EDIT because prior to 
						// this ADD and EDIT had identical behavior.
						SFInt32 perm = atoi((const char *)line);
						if (m_version.earlier(3,5,0) && perm == PERMISSION_ADD)
							perm = PERMISSION_EDIT;
						setRemotePerm(perm);
					}
					break;
				case 2:
					setLoginTimeout(atoi((const char *)line));
					if (getLoginTimeout() == NEVER_TIMEOUT)
						setLoginTimeout(1*60);
					break;
				}
			}

			linesRead++;
			if (linesRead == nLines)
				break;
		}
	}

	return;
}

//---------------------------------------------------------------------------------------
SFBool CUserDatabase::mailMethod(void) const
{
	if (m_emailSettings.m_Method && 
				! m_emailSettings.m_Server.IsEmpty() && 
				!(m_emailSettings.m_Server % "none") &&
				! m_emailSettings.m_FromAddr.IsEmpty() &&
				!(m_emailSettings.m_FromAddr % "none"))
	{
		return m_emailSettings.m_Method;
	}
	return FALSE;
}

//---------------------------------------------------------------------------------------
SFBool CUserDatabase::mailEnabled(void) const
{
	SFString unused;
	return ((mailMethod() != EMAIL_NEVER) && CMailer::Status(unused));
}

//---------------------------------------------------------------------------------------
SFBool CUserDatabase::mailDisabled(void) const
{
	return !mailEnabled();
}
