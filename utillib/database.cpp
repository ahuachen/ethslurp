/*--------------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2016 Great Hill Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
--------------------------------------------------------------------------------*/
#include "basetypes.h"

#include "database.h"

#include "dates.h"
#include "sfos.h"

#define maxSecondsLock 3

#define LK_NO_CREATE_LOCK_FILE 1
#define LK_FILE_NOT_EXIST      2
#define LK_BAD_OPEN_MODE       3
#define LK_NO_REMOVE_LOCK      4

//----------------------------------------------------------------------
SFBool CSharedResource::g_locking = TRUE;

//----------------------------------------------------------------------
SFBool CSharedResource::setLocking(SFBool val)
{
	SFBool ret = g_locking;
	g_locking = val;
	return ret;
}

//----------------------------------------------------------------------
SFBool CSharedResource::createLockFile(const SFString& lockfilename)
{
	if (!g_locking)
		return TRUE;

	m_ownsLock = FALSE;
	FILE *fp = fopen((const char *)lockfilename, asciiWriteCreate);
	if (fp)
	{
		fprintf(fp, "%s;%s\n", (const char *)Now().Format(FMT_DEFAULT), (const char *)m_lockingUser);
		fclose(fp);
		m_ownsLock = TRUE;
		return m_ownsLock;
	}
	m_error = LK_NO_CREATE_LOCK_FILE;
	m_errorMsg = "Could not create lock file: " + lockfilename;
	return FALSE;  // why could I not create the lock file?
}

//----------------------------------------------------------------------
SFBool CSharedResource::createLock(SFBool createOnFail)
{
	if (!g_locking)
		return TRUE;

	SFString lockFilename = m_filename + ".lck";

	int i=0;
	while (i < maxSecondsLock)
	{
		if (!SFos::fileExists(lockFilename))
			return createLockFile(lockFilename);
		SFos::sleep(1.0);
		i++;
	}

	// Someone has had the lock for maxSecondsLock seconds -- if told to blow that lock away
	if (createOnFail)
	{
		// Release the old lock and create a new one
		m_ownsLock = TRUE;
		Release();
		return createLockFile(lockFilename);
	}

	return FALSE;
}

//----------------------------------------------------------------------
SFBool CSharedResource::waitOnLock(SFBool deleteOnFail) const
{
	if (!g_locking)
		return TRUE;

	SFString lockFilename = m_filename + ".lck";

	int i=0;
	while (i < maxSecondsLock)
	{
		if (!SFos::fileExists(lockFilename))
			return TRUE;
		SFos::sleep(1.0);
		i++;
	}

	// Someone has had the lock for maxSecondsLock seconds -- if told to blow that lock away
	if (deleteOnFail)
	{
		// Release the old lock and create a new one
		SFInt32 owns = m_ownsLock;
		((CSharedResource*)this)->m_ownsLock = TRUE;
		((CSharedResource*)this)->Release();
		((CSharedResource*)this)->m_ownsLock = owns;
		return TRUE;
	}

	return FALSE;
}

//----------------------------------------------------------------------
static SFBool isAscii(const SFString& mode)
{
	return (mode % asciiReadOnly ||
					mode % asciiReadWrite ||
					mode % asciiWriteCreate ||
					mode % asciiWriteAppend);
}

//----------------------------------------------------------------------
SFBool CSharedResource::ReLock(const SFString& mode)
{
	ASSERT(isOpen());
	ASSERT(m_ownsLock);

	// Close and re-open the file without relinqishing the lock
	Close();
	m_fp = fopen((const char *)m_filename, mode);
	m_isascii = ::isAscii(mode);

	return isOpen();
}

