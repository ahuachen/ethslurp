#ifndef _DATABASE_H
#define _DATABASE_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "version.h"
#include "fielddata.h"

#define READ_OK 1
#define READ_EOF 2
#define READ_BAD 3

#define asciiReadOnly     "r"  // ascii read - fails if not present
#define asciiReadWrite    "r+" // ascii read-write (file must exist)
#define asciiWriteCreate  "w"  // ascii writing - destroys previous contents or creates
#define asciiWriteAppend  "a+" // ascii read/writing - appends

#define binaryReadOnly    "rb"  // binary read - fails if not present
#define binaryReadWrite   "rb+" // binary read/write - fails if not present
#define binaryWriteCreate "wb"  // binary write - destroys previous contents or creates

#define LOCK_NOWAIT       1 // read only - do not even check for a lock
#define LOCK_WAIT         2 // Wait for lock to release return TRUE - if wait too long return FALSE
#define LOCK_CREATE       3 // Wait for lock to release - if waiting longer than preset
                            // time destroy existing lock and create a new one

// So we know on which operating system the file was created
#define ON_WINDOWS 1 // WINDOWS
#define ON_LINUX   2 
#define OPERATING_SYSTEM ON_LINUX

//------------------------------------------------------------------------
// Class used as the first line of data files to identify this file and version
//------------------------------------------------------------------------
// IMPORTANT NOTE: DO NOT CHANGE THE FORMAT IF THIS CLASS - ALL VERSION STORE THE FIRST
// BYTES OF EVENT DATA FILES WITH THIS DATA IN THIS FORMAT - THE FORMAT MUST NOT CHANGE
// FROM VERSION TO VERSION
class CHeaderRecord
{
public:
	SFInt32 magic;
	SFInt32 major;
	SFInt32 minor1;
	SFInt32 minor2;
	SFInt32 nextID;
	SFInt32 expectedSize;
	SFInt32 os;
private:
	SFInt32 nRecords;

public:
	CHeaderRecord(void)
		{
			Init();
		}

private:
	void Init(void)
		{
			magic        =  0;
			nextID       =  1;
			nRecords     =  0;
			major        = -1;
			minor1       = -1;
			minor2       = -1;
			expectedSize = 0;
			os           = ON_LINUX;
		}

public:

	SFInt32 getRecordCount(void) const    { return nRecords;  }
	void    setRecordCount(SFInt32 nRecs) { nRecords = nRecs; }

	SFString toAscii(void) const;
};

//------------------------------------------------------------------------
inline SFString CHeaderRecord::toAscii(void) const
{
	SFString ret;
	ret += asString(magic)        + "|";
	ret += asString(major)        + "|";
	ret += asString(minor1)       + "|";
	ret += asString(minor2)       + "|";
	ret += asString(nextID)       + "|";
	ret += asString(expectedSize) + "|";
	ret += asString(os)           + "|";
	ret += asString(nRecords)     + "|";
	return ret;
}

//------------------------------------------------------------------------
// A file that must be locked before being accessed (such as a datafile)
//------------------------------------------------------------------------
class CSharedResource
{
private:
	SFString  m_mode;
	SFString  m_errorMsg;
	SFInt32   m_error;
	SFBool    m_ownsLock;
	SFString  m_lockingUser;
	SFBool    m_isascii;

protected:
	CVersion  m_version;
	SFString  m_filename;

public:
	FILE     *m_fp;

	CSharedResource(void)
		{
			m_fp       = NULL;
			m_error    = 0;
			m_ownsLock = FALSE;;
		  //m_lockingUser = "";
			//m_mode = "";
			//m_errorMsg = "";
			// m_version (leave it unset so we will only use it if its explicitly set)
			m_isascii  = FALSE;
		};
	virtual ~CSharedResource(void)
		{
			Release();
		}

  SFBool Lock    (const SFString& fn, const SFString& mode, SFBool obeyLock);
  SFBool ReLock  (const SFString& mode);
  void   Release (void);
	void   Close   (void);

	CVersion getVersion(void) const { return m_version; }

	SFBool isOpen(void) const
		{
			return (m_fp != NULL);
		}

	SFInt32  Tell(void) const;
	void     Seek        (SFInt32 offset, SFInt32 whence) const;
	SFInt32  Write       (const void *buff, SFInt32 size, SFInt32 cnt) const;
	SFInt32  Write       (const SFString& str) const;
	SFInt32  Read        (void *buff, SFInt32 cnt, SFInt32 size);
	SFBool   Eof         (void) const;
	char    *ReadLine    (char *buff, SFInt32 maxBuff);
	void     WriteLine   (const SFString& str);
	SFString LockFailure (void) const;

	virtual SFBool Upgrade(void)
		{
			return FALSE; // did not upgrade anything
		}

	SFString getFilename(void) const { return m_filename; }
	void     setFilename(const SFString& fn) { m_filename = fn; }

	// Use only for cases where file deletion does not work
	static SFBool setLocking(SFBool val);

	SFBool isAscii(void) const
		{
			return m_isascii;
		}

private:
	SFBool waitOnLock     (SFBool deleteOnFail) const;
	SFBool createLockFile (const SFString& lockfilename);
	SFBool createLock     (SFBool createOnFail);

	                 CSharedResource (const CSharedResource& l);
	CSharedResource& operator=       (const CSharedResource& l);

	// Turns on and off file locking for machines that do not allow file delete such as my ISP
	static SFBool g_locking; // = TRUE;
};

//----------------------------------------------------------------------
extern SFInt32  stringToAsciiFile (const SFString& fileName, const SFString& contents);
extern SFString binaryFileToString(const SFString& filename);
extern SFBool   binaryFileToBuffer(const SFString& filename, SFInt32& nChars, char *buffer);
extern SFBool   asciiFileToBuffer (const SFString& filename, SFInt32& nChars, SFString *buffer, SFUint32 maxLines=INT_MAX);
extern SFInt32  appendToAsciiFile (const SFString& fileName, const SFString& addContents);

//----------------------------------------------------------------------
inline SFBool asciiFileToBuffer(const SFString& filename, SFString *contents, SFUint32 maxLines=INT_MAX)
{
	SFInt32 unused = 0;
	return asciiFileToBuffer(filename, unused, contents, maxLines);
}

//----------------------------------------------------------------------
inline SFString asciiFileToString(const SFString& filename)
{
	SFString contents;
	asciiFileToBuffer(filename, &contents);
	return contents;
}

//----------------------------------------------------------------------
extern SFString docxToString     (const SFString& filename);
extern SFInt32  stringToDocxFile (const SFString& fileName, const SFString& contents);
extern SFInt32  stringToPDF      (const SFString& fileName, const SFString& contents);
extern SFString excelFileToString(const SFString& filename);

#endif
