/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
 * The CGI_C library, by Thomas Boutell, version 1.0. CGI_C is intended
 * to be a high-quality API to simplify CGI programming tasks.
 * 
 * This code is not thread safe but since it is only used under CGI
 * it should not be a problem.  This code is not even linked in under ISAPI
 *-----------------------------------------------------------------------*/
#include "basetypes.h"

#include "cgilib.h"
#include "database.h"

#define cgicTempDir "/tmp"

#include <sys/stat.h>
#include <unistd.h>

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
char *cgiAccept;
char *cgiAuthType;
char *cgiGatewayInterface;
char *cgiPathInfo;
char *cgiPathTranslated;
char *cgiQueryString;
char *cgiReferrer;
char *cgiRemoteAddr;
char *cgiRemoteHost;
char *cgiRemoteIdent;
char *cgiRemoteUser;
char *cgiRequestMethod;
char *cgiScriptName;
char *cgiServerName;
char *cgiCookie;
char *cgiServerPort;
char *cgiServerProtocol;
char *cgiServerSoftware;
char *cgiUserAgent;
int   cgiContentLength;
char *cgiMultipartBoundary;
char  cgiContentTypeData[1024];
char *cgiContentType = cgiContentTypeData;

FILE *cgiIn;
FILE *cgiOut;

//--------------------------------------------------------------------------
typedef enum
{
	cgiParseSuccess,
	cgiParseMemory,
	cgiParseIO
} cgiParseResultType;

//--------------------------------------------------------------------------
typedef enum
{
	cgiEscapeRest,
	cgiEscapeFirst,
	cgiEscapeSecond
} cgiEscapeState;

//--------------------------------------------------------------------------
typedef enum
{
	cgiUnescapeSuccess,
	cgiUnescapeMemory
} cgiUnescapeResultType;

//--------------------------------------------------------------------------
// One form entry, consisting of an attribute-value pair,
// and an optional filename and content type. All of
// these are guaranteed to be valid null-terminated strings,
// which will be of length zero in the event that the
// field is not present, with the exception of tfileName
// which will be null when 'in' is null. DO NOT MODIFY THESE 
// VALUES. Make local copies if modifications are desired. */
typedef struct cgiFormEntryStruct
{
        char *attr;
	// value is populated for regular form fields only.
	// For file uploads, it points to an empty string, and file
	// upload data should be read from the file tfileName. 
	char *value;

	// When fileName is not an empty string, tfileName is not null,
	// and 'value' points to an empty string.  Valid for both files 
	// and regular fields; does not include terminating null of regular fields.
	int valueLength;

	char *fileName;	
	char *contentType;

	// Temporary file name for working storage of file uploads.
	char *tfileName;
        struct cgiFormEntryStruct *next;
} cgiFormEntry;

static cgiFormEntry *cgiFormEntryFirst = NULL;

static void cgiSetupConstants(void);

static cgiFormEntry *cgiFormEntryFindFirst(const char *name);
static cgiFormEntry *cgiFormEntryFindNext(void);

static cgiFormResultType cgiFormEntryString(cgiFormEntry *e, char *result, int max, int newlines);

static cgiParseResultType cgiParseGetFormInput(void);
static cgiParseResultType cgiParsePostFormInput(void);
static cgiParseResultType cgiParsePostMultipartInput(void);
static cgiParseResultType cgiParseFormInput(char *data, int length);

static int cgiHexValues[256];

//-------------------------------------------------------------------
static void cgiGetenv(char **s, char *var)
{
	*s = getenv(var);
	if (!(*s))
		*s = (char*)"";
}

//-------------------------------------------------------------------------
int cgiInit(const SFString& type)
{
	srand((unsigned)time(NULL));
	cgiSetupConstants();
	cgiGetenv(&cgiServerSoftware,     (char*)"SERVER_SOFTWARE"  );
	cgiGetenv(&cgiServerName,         (char*)"SERVER_NAME"      );
	cgiGetenv(&cgiCookie,             (char*)"HTTP_COOKIE"      );
	cgiGetenv(&cgiGatewayInterface,   (char*)"GATEWAY_INTERFACE");
	cgiGetenv(&cgiServerProtocol,     (char*)"SERVER_PROTOCOL"  );
	cgiGetenv(&cgiServerPort,         (char*)"SERVER_PORT"      );
	cgiGetenv(&cgiRequestMethod,      (char*)"REQUEST_METHOD"   );
	cgiGetenv(&cgiPathInfo,           (char*)"PATH_INFO"        );
	cgiGetenv(&cgiPathTranslated,     (char*)"PATH_TRANSLATED"  );
	if ( NULL == cgiPathTranslated || 0 == strlen(cgiPathTranslated) )
		cgiGetenv(&cgiPathTranslated, (char*)"DOCUMENT_ROOT"    );
	cgiGetenv(&cgiScriptName,         (char*)"SCRIPT_NAME"      );
	cgiGetenv(&cgiQueryString,        (char*)"QUERY_STRING"     );
	cgiGetenv(&cgiRemoteHost,         (char*)"REMOTE_HOST"      );
	cgiGetenv(&cgiRemoteAddr,         (char*)"REMOTE_ADDR"      );
	cgiGetenv(&cgiAuthType,           (char*)"AUTH_TYPE"        );
	cgiGetenv(&cgiRemoteUser,         (char*)"REMOTE_USER"      );
	cgiGetenv(&cgiRemoteIdent,        (char*)"REMOTE_IDENT"     );
    
	// The content type string needs to be parsed and modified, so copy it to a buffer.
	char *e = getenv("CONTENT_TYPE");
	if (e)
	{
		if (strlen(e) < sizeof(cgiContentTypeData))
		{
			strcpy(cgiContentType, e);
		} else
		{
			// Truncate safely in the event of what is almost certainly
			// a hack attempt
			strncpy(cgiContentType, e, sizeof(cgiContentTypeData));
			cgiContentType[sizeof(cgiContentTypeData) - 1] = '\0';
		}
	} else
	{
		cgiContentType[0] = '\0';
	}

	// Never null
	cgiMultipartBoundary = (char*)"";

	// 2.0: parse semicolon-separated additional parameters of the
	// content type. The one we're interested in is 'boundary'.
	// We discard the rest to make cgiContentType more useful
	// to the typical programmer.
	if (strchr(cgiContentType, ';'))
	{
		char *sat = strchr(cgiContentType, ';');
		while (sat)
		{
			*sat = '\0';
			sat++;
			while (isspace(*sat))
			{
				sat++;
			}
			if (SFString(sat).Left(9) % "boundary=")
			{
				char *s;
				cgiMultipartBoundary = sat + strlen("boundary=");
				s = cgiMultipartBoundary;
				while ((*s) && (!isspace(*s)))
				{
					s++;
				}
				*s = '\0';
				break;
			} else
			{
				sat = strchr(sat, ';');
			} 	
		}
	}

	cgiGetenv(&cgiAccept,              (char*)"HTTP_ACCEPT");
	cgiGetenv(&cgiUserAgent,           (char*)"HTTP_USER_AGENT");
	cgiGetenv(&cgiReferrer,            (char*)"HTTP_REFERER");

	char *cgiContentLengthString;
	cgiGetenv(&cgiContentLengthString, (char*)"CONTENT_LENGTH");
	cgiContentLength = atoi(cgiContentLengthString);	

	cgiFormEntryFirst = 0;
	cgiIn  = stdin;
	cgiOut = stdout;

	if (cgiRequestMethod % SFString("post"))
	{
		if (cgiContentType % SFString("application/x-www-form-urlencoded"))
		{	
			if (cgiParsePostFormInput() != cgiParseSuccess)
			{
				cgiRelease();
				return -1;
			}	
		} else if (cgiContentType % SFString("multipart/form-data"))
		{
			if (cgiParsePostMultipartInput() != cgiParseSuccess)
			{
				cgiRelease();
				return -1;
			}	
		}
	} else if (cgiRequestMethod % SFString("get"))
	{	
		// The spec says this should be taken care of by
		// the server, but... it isn't
		cgiContentLength = (int)strlen(cgiQueryString);
		if (cgiParseGetFormInput() != cgiParseSuccess)
		{
			cgiRelease();
			return -1;
		}
	}

	if (!type.IsEmpty())
	{
		printf("%s", (const char*)("Content-Type: " + type + "\n\n"));
		if (SFos::fileExists("./message.htm"))
		{
			printf("%s", (const char*)asciiFileToString("./message.htm"));
			exit(0);
		}
	}
	return 0;
}


