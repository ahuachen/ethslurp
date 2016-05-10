#ifndef _SFARCHIVE_H
#define _SFARCHIVE_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#define DEF_DELIMITER ((char)0x5)

#include "exportcontext.h"
#include "importcontext.h"
#include "database.h"

#define ARCHIVE_READ_ONLY  (100)
#define ARCHIVE_WRITE_ONLY (200)

//-----------------------------------------------------------------------------------------
class SFArchive
{
public:
	char                m_fieldEnd;
	char                m_recordEnd;
	SFBool              m_writeDeleted;
	SFInt32             m_schema;
	CFileExportContext *m_writeFile;
	CFileImportContext *m_readFile;

	SFArchive(const SFString& filename, SFBool storing, SFInt32 schema, SFBool writeDeleted)
		{
			m_writeDeleted = writeDeleted;
			m_schema       = schema;
			m_fieldEnd     = DEF_DELIMITER;
			m_recordEnd    = ((char)0x6);
			// open it for writing if we are told to
			if (storing == ARCHIVE_WRITE_ONLY)
			{
				m_writeFile = new CFileExportContext(filename, asciiWriteCreate);
				m_readFile = NULL;

			} else
			{
				m_readFile = new CFileImportContext(filename, asciiReadOnly);
				m_writeFile = NULL;
			}
		}
	
	void Close(void)
		{
			if (m_writeFile) m_writeFile->Close();
			if (m_readFile)  m_readFile->Close();
			m_writeFile = NULL;
			m_readFile  = NULL;
		}
	
	SFBool  writeDeleted(void) const
		{
			return (m_writeDeleted);
		}
	SFInt32 getSchema(void) const
		{
			return (m_schema);
		}
	SFBool isWriting(void) const
		{
			return (m_writeFile != NULL);
		}
	SFBool isReading(void) const
		{
			return (m_readFile != NULL);
		}

	SFArchive& operator<<(char c);
	SFArchive& operator<<(long dw);
	SFArchive& operator<<(float f);
	SFArchive& operator<<(double f);
	SFArchive& operator<<(const char *str);
	SFArchive& operator<<(const SFString& str);
	SFArchive& operator<<(const SFTime& tm);
	SFArchive& operator<<(const SFAttribute& attr);
	SFArchive& operator<<(const CDoublePoint& pt);
	SFArchive& operator<<(const CDoubleRect& rect);

	SFArchive& operator>>(char& c);
	SFArchive& operator>>(long& dw);
	SFArchive& operator>>(float& f);
	SFArchive& operator>>(double& f);
	SFArchive& operator>>(char* &str);
	SFArchive& operator>>(SFString& str);
	SFArchive& operator>>(SFTime& tm);
	SFArchive& operator>>(SFAttribute& attr);
	SFArchive& operator>>(CDoublePoint& pt);
	SFArchive& operator>>(CDoubleRect& rect);
};

extern SFArchive& operator<<(SFArchive& archive, SFStringArray& array);
extern SFArchive& operator>>(SFArchive& archive, SFStringArray& array);

#endif
