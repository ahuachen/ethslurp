/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "database.h"
#include "parser.h"

//-------------------------------------------------------------------------
void CParser::markStart(CParserElement& e, const char* s)
{
	if (!e.openTokenStart)
	{
		e.openTokenStart = s;
        
	} else
	{
		e.closeTokenStart = s;
		e.innerLen        = SFInt32(s - e.innerStart);
	}
}

//-------------------------------------------------------------------------
void CParser::markEnd(CParserElement& e, const char* s)
{
	if (!e.openTokenLen)
	{
		e.openTokenLen = SFInt32(s - e.openTokenStart + 1);
		e.innerStart = s+1;
        
	} else
	{
		e.closeTokenLen = SFInt32(s - e.closeTokenStart + 1);
	}
}

//-------------------------------------------------------------------------
void CParser::loadString(const char* str, SFBool clean)
{
	if (contents.IsEmpty())
		contents = str;

    // Always clean these pesky MS Word things
    contents.ReplaceAll("”", "\"");
    contents.ReplaceAll("“", "\"");
    contents.ReplaceAll("–", "-");
    contents.ReplaceAll("’", "'");

	// It told to, eliminate white space
	if (clean)
	{
		char *p = (char *)contents;
		while (*p)
		{
			if (*p == '\t' || *p == '\r' || *p == '\n')
				*p = ' ';
			p++;
		}
	}
	if (!contents.endsWith('\n'))
		contents += "\n";
}

//-------------------------------------------------------------------------
void CParser::loadFile(const char* fileName, SFBool clean)
{
	loadString(asciiFileToString(fileName), clean);
}

//-------------------------------------------------------------------------
static char *strikeCharacter(char *s, char ch)
{
    char *start = s;
    char *p = s;
    while (*s)
    {
        if (*s != ch)
        {
            *p = *s;
            p++;
        }
        s++;
    }
    *p = '\0';
    return start;
}

//-------------------------------------------------------------------------
void CParser::cleanChars(const char* cleanStr)
{
    char *p = (char *)contents;
    char *s = (char*)cleanStr;
    while (*s)
    {
        p = strikeCharacter(p, *s);
        s++;
    }
}

//-------------------------------------------------------------------------
const char *CXmlParser::parseBase(const char *s, PARSERFUNC openFunc, PARSERFUNC closeFunc, void *data)
{
	CParserElement e(s++);
	while (*s)
	{
		switch (*s)
		{
            case '<':
                if (*(s+1) && *(s+1) != '/')
                {
                    // We are already interior and we've encountered a new opening token, recursively parse it
                    s = parseBase(s, openFunc, closeFunc, data);
                    
                } else
                {
                    // We are exterior so we mark this parse
                    markStart(e, s);
                }
                break;
                
            case '>':
                markEnd(e, s);
                if (e.isFull())
                {
                    if (closeFunc)
                        (closeFunc)(e, data);
                    return s;
                } else
                {
                    if (openFunc)
                        (openFunc)(e, data);
                }
                break;
		}
		s++;
	}
    
	return s;
}

//-------------------------------------------------------------------------
void CTextParser::loadFile(const char* fileName, SFBool clean)
{
	CParser::loadFile(fileName, clean);
    
	// Strip any XML tokens
	if (clean)
        contents = cleanXML((char*)contents);
}

//-------------------------------------------------------------------------
void CTextParser::markStart(CParserElement& e, const char* s)
{
	e.openTokenStart = s;
}

//-------------------------------------------------------------------------
void CTextParser::markEnd(CParserElement& e, const char* s)
{
	e.openTokenLen = SFInt32(s - e.openTokenStart + 1);
}

//-------------------------------------------------------------------------
const char *CQuoteParser::parseBase(const char *s, PARSERFUNC openFunc, PARSERFUNC closeFunc, void *data)
{
	CParserElement e(s);
	SFBool isInside = FALSE;
    
	while (*s)
	{
		switch (*s)
		{
            case '"':
                if (!isInside)
                {
                    isInside=TRUE;
                    markStart(e, s);
					if (openFunc)
						(openFunc)(e, data);
                } else
                {
                    isInside=FALSE;
                    markEnd(e, s);
					if (closeFunc)
						(closeFunc)(e, data);
                }
                break;
		}
		s++;
	}
    
	return s;
}

//-------------------------------------------------------------------------
const char *CWordParser::parseBase(const char *s, PARSERFUNC openFunc, PARSERFUNC closeFunc, void *data)
{
	CParserElement e(s);
    while (*s)
	{
		switch (*s)
		{
            case ' ':
                if (e.openTokenStart)
                {
                    markEnd(e, s);
					if (closeFunc)
						(closeFunc)(e, data);
                }
                markStart(e, s);
				if (openFunc)
					(openFunc)(e, data);
                break;
		}
		s++;
	}
    
	return s;
}

//-------------------------------------------------------------------------
void CWordParser::loadFile(const char* fileName, SFBool clean)
{
	CTextParser::loadFile(fileName, clean);
	contents = SFString(' ') + contents;
}

//-------------------------------------------------------------------------
const char *CSentenceParser::parseBase(const char *s, PARSERFUNC openFunc, PARSERFUNC closeFunc, void *data)
{
	CParserElement e(s);
	while (*s)
	{
		switch (*s)
		{
            case '!':
            case '?':
            case '.':
			{
				const char *x = s+1;
				if (*(s+1) == '"')
					x = s+2;
				if (e.openTokenStart)
				{
					markEnd(e, x);
					if (closeFunc)
						(closeFunc)(e, data);
				}
				markStart(e, x);
				if (openFunc)
					(openFunc)(e, data);
			}
                break;
		}
		s++;
	}
    
	return s;
}

//-------------------------------------------------------------------------
void CSentenceParser::loadFile(const char* fileName, SFBool clean)
{
	CTextParser::loadFile(fileName, clean);
	contents = SFString('.') + contents;
}

//-------------------------------------------------------------------------
const char *CLineParser::parseBase(const char *s, PARSERFUNC openFunc, PARSERFUNC closeFunc, void *data)
{
	CParserElement e(s);
	while (*s)
	{
		switch (*s)
		{
            case '\n':
			{
				if (e.openTokenStart)
				{
					markEnd(e, s);
					if (closeFunc)
						(closeFunc)(e, data);
				}

				markStart(e, s+1);
				if (openFunc)
					(openFunc)(e, data);
			}
			break;
		}
		s++;
	}

	return s;
}

//-------------------------------------------------------------------------
void CLineParser::loadFile(const char* fileName, SFBool clean)
{
	contents = asciiFileToString(fileName);
	contents.ReplaceAll('\n', (char)0x5);
	CTextParser::loadFile(fileName, clean);
	contents.ReplaceAll((char)0x5, '\n');
}
