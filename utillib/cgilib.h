#ifndef CGI_C
#define CGI_C
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

//--------------------------------------------------------------------------
// The CGI_C library, by Thomas Boutell, version 2.01. CGI_C is intended
// to be a high-quality API to simplify CGI programming tasks.
//--------------------------------------------------------------------------

#include <stdio.h>

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

//--------------------------------------------------------------------------
// The various CGI environment variables. Instead of using getenv(),
// the programmer should refer to these, which are always
// valid null-terminated strings (they may be empty, but they 
// will never be null). If these variables are used instead
// of calling getenv(), then it will be possible to save
// and restore CGI environments, which is highly convenient
// for debugging.
//--------------------------------------------------------------------------
extern char *cgiServerSoftware;
extern char *cgiServerName;
extern char *cgiGatewayInterface;
extern char *cgiServerProtocol;
extern char *cgiServerPort;
extern char *cgiRequestMethod;
extern char *cgiPathInfo;
extern char *cgiPathTranslated;
extern char *cgiScriptName;
extern char *cgiQueryString;
extern char *cgiRemoteHost;
extern char *cgiRemoteAddr;
extern char *cgiAuthType;
extern char *cgiRemoteUser;
extern char *cgiRemoteIdent;
extern char *cgiContentType;
extern char *cgiAccept;
extern char *cgiUserAgent;
extern char *cgiReferrer;

//--------------------------------------------------------------------------
// Cookies as sent to the server. You can also get them
// individually, or as a string array; see the documentation.
//--------------------------------------------------------------------------
extern char *cgiCookie;

//--------------------------------------------------------------------------
// Possible return codes from the cgiForm family of functions (see below).
//--------------------------------------------------------------------------
typedef enum
{
	cgiFormSuccess,
	cgiFormTruncated,
	cgiFormBadType,
	cgiFormEmpty,
	cgiFormNotFound,
	cgiFormNoSuchChoice,
	cgiFormMemory,
	cgiFormConstrained,
	cgiFormNoFileName,
	cgiFormNoContentType,
	cgiFormNotAFile,
	cgiFormOpenFailed,
	cgiFormIO,
	cgiFormEOF
} cgiFormResultType;

//--------------------------------------------------------------------------
typedef struct cgiFileStruct *cgiFilePtr;

//--------------------------------------------------------------------------
typedef enum
{
	cgiEnvironmentIO,
	cgiEnvironmentMemory,
	cgiEnvironmentSuccess,
	cgiEnvironmentWrongVersion
} cgiEnvironmentResultType;

//--------------------------------------------------------------------------
// A macro providing the same incorrect spelling that is
// found in the HTTP/CGI specifications
//--------------------------------------------------------------------------
#define cgiReferer cgiReferrer

//--------------------------------------------------------------------------
// The number of bytes of data received.
// Note that if the submission is a form submission
// the library will read and parse all the information
// directly from cgiIn; the programmer need not do so.
//--------------------------------------------------------------------------
extern int cgiContentLength;

//--------------------------------------------------------------------------
// Pointer to CGI output. The cgiHeader functions should be used
// first to output the mime headers; the output HTML
// page, GIF image or other web document should then be written
// to cgiOut by the programmer. In the standard CGIC library,
// cgiOut is always equivalent to stdout.
//--------------------------------------------------------------------------
extern FILE *cgiOut;

//--------------------------------------------------------------------------
// Pointer to CGI input. The programmer does not read from this.
// We have continued to export it for backwards compatibility
// so that cgic 1.x applications link properly.
//--------------------------------------------------------------------------
extern FILE *cgiIn;

//--------------------------------------------------------------------------
// Initialization and destruction
//--------------------------------------------------------------------------
extern int                cgiInit                 (const SFString& type=nullString);
extern void               cgiRelease              (void);

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
extern cgiFormResultType cgiFormString           (const char *name, char *result, int max);
extern cgiFormResultType cgiFormStringMultiple   (const char *name, char ***ptrToStringArray);
extern cgiFormResultType cgiFormInteger          (const char *name, int *result, int defaultV);
extern cgiFormResultType cgiFormDouble           (const char *name, double *result, double defaultV);

extern cgiFormResultType cgiFormRadio            (const char *name, int *result, int defaultV);
extern cgiFormResultType cgiFormCheckboxSingle   (const char *name);
extern cgiFormResultType cgiFormCheckboxMultiple (const char *name, char **valuesText,  int valuesTotal, int *result, int *invalid);