//----------------------------------------------------------------------
SFBool CSharedResource::Lock(const SFString& fn, const SFString& mode, SFBool lockType)
{
	ASSERT(!isOpen());

	m_filename = fn;
	m_mode     = mode;
	m_fp       = NULL;

	// If the file we are trying to lock does not exist but we are not trying to open
	// it under one of the create modes then do not create a lock, do not open the file,
	// and let the user know.
	if (!SFos::fileExists(m_filename) &&
				(m_mode != asciiWriteCreate &&
				 m_mode != asciiWriteAppend &&
				 m_mode != binaryWriteCreate))
	{
		m_error    = LK_FILE_NOT_EXIST;
		m_errorMsg = "File does not exist: " + m_filename;
		return FALSE;
	}

	SFBool openIt = TRUE; //FALSE;

	if (m_mode == binaryReadOnly ||
			m_mode == asciiReadOnly)
	{

		// Wait for lock to clear...
		if (lockType == LOCK_WAIT)
			waitOnLock(TRUE);

		// ... proceed even if it doesn't....
		openIt = TRUE;

	} else if (m_mode == binaryReadWrite ||
							m_mode == binaryWriteCreate ||
							m_mode == asciiReadWrite ||
							m_mode == asciiWriteAppend ||
							m_mode == asciiWriteCreate)
	{

		ASSERT(lockType == LOCK_CREATE || lockType == LOCK_WAIT);
		openIt = createLock(TRUE);

	} else
	{
		m_error    = LK_BAD_OPEN_MODE;
		m_errorMsg = "Bad file open mode: " + m_mode;
		return FALSE;
	}

	if (openIt)
	{
		m_fp = fopen((const char *)m_filename, (const char *)m_mode);  // operator on event database
		m_isascii = ::isAscii(m_mode);
	}

	return isOpen();
}

//----------------------------------------------------------------------
void CSharedResource::Release(void)
{
	Close();

	if (g_locking && m_ownsLock)
	{
		SFInt32 ret = SFos::removeFile((const char *)(m_filename + ".lck"));
		if (ret != 0)
		{
//			fprintf(stdout, "What happened?: %d: %s\n", ret, (const char *)(m_filename + ".lck"));
			m_error    = LK_NO_REMOVE_LOCK;
			m_errorMsg = "Could not remove lock";
		} else
		{
//			fprintf(stdout, "Removed it: %s\n", (const char *)(m_filename + ".lck"));
		}
	}

	m_ownsLock = FALSE;
}

//----------------------------------------------------------------------
void CSharedResource::Close(void)
{
	if (m_fp)
		fclose(m_fp);
	m_fp = NULL;
	m_isascii = FALSE;
}

//----------------------------------------------------------------------
SFString CSharedResource::LockFailure(void) const
{
	// In some cases (for example when can't open event file because it has not yet
	// been created) this may not be an error -- Lock should set an error flag
	// which this guy should read and do the right thing
    return asString(m_error) + ": " + m_errorMsg;
}

//----------------------------------------------------------------------
SFString binaryFileToString(const SFString& filename)
{
	SFString ret; SFInt32 nChars = 0;
	if (binaryFileToBuffer(filename, nChars, NULL))
	{
		char *buffer = new char[nChars + 100]; // safty factor
		if (binaryFileToBuffer(filename, nChars, buffer))
			ret = buffer;
		if (buffer)
			delete [] buffer;
	}
	return ret;
}

//----------------------------------------------------------------------
SFBool binaryFileToBuffer(const SFString& filename, SFInt32& nChars, char *buffer)
{
	if (!SFos::fileExists(filename))
	{
		nChars = 0;
		if (buffer)
			buffer[0] = '\0';
		return FALSE;
	}

	nChars = SFos::fileSize(filename);
	if (buffer)
	{
		CSharedResource lock;
		if (lock.Lock(filename, binaryReadOnly, LOCK_NOWAIT)) // do not wait for lock - read only file
		{
			ASSERT(lock.isOpen());
			lock.Seek(0, SEEK_SET);
			lock.Read(buffer, nChars, sizeof(char));
			buffer[nChars] = '\0';
			lock.Release();
		} else
			return FALSE;
	}
	return TRUE;
}

//----------------------------------------------------------------------
SFBool CSharedResource::Eof(void) const
{
	ASSERT(isOpen());
	return feof(m_fp);
}

//----------------------------------------------------------------------
void CSharedResource::Seek(SFInt32 offset, SFInt32 whence) const
{
	ASSERT(isOpen());
	fseek(m_fp, offset, (int)whence);
}

//----------------------------------------------------------------------
SFInt32 CSharedResource::Tell(void) const
{
	ASSERT(isOpen());
	return ftell(m_fp);
}

