/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "peopledatabase.h"
#include "person.h"

//-------------------------------------------------------------------------
void CUserDatabase::LoadHeader(SFString& contents)
{
	LoadObjectTable();

	while (!contents.IsEmpty())
	{
		SFString line = nextTokenClearDecrypt(contents, '\n');
		if (line.Contains("; users"))
			return;

		if (!m_version.isSet())
			m_version.setVersionStr(line);
		else
			m_registration.fromAscii(line, m_version);
	}

	return;
}
