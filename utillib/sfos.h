#ifndef _SFOS_H_
#define _SFOS_H_
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "datetypes.h"

typedef struct timezone TIME_ZONE_INFORMATION;

#define LOCALE_USER_DEFAULT         0
#define LOCALE_SDAYNAME1            11
#define LOCALE_SDAYNAME2            12
#define LOCALE_SDAYNAME3            13
#define LOCALE_SDAYNAME4            14
#define LOCALE_SDAYNAME5            15
#define LOCALE_SDAYNAME6            16
#define LOCALE_SDAYNAME7            17

#define LOCALE_SABBREVDAYNAME1      111
#define LOCALE_SABBREVDAYNAME2      112
#define LOCALE_SABBREVDAYNAME3      113
#define LOCALE_SABBREVDAYNAME4      114
#define LOCALE_SABBREVDAYNAME5      115
#define LOCALE_SABBREVDAYNAME6      116
#define LOCALE_SABBREVDAYNAME7      117

#define LOCALE_SMONTHNAME1          21
#define LOCALE_SMONTHNAME2          22
#define LOCALE_SMONTHNAME3          23
#define LOCALE_SMONTHNAME4          24
#define LOCALE_SMONTHNAME5          25
#define LOCALE_SMONTHNAME6          26
#define LOCALE_SMONTHNAME7          27
#define LOCALE_SMONTHNAME8          28
#define LOCALE_SMONTHNAME9          29
#define LOCALE_SMONTHNAME10         30
#define LOCALE_SMONTHNAME11         31
#define LOCALE_SMONTHNAME12         32

#define LOCALE_SABBREVMONTHNAME1    41
#define LOCALE_SABBREVMONTHNAME2    42
#define LOCALE_SABBREVMONTHNAME3    43
#define LOCALE_SABBREVMONTHNAME4    44
#define LOCALE_SABBREVMONTHNAME5    45
#define LOCALE_SABBREVMONTHNAME6    46
#define LOCALE_SABBREVMONTHNAME7    47
#define LOCALE_SABBREVMONTHNAME8    48
#define LOCALE_SABBREVMONTHNAME9    49
#define LOCALE_SABBREVMONTHNAME10   50
#define LOCALE_SABBREVMONTHNAME11   51
#define LOCALE_SABBREVMONTHNAME12   52

#define DATE_SHORTDATE              60
#define DATE_LONGDATE               61
#define LOCALE_IDAYLZERO            62
#define LOCALE_IMONLZERO            63
#define LOCALE_SDATE                64
#define LOCALE_IDATE                65
#define LOCALE_ITLZERO              66
#define LOCALE_STIME                67
#define LOCALE_S1159                68
#define LOCALE_S2359                69
#define LOCALE_ITIME                70

#define TIME_ZONE_ID_UNKNOWN        0
#define TIME_ZONE_ID_DAYLIGHT       1

#define LOCALE_IFIRSTDAYOFWEEK      90

const SFInt64 _I64_MIN = BAD_NUMBER;

#define LOCALE_S1159SH             271
#define LOCALE_S2359SH             272

//-------------------------------------------------------------------------------
extern SFString getCWD(const SFString& filename=nullString);

//-------------------------------------------------------------------------------
class SFTime;

//-------------------------------------------------------------------------------
// os specific definitions
//-------------------------------------------------------------------------------
class SFos
{
public:
	static void     fileCreateDate       ( const SFString& filename, SFTime *ret);
	static void     fileLastModifyDate   ( const SFString& filename, SFTime *ret);
	static SFTime   fileCreateDate       ( const SFString& filename);
	static SFTime   fileLastModifyDate   ( const SFString& filename);
	static int      fileSize             ( const SFString& file );
	static int      fileExists           ( const SFString& file );

	static SFInt32  nFiles               ( const SFString& path );
	static void     listFiles            ( SFInt32& nFiles, SFString *files, const SFString& mask );
	static void     listFilesOrFolders   ( SFInt32& nItems, SFString *items, const SFString& mask );
	static SFInt32  nFolders             ( const SFString& path );
	static void     listFolders          ( SFInt32& nDirs, SFString *dirs, const SFString& mask);
	static int      folderEmpty          ( const SFString& folderName );
	static int      folderExists         ( const SFString& folderName );
	static SFBool   isFolder             ( const SFString& folderName );
	static int      establishFolder      ( const SFString& fileName   );
	static int      removeFolder         ( const SFString& folder, SFString& removedList, SFString& notRemovedList, SFBool force=FALSE);
	static void     copyFolder           ( const SFString& from, const SFString& to );

	static int      removeFile           ( const SFString& file );
	static int      copyFile             ( const SFString& from, const SFString& to );
	static int      copyFile_Special     ( const SFString& from, const SFString& to );
	static int      copyFile_ifNewer     ( const SFString& from, const SFString& to );
	static int      moveFile             ( const SFString& from, const SFString& to );

	static int      mkdir                ( const SFString& name );
	static int      rmdir                ( const SFString& name );

	static void     sleep                ( float units );

