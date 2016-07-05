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

#include "sfos.h"
#include "sftime.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <glob.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/time.h>
#include <iostream>

#define ANY_FILETYPE -1

#include "string.h"

#define remove unlink

//------------------------------------------------------------------
static SFString escapePath_Special(const SFString& nameIn)
{
	SFString name = nameIn;
	name.ReplaceAll(" ", "\\ ");
	name.ReplaceAll("&", "\\&");
	name.ReplaceAll("(", "\\(");
	name.ReplaceAll(")", "\\)");
	name.ReplaceAll("'", "\\'");
	return name;
}

//------------------------------------------------------------------
int SFos::copyFile_Special( const SFString& fromIn, const SFString& toIn )
{
	SFString from = escapePath_Special(fromIn);
	SFString to   = escapePath_Special(toIn);

	const SFString copyCmd = "cp -pf";

	SFString command = copyCmd + " " + from + " " + to;
	system((const char *)command);
	return (int)fileExists(to);
}

//------------------------------------------------------------------
int SFos::copyFile_ifNewer( const SFString& fromIn, const SFString& toIn )
{
	SFTime fromDate = SFos::fileLastModifyDate(fromIn);
	SFTime toDate   = SFos::fileLastModifyDate(toIn);
	if (fromDate > toDate)
		return SFos::copyFile_Special(fromIn, toIn);
	return TRUE;
}

//------------------------------------------------------------------
static SFString escapePath(const SFString& nameIn)
{
	return nameIn;
}

//------------------------------------------------------------------
int SFos::copyFile( const SFString& fromIn, const SFString& toIn )
{
	SFString from = escapePath(fromIn);
	SFString to   = escapePath(toIn);

	const SFString copyCmd = "cp -pf";

	if (!SFos::fileExists(from))
		return 0;

	SFString toPath = to;
	nextTokenClearReverse(toPath, '/');
	if (!SFos::folderExists(toPath))
		return 0;

	SFString command = copyCmd + " " + from + " " + to;
	system((const char *)command);
	return (int)fileExists(to);
}

//------------------------------------------------------------------
static int globErrFunc(const char *epath, int eerrno)
{
//	perror(epath);
	return 0;
}

//------------------------------------------------------------------
// Returns a list of either files or folders, but not both.
//------------------------------------------------------------------
static void doGlob(SFInt32& nStrs, SFString *strs, const SFString& maskIn, SFBool wantFiles, SFBool keepPaths )
{
	ASSERT(!strs || nStrs);

	glob_t globBuf;

    SFString mask = maskIn;

	// should check return code
	glob( (const char *)mask, GLOB_MARK, globErrFunc, &globBuf);

	int n = (int)globBuf.gl_pathc;
	int max = (int)nStrs;
	nStrs = 0;
	char c;

	int i;
	for ( i = 0 ; i < n; i++ )
	{
		// get path
		char *tmp = globBuf.gl_pathv[i];
		// last char
		c   = tmp[strlen(tmp)-1];

		// if path ends in '/' then this is directory, filter accordingly

 		SFBool isDir = ('/' == c);
		SFBool listEm = ((isDir) ? !wantFiles : wantFiles);
		if (wantFiles == ANY_FILETYPE)
			listEm = TRUE;

		if ( listEm )
		{
			if ( NULL != strs )
			{
				SFString path = globBuf.gl_pathv[i] ;

				// filter specified directories and remove trailing '/'
				if (path.endsWith('/'))
					path = path.Left( path.GetLength() - 1 );

				if (!keepPaths)
				{
					// trim path to last directory / file
					path = basename( path );
					if (path.startsWith('/'))
						path = path.Mid(1);
					// The path we return is always just the name of the folder or file
					// without any leading (or even existing) '/'
					ASSERT(path.GetLength() && path[0] != '/');
				}

				if (wantFiles == ANY_FILETYPE)
				{
					if (isDir)
						path = "d-" + path;
					else
						path = "f-" + path;
				}

				strs[nStrs] = path;
			}

			nStrs++;
			if ( NULL != strs && nStrs >= max )
			{
				break;
			}
		}
	}

	globfree( &globBuf );
}

