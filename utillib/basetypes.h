#ifndef _BASETYPES_H
#define _BASETYPES_H

/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

// Do not move or remove
#define LIGHT_WEIGHT 1

//-------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <iostream>
using namespace std;
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <utime.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>

//-------------------------------------------------------------------------
#define FORCE_ANSI    0x10000 // needed for Format function for some reason
#define FORCE_UNICODE 0x20000
typedef struct tm     SF_TIMESTRUCT;
typedef long          SFBool;
typedef long          SFInt32;
typedef unsigned long SFUint32;
typedef long long     SFInt64;
typedef unsigned char uchar;

//-------------------------------------------------------------------------
#ifndef _MAX_PATH
#define _MAX_PATH _POSIX_PATH_MAX
#endif

//-------------------------------------------------------------------------
#ifndef TRUE
#define FALSE (0)
#define TRUE  (1)
#endif

//-------------------------------------------------------------------------
#ifndef MIN
#define MIN(a, b) (((a)>(b)) ? (b) : (a))
#define MAX(a, b) (((a)>(b)) ? (a) : (b))
#define CLAMP(_vv, _ll, _hh) MIN(_hh, MAX(_ll, _vv))
#endif

//-------------------------------------------------------------------------
// Very important - do not change these values - they may be stored inside of binary files
#define MAGIC_NUMBER     ((SFInt32)0xdeadbeef)
#define BAD_NUMBER       ((SFInt32)0xdeadbeef)
#define TRASHCAN_REMOVE  ((SFInt32)0xdeaddead)

//-------------------------------------------------------------------------
#define SECS_PER_DAY  (86400L)
#define SECS_PER_HOUR  (3600L)
#define SECS_PER_MIN     (60L)

//-------------------------------------------------------------------------
#define PI         ((double)3.141592653589793)
#define DTOR		(PI/180.0)
#define RTOD		(180.0/PI)
#define RAD2DEG(r) ((double)RTOD*(r))
#define DEG2RAD(d) ((double)DTOR*(d))

//-------------------------------------------------------------------------
#ifdef _DEBUG
extern SFInt32 assertHit;
extern SFInt32 quitOnAssert;
extern SFInt32 showAssert;
#define ASSERT(a) { if (!(a)) { assertHit = 1; if (showAssert) printf("error at %s(%d)\n", __FILE__, __LINE__); }      }
#define VERIFY(a) { if (!(a)) { assertHit = 2; if (showAssert) printf("error at %s(%d)\n", __FILE__, __LINE__); } (a); }
#else
#define ASSERT(a)
#define VERIFY(a) (a)
#endif

//-------------------------------------------------------------------------
inline SFInt32 toLong(const char *str) { return (SFInt32)strtol((const char*)(str),NULL,10); }
inline SFInt32 toLong(char *str)       { return (SFInt32)strtol((const char*)(str),NULL,10); }

//-------------------------------------------------------------------------
// The one class that is used almost everywhere
#include "sfstring.h"

//-------------------------------------------------------------------------
inline void SAFE_COPY(char *to, const char *from, SFInt32 maxLen)
{
	strncpy(to, from, maxLen);
	to[MIN((SFInt32)maxLen-1, (SFInt32)strlen(from))] = '\0';
}

//-------------------------------------------------------------------------
// swap two values of any type
template <class T>
inline void Swap(T& t1, T& t2)
{
	T tmp;
	tmp = t1;
	t1 = t2;
	t2 = tmp;
}

//-------------------------------------------------------------------------
// is first value between min and max?
template<class T>
inline SFBool inRange(T val, T min, T max)
{
  return (val >= min && val <= max);
}

//-------------------------------------------------------------------------

typedef int    (*SEARCHFUNC)    (const void *ob1, const void *ob2);
typedef int    (*SORTINGFUNC)   (const void *ob1, const void *ob2);
typedef int    (*DUPLICATEFUNC) (const void *ob1, const void *ob2);
typedef SFBool (*APPLYFUNC)     (SFString& line, void *data);

//-------------------------------------------------------------------------
#define TESTING
#define DO_HEADER
#define BUILDABLE
#define ENABLE_EXPORT_ALL
#undef TESTING
#undef DO_HEADER
#undef BUILDABLE
#undef ENABLE_EXPORT_ALL

//-------------------------------------------------------------------------
#ifdef TESTING

extern SFBool  g_unitTesting;
extern SFInt32 getCurTestID(void);

#endif // TESTING

// Main folder locators - used only in testing code
#ifdef LINUX
#define UTESTING_MAIN                            SFString("/home/jrush/source/")
#define UTESTING_INSTALL                         SFString("/home/jrush/public_html/gh500/")
#else
#define UTESTING_MAIN                            SFString("/Users/jrush/")
#define UTESTING_INSTALL                         SFString("/Users/jrush/Sites/")
//src.installs/")
#endif

#define UTESTING_SCRIPTNAME                      SFString("/cgi-bin/calweb/calweb.cgi")
#define UTESTING_FOLDER      (UTESTING_INSTALL + SFString("data/calweb/"))
#define UTESTING_SOURCE      (UTESTING_MAIN    + SFString("src.calweb/"))
#define UTESTING_LIB_SOURCE  (UTESTING_MAIN    + SFString("src/libs/"))
#define UTESTING_JUNKPAGES   (UTESTING_INSTALL + SFString("junkPages/"))

//-------------------------------------------------------------------------
// This should never change but if it does it will break checkHosting functions
//-------------------------------------------------------------------------
#ifdef LINUX
#define VOXEL_HOST_NAME SFString("host1.lga6.greathill.com")
#else
#define VOXEL_HOST_NAME SFString("imac.home")
#endif

//-------------------------------------------------------------------------
extern SFInt32  lfloor      (SFInt32 a, SFInt32 b);
extern SFInt32  RandomValue (SFInt32 a, SFInt32 b);

//-----------------------------------------------------------------------------------------
inline void XX_C(const SFString& p, const SFString& v)
{
	static int xx_a_cnt=0;

	SFString fmt = "%d.<span class=warning>%s:</span>%s<br>\n";
	if (p.IsEmpty())
		fmt.Replace(":", EMPTY);

	fmt.Replace("<span class=warning>", EMPTY);
	fmt.Replace("</span>", EMPTY);
	fmt.Replace("<br>", EMPTY);

	printf((const char*)fmt, xx_a_cnt++, (const char*)p, (const char*)v);
//	fmt.ReplaceAll("<", "&lt;");
//	fmt.ReplaceAll(">", "&gt;");
//	printf("%s", (const char*)fmt);

	fflush(stdout);
}
#define XX_A(a) { SFString cp = (a); cp.ReplaceAll("<", "&lt;"); cp.ReplaceAll(">", "&gt;"); XX_C(#a, cp         );       }
#define XX_B(a) {                                                                            XX_C(#a, asString(a));       }

#define XX_H( ) { XX_C("", "HERE" ); }
#define XX_L( ) { XX_C("", SFString('~', 80)); }
#define XX_O(a) { XX_C("", (a)); }
#define XX_T( ) { XX_C("", "THERE"); }

#endif