//-------------------------------------------------------------------
static cgiParseResultType cgiParsePostFormInput(void)
{
	if (!cgiContentLength)
		return cgiParseSuccess;

	char *input = (char *) malloc(cgiContentLength);
	if (!input)
		return cgiParseMemory;	

	if (fread(input, 1, cgiContentLength, cgiIn) != (unsigned int)cgiContentLength) 
	{
		free(input);
		return cgiParseIO;
	}

	cgiParseResultType result = cgiParseFormInput(input, cgiContentLength);
	free(input);

	return result;
}

//-------------------------------------------------------------------
static cgiParseResultType cgiParseGetFormInput(void) 
{
	return cgiParseFormInput(cgiQueryString, cgiContentLength);
}

//-------------------------------------------------------------------
static cgiUnescapeResultType cgiUnescapeChars(char **sp, char *cp, int len) 
{
	static int initialized=FALSE;
	if (!initialized)
		cgiSetupConstants();
	initialized=TRUE;
	
	char *s;
	cgiEscapeState escapeState = cgiEscapeRest;
	int escapedValue = 0;
	int srcPos = 0;
	int dstPos = 0;
	s = (char *) malloc(len + 1);
	if (!s) 
		return cgiUnescapeMemory;

	while (srcPos < len) 
	{
		int ch = cp[srcPos];
		switch (escapeState) 
		{
			case cgiEscapeRest:
			if (ch == '%') 
			{
				escapeState = cgiEscapeFirst;
			} else if (ch == '+') 
			{
				s[dstPos++] = ' ';
			} else
			{
				s[dstPos++] = (char)ch;	
			}
			break;
			case cgiEscapeFirst:
			escapedValue = cgiHexValues[ch] << 4;	
			escapeState = cgiEscapeSecond;
			break;
			case cgiEscapeSecond:
			escapedValue += cgiHexValues[ch];
			s[dstPos++] = (char)escapedValue;
			escapeState = cgiEscapeRest;
			break;
		}
		srcPos++;
	}
	s[dstPos] = '\0';
	*sp = s;
	return cgiUnescapeSuccess;
}		
	
//-------------------------------------------------------------------
static cgiParseResultType cgiParseFormInput(char *data, int length) 
{
	/* Scan for pairs, unescaping and storing them as they are found. */
	int pos = 0;
	cgiFormEntry *n;
	cgiFormEntry *l = 0;
	while (pos != length) 
	{
		int foundEq = 0;
		int foundAmp = 0;
		int start = pos;
		int len = 0;
		char *attr;
		char *value;
		while (pos != length) 
		{
			if (data[pos] == '=') 
			{
				foundEq = 1;
				pos++;
				break;
			}
			pos++;
			len++;
		}
		if (!foundEq) 
			break;

		if (cgiUnescapeChars(&attr, data+start, len) != cgiUnescapeSuccess) 
			return cgiParseMemory;

		start = pos;
		len = 0;
		while (pos != length) 
		{
			if (data[pos] == '&') 
			{
				foundAmp = 1;
				pos++;
				break;
			}
			pos++;
			len++;
		}
		// The last pair probably won't be followed by a &, but
		// that's fine, so check for that after accepting it
		if (cgiUnescapeChars(&value, data+start, len) != cgiUnescapeSuccess) 
		{
			free(attr);
			return cgiParseMemory;
		}	
		// OK, we have a new pair, add it to the list.
		n = (cgiFormEntry *) malloc(sizeof(cgiFormEntry));	
		if (!n) 
		{
			free(attr);
			free(value);
			return cgiParseMemory;
		}
		n->attr = attr;
		n->value = value;
		n->valueLength = (int)strlen(n->value);
		n->fileName = (char *) malloc(1);
		if (!n->fileName) 
		{
			free(attr);
			free(value);
			free(n);
			return cgiParseMemory;
		}	
		n->fileName[0] = '\0';
		n->contentType = (char *) malloc(1);
		if (!n->contentType) 
		{
			free(attr);
			free(value);
			free(n->fileName);
			free(n);
			return cgiParseMemory;
		}	
		n->contentType[0] = '\0';
		n->tfileName = (char *) malloc(1);
		if (!n->tfileName) 
		{
			free(attr);
			free(value);
			free(n->fileName);
			free(n->contentType);
			free(n);
			return cgiParseMemory;
		}	
		n->tfileName[0] = '\0';
		n->next = 0;
		if (!l) 
			cgiFormEntryFirst = n;
		else
			l->next = n;
		l = n;
		if (!foundAmp) 
			break;
	}
	return cgiParseSuccess;
}

//--------------------------------------------------------------------------
static void cgiSetupConstants(void) 
{
	int i=0;
	for (i=0; (i < 256); i++) 
		cgiHexValues[i] = 0;
	cgiHexValues[(int)'0'] = 0;	
	cgiHexValues[(int)'1'] = 1;	
	cgiHexValues[(int)'2'] = 2;	
	cgiHexValues[(int)'3'] = 3;	
	cgiHexValues[(int)'4'] = 4;	
	cgiHexValues[(int)'5'] = 5;	
	cgiHexValues[(int)'6'] = 6;	
	cgiHexValues[(int)'7'] = 7;	
	cgiHexValues[(int)'8'] = 8;	
	cgiHexValues[(int)'9'] = 9;
	cgiHexValues[(int)'A'] = 10;
	cgiHexValues[(int)'B'] = 11;
	cgiHexValues[(int)'C'] = 12;
	cgiHexValues[(int)'D'] = 13;
	cgiHexValues[(int)'E'] = 14;
	cgiHexValues[(int)'F'] = 15;
	cgiHexValues[(int)'a'] = 10;
	cgiHexValues[(int)'b'] = 11;
	cgiHexValues[(int)'c'] = 12;
	cgiHexValues[(int)'d'] = 13;
	cgiHexValues[(int)'e'] = 14;
	cgiHexValues[(int)'f'] = 15;
}

//--------------------------------------------------------------------------
void cgiRelease(void) 
{
	cgiFormEntry *c = cgiFormEntryFirst;
	while (c) 
	{
		cgiFormEntry *n = c->next;
		free(c->attr);
		free(c->value);
		free(c->fileName);
		free(c->contentType);
		if (strlen(c->tfileName)) 
			unlink(c->tfileName);
		free(c->tfileName);
		free(c);
		c = n;
	}

	// 2.0: to clean up the environment for cgiReadEnvironment,
	// we must set these correctly
	cgiFormEntryFirst = NULL;
}