extern SFBool binaryFileToBuffer(const SFString& filename, SFInt32& nChars, char *buffer);
//------------------------------------------------------------------------------------------
SFString SFos::doCommand(const SFString& cmd)
{
	return SFos::doCommand(EMPTY, cmd);
}

//------------------------------------------------------------------------------------------
SFString SFos::doCommand(const SFString& tmpPathIn, const SFString& cmd)
{
	SFString tmpPath = tmpPathIn;
	if (tmpPath.IsEmpty())
		tmpPath = "/tmp/";
	if (!tmpPath.endsWith('/'))
		tmpPath += "/";

	SFString ret = "Refresh your browser to see this value.";

	SFString c = cmd;
	SFString filename = makeValidName(tmpPath + "cw_" + nextTokenClear(c, ' '));
	system((const char *)(cmd + ">" + filename));

	// Check twice for existance since the previous command creates the file but may take some time
	waitForCreate(filename);
	waitForCreate(filename);

	SFInt32 nChars;
	if (binaryFileToBuffer(filename, nChars, NULL))
	{
		char *buffer = new char[nChars + 100]; // safty factor
		if (binaryFileToBuffer(filename, nChars, buffer))
			ret.operator=((const char *)buffer);
		if (buffer)
			delete [] buffer;
	}
	SFos::removeFile(filename); //clear for next time
	return Strip(ret,'\n');
}

SFString SFos::getOS(void)
{
	return SFString("Linux");
}

