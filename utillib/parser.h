#ifndef _PARSER_H_
#define _PARSER_H_
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

//-------------------------------------------------------------------------
class CParserElement
{
public:
	SFBool      isOpen;	
	char        tokenName[40];
	
	const char *openTokenStart;
	const char *closeTokenStart;
	const char *innerStart;
	
	SFInt32     openTokenLen;
	SFInt32     closeTokenLen;
	SFInt32     innerLen;
	
	CParserElement(const char *s)
		{
			isOpen = -1;
			tokenName[0] = '\0';
			openTokenStart  = s;
			closeTokenStart = innerStart = (const char*)NULL;
			openTokenLen    = closeTokenLen   = innerLen   = 0;
		}
	SFBool isFull(void) const { return (openTokenStart && closeTokenStart); }
};

//-------------------------------------------------------------------------
typedef SFBool (*PARSERFUNC) (const CParserElement& element, void *data);

//-------------------------------------------------------------------------
class CParser
{
public:
	SFString contents;
	SFString lastTenChars;
	
	                    CParser    (void) { }
	virtual            ~CParser    (void) { }
	
	virtual void        loadFile   (const char* fileName, SFBool clean=TRUE);
	virtual void        loadString (const char* string,   SFBool clean=TRUE);
	virtual void        cleanChars (const char* cleanStr);

	virtual const char *parseBase  (const char* s, PARSERFUNC openFunc, PARSERFUNC closeFunc, void *data) = 0;
	virtual const char *parse      (PARSERFUNC openFunc, PARSERFUNC closeFunc, void *data);
	
	virtual void        markStart  (CParserElement& e, const char* s);
	virtual void        markEnd    (CParserElement& e, const char* s);
};

//-------------------------------------------------------------------------
inline const char *CParser::parse(PARSERFUNC openFunc, PARSERFUNC closeFunc, void *data)
{
    return parseBase((const char*)contents, openFunc, closeFunc, data);
};

//-------------------------------------------------------------------------
class CXmlParser : public CParser
{
public:
                CXmlParser     (void) {};
	const char *parseBase      (const char* s, PARSERFUNC openFunc, PARSERFUNC closeFunc, void *data);
};

//-------------------------------------------------------------------------
class CTextParser : public CParser
{
public:
                CTextParser    (void) {}
	void        loadFile       (const char* string, SFBool clean=TRUE);
	void        markStart      (CParserElement& e, const char* s);
	void        markEnd        (CParserElement& e, const char* s);
};

//-------------------------------------------------------------------------
class CSentenceParser : public CTextParser
{
public:
                CSentenceParser(void) {};
	void        loadFile       (const char* string, SFBool clean=TRUE);
	const char *parseBase      (const char* s, PARSERFUNC openFunc, PARSERFUNC closeFunc, void *data);
};

//-------------------------------------------------------------------------
class CLineParser : public CTextParser
{
public:
	            CLineParser    (void) {};
	void        loadFile       (const char* string, SFBool clean=TRUE);
	const char *parseBase      (const char* s, PARSERFUNC openFunc, PARSERFUNC closeFunc, void *data);
};

//-------------------------------------------------------------------------
class CQuoteParser : public CTextParser
{
public:
                CQuoteParser   (void) {}
	const char *parseBase      (const char* s, PARSERFUNC openFunc, PARSERFUNC closeFunc, void *data);
};

//-------------------------------------------------------------------------
class CWordParser : public CTextParser
{
public:
                CWordParser    (void) {};
	void        loadFile       (const char* string, SFBool clean=TRUE);
	const char *parseBase      (const char* s, PARSERFUNC openFunc, PARSERFUNC closeFunc, void *data);
};

//----------------------------------------------------------------------------------
#define LOCAL_COPY(TO, FROM, LEN) char *TO = new char[LEN+1]; strncpy(TO, FROM, LEN); TO[LEN] = '\0';

//----------------------------------------------------------------------------------
inline char *stripStr(char *pp)
{
    char *s = pp;
    while (*s)
    {
        if (!isWhiteSpace(*s))
            break;
        else
            s++;
    }
    
    long len = strlen(s);
    char *p = &s[len-1];
    while (*p)
    {
        if (!isWhiteSpace(*p))
            break;
        else
        {
            *p = '\0';
            p--;
        }
    }
    return s;
}

//----------------------------------------------------------------------------------
inline char *cleanXML(char *pp)
{
    SFInt32 inToken=FALSE;
    char *s = pp;
    char *p = pp;
    while (*s)
    {
        switch (*s)
        {
            case '<':
                inToken=TRUE;
                break;
            case '>':
                inToken=FALSE;
                break;
        }

        if (!inToken && *s != '>')
        {
            *p = *s;
            p++;
        }
        s++;
    }
    *p = '\0';
    
    // now strip the resulting string of white space
    return stripStr(pp);
}

//----------------------------------------------------------------------------------
inline SFString cleanXML(const SFString& strIn)
{
	SFString out = strIn;
	char *s = (char*)(const char*)out;
	cleanXML(s);
	return s;
}

#endif