//-------------------------------------------------------------------
cgiFormResultType cgiFormString(const char *name, char *result, int max) 
{
	ASSERT(result);

	cgiFormEntry *e = cgiFormEntryFindFirst(name);
	if (!e) 
	{
		strcpy(result, "");
		return cgiFormNotFound;
	}

	return cgiFormEntryString(e, result, max, TRUE);
}

//-------------------------------------------------------------------
cgiFormResultType cgiFormStringNoNewlines(const char *name, char *result, int max)
{
	ASSERT(result);

	cgiFormEntry *e = cgiFormEntryFindFirst(name);
	if (!e)
	{
		strcpy(result, "");
		return cgiFormNotFound;
	}

	return cgiFormEntryString(e, result, max, FALSE);
}

//-------------------------------------------------------------------
cgiFormResultType cgiFormStringMultiple(const char *name, char ***result) 
{
	char **stringArray;
	int i;
	int total = 0;
	// Make two passes. One would be more efficient, but this
	// function is not commonly used. The select menu and
	// radio box functions are faster.
	cgiFormEntry *e = cgiFormEntryFindFirst(name);
	if (e != 0) 
	{
		do
		{
			total++;
		} while ((e = cgiFormEntryFindNext()) != 0); 
	}
	stringArray = (char **) malloc(sizeof(char *) * (total + 1));
	if (!stringArray) 
	{
		*result = 0;
		return cgiFormMemory;
	}
	// initialize all entries to null; the last will stay that way
	for (i=0; (i <= total); i++) 
		stringArray[i] = 0;

	// now go get the entries
	e = cgiFormEntryFindFirst(name);
	if (e) 
	{
		i = 0;
		do
		{
			int max = (int) (strlen(e->value) + 1);
			stringArray[i] = (char *) malloc(max);
			if (stringArray[i] == 0) 
			{
				/* Memory problems */
				cgiStringArrayFree(stringArray);
				*result = 0;
				return cgiFormMemory;
			}	
			strcpy(stringArray[i], e->value);
			cgiFormEntryString(e, stringArray[i], max, 1);
			i++;
		} while ((e = cgiFormEntryFindNext()) != 0); 
		*result = stringArray;
		return cgiFormSuccess;
	} else
	{
		*result = stringArray;
		return cgiFormNotFound;
	}	
}

//-------------------------------------------------------------------
static cgiFormResultType cgiFormEntryString(cgiFormEntry *e, char *result, int max, int newlines) 
{
	int truncated = 0;
	int len = 0;
	int avail = max-1;
	int crCount = 0;
	int lfCount = 0;	
	char *dp = result;
	char *sp = e->value;	
	while (TRUE) 
	{
		// 1.07: don't check for available space now.
		// We check for it immediately before adding
		// an actual character. 1.06 handled the
		// trailing null of the source string improperly,
		// resulting in a cgiFormTruncated error.

		int ch = *sp;
		if (len >= avail)
		{
			truncated = 1;
			break;
		}	 

		// Fix the CR/LF, LF, CR nightmare: watch for
		// consecutive bursts of CRs and LFs in whatever
		// pattern, then actually output the larger number 
		// of LFs. Consistently sane, yet it still allows
		// consecutive blank lines when the user
		// actually intends them.
		if ((ch == 13) || (ch == 10)) 
		{
			if (ch == 13) 
			{
				crCount++;
			} else
			{
				lfCount++;
			}	
		} else
		{
			if (crCount || lfCount) 
			{
				int lfsAdd = crCount;
				if (lfCount > crCount) 
				{
					lfsAdd = lfCount;
				}
				/* Stomp all newlines if desired */
				if (!newlines) 
				{
					lfsAdd = 0;
				}
				while (lfsAdd) 
				{
					if (len >= avail) 
					{
						truncated = 1;
						break;
					}
					*dp = 10;
					dp++;
					lfsAdd--;
					len++;		
				}
				crCount = 0;
				lfCount = 0;
			}
			if (ch == '\0') 
			{
				/* The end of the source string */
				break;				
			}	
			/* 1.06: check available space before adding
				the character, because a previously added
				LF may have brought us to the limit */
			if (len >= avail) 
			{
				truncated = 1;
				break;
			}
			*dp = (char)ch;
			dp++;
			len++;
		}
		sp++;	
	}	
	*dp = '\0';
	if (truncated) 
		return cgiFormTruncated;
	else if (!len) 
		return cgiFormEmpty;

	return cgiFormSuccess;
}

//-------------------------------------------------------------------
cgiFormResultType cgiFormInteger(const char *name, int *result, int defaultV) 
{
	ASSERT(result);
	*result = defaultV;

	cgiFormEntry *e = cgiFormEntryFindFirst(name);
	if (!e) 
		return cgiFormNotFound; 

	if (!strlen(e->value)) 
		return cgiFormEmpty;

	int ch = e->value[strspn(e->value, " \n\r\t")];
	if (!(isdigit(ch)) && (ch != '-') && (ch != '+')) 
		return cgiFormBadType;

	*result = atoi(e->value);
	return cgiFormSuccess;
}

//-------------------------------------------------------------------
cgiFormResultType cgiFormIntegerBounded(const char *name, int *result, int defaultV, int min, int max) 
{
	ASSERT(result);
	cgiFormResultType ret = cgiFormInteger(name, result, defaultV);
	*result = MAX(min, MIN(max, *result));
	return ret;
}

//-------------------------------------------------------------------
cgiFormResultType cgiFormDouble(const char *name, double *result, double defaultV) 
{
	ASSERT(result);
	*result = defaultV;

	cgiFormEntry *e = cgiFormEntryFindFirst(name);
	if (!e) 
		return cgiFormNotFound; 

	if (!strlen(e->value)) 
		return cgiFormEmpty;

	int ch = e->value[strspn(e->value, " \n\r\t")];
	if (!(isdigit(ch)) && (ch != '.') && (ch != '-') && (ch != '+')) 
		return cgiFormBadType;

	*result = atof(e->value);
	return cgiFormSuccess;
}

//-------------------------------------------------------------------
cgiFormResultType cgiFormDoubleBounded(const char *name, double *result, double defaultV, double min, double max) 
{
	ASSERT(result);
	cgiFormResultType ret = cgiFormDouble(name, result, defaultV);
	*result = MAX(min, MIN(max, *result));
	return ret;
}

//-------------------------------------------------------------------
cgiFormResultType cgiFormRadio(const char *name, int *result, int defaultV)
{
	/* Implementation is identical to cgiFormSelectSingle. */
	//	return cgiFormSelectSingle(name, result, defaultV);
	ASSERT(result);
	*result = defaultV;
	
	cgiFormEntry *e = cgiFormEntryFindFirst(name);
	if (e)
		*result = atoi(e->value); // assumes radio controls store '0', '1', etc...
	return ((e)?cgiFormSuccess:cgiFormEmpty);
}

//-------------------------------------------------------------------
cgiFormResultType cgiFormSelectSingle(const char *name, char **choicesText, int choicesTotal, int *result, int defaultV) 
{
	ASSERT(result);
	*result = defaultV;

	cgiFormEntry *e = cgiFormEntryFindFirst(name);
	if (!e) 
		return cgiFormNotFound;

	int i=0;
	for (i=0;i<choicesTotal;i++) 
	{
		SFString choice = choicesText[i];
		if (choice == SFString(e->value)) 
		{
			*result = i;
			return cgiFormSuccess;
		}
	}

	return cgiFormNoSuchChoice;
}