//----------------------------------------------------------------------
SFInt32 CSharedResource::Read(void *buff, SFInt32 size, SFInt32 cnt)
{
	ASSERT(isOpen());
	return (SFInt32)fread(buff, cnt, size, m_fp);
}

//----------------------------------------------------------------------
SFInt32 CSharedResource::Read(char&   val) { return Read(&val, sizeof(char),   1); }
SFInt32 CSharedResource::Read(long&   val) { return Read(&val, sizeof(long),   1); }
SFInt32 CSharedResource::Read(float&  val) { return Read(&val, sizeof(float),  1); }
SFInt32 CSharedResource::Read(double& val) { return Read(&val, sizeof(double), 1); }
SFInt32 CSharedResource::Read(SFTime& val) { return Read(&val.m_nSeconds, sizeof(SFInt64), 1); }

//----------------------------------------------------------------------
SFInt32 CSharedResource::Read(SFString& str)
{
	ASSERT(isOpen());
	ASSERT(!isAscii());

	SFInt32 len;
	Read(&len, sizeof(SFInt32), 1);

	if (len < 8192)
	{
		char buff[8192];
		Read(buff, sizeof(char), len);
		buff[len] = '\0';
		str = buff;
	} else
	{
		char *buff = new char[len+1];
		Read(buff, sizeof(char), len);
		buff[len] = '\0';
		str = buff;
		delete [] buff;
	}

	return len;
}