extern cgiFormResultType cgiFormSelectSingle     (const char *name, char **choicesText, int choicesTotal, int *result, int defaultV);
extern cgiFormResultType cgiFormSelectMultiple   (const char *name, char **choicesText, int choicesTotal, int *result, int *invalid); 

extern cgiFormResultType cgiFormStringNoNewlines (const char *name, char *result, int max);
extern cgiFormResultType cgiFormStringSpaceNeeded(const char *name, int *length);
extern cgiFormResultType cgiFormIntegerBounded   (const char *name, int *result, int defaultV, int min, int max);
extern cgiFormResultType cgiFormDoubleBounded    (const char *name, double *result, double defaultV, double min, double max);

extern void cgiStringArrayFree(char **stringArray);

#define cgiFormSubmitClicked cgiFormCheckboxSingle

//--------------------------------------------------------------------------
// The paths returned by this function are the original names of files
// as reported by the uploading web browser and shoult NOT be
// blindly assumed to be "safe" names for server-side use! */
// The content type of the uploaded file, as reported by the browser.
// It should NOT be assumed that browsers will never falsify
// such information.
//--------------------------------------------------------------------------
extern cgiFormResultType cgiFormFileName        (const char *name, char *result, int max);
extern cgiFormResultType cgiFormFileContentType (const char *name, char *result, int max);
extern cgiFormResultType cgiFormFileSize        (const char *name, int *sizeP);
extern cgiFormResultType cgiFormFileOpen        (const char *name, cgiFilePtr *cfpp);
extern cgiFormResultType cgiFormFileRead        (cgiFilePtr cfp, char *buffer, int bufferSize, int *gotP);
extern cgiFormResultType cgiFormFileClose       (cgiFilePtr cfp);

//--------------------------------------------------------------------------
// path can be null or empty in which case a path of / (entire site) is set. 
// domain can be a single web site; if it is an entire domain, such as
// 'boutell.com', it should begin with a dot: '.boutell.com'
//--------------------------------------------------------------------------
extern cgiFormResultType cgiCookieString  (const char *name, char *result, int max);
extern cgiFormResultType cgiCookieInteger (const char *name, int *result, int defaultV);
extern cgiFormResultType cgiCookies       (char ***ptrToStringArray);
extern void cgiHeaderCookieSetString      (const char *name, char *value, int secondsToLive, char *path, char *domain);
extern void cgiHeaderCookieSetInteger     (const char *name, int value, int secondsToLive, char *path, char *domain);
extern void cgiHeaderLocation             (char *redirectUrl);
extern void cgiHeaderStatus               (int status, char *statusMessage);
extern void cgiHeaderContentType          (const char *mimeType);

//--------------------------------------------------------------------------
extern cgiEnvironmentResultType cgiWriteEnvironment(char *filename);

//--------------------------------------------------------------------------
extern cgiEnvironmentResultType cgiReadEnvironment(char *filename);

//--------------------------------------------------------------------------
extern int cgiMain();

//--------------------------------------------------------------------------
extern cgiFormResultType cgiFormEntries(char ***ptrToStringArray);

//--------------------------------------------------------------------------
// Output string with the <, &, and > characters HTML-escaped. 
// 's' is null-terminated. Returns cgiFormIO in the event
// of error, cgiFormSuccess otherwise.
//--------------------------------------------------------------------------
cgiFormResultType cgiHtmlEscape(char *s);

//--------------------------------------------------------------------------
// Output data with the <, &, and > characters HTML-escaped. 
// 'data' is not null-terminated; 'len' is the number of
// bytes in 'data'. Returns cgiFormIO in the event
// of error, cgiFormSuccess otherwise.
//--------------------------------------------------------------------------
cgiFormResultType cgiHtmlEscapeData(char *data, int len);

//--------------------------------------------------------------------------
// Output string with the " character HTML-escaped, and no
// other characters escaped. This is useful when outputting
// the contents of a tag attribute such as 'href' or 'src'.
// 's' is null-terminated. Returns cgiFormIO in the event
// of error, cgiFormSuccess otherwise.
//--------------------------------------------------------------------------
cgiFormResultType cgiValueEscape(char *s);

//--------------------------------------------------------------------------
// Output data with the " character HTML-escaped, and no
// other characters escaped. This is useful when outputting
// the contents of a tag attribute such as 'href' or 'src'.
// 'data' is not null-terminated; 'len' is the number of
// bytes in 'data'. Returns cgiFormIO in the event
// of error, cgiFormSuccess otherwise.
//--------------------------------------------------------------------------
cgiFormResultType cgiValueEscapeData(char *data, int len);