//-------------------------------------------------------------------
cgiFormResultType cgiFormSelectMultiple(const char *name, char **choicesText, int choicesTotal, int *result, int *invalid) 
{
	int hits = 0;
	int invalidE = 0;
	int i=0;
	for (i=0; (i < choicesTotal); i++) 
		result[i] = 0;

	cgiFormEntry *e = cgiFormEntryFindFirst(name);
	if (!e) 
	{
		*invalid = invalidE;
		return cgiFormNotFound;
	}
	do
	{
		int hit = 0;
		for (i=0; (i < choicesTotal); i++) 
		{
			SFString choice = choicesText[i];
			if (choice == SFString(e->value)) 
			{
				result[i] = 1;
				hits++;
				hit = 1;
				break;
			}
		}
		if (!(hit)) 
		{
			invalidE++;
		}
	} while ((e = cgiFormEntryFindNext()) != 0);

	*invalid = invalidE;

	if (hits) 
	{
		return cgiFormSuccess;
	} else
	{
		return cgiFormNotFound;
	}
}

//-------------------------------------------------------------------
cgiFormResultType cgiFormCheckboxSingle(const char *name)
{
	cgiFormEntry *e = cgiFormEntryFindFirst(name);
	if (!e) 
	{
		return cgiFormNotFound;
	}
	return cgiFormSuccess;
}

//-------------------------------------------------------------------
cgiFormResultType cgiFormCheckboxMultiple(const char *name, char **valuesText, int valuesTotal, int *result, int *invalid)
{
	/* Implementation is identical to cgiFormSelectMultiple. */
	return cgiFormSelectMultiple(name, valuesText, valuesTotal, result, invalid);
}

//-------------------------------------------------------------------
void cgiHeaderLocation(char *redirectUrl) 
{
	fprintf(cgiOut, "Location: %s\r\n\r\n", redirectUrl);
}

//-------------------------------------------------------------------
void cgiHeaderStatus(int status, char *statusMessage) 
{
	fprintf(cgiOut, "Status: %d %s\r\n\r\n", status, statusMessage);
}

//-------------------------------------------------------------------
void cgiHeaderContentType(const char *mimeType) 
{
	fprintf(cgiOut, "Content-type: %s\r\n\r\n", mimeType);
}

//-------------------------------------------------------------------
static char *cgiFindTarget = 0;
static cgiFormEntry *cgiFindPos = 0;

//-------------------------------------------------------------------
static cgiFormEntry *cgiFormEntryFindFirst(const char *name) 
{
	cgiFindTarget = (char *)name;
	cgiFindPos = cgiFormEntryFirst;
	return cgiFormEntryFindNext();
}

//-------------------------------------------------------------------
static cgiFormEntry *cgiFormEntryFindNext(void) 
{
	while (cgiFindPos) 
	{
		cgiFormEntry *c = cgiFindPos;
		cgiFindPos = c->next;
		if (!strcmp(c -> attr, cgiFindTarget)) 
			return c;
	}
	return NULL;
}

//---------------------------------------------------------------------------------------
SFBool isFieldPresent(const SFString& name)
{
	return (cgiFormEntryFindFirst((const char *)name) != NULL);
}

//-------------------------------------------------------------------
void cgiStringArrayFree(char **stringArray) 
{
	char *p;
	char **arrayItself = stringArray;
	p = *stringArray;
	while (p) 
	{
		free(p);
		stringArray++;
		p = *stringArray;
	}
	/* 2.0: free the array itself! */
	free(arrayItself);
}	

//-------------------------------------------------------------------
cgiFormResultType cgiCookieString(const char *name, char *value, int space)
{
	char *p = cgiCookie;
	while (*p) 
	{
		char *n = (char *)name;
		/* 2.02: if cgiCookie is exactly equal to name, this
			can cause an overrun. The server probably wouldn't
			allow it, since a name without values makes no sense 
			-- but then again it might not check, so this is a
			genuine security concern. Thanks to Nicolas 
			Tomadakis. */
		while (*p == *n) 
		{
			if ((p == (char*)'\0') && (n == (char*)'\0'))
			{
				/* Malformed cookie header from client */
				return cgiFormNotFound;
			}
			p++;
			n++;
		}
		if ((!*n) && (*p == '=')) 
		{
			p++;
			while ((*p != ';') && (*p != '\0') &&
				(space > 1)) 
			{
				*value = *p;
				value++;
				p++;
				space--;
			}
			if (space > 0) 
			{
				*value = '\0';
			}
			/* Correct parens: 2.02. Thanks to
				Mathieu Villeneuve-Belair. */
			if (!(((*p) == ';') || ((*p) == '\0')))
			{
				return cgiFormTruncated;
			} else
			{	
				return cgiFormSuccess;
			}
		} else
		{
			/* Skip to next cookie */	
			while (*p) 
			{
				if (*p == ';') 
				{
					break;
				}
				p++;
			}
			if (!*p) 
			{
				/* 2.01: default to empty */
				if (space) 
				{
					*value = '\0';
				}
				return cgiFormNotFound;
			}
			p++;	
			/* Allow whitespace after semicolon */
			while ((*p) && isspace(*p)) 
			{
				p++;
			} 
		}
	}
	/* 2.01: actually the above loop never terminates except
		with a return, but do this to placate gcc */
	if (space) 
	{
		*value = '\0';
	}
	return cgiFormNotFound;
}

//-------------------------------------------------------------------
cgiFormResultType cgiCookieInteger(const char *name, int *result, int defaultV)
{
	char buffer[256];
	cgiFormResultType r = 
		cgiCookieString(name, buffer, sizeof(buffer));
	if (r != cgiFormSuccess) 
	{
		*result = defaultV;
	} else
	{
		*result = atoi(buffer);
	}
	return r;
}

//-------------------------------------------------------------------
void cgiHeaderCookieSetInteger(const char *name, int value, int secondsToLive, char *path, char *domain)
{
	char svalue[256];
	sprintf(svalue, "%d", value);
	cgiHeaderCookieSetString(name, svalue, secondsToLive, path, domain);
}

//-------------------------------------------------------------------
 const char *days[] = {
	"Sun",
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat"
};