//----------------------------------------------------------------------
SFInt32 CSharedResource::Read(SFAttribute&  val)
{
	SFString name, value;
	if (Read(name) && Read(value))
	{
		val.set(name, value);
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------
SFInt32 CSharedResource::Read(CDoublePoint& val)
{
	double arr[2];
	SFInt32 ret = Read(&arr, sizeof(double), 2);
	if (ret)
	{
		val.x   = arr[0];
		val.y    = arr[1];
	}
	return ret;
}

//----------------------------------------------------------------------
SFInt32 CSharedResource::Read(CDoubleRect& val)
{
	double arr[4];
	SFInt32 ret = Read(&arr, sizeof(double), 4);
	if (ret)
	{
		val.left   = arr[0];
		val.top    = arr[1];
		val.right  = arr[2];
		val.bottom = arr[3];
	}
	return ret;
}

//----------------------------------------------------------------------
char *CSharedResource::ReadLine(char *buff, SFInt32 maxBuff)
{
	ASSERT(isOpen());
	ASSERT(isAscii());
	return fgets(buff, (int)maxBuff, m_fp);
}

//----------------------------------------------------------------------
SFInt32 CSharedResource::Write(const void *buff, SFInt32 size, SFInt32 cnt) const
{
	ASSERT(isOpen());
	return (SFInt32)fwrite(buff, size, cnt, m_fp);
}

//----------------------------------------------------------------------
SFInt32 CSharedResource::Write(char          val) const { return Write(&val, sizeof(char), 1); }
SFInt32 CSharedResource::Write(long          val) const { return Write(&val, sizeof(long), 1); }
SFInt32 CSharedResource::Write(float         val) const { return Write(&val, sizeof(float), 1); }
SFInt32 CSharedResource::Write(double        val) const { return Write(&val, sizeof(double), 1); }
SFInt32 CSharedResource::Write(const SFTime& val) const { return Write(&val.m_nSeconds, sizeof(SFInt64), 1); }

//----------------------------------------------------------------------
SFInt32 CSharedResource::Write(const SFString& val) const
{
	ASSERT(isOpen());
	ASSERT(!isAscii());

	SFInt32 len = val.GetLength();

	SFInt32 ret = Write(&len, sizeof(SFInt32), 1);
	return Write((const char *)val, sizeof(char), len) + ret;
}

//----------------------------------------------------------------------
SFInt32 CSharedResource::Write(const SFAttribute& val) const
{
	SFInt32 ret = Write(val.getName());
	return Write(val.getValue()) + ret;
}

//----------------------------------------------------------------------
SFInt32 CSharedResource::Write(const CDoublePoint& val) const
{
	double arr[] = { val.x, val.y };
	return Write(arr, sizeof(double), 2);
}

//----------------------------------------------------------------------
SFInt32 CSharedResource::Write(const CDoubleRect&  val) const
{
	double arr[] = { val.left, val.top, val.right, val.bottom };
	return Write(arr, sizeof(double), 4);
}

//----------------------------------------------------------------------
void CSharedResource::WriteLine(const SFString& str)
{
	ASSERT(isOpen());
	ASSERT(isAscii());
	fprintf(m_fp, "%s", (const char *)str);
}

//----------------------------------------------------------------------
SFBool asciiFileToBuffer(const SFString& filename, SFInt32& nChars, SFString *buffer, SFUint32 maxLines)
{
	SFBool isFolder = SFos::folderExists(filename);
	if (!SFos::fileExists(filename) || isFolder)
	{
		nChars = 0;
		if (buffer)
			*buffer = EMPTY;
		return FALSE;
	}

	SFInt32 nBytes = SFos::fileSize(filename);

	// Allocate a enough space to store the file so its not realloced each
	// time we do +=
	char *val = new char[nBytes+100];
	memset(val, 0, nBytes+100);
	char *s = val;

	CSharedResource lock;
	if (lock.Lock(filename, asciiReadOnly, LOCK_NOWAIT)) // do not wait for lock - read only file
	{
		SFUint32 nLines = 0;

		ASSERT(lock.isOpen());
		char buff[4096];
		while (nLines < maxLines && lock.ReadLine(buff, 4096))
		{
			nChars += strlen(buff);
			if (buffer)
			{
				int len = (int)strlen(buff);
				strncpy(s, buff, len);
				s += len;
			}

			nLines++;
		}

		lock.Release();
	} else
	{
		// ...and not report any errors since errors will have already been reported by InstallCheck
		//lock.LockFailure();
		if (val)
			delete [] val;
		return FALSE;
	}

	ASSERT(s);
	*s = '\0';

	if (buffer && val)
		*buffer = val;

	if (val)
		delete [] val;

	return TRUE;
}

//----------------------------------------------------------------------
SFString excelFileToString(const SFString& excelFilename)
{
	if (excelFilename.Contains(".xlsx"))
		return "Only .xls Excel files are supported";
	return SFos::doCommand("exportExcel " + excelFilename);
}

//----------------------------------------------------------------------
SFString docxToString(const SFString& filename)
{
    if (!filename.Contains(".docx"))
		return "Only .docx files are supported";
	return SFos::doCommand("/Users/jrush/source/docx2txt.pl " + filename.Substitute(" ", "\\ ").Substitute("'", "\\'") + " -");
}

//----------------------------------------------------------------------
SFInt32 stringToDocxFile(const SFString& fileName, const SFString& contents)
{
	SFString cmd = "/Users/jrush/source/createDocx \"" + fileName + "\" \"" + contents.Substitute("\"", "''") + "\"";
	SFString ret = SFos::doCommand(cmd);
	fprintf(stderr, "ret: %s\n", (const char*)ret);
	return TRUE;
}

//----------------------------------------------------------------------
SFInt32 stringToPDF(const SFString& fileName, const SFString& contents)
{
	SFString tmpName = "/tmp/toPDF.txt";
	SFString pdfName = "/tmp/toPDF.pdf";
	stringToAsciiFile(tmpName, contents);

	SFString cmd = "/Users/jrush/source/toPDF \"" + tmpName + "\" \"" + pdfName + "\" 2>/dev/null";
	SFos::doCommand(cmd);
	SFos::copyFile_Special(pdfName, fileName);

	SFos::removeFile(tmpName);
	SFos::removeFile(pdfName);

	return TRUE;
}

//----------------------------------------------------------------------
SFInt32 stringToAsciiFile(const SFString& fileName, const SFString& contents)
{
	CSharedResource lock;
	if (lock.Lock(fileName, asciiWriteCreate, LOCK_WAIT))
	{
		lock.WriteLine((const char*)contents);
		lock.Release();
	} else
	{
		fprintf(stderr, "%s\n", (const char*)SFString("Could not open file: " + fileName));
		return FALSE;
	}
	return TRUE;
}

//----------------------------------------------------------------------
SFInt32 appendToAsciiFile(const SFString& fileName, const SFString& addContents)
{
    SFString existing = asciiFileToString(fileName);
    stringToAsciiFile(fileName, existing + addContents);
    return TRUE;
}