	static void     encryptEnable        ( SFBool onOff );
	static char    *encrypt              ( char *in, char *out, int maxLength );
	static char    *decrypt              ( char *in, char *out, int maxLength );

	// customizing date settings
	static SFString GetLocaleInfo        ( SFInt32 type );
	static int      dateNamesSet         (void);
	static void     resetDateNames       (SFString *mNames, SFString *smNames, SFString *dNames, SFString *sdNames, SFBool e, SFBool m, const SFString& d, const SFString& t, SFInt32 fDay, SFInt32 tz);
	static SFString asciiDateNames       (SFInt32 which);
	static void     setDateNameDefaults  (void);
	static SFString getDateItemAt        (SFInt32 which, SFInt32 n);
	static void     getDateLocaleSetting (SFBool& euro, SFBool& mil, SFString& dSep, SFString& tSep, SFInt32& fDay, SFInt32& tz);

	static SFString getValue             (SFInt32 type);
	static SFBool   isMilitary           (void);
	static SFBool   isEuropean           (void);
	static SFString getDateSep           (void);
	static SFString getTimeSep           (void);
	static SFInt32  getFirstDay          (void);
	static SFInt32  getTimeZone          (void);
	static int      GetLocalTime         ( SF_TIMESTRUCT& );
	static int      GetSystemTime        ( SF_TIMESTRUCT& );

	static SFString doCommand            (const SFString& cmd);
	static SFString doCommand            (const SFString& tmpPath, const SFString& cmd);
	static SFString getOS                (void);

private:
	static void     setDateNames         (SFString *mNames, SFString *smNames, SFString *dNames, SFString *sdNames);
};

//-------------------------------------------------------------------------------
inline SFBool validPath(const SFString& str)
{
	// No empty paths, path must end with '/', no DOS paths, no double slashes
	return (!str.IsEmpty() && str.endsWith('/') && !str.Contains("\\") && !str.Contains("//"));
}

//-------------------------------------------------------------------------------
inline SFInt32 nFilesInFolder(const SFString& mask)
{
	SFInt32 nFiles = 0;
	SFos::listFiles(nFiles, NULL, mask);
	return nFiles;
}

#define MILITARY 1
#define EUROPEAN 2
#define DSEP     3
#define TSEP     4
#define FDAY     5
#define TZ       6
//-------------------------------------------------------------------------------
inline SFString SFos::getValue(SFInt32 type)
{
	SFBool   euro = FALSE, mil  = FALSE;
	SFString dSep = "/",   tSep = ":";
	SFInt32  fDay = SUNDAY;
	SFInt32  tz   = 0;
	SFos::getDateLocaleSetting(euro, mil, dSep, tSep, fDay, tz);
	switch (type)
	{
	case MILITARY: return asString(mil);
	case EUROPEAN: return asString(euro);
	case DSEP:     return dSep;
	case TSEP:     return tSep;
	case FDAY:     return asString(fDay);
	case TZ:       return asString(tz);
	}

	return EMPTY;
}

//-------------------------------------------------------------------------------
inline SFBool   SFos::isMilitary  (void) { return atoi((const char*)getValue(MILITARY)); }
inline SFBool   SFos::isEuropean  (void) { return atoi((const char*)getValue(EUROPEAN)); }
inline SFString SFos::getDateSep  (void) { return                   getValue(DSEP);      }
inline SFString SFos::getTimeSep  (void) { return                   getValue(TSEP);      }
inline SFInt32  SFos::getFirstDay (void) { return atoi((const char*)getValue(FDAY));     }
inline SFInt32  SFos::getTimeZone (void) { return atoi((const char*)getValue(TZ));       }

extern SFString unEscapeString    (const char *in);
extern SFString parseQueryString  (const SFString& queryString, const SFString& fieldName);

#define DELETEDFILENAME "deleted"
#define HIDEFILENAME    "hidden.fil"
#define ARCHIVEFILENAME "archive.fil"
#define COMPACTFILENAME "compact.readme"

extern SFInt32  countImages     (const SFString& p, SFString *strs);
extern SFBool   isImageFilename (const SFString& filename);
extern SFString niceFileSize    (const SFString& fileName);

//-------------------------------------------------------------------------------------------------------------
inline SFBool waitForCreate(const SFString& filename)
{
	SFInt32 max = 500;

	SFInt32 cnt=0;
	while (cnt<max && !SFos::fileExists(filename))
		cnt++;
	
	return SFos::fileExists(filename);
}

//-------------------------------------------------------------------------------
inline void copyAndWait(const SFString& from, const SFString& to)
{
	// hang out until the copy operation happens

	SFos::removeFile(to);
	SFos::copyFile(from, to);
	SFBool done=0;
	while (done<25)
	{
		if (SFos::fileExists((const char *)to))
			break;
		done++;
	}
}

//------------------------------------------------------------------------------
extern void   listItems  (SFInt32& nItems, SFString *items, const SFString& mask, SFBool folders);
extern SFBool isFileOlder(const SFString& file1, const SFString& file2);
extern SFBool isFileNewer(const SFString& file1, const SFString& file2);
extern void   checkFolderDate(const SFString& folderIn, const SFString& fileIn);

#endif