//--------------------------------------------------------------------------
// These functions may have to be implemented in the applications
//--------------------------------------------------------------------------
inline SFString cgiGetLink(const SFString& url, const SFString& display)
{
        SFString ret = "<a href=\"" + encode(url) + "\">" + display + "</a>";
        ret.Replace(encode("http://"), "http://");
        return ret;
}

//-------------------------------------------------------------------
extern SFBool isCGI          (void);
extern SFBool isFieldPresent (const SFString& name);

#define cgi_uCookie() unencode(cgiCookie)

//-------------------------------------------------------------------
#include "fielddata.h"

//-------------------------------------------------------------------
inline SFString getString_unSafe(const SFString& fieldName, const SFString& defVal=nullString)
{
	SFString ret;
	int maxSize=0;
	cgiFormStringSpaceNeeded(fieldName, &maxSize);
	if (maxSize)
	{
		char *val = new char[maxSize];
		cgiFormString(fieldName, val, maxSize);
		ret = val;
		if (val)
			delete [] val;
	}

	if (ret.IsEmpty())
		ret = defVal;
	return ret;
}

//-------------------------------------------------------------------
inline SFString getString_unSafe(CFieldData *field)
{
	ASSERT(field);
	SFString ret = getString_unSafe(field->getFieldName(), field->getDefStr());
	field->setValue(ret);
	return ret;
}

//-------------------------------------------------------------------
inline SFString getMultiline_unSafe(const SFString& fieldName, const SFString& defVal=nullString)
{
	return getString_unSafe(fieldName, defVal);
}

//-----------------------------------------------------------------------------------------
inline SFBool isCommand(const SFString& cmd)
{
	return (getString_unSafe("cmd", "not-set") == cmd);
}

//-------------------------------------------------------------------
inline SFInt32 getInt32(CFieldData *field)
{
	// Because this gets converted into an integer there is no danger of
	// an end user entering some sort of malicious of code so we allow
	// the integer form of CGI routines to be used freely.  CServer::getString
	// and its derivitives must be used for all other CGI access
	return atoi((const char *)getString_unSafe( field ));
}

//-------------------------------------------------------------------
inline SFInt32 getInt32(const SFString& fieldName, SFInt32 defVal)
{
	return atoi((const char *)getString_unSafe(fieldName, asString(defVal)));
}

//-------------------------------------------------------------------
inline SFInt32 getRadio(const SFString& fieldName, SFInt32 defVal)
{
	return getInt32(fieldName, defVal);
}

//-------------------------------------------------------------------
inline SFBool getCheckbox(const SFString& fieldName)
{
	return (getInt32(fieldName, BAD_NUMBER) != BAD_NUMBER);
}

//-------------------------------------------------------------------
inline SFString getContentType(const SFString& fieldName)
{
#ifdef TESTING
	if (g_unitTesting && !getString_unSafe("test_type").IsEmpty())
		return getString_unSafe("test_type");
#endif

	char contentType[1024];
	cgiFormFileContentType(fieldName, contentType, sizeof(contentType));
	return SFString(contentType);
}

inline SFBool isPost(void)
{
	return (SFString("POST") % SFString(cgiRequestMethod));
}

inline SFString getAgentString(void) { return toLower(cgiUserAgent); }
inline SFBool   isOpera       (void) { return getAgentString().ContainsI("opera");    }
inline SFBool   isFireFox     (void) { return getAgentString().ContainsI("firefox");  }
inline SFBool   isAOL         (void) { return getAgentString().ContainsI("aol");      }
inline SFBool   isMacBrowser  (void) { return getAgentString().ContainsI("macintosh");}
inline SFBool   isMSIE        (void) { if (isOpera()) return FALSE; return getAgentString().ContainsI("mozilla") && getAgentString().ContainsI("msie"); }
inline SFBool   isNetscape    (void) { if (isMSIE()||isFireFox()||isOpera()) return FALSE; return getAgentString().ContainsI("mozilla"); }

//-----------------------------------------------------------------------------------
inline SFString getUserAgent(void)
{
	if (::isNetscape()) return "netscape";
	if (::isAOL     ()) return "aol";
	if (::isFireFox ()) return "firefox";
	if (::isOpera   ()) return "opera";
	if (::isMSIE    ()) return "msie";

	SFString ret = toLower(cgiUserAgent);
	return toLower(nextTokenClear(ret, '/')); // strip junk
}

#endif