//------------------------------------------------------------------
// this global data makes these date strings a per-installation value - these strings will
// be the same for all calendars on a machine
// day_names
//------------------------------------------------------------------
static SFString monthNames[] =
	{ "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
static SFString shortMonths[] =
	{ "Jan",     "Feb",      "Mar",   "Apr",   "May", "Jun",  "Jul",  "Aug",    "Sep",       "Oct",     "Nov",      "Dec" };
static SFString weekdayNames[] =
	{ "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
static SFString shortWeekdayNames[] =
	{ "Mon",    "Tue",     "Wed",       "Thu",     "Fri",    "Sat",      "Sun" };

//------------------------------------------------------------------
static SFInt32  stringsSet = FALSE;
static SFBool   euro       = FALSE;
static SFBool   mil        = FALSE;
static SFString dSep       = "/";
static SFString tSep       = ":";
static SFInt32  fDay       = SUNDAY;
static SFInt32  sTZ        = 0;

//------------------------------------------------------------------
SFString getCWD(const SFString& filename)
{
	char buffer[_MAX_PATH];
	::getcwd(buffer, _MAX_PATH);
	SFString folder = buffer;
	if (!folder.endsWith('/'))
		folder += "/";
	return folder + filename; // may be empty
}

//------------------------------------------------------------------
// day_names
void SFos::setDateNameDefaults(void)
{
	SFString a1[] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
	SFString a2[] = { "Jan",     "Feb",      "Mar",   "Apr",   "May", "Jun",  "Jul",  "Aug",    "Sep",       "Oct",     "Nov",      "Dec" };
	SFString a3[] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
	SFString a4[] = { "Mon",    "Tue",     "Wed",       "Thu",     "Fri",    "Sat",      "Sun" };
	resetDateNames(a1, a2, a3, a4, FALSE, FALSE, "/", ":", SUNDAY, 0);
}

//------------------------------------------------------------------
SFString SFos::asciiDateNames(SFInt32 which)
{
	SFInt32 n = 12;
	if (which > 1)
		n = 7;

	SFString ret;

	for (int i=0;i<n;i++)
	{
		switch(which)
		{
		case 0:
			ret += monthNames[i] + ";";
			break;
		case 1:
			ret += shortMonths[i] + ";";
			break;
		case 2:
			ret += weekdayNames[i] + ";";
			break;
		case 3:
			ret += shortWeekdayNames[i] + ";";
			break;
		default:
			// should not happen
			ASSERT(0);
			break;
		}
	}

	return ret;
}

//------------------------------------------------------------------
int SFos::dateNamesSet(void)
{
	return (int)stringsSet;
}

//------------------------------------------------------------------
void SFos::resetDateNames(SFString *mNames, SFString *smNames, SFString *dNames, SFString *sdNames, SFBool e, SFBool m, const SFString& d, const SFString& t, SFInt32 f, SFInt32 tz)
{
	stringsSet = FALSE;
	setDateNames(mNames, smNames, dNames, sdNames);
	euro = e;
	mil  = m;
	dSep = d;
	tSep = t;
	fDay = f;
	sTZ  = tz;
}

//------------------------------------------------------------------
void SFos::setDateNames(SFString *mNames, SFString *smNames, SFString *dNames, SFString *sdNames)
{
	if (!stringsSet)
	{
		for (int i=0;i<12;i++)
		{
			monthNames[i]  = mNames[i];
			shortMonths[i] = smNames[i];
			if (i<7)
			{
				weekdayNames[i]      = dNames[i];
				shortWeekdayNames[i] = sdNames[i];
			}
		}
	}
	stringsSet = TRUE;
}

//------------------------------------------------------------------
void SFos::getDateLocaleSetting(SFBool& e, SFBool& m, SFString& d, SFString& t, SFInt32& f, SFInt32& tz)
{
	e  = euro;
	m  = mil;
	d  = dSep;
	t  = tSep;
	f  = fDay;
	tz = sTZ;
}

//------------------------------------------------------------------
SFString SFos::getDateItemAt(SFInt32 which, SFInt32 n)
{
	ASSERT(which > -1 && which < 4 && n > -1);
	ASSERT(which < 2 || n < 7);
	switch (which)
	{
	case 0:
		return monthNames[n];
		break;
	case 1:
		return shortMonths[n];
		break;
	case 2:
		return weekdayNames[n];
		break;
	case 3:
		return shortWeekdayNames[n];
		break;
	}

	// should not happen
	ASSERT(0);
	return EMPTY;
}

//------------------------------------------------------------------
SFString SFos::GetLocaleInfo(SFInt32 type)
{
	switch( type )
	{
		case LOCALE_IDAYLZERO:			return "0";
		case LOCALE_STIME:				return tSep;
		case LOCALE_SDATE:				return dSep;
		case LOCALE_S1159:				return (mil ? " " : "am");
		case LOCALE_S2359:				return (mil ? " " : "pm");
		case LOCALE_S1159SH:			return (mil ? " " : "a");
		case LOCALE_S2359SH:			return (mil ? " " : "p");
		case LOCALE_SDAYNAME1:			return weekdayNames[0];
		case LOCALE_SDAYNAME2:			return weekdayNames[1];
		case LOCALE_SDAYNAME3:			return weekdayNames[2];
		case LOCALE_SDAYNAME4:			return weekdayNames[3];
		case LOCALE_SDAYNAME5:			return weekdayNames[4];
		case LOCALE_SDAYNAME6:			return weekdayNames[5];
		case LOCALE_SDAYNAME7:			return weekdayNames[6];
		case LOCALE_SABBREVDAYNAME1:	return shortWeekdayNames[0];
		case LOCALE_SABBREVDAYNAME2:	return shortWeekdayNames[1];
		case LOCALE_SABBREVDAYNAME3:	return shortWeekdayNames[2];
		case LOCALE_SABBREVDAYNAME4:	return shortWeekdayNames[3];
		case LOCALE_SABBREVDAYNAME5:	return shortWeekdayNames[4];
		case LOCALE_SABBREVDAYNAME6:	return shortWeekdayNames[5];
		case LOCALE_SABBREVDAYNAME7:	return shortWeekdayNames[6];
		case LOCALE_SMONTHNAME1:		return monthNames[0];
		case LOCALE_SMONTHNAME2:		return monthNames[1];
		case LOCALE_SMONTHNAME3:		return monthNames[2];
		case LOCALE_SMONTHNAME4:		return monthNames[3];
		case LOCALE_SMONTHNAME5:		return monthNames[4];
		case LOCALE_SMONTHNAME6:		return monthNames[5];
		case LOCALE_SMONTHNAME7:		return monthNames[6];
		case LOCALE_SMONTHNAME8:		return monthNames[7];
		case LOCALE_SMONTHNAME9:		return monthNames[8];
		case LOCALE_SMONTHNAME10:		return monthNames[9];
		case LOCALE_SMONTHNAME11:		return monthNames[10];
		case LOCALE_SMONTHNAME12:		return monthNames[11];
		case LOCALE_SABBREVMONTHNAME1:	return shortMonths[0];
		case LOCALE_SABBREVMONTHNAME2:	return shortMonths[1];
		case LOCALE_SABBREVMONTHNAME3:	return shortMonths[2];
		case LOCALE_SABBREVMONTHNAME4:	return shortMonths[3];
		case LOCALE_SABBREVMONTHNAME5:	return shortMonths[4];
		case LOCALE_SABBREVMONTHNAME6:	return shortMonths[5];
		case LOCALE_SABBREVMONTHNAME7:	return shortMonths[6];
		case LOCALE_SABBREVMONTHNAME8:	return shortMonths[7];
		case LOCALE_SABBREVMONTHNAME9:	return shortMonths[8];
		case LOCALE_SABBREVMONTHNAME10:	return shortMonths[9];
		case LOCALE_SABBREVMONTHNAME11:	return shortMonths[10];
		case LOCALE_SABBREVMONTHNAME12:	return shortMonths[11];
	}
	return "0";
}

//------------------------------------------------------------------
int SFos::mkdir( const SFString& name )
{
	return ::mkdir( (const char *)name , (mode_t)0755 );
}

//------------------------------------------------------------------
int SFos::rmdir( const SFString& name )
{
	return ::rmdir( (const char *)name );
}

//------------------------------------------------------------------
int SFos::removeFile( const SFString& name )
{
	return ::remove( (const char *)name );
}

//------------------------------------------------------------------
int SFos::moveFile( const SFString& from, const SFString& to )
{
	if (from % to)
		return TRUE;

	if (copyFile(from, to))
		return !removeFile(from); // remove file returns '0' on success
	return FALSE;
}

//------------------------------------------------------------------
static SFBool enabled = TRUE;
void SFos::encryptEnable(SFBool onOff)
{
	enabled = onOff;
}

//------------------------------------------------------------------
// admittedly very weak but we can fix them later
char *SFos::encrypt(char *in, char *out, int maxlen)
{
	if (!enabled
#ifdef TESTING
		|| g_unitTesting
#endif
		)
	{
		strcpy(out, in);
		return out;
	}

	ASSERT(strlen(in)+2 < (unsigned)maxlen);

	// since no valid string will have (char)1 in it we use this character to
	// show this string as being encrypted
	out[0] = 1;
	out[1] = 4; // random number to add to each character

	unsigned char *s    = (unsigned char *)in;
	unsigned char *sout = (unsigned char *)&out[2];

	SFInt32 cnt = 0;
	while (*s && cnt < maxlen)
	{
		*sout = (unsigned char)(*s + out[1]);
		s++;
		sout++;
		cnt++;
	}

	*sout = '\0';

	return out;
}

//------------------------------------------------------------------
char *SFos::decrypt(char *in, char *out, int maxlen)
{
	// test for problems...
	ASSERT(strlen(in)+2 < (unsigned)maxlen);

	// just in case...
	SFInt32 len = MIN((unsigned int)maxlen, strlen(in));
	if (in && in[0] != 1)
	{
		// the string is not encrypted
		strncpy(out, in, len);
		out[len] = '\0';
		return out;
	}

	unsigned char *s    = (unsigned char *)&in[2];
	unsigned char *sout = (unsigned char *)out;

	SFInt32 cnt = 0;
	while (*s && cnt < maxlen)
	{
		*sout = (unsigned char)(*s - in[1]);
		s++;
		sout++;
		cnt++;
	}

	*sout = '\0';

	return out;
}

//------------------------------------------------------------------
// Send a float representing seconds - adjust because Windows takes 1/1000 (thousandth)
// of a second and Linux takes 1/1000000 (millionth)
void SFos::sleep(float units)
{
	::usleep( (int)(units * 1000000.) );
}

//------------------------------------------------------------------
int SFos::folderEmpty(const SFString& folder)
{
	if (!SFos::fileExists(folder))
		return TRUE;

	SFInt32 nFiles = 0;
	SFos::listFiles(nFiles, NULL, folder + "*");
	if (nFiles == 0)
		return TRUE;
	if (nFiles != 2)
		return FALSE;

	// might be '.' and '..'
	SFString files[3];
	SFos::listFiles(nFiles, files, folder + "*");
	if (nFiles == 2)
		return ((files[0] == "."  && files[1] == "..") ||
		        (files[0] == ".." && files[1] == "."));

	return FALSE;
}

//------------------------------------------------------------------
SFBool SFos::isFolder(const SFString& folderName)
{
	SFString folder = folderName;
	if (!folder.endsWith('/'))
		folder += "/";
	return SFos::fileExists(folder+".");
}

//------------------------------------------------------------------
int SFos::folderExists(const SFString& folderName)
{
	if (folderName.IsEmpty())
		return FALSE;

	SFString folder = folderName;
	if (!folder.endsWith('/'))
		folder += '/';

	SFInt32 nFiles=0;
	SFos::listFiles(nFiles, NULL, folder+"*.*");
	// check to see if it is just folders
	if (!nFiles)
		SFos::listFolders(nFiles, NULL, folder+"*.*");
	if (!nFiles)
		SFos::listFolders(nFiles, NULL, folder+".");

	return (nFiles>0);
}

//----------------------------------------------------------------------------
int SFos::establishFolder(const SFString& fileName)
{
	if (SFos::fileExists(fileName) || SFos::folderExists(fileName))
		return TRUE;

	SFString targetFolder = fileName;

	SFInt32 find = targetFolder.ReverseFind('/');
	targetFolder = targetFolder.Left(find) + "/";
	if (targetFolder.startsWith("/Users/jrush/"))
	{
		SFString folder = targetFolder;
		folder.Replace("/Users/jrush/", EMPTY);
		SFString curFolder = "/Users/jrush/";
		while (!folder.IsEmpty())
		{
			curFolder += nextTokenClear(folder, '/') + "/";
			if (!SFos::folderExists(curFolder))
				SFos::mkdir(curFolder);
		}
	}
	return SFos::folderExists(targetFolder);
}

//----------------------------------------------------------------------------
// convert %dd hex values back to characters
//----------------------------------------------------------------------------
static char hex2Ascii(char *str)
{
	char c;
	c =  (char)((str[0] >= 'A' ? ((str[0]&0xDF)-'A')+10 : (str[0]-'0')));
	c *= 16;
	c = (char)(c + (str[1] >= 'A' ? ((str[1]&0xDF)-'A')+10 : (str[1]-'0')));
	return c;
}

//----------------------------------------------------------------------------
SFString hex2String(const SFString& inHex)
{
	SFString ret, in = inHex.startsWith("0x") ? inHex.Mid(2,inHex.GetLength()) : inHex;
	while (!in.IsEmpty())
	{
		SFString nibble = in.Left(2);
		in = in.Mid(2,in.GetLength());
		ret += hex2Ascii((char*)(const char*)nibble);
	}
	return ret;
}

//----------------------------------------------------------------------------
// unescape strings (i.e. convert hex back to chars)
//----------------------------------------------------------------------------
SFString unEscapeString(const char *in)
{
	char *dup = strdup(in);
	int i;
	int j;
	for(i=0, j=0; dup[j]; ++i, ++j)
	{
	    if((dup[i] = dup[j]) == '%')
	    {
		dup[i] = hex2Ascii(&dup[j+1]);
		j+=2;
	    }
	}
	dup[i] = '\0';
	SFString ret = dup;
	free(dup);
	return ret;
}

//----------------------------------------------------------------------------
// search for and return a value in the queryString (or "" if not found)
//----------------------------------------------------------------------------
SFString parseQueryString(const SFString& queryString, const SFString& fieldName)
{
	// queryString has the form: name=val&name=val&.... (no spaces)
	// it does not contain ? so find the value by name and then
	// return it's value.  Name search is case sensitive.

	// Look for the name surrounded by &name= first (this eliminates finding
	// johnsmith= when looking just for smith= for example).
	// Note: we add a leading '&' to make the following easier
	// (i.e. there is no special case for the first item in the
 	// name/value pairs

	SFInt32 start = queryString.Find("&"+fieldName+"=");
	if (start!=-1)
	{
		SFString value="";
		SFInt32 valStart = start+fieldName.GetLength()+2;
		if (valStart<queryString.GetLength())
			value = queryString.Mid(valStart);
		// find the end of this value
		SFInt32 end = value.Find("&");
		if (end==0)
		{
			return EMPTY; // no value was specified
		}

		if (end!=-1)
		{
			value = value.Left(end); // snag the value if not at end of queryString
		} else if (queryString.GetAt(queryString.GetLength()-1) == '=')
		{
			return EMPTY;
		}

		value.ReplaceAll("+", " ");
		value = unEscapeString((const char *)value);
		return value;
	}

	// name not found
	return EMPTY;
}

//------------------------------------------------------------------
long RandomValue(long a, long b)
{
	long range = labs(a-b);
	if (range == 0)
		return a;
	return MIN(a, b) + (rand() % range);
}

//------------------------------------------------------------------
static SFBool isKnownFiletype(const SFString& filename)
{
	return (filename == "."              ||
			filename == ".."             ||
			filename % HIDEFILENAME      ||
			filename % ARCHIVEFILENAME   ||
			filename % DELETEDFILENAME   ||
			filename % COMPACTFILENAME   ||

			filename % "email.log"       ||

			filename.Contains(".csv")    ||
			filename.Contains(".bak")    ||
			filename.Contains(".lck"));
}

//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
int SFos::removeFolder(const SFString& folderIn, SFString& removedList, SFString& notRemovedList, SFBool force)
{
	SFString folder = folderIn;
	if (!folder.endsWith('/'))
		folder += "/";
	ASSERT(folder.endsWith('/'));

	if (!SFos::folderEmpty(folder))
	{
		SFInt32 nFiles=0;
		SFos::listFiles(nFiles, NULL, folder + "*");
		SFString *files = new SFString[nFiles];
		SFos::listFiles(nFiles, files, folder + "*");

		for (int j=0;j<nFiles;j++)
		{
			if (files[j] != "." && files[j] != "..")
			{
				SFString filename = folder + files[j];

                SFBool shouldRemove = force || isKnownFiletype(files[j]);
                SFBool didRemove = FALSE;
                if (shouldRemove)
                    didRemove = !SFos::removeFile(filename); // returns '0' on sucess

                if (!didRemove)
				{
					// it was not removed
					notRemovedList += ("<li>./" + files[j] + " <b><u>not removed</u></b></li>");
				} else
				{
					// it was removed if removeFile return '0'
					removedList += ("<li>./" + files[j] + " removed</li>");
				}
			}
		}

		if (files)
			delete [] files;
	}

	// Try to remove the folder also
	if (SFos::folderEmpty(folder))
		SFos::rmdir((const char *)folder);

	return !SFos::fileExists((const char *)folder);
}

//------------------------------------------------------------------
SFInt32 lfloor(SFInt32 a, SFInt32 b)
{
  ASSERT(b > 0);
  return (a >= 0L ? a/b : (a%b == 0L) - 1 - labs(a)/b);
}

//------------------------------------------------------------------
int SFos::GetLocalTime( SF_TIMESTRUCT& timer )
{
	SF_TIMESTRUCT *tPtr;
	time_t now;
	time( &now );
	tPtr = localtime( &now );
	timer = *tPtr;
	timer.tm_year += 1900;
	timer.tm_mon += 1;

	return 0;
}

//------------------------------------------------------------------
int SFos::GetSystemTime( SF_TIMESTRUCT& timer )
{
	return GetLocalTime( timer );
}

extern SFInt32 doStat( const SFString& path );

//------------------------------------------------------------------
int SFos::fileExists(const SFString& file)
{
	if (file.IsEmpty())
		return FALSE;
	return !doStat(file);
}

//------------------------------------------------------------------
void SFos::fileCreateDate(const SFString& filename, SFTime *ret)
{
	if (!SFos::fileExists(filename))
		return;

	struct stat statBuf;
	stat( filename, &statBuf );
	// Convert time_t to struct tm
	tm *t = localtime(&statBuf.st_ctime);
	ASSERT(t);
	*ret = SFTime(t->tm_year + 1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}

//------------------------------------------------------------------
void SFos::fileLastModifyDate(const SFString& filename, SFTime *ret)
{
	if (!SFos::fileExists(filename))
		return;

	struct stat statBuf;
	stat( filename, &statBuf );

	// Convert time_t to struct tm
	tm *t = localtime(&statBuf.st_mtime);
	ASSERT(t);
	if (ret)
		*ret = SFTime(t->tm_year + 1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}

//------------------------------------------------------------------
SFTime SFos::fileCreateDate( const SFString& filename)
{
	SFTime date;
	SFos::fileCreateDate(filename, &date);
	return date;
}

//------------------------------------------------------------------
SFTime SFos::fileLastModifyDate( const SFString& filename)
{
	SFTime date;
	SFos::fileLastModifyDate(filename, &date);
	return date;
}

//------------------------------------------------------------------
// return 0 if file exists
//------------------------------------------------------------------
SFInt32 doStat( const SFString& path )
{
	SFString str = path;
	struct stat statBuf;
	SFInt32 rc = stat( (const char *)str, &statBuf );
	return rc;
}

//------------------------------------------------------------------
SFBool isImageFilename(const SFString& filename)
{
	if (filename.FindI(".jpg")!=-1) return TRUE;
	if (filename.FindI(".gif")!=-1) return TRUE;
	if (filename.FindI(".png")!=-1) return TRUE;
	return FALSE;
}

//------------------------------------------------------------------
SFInt32 countImages(const SFString& p, SFString *strs)
{
	SFString path = p;
	if (!path.endsWith('\\'))
		path += "\\";

	SFInt32 nGIFs = nFilesInFolder(path + "*.gif");
	SFInt32 nJPGs = nFilesInFolder(path + "*.jpg");
	SFInt32 nPNGs = nFilesInFolder(path + "*.png");

	if (!strs)
		return nGIFs + nJPGs + nPNGs;

	SFos::listFiles(nGIFs, strs,               path + "*.gif");
	SFos::listFiles(nJPGs, &strs[nGIFs],       path + "*.jpg");
	SFos::listFiles(nPNGs, &strs[nGIFs+nJPGs], path + "*.png");

	return nGIFs + nJPGs + nPNGs;
}

/*
 //------------------------------------------------------------------
int SFos::GetTimeZoneInformation(TIME_ZONE_INFORMATION& tzi)
{
	struct timeval now;
	gettimeofday( &now, &tzi );
	return tzi.tz_dsttime;
}
*/

//------------------------------------------------------------------
extern void doGlob(SFInt32& nStrs, SFString *strs, const SFString& mask, SFBool wantFiles, SFBool keepPaths );

//------------------------------------------------------------------
SFInt32 SFos::nFiles(const SFString& path)
{
	SFInt32 count=0;
	SFos::listFiles(count, NULL, path);
	return MAX(0, count);
}

//------------------------------------------------------------------
void SFos::listFiles(SFInt32& nStrs, SFString *strs, const SFString& mask)
{
	::doGlob( nStrs, strs, mask, true, mask.Contains("/*/"));
}

//------------------------------------------------------------------
SFInt32 SFos::nFolders(const SFString& path)
{
	SFInt32 count=0;
	SFos::listFolders(count, NULL, path);
	return count; // don't count '.' or '..'
}

//------------------------------------------------------------------
void SFos::listFolders(SFInt32& nStrs, SFString *strs, const SFString& mask)
{
	::doGlob( nStrs, strs, mask, false, mask.Contains("/*/"));
}

//------------------------------------------------------------------
void SFos::listFilesOrFolders(SFInt32& nStrs, SFString *strs, const SFString& mask)
{
	::doGlob( nStrs, strs, mask, ANY_FILETYPE, mask.Contains("/*/"));
}

//------------------------------------------------------------------
int SFos::fileSize(const SFString& filename)
{
	if (!SFos::fileExists(filename))
		return 0;

	struct stat statBuf;
	stat( (const char *)filename, &statBuf );
	return (int)statBuf.st_size;
}

//-------------------------------------------------------------
void SFos::copyFolder(const SFString& from, const SFString& to)
{
	SFString fromFolder = from;
	if (!fromFolder.endsWith('/'))
		fromFolder += "/";
	if (!SFos::folderExists(fromFolder))
		return;

	SFString toFolder = to;
	if (!toFolder.endsWith('/'))
		toFolder += "/";

	SFString mask = fromFolder + "*.*";
	SFInt32 nItems = nFilesInFolder(mask);
	if (nItems)
	{
		SFString *files = new SFString[nItems];
		if (files)
		{
			SFos::mkdir(toFolder);

			SFos::listFiles(nItems, files, mask);
			for (int i=0;i<nItems;i++)
			{
				SFString filename = fromFolder + files[i];
				if (files[i] != "." && files[i] != "..")
				{
					SFString source = fromFolder + files[i];
					SFString dest   = toFolder   + files[i];
					SFos::copyFile(source, dest);
				}
			}
			delete [] files;
		}
	}
}

//----------------------------------------------------------------------------------------
SFString nextTokenClearDecrypt(SFString& contents, char token)
{
	SFString line = nextTokenClear(contents, token);
	char out[4096];
	SFos::decrypt((char*)(const char*)line, out, 4096);
	line = out;
	line.ReplaceAny("\t\n\r", EMPTY);
	return line;
}

//---------------------------------------------------------------------------------------
SFString niceFileSize(const SFString& fileName)
{
	float fileSize = (float)(SFos::fileSize(fileName) / 1024.);
	SFString size = "KB";
	if (fileSize > 1000.0)
	{
		fileSize /= 1024.;
		size = "MB";
	}
	char s[20];
	sprintf(s, "%-15.2f %s", fileSize, (const char *)size);
	return SFString(s);
}

//----------------------------------------------------------------------------------
void listItems(SFInt32& nItems, SFString *items, const SFString& mask, SFBool folders)
{
	if (folders)
		SFos::listFolders(nItems, items, mask);
	else
		SFos::listFiles(nItems, items, mask);
}

//----------------------------------------------------------------------------------
SFBool isFileNewer(const SFString& file1, const SFString& file2)
{
	SFTime tm1 = SFos::fileLastModifyDate( file1 );
	SFTime tm2 = SFos::fileLastModifyDate( file2 );
	if (tm1 == tm2)
		return FALSE;
	return tm1 > tm2;
}

//----------------------------------------------------------------------------------
SFBool isFileOlder(const SFString& file1, const SFString& file2)
{
	SFTime tm1 = SFos::fileLastModifyDate( file1 );
	SFTime tm2 = SFos::fileLastModifyDate( file2 );
	if (tm1 == tm2)
		return FALSE;
	return !isFileNewer(file1, file2);
}