//-------------------------------------------------------------------
 const char *months[] = {
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

//-------------------------------------------------------------------
void cgiHeaderCookieSetString(const char *name, char *value, int secondsToLive, char *path, char *domain)
{
	/* cgic 2.02: simpler and more widely compatible implementation.
		Thanks to Chunfu Lai. 
	   cgic 2.03: yes, but it didn't work. Reimplemented by
		Thomas Boutell. ; after last element was a bug. 
	   Examples of real world cookies that really work:
   	   Set-Cookie: MSNADS=UM=; domain=.slate.com; 
             expires=Tue, 26-Apr-2022 19:00:00 GMT; path=/
	   Set-Cookie: MC1=V=3&ID=b5bc08af2b8a43ff85fcb5efd8b238f0; 
             domain=.slate.com; expires=Mon, 04-Oct-2021 19:00:00 GMT; path=/
	*/
	time_t now;
	time_t then;
	struct tm *gt;
	time(&now);
	then = now + secondsToLive;
	gt = gmtime(&then);
	fprintf(cgiOut, 
		"Set-Cookie: %s=%s; domain=%s; expires=%s, %02d-%s-%04d %02d:%02d:%02d GMT; path=%s\r\n",
		name, value, domain, 
		days[gt->tm_wday],
		gt->tm_mday,
		months[gt->tm_mon],
		gt->tm_year + 1900, 	
		gt->tm_hour,
		gt->tm_min,
		gt->tm_sec,
		path);
}

//-------------------------------------------------------------------
cgiFormResultType cgiCookies(char ***result) 
{
	char **stringArray;
	int i;
	int total = 0;
	char *p;
	char *n;
	p = cgiCookie;
	while (*p) 
	{
		if (*p == '=') 
		{
			total++;
		}
		p++;
	}
	stringArray = (char **) malloc(sizeof(char *) * (total + 1));
	if (!stringArray) 
	{
		*result = 0;
		return cgiFormMemory;
	}
	/* initialize all entries to null; the last will stay that way */
	for (i=0; (i <= total); i++) 
	{
		stringArray[i] = 0;
	}
	i = 0;
	p = cgiCookie;
	while (*p) 
	{
		while (*p && isspace(*p)) 
		{
			p++;
		}
		n = p;
		while (*p && (*p != '=')) 
		{
			p++;
		}
		if (p != n) 
		{
			stringArray[i] = (char *) malloc((p - n) + 1);
			if (!stringArray[i]) 
			{
				cgiStringArrayFree(stringArray);
				*result = 0;
				return cgiFormMemory;
			}	
			memcpy(stringArray[i], n, p - n);
			stringArray[i][p - n] = '\0';
			i++;
		}
		while (*p && (*p != ';')) 
		{
			p++;	
		}
		if (!*p) 
		{
			break;
		}
		if (*p == ';') 
		{
			p++;
		}
	}
	*result = stringArray;
	return cgiFormSuccess;
}

//-------------------------------------------------------------------
cgiFormResultType cgiFormEntries(char ***result) 
{
	char **stringArray;
	cgiFormEntry *e, *pe;
	int i;
	int total = 0;
	e = cgiFormEntryFirst;
	while (e) 
	{
		/* Don't count a field name more than once if
			multiple values happen to be present for it */
		pe = cgiFormEntryFirst;
		while (pe != e) 
		{
			if (!strcmp(e->attr, pe->attr)) 
			{
				goto skipSecondValue;
			}
			pe = pe->next;					
		}
		total++;
skipSecondValue:
		e = e->next;
	}
	stringArray = (char **) malloc(sizeof(char *) * (total + 1));
	if (!stringArray) 
	{
		*result = 0;
		return cgiFormMemory;
	}
	/* initialize all entries to null; the last will stay that way */
	for (i=0; (i <= total); i++) 
	{
		stringArray[i] = 0;
	}
	/* Now go get the entries */
	e = cgiFormEntryFirst;
	i = 0;
	while (e) 
	{
		int space;
		/* Don't return a field name more than once if
			multiple values happen to be present for it */
		pe = cgiFormEntryFirst;
		while (pe != e) 
		{
			if (!strcmp(e->attr, pe->attr)) 
			{
				goto skipSecondValue2;
			}
			pe = pe->next;					
		}		
		space = (int) strlen(e->attr) + 1;
		stringArray[i] = (char *) malloc(space);
		if (stringArray[i] == 0) 
		{
			/* Memory problems */
			cgiStringArrayFree(stringArray);
			*result = 0;
			return cgiFormMemory;
		}	
		strcpy(stringArray[i], e->attr);
		i++;
skipSecondValue2:
		e = e->next;
	}
	*result = stringArray;
	return cgiFormSuccess;
}

//-------------------------------------------------------------------
#define TRYPUTC(ch) \
	{ \
		if (putc((ch), cgiOut) == EOF) { \
			return cgiFormIO; \
		} \
	} 

//-------------------------------------------------------------------
cgiFormResultType cgiHtmlEscapeData(char *data, int len)
{
	while (len--) 
	{
		if (*data == '<') 
		{
			TRYPUTC('&');
			TRYPUTC('l');
			TRYPUTC('t');
			TRYPUTC(';');
		} else if (*data == '&') 
		{
			TRYPUTC('&');
			TRYPUTC('a');
			TRYPUTC('m');
			TRYPUTC('p');
			TRYPUTC(';');
		} else if (*data == '>') 
		{
			TRYPUTC('&');
			TRYPUTC('g');
			TRYPUTC('t');
			TRYPUTC(';');
		} else
		{
			TRYPUTC(*data);
		}
		data++;
	}
	return cgiFormSuccess;
}

//-------------------------------------------------------------------
cgiFormResultType cgiHtmlEscape(char *s)
{
	return cgiHtmlEscapeData(s, (int) strlen(s));
}

//-------------------------------------------------------------------
// Output data with the " character HTML-escaped, and no
// other characters escaped. This is useful when outputting
// the contents of a tag attribute such as 'href' or 'src'.
// 'data' is not null-terminated; 'len' is the number of
// bytes in 'data'. Returns cgiFormIO in the event
// of error, cgiFormSuccess otherwise. */
//-------------------------------------------------------------------
cgiFormResultType cgiValueEscapeData(char *data, int len)
{
	while (len--) 
	{
		if (*data == '\"') 
		{
			TRYPUTC('&');
			TRYPUTC('#');
			TRYPUTC('3');
			TRYPUTC('4');
			TRYPUTC(';');
		} else
		{
			TRYPUTC(*data);
		}
		data++;
	}
	return cgiFormSuccess;
}

//-------------------------------------------------------------------
cgiFormResultType cgiValueEscape(char *s)
{
	return cgiValueEscapeData(s, (int) strlen(s));
}

//-------------------------------------------------------------------
// 2.0: A virtual datastream supporting putback of 
// enough characters to handle multipart boundaries easily.
// A simple memset(&mp, 0, sizeof(mp)) is suitable initialization.
//-------------------------------------------------------------------
typedef struct
{
	// Buffer for putting characters back
	char putback[1024];
	
	// Position in putback from which next character will be read.
	// If readPos == writePos, then next character should
	// come from cgiIn.
	int readPos;

	// Position in putback to which next character will be put back.
	// If writePos catches up to readPos, as opposed to the other
	// way around, the stream no longer functions properly.
	// Calling code must guarantee that no more than 
	// sizeof(putback) bytes are put back at any given time.

	int writePos;
	// Offset in the virtual datastream; can be compared
	// to cgiContentLength
	int offset;

} mpStream, *mpStreamPtr;

//-------------------------------------------------------------------
static int mpRead(mpStreamPtr mpp, char *buffer, int len)
{
	int ilen = len;
	int got = 0;
	while (len)
	{
		if (mpp->readPos != mpp->writePos)
		{
			*buffer++ = mpp->putback[mpp->readPos++];
			mpp->readPos %= sizeof(mpp->putback);
			got++;
			len--;
		} else
		{
			break;
		}	
	}

	// Refuse to read past the declared length in order to
	// avoid deadlock
	if (len > (cgiContentLength - mpp->offset))
	{
		len = cgiContentLength - mpp->offset;
	}
	if (len)
	{
		int fgot = (int)fread(buffer, 1, len, cgiIn);
		if (fgot >= 0)
		{
			mpp->offset += (got + fgot);
			return got + fgot;
		} else if (got > 0)
		{
			mpp->offset += got;
			return got;
		} else
		{
			/* EOF or error */
			return fgot;
		}
	} else if (got)
	{
		return got;
	} else if (ilen)
	{	
		return EOF;
	} else
	{
		// 2.01
		return 0;
	}
}

//-------------------------------------------------------------------
static void mpPutBack(mpStreamPtr mpp, char *data, int len)
{
	mpp->offset -= len;
	while (len)
	{
		mpp->putback[mpp->writePos++] = *data++;
		mpp->writePos %= sizeof(mpp->putback);
		len--;
	}
}

//-------------------------------------------------------------------
// This function copies the body to outf if it is not null, otherwise to
// a newly allocated character buffer at *outP, which will be null
// terminated; if both outf and outP are null the body is not stored.
// If bodyLengthP is not null, the size of the body in bytes is stored
// to *bodyLengthP, not including any terminating null added to *outP. 
// If 'first' is nonzero, a preceding newline is not expected before
// the boundary. If 'first' is zero, a preceding newline is expected.
// Upon return mpp is positioned after the boundary and its trailing 
// newline, if any; if the boundary is followed by -- the next two 
// characters read after this function returns will be --. Upon error, 
// if outP is not null, *outP is a null pointer; *bodyLengthP 
// is set to zero. Returns cgiParseSuccess, cgiParseMemory 
// or cgiParseIO.
//-------------------------------------------------------------------
static cgiParseResultType afterNextBoundary(mpStreamPtr mpp, FILE *outf, char **outP, int *bodyLengthP, int first);

//-------------------------------------------------------------------
static int readHeaderLine(
	mpStreamPtr mpp,	
	char *attr,
	int attrSpace,
	char *value,
	int valueSpace);

//-------------------------------------------------------------------
static void decomposeValue(char *value,
	char *mvalue, int mvalueSpace,
	char **argNames,
	char **argValues,
	int argValueSpace);

//-------------------------------------------------------------------
// tfileName must be 1024 bytes to ensure adequacy on
// win32 (1024 exceeds the maximum path length and
// certainly exceeds observed behavior of _tmpnam).
// May as well also be 1024 bytes on Unix, although actual
// length is strlen(cgiTempDir) + a short unique pattern.
//-------------------------------------------------------------------
static cgiParseResultType getTempFileName(char *tfileName);

//-------------------------------------------------------------------
static cgiParseResultType cgiParsePostMultipartInput(void)
{
	cgiParseResultType result;
	cgiFormEntry *n = 0, *l = 0;
	int got;
	FILE *outf = 0;
	char *out = 0;
	char tfileName[1024];
	mpStream mp;
	mpStreamPtr mpp = &mp;
	memset(&mp, 0, sizeof(mp));
	if (!cgiContentLength)
	{
		return cgiParseSuccess;
	}
	/* Read first boundary, including trailing newline */
	result = afterNextBoundary(mpp, 0, 0, 0, 1);
	if (result == cgiParseIO)
	{	
		/* An empty submission is not necessarily an error */
		return cgiParseSuccess;
	} else if (result != cgiParseSuccess)
	{
		return result;
	}
	while (TRUE)
	{
		char d[1024];
		char fvalue[1024];
		char fname[1024];
		int bodyLength = 0;
		char ffileName[1024];
		char fcontentType[1024];
		char attr[1024];
		char value[1024];
		fvalue[0] = 0;
		fname[0] = 0;
		ffileName[0] = 0;
		fcontentType[0] = 0;
		out = 0;
		outf = 0;
		/* Check for EOF */
		got = mpRead(mpp, d, 2);
		if (got < 2)
		{
			/* Crude EOF */
			break;
		}
		if ((d[0] == '-') && (d[1] == '-'))
		{
			/* Graceful EOF */
			break;
		}
		mpPutBack(mpp, d, 2);
		/* Read header lines until end of header */
		while (readHeaderLine(
				mpp, attr, sizeof(attr), value, sizeof(value))) 
		{
			char *argNames[3];
			char *argValues[2];
			/* Content-Disposition: form-data; 
				name="test"; filename="googley.gif" */
			if (attr % SFString("Content-Disposition"))
			{
				argNames[0] = (char*)"name";
				argNames[1] = (char*)"filename";
				argNames[2] = 0;
				argValues[0] = fname;
				argValues[1] = ffileName;
				decomposeValue(value, 
					fvalue, sizeof(fvalue),
					argNames,
					argValues,
					1024);	
			} else if (attr % SFString("Content-Type"))
			{
				argNames[0] = 0;
				decomposeValue(value, 
					fcontentType, sizeof(fcontentType),
					argNames,
					0,
					0);
			}
		}
		if (!(fvalue % SFString("form-data")))
		{
			/* Not form data */	
			continue;
		}
		/* Body is everything from here until the next 
			boundary. So, set it aside and move past boundary. 
			If a filename was submitted as part of the
			disposition header, store to a temporary file.
			Otherwise, store to a memory buffer (it is
			presumably a regular form field). */
		if (strlen(ffileName))
		{
			if (getTempFileName(tfileName) != cgiParseSuccess)
			{
				return cgiParseIO;
			}	
			outf = fopen(tfileName, "w+b");
		} else
		{
			outf = 0;
			tfileName[0] = '\0';
		}	
		result = afterNextBoundary(mpp, outf, &out, &bodyLength, 0);
		if (result != cgiParseSuccess)
		{
			/* Lack of a boundary here is an error. */
			if (outf)
			{
				fclose(outf);
				unlink(tfileName);
			}
			if (out)
			{
				free(out);
			}
			return result;
		}
		/* OK, we have a new pair, add it to the list. */
		n = (cgiFormEntry *) malloc(sizeof(cgiFormEntry));	
		if (!n)
		{
			goto outOfMemory;
		}
		memset(n, 0, sizeof(cgiFormEntry));
		/* 2.01: one of numerous new casts required
			to please C++ compilers */
		n->attr = (char *) malloc(strlen(fname) + 1);
		if (!n->attr)
		{
			goto outOfMemory;
		}
		strcpy(n->attr, fname);
		if (out)
		{
			n->value = out;
			out = 0;
		} else if (outf)
		{
			n->value = (char *) malloc(1);
			if (!n->value)
			{
				goto outOfMemory;
			}
			n->value[0] = '\0';
			fclose(outf);
		}
		n->valueLength = bodyLength;
		n->next = 0;
		if (!l)
		{
			cgiFormEntryFirst = n;
		} else
		{
			l->next = n;
		}
		n->fileName = (char *) malloc(strlen(ffileName) + 1);
		if (!n->fileName)
		{
			goto outOfMemory;
		}
		strcpy(n->fileName, ffileName);
		n->contentType = (char *) malloc(strlen(fcontentType) + 1);
		if (!n->contentType)
		{
			goto outOfMemory;
		}
		strcpy(n->contentType, fcontentType);
		n->tfileName = (char *) malloc(strlen(tfileName) + 1);
		if (!n->tfileName)
		{
			goto outOfMemory;
		}
		strcpy(n->tfileName, tfileName);

		l = n;			
	}	
	return cgiParseSuccess;
outOfMemory:
	if (n)
	{
		if (n->attr)
		{
			free(n->attr);
		}
		if (n->value)
		{
			free(n->value);
		}
		if (n->fileName)
		{
			free(n->fileName);
		}
		if (n->tfileName)
		{
			free(n->tfileName);
		}
		if (n->contentType)
		{
			free(n->contentType);
		}
		free(n);
	}
	if (out)
	{
		free(out);
	}
	if (outf)
	{
		fclose(outf);
		unlink(tfileName);
	}
	return cgiParseMemory;
}

//-------------------------------------------------------------------
static cgiParseResultType getTempFileName(char *tfileName)
{
	/* Unix. Use the robust 'mkstemp' function to create
		a temporary file that is truly unique, with
		permissions that are truly safe. The 
		fopen-for-write destroys any bogus information
		written by potential hackers during the brief
		window between the file's creation and the
		chmod call (glibc 2.0.6 and lower might
		otherwise have allowed this). */
	int outfd; 
	strcpy(tfileName, cgicTempDir "/cgicXXXXXX");
	outfd = mkstemp(tfileName);
	if (outfd == -1)
	{
		return cgiParseIO;
	}
	close(outfd);
	/* Fix the permissions */
	if (chmod(tfileName, 0600) != 0)
	{
		unlink(tfileName);
		return cgiParseIO;
	}
	return cgiParseSuccess;
}


//-------------------------------------------------------------------
#define APPEND(string, char) \
	{ \
		if ((string##Len + 1) < string##Space) { \
			string[string##Len++] = (char); \
		} \
	}


//-------------------------------------------------------------------
#define RAPPEND(string, ch) \
	{ \
		if ((string##Len + 1) == string##Space)  { \
			char *sold = string; \
			string##Space *= 2; \
			string = (char *) realloc(string, string##Space); \
			if (!string) { \
				string = sold; \
				goto outOfMemory; \
			} \
		} \
		string[string##Len++] = (ch); \
	}
		

//-------------------------------------------------------------------
#define BAPPEND(ch) \
	{ \
		if (outf) { \
			putc(ch, outf); \
			outLen++; \
		} else if (out) { \
			RAPPEND(out, ch); \
		} \
	}


//-------------------------------------------------------------------
cgiParseResultType afterNextBoundary(mpStreamPtr mpp, FILE *outf, char **outP, int *bodyLengthP, int first)
{
	int outLen = 0;
	int outSpace = 256;
	char *out = 0;
	cgiParseResultType result;
	int boffset;
	int got;
	char d[2];	
	/* This is large enough, because the buffer into which the
		original boundary string is fetched is shorter by more
		than four characters due to the space required for
		the attribute name */
	char workingBoundaryData[1024];
	char *workingBoundary = workingBoundaryData;
	int workingBoundaryLength;
	if ((!outf) && (outP))
	{
		out = (char *) malloc(outSpace);
		if (!out)
		{
			goto outOfMemory;
		}
	}
	boffset = 0;
	sprintf(workingBoundaryData, "\r\n--%s", cgiMultipartBoundary);
	if (first)
	{
		workingBoundary = workingBoundaryData + 2;
	}
	workingBoundaryLength = (int)strlen(workingBoundary);
	while (TRUE)
	{
		got = mpRead(mpp, d, 1);
		if (got != 1)
		{
			/* 2.01: cgiParseIO, not cgiFormIO */
			result = cgiParseIO;
			goto error;
		}
		if (d[0] == workingBoundary[boffset])
		{
			/* We matched the next byte of the boundary.
				Keep track of our progress into the
				boundary and don't emit anything. */
			boffset++;
			if (boffset == workingBoundaryLength)
			{
				break;
			} 
		} else if (boffset > 0)
		{
			/* We matched part, but not all, of the
				boundary. Now we have to be careful:
				put back all except the first
				character and try again. The 
				real boundary could begin in the
				middle of a false match. We can
				emit the first character only so far. */
			BAPPEND(workingBoundary[0]);
			mpPutBack(mpp, 
				workingBoundary + 1, boffset - 1);
			mpPutBack(mpp, d, 1);
			boffset = 0;
		} else
		{		
			/* Not presently in the middle of a boundary
				match; just emit the character. */
			BAPPEND(d[0]);
		}	
	}
	/* Read trailing newline or -- EOF marker. A literal EOF here
		would be an error in the input stream. */
	got = mpRead(mpp, d, 2);
	if (got != 2)
	{
		result = cgiParseIO;
		goto error;
	}	
	if ((d[0] == '\r') && (d[1] == '\n'))
	{
		/* OK, EOL */
	} else if (d[0] == '-')
	{
		/* Probably EOF, but we check for
			that later */
		mpPutBack(mpp, d, 2);
	}	
	if (out && outSpace)
	{
		char *oout = out;
		out[outLen] = '\0';
		out = (char *) realloc(out, outLen + 1);
		if (!out)
		{
			/* Surprising if it happens; and not fatal! We were
				just trying to give some space back. We can
				keep it if we have to. */
			out = oout;
		}
		*outP = out;
	}
	
	// Note: xCode says the memory stored in 'out' is not released. This is true because
	// the function returns the allocated memory to the caller.
	if (bodyLengthP)
	{
		*bodyLengthP = outLen;
	}

	return cgiParseSuccess;
outOfMemory:
	result = cgiParseMemory;
	if (outP)
	{
		if (out)
		{
			free(out);
		}
		*outP = (char*)'\0';
	}
error:
	if (bodyLengthP)
	{
		*bodyLengthP = 0;
	}
	if (out)
	{
		free(out);
	}
	if (outP)
	{
		*outP = 0;	
	}
	return result;
}

//-------------------------------------------------------------------
static void decomposeValue(char *value, char *mvalue, int mvalueSpace, char **argNames, char **argValues, int argValueSpace)
{
	char argName[1024];
	int argNameSpace = sizeof(argName);
	int argNameLen = 0;
	int mvalueLen = 0;
	char *argValue;
	int argNum = 0;
	while (argNames[argNum])
	{
		if (argValueSpace)
		{
			argValues[argNum][0] = '\0';
		}
		argNum++;
	}
	while (isspace(*value))
	{
		value++;
	}
	/* Quoted mvalue */
	if (*value == '\"')
	{
		value++;
		while ((*value) && (*value != '\"'))
		{
			APPEND(mvalue, *value);
			value++;
		}
		while ((*value) && (*value != ';'))
		{
			value++;
		}
	} else
	{
		/* Unquoted mvalue */
		while ((*value) && (*value != ';'))
		{
			APPEND(mvalue, *value);
			value++;
		}	
	}	
	if (mvalueSpace)
	{
		mvalue[mvalueLen] = '\0';
	}
	int argValueLen=0;
	while (*value == ';')
	{
		int argNummy;
		argValueLen = 0;

		/* Skip the ; between parameters */
		value++;
		/* Now skip leading whitespace */
		while ((*value) && (isspace(*value)))
		{ 
			value++;
		}
		/* Now read the parameter name */
		argNameLen = 0;
		while ((*value) && (isalnum(*value)))
		{
			APPEND(argName, *value);
			value++;
		}
		if (argNameSpace)
		{
			argName[argNameLen] = '\0';
		}
		while ((*value) && isspace(*value))
		{
			value++;
		}
		if (*value != '=')
		{
			/* Malformed line */
			return;	
		}
		value++;
		while ((*value) && isspace(*value))
		{
			value++;
		}
		/* Find the parameter in the argument list, if present */
		argNummy = 0;
		argValue = 0;
		while (argNames[argNummy])
		{
			if (argName % SFString(argNames[argNummy]))
			{
				argValue = argValues[argNummy];
				break;
			}
			argNummy++;
		}		
		/* Finally, read the parameter value */
		if (*value == '\"')
		{
			value++;
			while ((*value) && (*value != '\"'))
			{
				if (argValue)
				{
					APPEND(argValue, *value);
				}
				value++;
			}
			while ((*value) && (*value != ';'))
			{
				value++;
			}
		} else
		{
			/* Unquoted value */
			while ((*value) && (*value != ';'))
			{
				if (argNames[argNummy])
				{
					APPEND(argValue, *value);
				}
				value++;
			}	
		}
			
		if (argValueSpace && argValue)
		{
			argValue[argValueLen] = '\0';
		}
	}	 	
}

//-------------------------------------------------------------------
static int readHeaderLine(mpStreamPtr mpp, char *attr, int attrSpace, char *value, int valueSpace)
{	
	int attrLen = 0;
	int valueLen = 0;
	int valueFound = 0;
	while (TRUE)
	{
		char d[1];
		int got = mpRead(mpp, d, 1);
		if (got != 1)
		{	
			return 0;
		}
		if (d[0] == '\r')
		{
			got = mpRead(mpp, d, 1);
			if (got == 1)
			{	
				if (d[0] == '\n')
				{
					/* OK */
				} else
				{
					mpPutBack(mpp, d, 1);
				}
			}
			break;
		} else if (d[0] == '\n')
		{
			break;
		} else if ((d[0] == ':') && attrLen) 
		{
			valueFound = 1;
			while (mpRead(mpp, d, 1) == 1) 
			{
				if (!isspace(d[0])) 
				{
					mpPutBack(mpp, d, 1);
					break;
				} 
			}
		} else if (!valueFound) 
		{
			if (!isspace(*d)) 
			{
				if (attrLen < (attrSpace - 1)) 
				{
					attr[attrLen++] = *d;
				}
			}		
		} else if (valueFound) 
		{	
			if (valueLen < (valueSpace - 1)) 
			{
				value[valueLen++] = *d;
			}
		}
	}
	if (attrSpace) 
	{
		attr[attrLen] = '\0';
	}
	if (valueSpace) 
	{
		value[valueLen] = '\0';
	}
	if (attrLen && valueLen) 
	{
		return 1;
	} else
	{
		return 0;
	}
}

//-------------------------------------------------------------------
cgiFormResultType cgiFormFileName(const char *name, char *result, int resultSpace)
{
	cgiFormEntry *e;
	int resultLen = 0;
	char *s;
	e = cgiFormEntryFindFirst(name);
	if (!e) 
	{
		strcpy(result, "");
		return cgiFormNotFound;
	}
	s = e->fileName;
	while (*s) 
	{
		APPEND(result, *s);
		s++;
	}	
	if (resultSpace) 
	{
		result[resultLen] = '\0';
	}
	if (!strlen(e->fileName)) 
	{
		return cgiFormNoFileName;
	} else if (((int) strlen(e->fileName)) > (resultSpace - 1)) 
	{
		return cgiFormTruncated;
	} else
	{
		return cgiFormSuccess;
	}
}

//-------------------------------------------------------------------
cgiFormResultType cgiFormFileContentType(const char *name, char *result, int resultSpace)
{
	cgiFormEntry *e;
	int resultLen = 0;
	char *s;
	e = cgiFormEntryFindFirst(name);
	if (!e) 
	{
		if (resultSpace) 
		{
			result[0] = '\0';
		}	
		return cgiFormNotFound;
	}
	s = e->contentType;
	while (*s) 
	{
		APPEND(result, *s);
		s++;
	}	
	if (resultSpace) 
	{
		result[resultLen] = '\0';
	}
	if (!strlen(e->contentType)) 
	{
		return cgiFormNoContentType;
	} else if (((int) strlen(e->contentType)) > (resultSpace - 1)) 
	{
		return cgiFormTruncated;
	} else
	{
		return cgiFormSuccess;
	}
}

//-------------------------------------------------------------------
cgiFormResultType cgiFormFileSize(const char *name, int *sizeP)
{
	cgiFormEntry *e;
	e = cgiFormEntryFindFirst(name);
	if (!e) 
	{
		if (sizeP) 
		{
			*sizeP = 0;
		}
		return cgiFormNotFound;
	} else if (!strlen(e->tfileName)) 
	{
		if (sizeP) 
		{
			*sizeP = 0;
		}
		return cgiFormNotAFile;
	} else
	{
		if (sizeP) 
		{
			*sizeP = e->valueLength;
		}
		return cgiFormSuccess;
	}
}

//-------------------------------------------------------------------
typedef struct cgiFileStruct
{
	FILE *in;
} cgiFile;

//-------------------------------------------------------------------
cgiFormResultType cgiFormFileOpen(const char *name, cgiFilePtr *cfpp)
{
	cgiFormEntry *e;
	cgiFilePtr cfp;
	e = cgiFormEntryFindFirst(name);
	if (!e) 
	{
		*cfpp = 0;
		return cgiFormNotFound;
	}
	if (!strlen(e->tfileName)) 
	{
		*cfpp = 0;
		return cgiFormNotAFile;
	}
	cfp = (cgiFilePtr) malloc(sizeof(cgiFile));
	if (!cfp) 
	{
		*cfpp = 0;
		return cgiFormMemory;
	}
	cfp->in = fopen(e->tfileName, "rb");
	if (!cfp->in) 
	{
		free(cfp);
		return cgiFormIO;
	}
	*cfpp = cfp;
	return cgiFormSuccess;
}

//-------------------------------------------------------------------
cgiFormResultType cgiFormFileRead(cgiFilePtr cfp, char *buffer, int bufferSize, int *gotP)
{
	int got = 0;
	if (!cfp) 
	{
		return cgiFormOpenFailed;
	}
	got = (int)fread(buffer, 1, bufferSize, cfp->in);
	if (got <= 0) 
	{
		return cgiFormEOF;
	}
	*gotP = got;
	return cgiFormSuccess;
}

//-------------------------------------------------------------------
cgiFormResultType cgiFormFileClose(cgiFilePtr cfp)
{
	if (!cfp) 
	{
		return cgiFormOpenFailed;
	}
	fclose(cfp->in);
	free(cfp);
	return cgiFormSuccess;
}

//-------------------------------------------------------------------
cgiFormResultType cgiFormStringSpaceNeeded(const char *name, int *result) 
{
	ASSERT(result);
	cgiFormEntry *e = cgiFormEntryFindFirst(name);
	if (!e) 
	{
		*result = 1;
		return cgiFormNotFound; 
	}
	*result = ((int) strlen(e->value)) + 1;
	return cgiFormSuccess;
}

//-------------------------------------------------------------------
SFBool isCGI(void)
{
	char *junk;
	cgiGetenv(&junk, (char*)"SERVER_NAME");
	return (strlen(junk) != 0);
}

//EXAMPLE CODE FOR FILE UPLOAD
/*
void File()
{
	cgiFilePtr file;
	char name[1024];
	char contentType[1024];
	char buffer[1024];
	int size;
	int got;

	if (cgiFormFileName("file", name, sizeof(name)) != cgiFormSuccess)
	{
		printf("<p>No file was uploaded.<p>\n");
		return;
	}

	fprintf(cgiOut, "The filename submitted was: ");
	cgiHtmlEscape(name);
	fprintf(cgiOut, "<p>\n");

	cgiFormFileSize("file", &size);
	fprintf(cgiOut, "The file size was: %d bytes<p>\n", size);

	cgiFormFileContentType("file", contentType, sizeof(contentType));
	fprintf(cgiOut, "The alleged content type of the file was: ");
	cgiHtmlEscape(contentType);
	fprintf(cgiOut, "<p>\n");

	fprintf(cgiOut, "Of course, this is only the claim the browser made when uploading the file. Much like the filename, it cannot be trusted.<p>\n");
	fprintf(cgiOut, "The file's contents are shown here:<p>\n");
	if (cgiFormFileOpen("file", &file) != cgiFormSuccess)
	{
		fprintf(cgiOut, "Could not open the file.<p>\n");
		return;
	}

	fprintf(cgiOut, "<pre>\n");
	while (cgiFormFileRead(file, buffer, sizeof(buffer), &got) == cgiFormSuccess)
	{
		cgiHtmlEscapeData(buffer, got);
	}
	fprintf(cgiOut, "</pre>\n");
	cgiFormFileClose(file);
}
*/
