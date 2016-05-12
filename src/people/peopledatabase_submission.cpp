/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "peopledatabase.h"

//---------------------------------------------------------------------------------------
void CUserDatabase::LoadSubmissionSettings(SFString& contents)
{
	if (m_version.eqEarlier(2,0,2))
		return;

	SFInt32 nLines = 6;
	if (m_version.eqEarlier(3,8,5))
		nLines = 5;

	SFInt32  linesRead = 0;
	while (!contents.IsEmpty())
	{
		SFString line = nextTokenClearDecrypt(contents, '\n');
		if (!isComment(line)) // is this line a comment?
		{
			if (m_version.eqLater(4,0,1))
			{
				nLines = 1;
				CSubSettings *settings = getSubSettingsEdit();
				if (linesRead == 0)
					settings->fromAscii(line);
				else
					ASSERT(0); // should not happen

			} else
			{
				switch (linesRead)
				{
				case 0:
					if (line != "none")
						m_subSettings.setIncludeList(line);
					break;
				case 1:
					m_subSettings.setAction(atoi((const char *)line));
					break;
				case 2:
					m_subSettings.setExact(atoi((const char *)line));
					break;
				case 3:
					if (line == "-*-")
						line = "";
					m_subSettings.setReplaceStr(line);
					break;
				case 4:
					m_subSettings.setTracking(atoi((const char *)line));
					break;
				case 5:
					m_subSettings.setVerbose(atoi((const char *)line));
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
