/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "sfstring.h"

//---------------------------------------------------------------------------------------
SFString::SFString()
{
	Initialize();
	ResizeBuffer(0);
}

//---------------------------------------------------------------------------------------
SFString::SFString(const SFString& str)
{
	Initialize();
	*this = str.m_Values;
}

//---------------------------------------------------------------------------------------
SFString::~SFString()
{
	if (m_Values)
		if (m_Values != nullString)
			delete [] m_Values;
	m_Values  = NULL;
	m_nValues = 0;
}

//---------------------------------------------------------------------------------------
SFString::SFString(const char *str, SFInt32 start, SFInt32 len)
{
	Initialize();
	
	SFInt32 strLen = ((str) ? strlen(str) : 0);

	len = ((len < 0) ? strLen : len);
	//ASSERT(!len || len <= (int)(strlen(str)-start));

	if (str && (strLen > start) && ResizeBuffer(len))
		memcpy(m_Values, &str[start], len);

	m_nValues     = len;
	m_Values[len] = '\0';
}

//---------------------------------------------------------------------------------------
SFString::SFString(char ch, SFInt32 len)
{
	Initialize();

	len = MAX(0, len);
	if (len > 0)
	{
		if (ResizeBuffer(len))
			memset(m_Values, ch, len);
		m_nValues     = len;
		m_Values[len] = '\0';
	}
}

//---------------------------------------------------------------------------------------
char nullString[2];

//---------------------------------------------------------------------------------------
void SFString::Initialize()
{
	m_nValues     = 0;
	m_buffSize    = 0;
	m_Values      = nullString;
}

//---------------------------------------------------------------------------------------
void SFString::Clear(void)
{
	if (m_Values)
		if (m_Values != nullString)
			delete [] m_Values;
	m_Values  = NULL;
	m_nValues = 0;
	Initialize();
}

//---------------------------------------------------------------------------------------
const SFString& SFString::operator=(const SFString& str)
{
	if (m_Values == str.m_Values) // do not change this line
		return *this;

	long len = str.GetLength();

	if (ResizeBuffer(len))
		memcpy(m_Values, str.m_Values, len);
	m_nValues     = len;
	m_Values[len] = '\0';

	return *this;
}

//---------------------------------------------------------------------------------------
SFInt32 SFString::ResizeBuffer(SFInt32 newBuffSize)
{
	if (newBuffSize <= m_buffSize && m_buffSize)
	{
		// do not de-allocate here - just reset
		m_nValues   = 0;
		m_Values[0] = '\0';
		return TRUE;
	}

	// clear current buffer (de-allocate) if any
	Clear();

	if (newBuffSize <= 0)
		return FALSE;

	// make sure we are not dropping memory
	ASSERT(m_Values == nullString);

#define GROW_SIZE 16
	m_Values      = new char[newBuffSize+GROW_SIZE+1];
	m_Values[0]   = '\0';
	m_buffSize    = newBuffSize+GROW_SIZE;
	m_nValues     = 0;

	return TRUE;
}

//---------------------------------------------------------------------------------------
SFString operator+(const SFString& str1, const SFString& str2)
{
	SFInt32 len = str1.GetLength() + str2.GetLength();
	SFString ret;
	if (ret.ResizeBuffer(len))
	{
		memcpy(ret.m_Values,               str1.m_Values, str1.GetLength());
		memcpy(ret.m_Values+str1.GetLength(), str2.m_Values, str2.GetLength());
		ret.m_nValues      = len;
		ret.m_Values[len] = '\0';
	}
	return ret;
}

//---------------------------------------------------------------------------------------
const SFString& SFString::operator+=(const SFString& add)
{
	if (add.GetLength())
		*this = (*this + add);
	return *this;
}

//---------------------------------------------------------------------------------------
char SFString::GetAt(SFInt32 index) const
{
	ASSERT(index >= 0);
	ASSERT(index < GetLength());

	return m_Values[index];
}

//---------------------------------------------------------------------------------------
char *SFString::GetBuffer(SFInt32 minBufferLen)
{
	if (m_buffSize < minBufferLen)
		ResizeBuffer(minBufferLen);
/*
	// This never worked right anyway so stop doing it
	// ResizeBuffer does it anyway and it should not change
	// if the buffer is already big enough
	m_nValues     = minBufferLen;
	m_Values[minBufferLen] = '\0';
*/
	ASSERT(m_Values);
	return m_Values;
}

//---------------------------------------------------------------------------------------
void SFString::ReleaseBuffer(void)
{
	ASSERT(m_Values);
	
	long len = strlen(m_Values);
	ASSERT(len <= m_buffSize);

	// May have been modified since it has been exposed
	m_nValues      = len;
	m_Values[len] = '\0';
}

//---------------------------------------------------------------------------------------
void SFString::SetAt(SFInt32 index, char ch)
{
	ASSERT(index >= 0);
	ASSERT(index < GetLength());
	m_Values[index] = ch;
}

//---------------------------------------------------------------------------------------
SFString SFString::Extract(SFInt32 start, SFInt32 len) const
{
	ASSERT(start >= 0);
	ASSERT(len   >= 0);
	ASSERT((char*)(start+start+len) <= (char*)((long)m_Values+strlen(m_Values)));	

	start = MAX((SFInt32)0, start); // must be positive
	len   = MAX((SFInt32)0, len);

	SFInt32 strStart = (SFInt32)m_Values;
	strStart += start;
	
	SFString ret;
	if (ret.ResizeBuffer(len))
		memcpy(ret.m_Values, (char *)strStart, len);
	ret.m_nValues     = len;
	ret.m_Values[len] = '\0';
	return ret;
}

//---------------------------------------------------------------------------------------
SFString SFString::Mid(SFInt32 first) const
{
	return Mid(first, GetLength() - first);
}

//---------------------------------------------------------------------------------------
SFString SFString::Mid(SFInt32 first, SFInt32 len) const
{
	first = MAX((SFInt32)0, first); // positive
	len   = MAX((SFInt32)0, len);
	if (first + len > GetLength()) len = GetLength() - first; // not past end
	if (first > GetLength()) len = 0; // not longer than string
	return Extract(first, len);
}

//---------------------------------------------------------------------------------------
SFString SFString::Right(SFInt32 len) const
{
	len = MAX((SFInt32)0, MIN(GetLength(), len));
	return Extract(GetLength()-len, len);
}

//---------------------------------------------------------------------------------------
SFString SFString::Left(SFInt32 len) const
{
	len = MAX((SFInt32)0, MIN(GetLength(), len));
	return Extract(0, len);
}

//---------------------------------------------------------------------------------------
// Find functions

//---------------------------------------------------------------------------------------
SFInt32 SFString::Find(char ch) const
{
	char *f = strchr(m_Values, ch);
	return (SFInt32)( f ? (f - m_Values) : -1);
}

//---------------------------------------------------------------------------------------
SFInt32 SFString::Find(const char *str) const
{
	char *f = strstr(m_Values, str);
	return (SFInt32)( f ? (f - m_Values) : -1);
}

//---------------------------------------------------------------------------------------
SFInt32 SFString::FindI(const char *search) const
{
	SFString me   = *this;  me.MakeLower();
	SFString test = search; test.MakeLower();
	char *f = strstr(me.m_Values, test.m_Values);
	return (SFInt32)( f ? (f - me.m_Values) : -1);
}

//---------------------------------------------------------------------------------------
SFBool SFString::ContainsAny(const SFString& search) const
{
	for (int i=0;i<search.GetLength();i++)
		if (Contains(search[i]))
			return TRUE;
	return FALSE;
}

//---------------------------------------------------------------------------------------
SFBool SFString::ContainsAll(const SFString& search) const
{
	for (int i=0;i<search.GetLength();i++)
		if (!Contains(search[i]))
			return FALSE;
	return TRUE;
}

//---------------------------------------------------------------------------------------
SFBool SFString::Contains(char ch) const
{
	return (Find(ch) != -1);
}

//---------------------------------------------------------------------------------------
SFBool SFString::Contains(const SFString& search) const
{
	return (Find(search) != -1);
}

//---------------------------------------------------------------------------------------
SFBool SFString::ContainsI(const SFString& search) const
{
	return (FindI(search) != -1);
}

//---------------------------------------------------------------------------------------
SFString SFString::Substitute(const SFString& what, const SFString& with) const
{
	SFString ret = *this;
	if (with.Contains("`"))
	{
		SFString whatStr = what;
		SFString withStr = with;
		while (!whatStr.IsEmpty()) // they should match but don't have to. With predominates
		{
			SFString wtStr = nextTokenClear(whatStr, '`');
			SFString whStr = nextTokenClear(withStr, '`');
			ret.ReplaceAll(wtStr, whStr);
		}
	} else
	{
		ret.ReplaceAll(what, with);
	}
	return ret;
}

//---------------------------------------------------------------------------------------
void SFString::Replace(const SFString& what, const SFString& with)
{
	SFInt32 i = Find(what);
	if (i != -1)
	{
		*this = Left(i) + with + Mid(i + what.GetLength());
	}
}

//---------------------------------------------------------------------------------------
void SFString::ReplaceI(const SFString& what, const SFString& with)
{
	SFInt32 i = FindI(what);
	if (i != -1)
	{
		*this = Left(i) + with + Mid(i + what.GetLength());
	}
}

//---------------------------------------------------------------------------------------
void SFString::ReplaceAll(char what, char with)
{
	char *s = m_Values;
	while (*s)
	{
		if (*s == what)
			*s = with;
		s++;
	}
}

//---------------------------------------------------------------------------------------
void SFString::ReplaceAll(const SFString& what, const SFString& with)
{
	if (what.IsEmpty())
		return;

	if (with.Contains(what))
	{
		// may cause endless recursions so do it in two steps instead
		ReplaceAll(what, SFString((char)0x5));
		ReplaceAll(SFString((char)0x5), with);
		return;
	}
	
	SFInt32 i = Find(what);
	while (i != -1)
	{
		Replace(what, with);
		i = Find(what);
	}
}

//---------------------------------------------------------------------------------------
void SFString::ReplaceAllI(const SFString& what, const SFString& with)
{
	if (what.IsEmpty())
		return;

	if (with.ContainsI(what))
	{
		// may cause endless recursions so do it in two steps instead
		ReplaceAllI(what, "]QXXQX[");
		ReplaceAllI("]QXXQX[", with);
		return;
	}
	
	SFInt32 i = FindI(what);
	while (i != -1)
	{
		ReplaceI(what, with);
		i = FindI(what);
	}
}

//---------------------------------------------------------------------------------------
void SFString::ReplaceAny(const SFString& list, const SFString& with)
{
	SFInt32 len = list.GetLength();
	for (int i=0;i<len;i++)
		ReplaceAll(list[i], with);
}

//---------------------------------------------------------------------------------------
void SFString::ReplaceAnyI(const SFString& list, const SFString& with)
{
	SFInt32 len = list.GetLength();
	for (int i=0;i<len;i++)
		ReplaceAllI(list[i], with);
}

//---------------------------------------------------------------------------------------
void SFString::ReplaceReverse(const SFString& whatIn, const SFString& withIn)
{
	SFString what = whatIn;
	SFString with = withIn;

	Reverse();
	what.Reverse();
	with.Reverse();
	Replace(what, with);
	Reverse();
}

//---------------------------------------------------------------------------------------
SFBool SFString::FindExactI(const SFString& search, char sep, const SFString& replaceables) const
{
	SFString me   = *this;  me.MakeLower();
	SFString test = search; test.MakeLower();
	return me.FindExact(test, sep, replaceables);
}

//---------------------------------------------------------------------------------------
SFBool SFString::ContainsExact(const SFString& search, char sep, const SFString& replaceables) const
{
	return (FindExact(search, sep, replaceables) != -1);
}

//---------------------------------------------------------------------------------------
SFBool SFString::ContainsExactI(const SFString& search, char sep, const SFString& replaceables) const
{
	return (FindExactI(search, sep, replaceables) != -1);
}

//---------------------------------------------------------------------------------------
void SFString::ReplaceExact(const SFString& what, const SFString& with, char sep, const SFString& replaceables)
{
	SFInt32 i = FindExact(what, sep, replaceables);
	if (i != -1)
	{
		*this = Left(i) + with + Mid(i + what.GetLength());
	}
}

//---------------------------------------------------------------------------------------
void SFString::ReplaceExactI(const SFString& what, const SFString& with, char sep, const SFString& replaceables)
{
	SFInt32 i = FindExactI(what, sep, replaceables);
	if (i != -1)
	{
		*this = Left(i) + with + Mid(i + what.GetLength());
	}
}

//---------------------------------------------------------------------------------------
void SFString::ReplaceAllExact(const SFString& what, const SFString& with, char sep, const SFString& replaceables)
{
	if (what.IsEmpty())
		return;

	if (with.ContainsExact(what, sep, replaceables))
	{
		// may cause endless recursions so do it in two steps instead
		ReplaceAllExact(what, "]QXXQX[", sep, replaceables);
		ReplaceAllExact("]QXXQX[", with, sep, replaceables);
		return;
	}
	
	SFInt32 i = FindExact(what, sep, replaceables);
	while (i != -1)
	{
		ReplaceExact(what, with, sep, replaceables);
		i = FindExact(what, sep, replaceables);
	}
}

//---------------------------------------------------------------------------------------
void SFString::ReplaceAllExactI(const SFString& what, const SFString& with, char sep, const SFString& replaceables)
{
	if (what.IsEmpty())
		return;

	if (with.ContainsExactI(what, sep, replaceables))
	{
		// may cause endless recursions so do it in two steps instead
		ReplaceAllExactI(what, "]QXXQX[", sep, replaceables);
		ReplaceAllExactI("]QXXQX[", with, sep, replaceables);
		return;
	}
	
	SFInt32 i = FindExactI(what, sep, replaceables);
	while (i != -1)
	{
		ReplaceExactI(what, with, sep, replaceables);
		i = FindExactI(what, sep, replaceables);
	}
}

//---------------------------------------------------------------------------------------
void SFString::ReplaceAnyExact(const SFString& list, const SFString& with, char sep, const SFString& replaceables)
{
	SFInt32 len = list.GetLength();
	for (int i=0;i<len;i++)
		ReplaceAllExact(list[i], with, sep, replaceables);
}

//---------------------------------------------------------------------------------------
void SFString::ReplaceAnyExactI(const SFString& list, const SFString& with, char sep, const SFString& replaceables)
{
	SFInt32 len = list.GetLength();
	for (int i=0;i<len;i++)
		ReplaceAllExactI(list[i], with, sep, replaceables);
}

//---------------------------------------------------------------------------------------
SFInt32 SFString::ReverseFind(char ch) const
{
	char *f = strrchr(m_Values, ch);
	return ((f)?((int)(f-m_Values)):-1);
}

//---------------------------------------------------------------------------------------
void SFString::FormatV(const char *lpszFormat, va_list argList) const
{
#define incPtr(x) ((x)+1)

	//ASSERT(AfxIsValidString(lpszFormat, FALSE));

	va_list argListSave;
	va_copy(argListSave, argList);

	// make a guess at the maximum length of the resulting string
	int nMaxLen = 0;

	const char *lpsz = lpszFormat;
	for (lpsz = lpszFormat; *lpsz != '\0'; lpsz = incPtr(lpsz))
	{
		// handle '%' character, but watch out for '%%'
		if (*lpsz != '%' || *(lpsz = incPtr(lpsz)) == '%')
		{
			nMaxLen += strlen(lpsz);
			continue;
		}

		int nItemLen = 0;

		// handle '%' character with format
		int nWidth = 0;
		for (; *lpsz != '\0'; lpsz = incPtr(lpsz))
		{
			// check for valid flags
			if (*lpsz == '#')
				nMaxLen += 2;   // for '0x'
			else if (*lpsz == '*')
				nWidth = va_arg(argList, int);
			else if (*lpsz == '-' || *lpsz == '+' || *lpsz == '0' ||
				*lpsz == ' ')
				;
			else // hit non-flag character
				break;
		}
		// get width and skip it
		if (nWidth == 0)
		{
			// width indicated by
			nWidth = atoi(lpsz);
			for (; *lpsz != '\0' && isdigit(*lpsz); lpsz = incPtr(lpsz))
				;
		}
		ASSERT(nWidth >= 0);

		int nPrecision = 0;
		if (*lpsz == '.')
		{
			// skip past '.' separator (width.precision)
			lpsz = incPtr(lpsz);

			// get precision and skip it
			if (*lpsz == '*')
			{
				nPrecision = va_arg(argList, int);
				lpsz = incPtr(lpsz);
			}
			else
			{
				nPrecision = atoi(lpsz);
				for (; *lpsz != '\0' && isdigit(*lpsz); lpsz = incPtr(lpsz))
					;
			}
			ASSERT(nPrecision >= 0);
		}

		// should be on type modifier or specifier
		int nModifier = 0;
		switch (*lpsz)
		{
		// modifiers that affect size
		case 'h':
			nModifier = FORCE_ANSI;
			lpsz = incPtr(lpsz);
			break;
		case 'l':
			nModifier = FORCE_UNICODE;
			lpsz = incPtr(lpsz);
			break;

		// modifiers that do not affect size
		case 'F':
		case 'N':
		case 'L':
			lpsz = incPtr(lpsz);
			break;
		}

		// now should be on specifier
		switch (*lpsz | nModifier)
		{
		// strings
		case 's':
		{
			const char *pstrNextArg = va_arg(argList, const char *);
			if (pstrNextArg == NULL)
			   nItemLen = 6;  // "(null)"
			else
			{
			   nItemLen = (int)strlen(pstrNextArg);
			   nItemLen = MAX(1, nItemLen);
			}
			break;
		}

		case 'S':
		{
			char* pstrNextArg = va_arg(argList, char*);
			if (pstrNextArg == NULL)
			   nItemLen = 6; // "(null)"
			else
			{
			   nItemLen = (int)strlen(pstrNextArg);
			   nItemLen = MAX(1, nItemLen);
			}
			break;
		}

		case 's'|FORCE_ANSI:
		case 'S'|FORCE_ANSI:
		{
			char *pstrNextArg = va_arg(argList, char*);
			if (pstrNextArg == NULL)
			   nItemLen = 6; // "(null)"
			else
			{
			   nItemLen = (int)strlen(pstrNextArg);
			   nItemLen = MAX(1, nItemLen);
			}
			break;
		}

		case 's'|FORCE_UNICODE:
		case 'S'|FORCE_UNICODE:
		{
			char *pstrNextArg = va_arg(argList, char*);
			if (pstrNextArg == NULL)
			   nItemLen = 6; // "(null)"
			else
			{
			   nItemLen = (int)strlen(pstrNextArg);
			   nItemLen = MAX(1, nItemLen);
			}
			break;
		}
		}

		// adjust nItemLen for strings
		if (nItemLen != 0)
		{
			nItemLen = MAX(nItemLen, nWidth);
			if (nPrecision != 0)
				nItemLen = MIN(nItemLen, nPrecision);
		}
		else
		{
			switch (*lpsz)
			{
			// integers
			case 'd':
			case 'i':
			case 'u':
			case 'x':
			case 'X':
			case 'o':
				va_arg(argList, int);
				nItemLen = 32;
				nItemLen = MAX(nItemLen, nWidth+nPrecision);
				break;

			case 'e':
			case 'f':
			case 'g':
			case 'G':
				va_arg(argList, double);
				nItemLen = 128;
				nItemLen = MAX(nItemLen, nWidth+nPrecision);
				break;

			case 'p':
				va_arg(argList, void*);
				nItemLen = 32;
				nItemLen = MAX(nItemLen, nWidth+nPrecision);
				break;

			// no output
			case 'n':
				va_arg(argList, int*);
				break;

			default:
				ASSERT(FALSE);  // unknown formatting option
			}
		}

		// adjust nMaxLen for output nItemLen
		nMaxLen += nItemLen;
	}

	((SFString*)this)->GetBuffer(nMaxLen);
	vsprintf(m_Values, lpszFormat, argListSave);
	((SFString*)this)->ReleaseBuffer();

	va_end(argListSave);
}

//---------------------------------------------------------------------------------------
// formatting (using wsprintf style formatting)
void SFString::Format(const char *lpszFormat, ...) const
{
	//ASSERT(AfxIsValidString(lpszFormat, FALSE));
	va_list argList;
	va_start(argList, lpszFormat);
	FormatV(lpszFormat, argList);
	va_end(argList);
}

//---------------------------------------------------------------------------------------
void SFString::Reverse()
{
	char tmp;
	unsigned int i,j;
	unsigned int n = (unsigned int)m_nValues ;
	
	for( i=0,j=n-1; i < n/2; i++,j-- )
	{
		tmp = m_Values[i];
		m_Values[i] = m_Values[j];
		m_Values[j] = tmp;
	}
}

//---------------------------------------------------------------------------------------
// Modifies input to make import easier
SFString unquoteLine(char *buff, char delim, const SFString& replace)
{
	SFString ret;
	ret.ResizeBuffer(strlen(buff)+1);
	
#define NORMAL 0
#define INQUOTE 1
	SFInt32 state = NORMAL;
	
	char *s = buff;
	while (*s)
	{
		switch (state)
		{
		case NORMAL:
			if (*s == '\"')
				state = INQUOTE;
			else if (*s == delim)
				ret += replace;
			else
				ret += *s;
			break;

		case INQUOTE:
			if (*s == '\"')
				state = NORMAL;
			else
				ret += *s;
			break;
		}
		s++;
	}
	
	// ends with a delimiter
	ret.ReplaceAll("\n", replace);
	ret.ReplaceAll("\r", "");

	return ret;
}

//---------------------------------------------------------------------------------------
SFString SFString::Center(SFInt32 width) const
{
	SFString str = *this;
	SFInt32  len = str.GetLength();

	SFInt32 n = 1;
	if (len < width)
		n = (width - len) >> 1;

	SFString space;
	for (SFInt32 i=0;i<n;i++)
		space += "&nbsp;";
	SFString ret = space + str.Left(MIN(width, len)) + space;

	return ret;
}

//---------------------------------------------------------------------------------------
SFInt32 SFString::Hash(SFInt32 max) const
{
	SFInt32 ret = 0;
	for (SFInt32 i=0;i<GetLength();i++)
		ret += (long)(char)m_Values[i];
	return (ret % max);		
}

//---------------------------------------------------------------------------------------
SFString RemoveOne(const SFString& path, char delim)
{
	if (!path.Contains(delim))
		return path;

	SFString ret = path;

	// strip trailing chars
	if (ret.endsWith(delim))
		ret = ret.Left(ret.GetLength()-1);
	
	// There better be at least one internal character of interest
	ASSERT(ret.ReverseFind(delim) != -1);

	// Strip the string back to the text preceding the delim
	return ret.Left(ret.ReverseFind(delim));
}

//---------------------------------------------------------------------------------------
SFInt32 SFString::Compare(const char *str) const
{
	return strcmp(m_Values, str);
}

//---------------------------------------------------------------------------------------
SFInt32 SFString::ICompare(const char *str) const
{
	return strcasecmp(m_Values, str);
}

//---------------------------------------------------------------------------------------
void SFString::MakeUpper()
{
	if (m_Values)
	{
		char *s = m_Values;
		while (*s)
		{
			*s = (char)toupper(*s);
			s++;
		}
	}
}

//---------------------------------------------------------------------------------------
void SFString::MakeLower()
{
	if (m_Values)
	{
		char *s = m_Values;
		while (*s)
		{
			*s = (char)tolower(*s);
			s++;
		}
	}
}

//---------------------------------------------------------------------------------------
void SFString::MakeProper()
{
	if (m_Values)
	{
		char last='\0';
		char *s = m_Values;
		while (*s)
		{
			if (last == '_' || isWhiteSpace(last))
				*s = (char)toupper(*s);

			else
				*s = (char)tolower(*s);
			last = *s;
			s++;
		}
	}
}

//----------------------------------------------------------------------------------------
SFString nextTokenClearReverse(SFString& str, char token)
{
	str.Reverse();
	SFString ret = nextTokenClear(str, token);
	ret.Reverse();
	str.Reverse();
	return ret;
}

//---------------------------------------------------------------------------------------
SFString snagFieldClearEx(SFString& in, const SFString& tagName, const SFString& defVal, SFString *fieldVals)
{
#ifdef _DEBUG
	SFBool hasStart = in.Contains("<"+tagName);
	SFBool hasStop  = in.Contains("</"+tagName+">");
	if (!hasStart || !hasStop)
	{
//		SFString out = in;
//		out.ReplaceAll("<", "&lt;");
//		out.ReplaceAll(">", "&gt;");
//		printf("<h3>'" + tagName + "' not found in '" + out + "'<h3><br>");
		return EMPTY;
	}
#endif
	SFString start = "<" + tagName;
	SFInt32  startLoc = in.Find(start);

	SFString stop  = "</" + tagName + ">";
	SFInt32  stopLoc  = in.Find(stop) + stop.GetLength();

	SFString ret = in.Mid(startLoc, (stopLoc-startLoc));
	in.Replace(ret, EMPTY);
	ret.Replace(stop, EMPTY);

	if (fieldVals)
		fieldVals[0] = EMPTY;
	SFString tag = nextTokenClear(ret, '>');
	tag.Replace(start, EMPTY);
	tag.ReplaceAll("'", "\"");
	while (!tag.IsEmpty())
	{
		SFString field = Strip(nextTokenClear(tag, '='), ' ');
		nextTokenClear(tag, '\"'); // skip leading quote
		SFString value = Strip(nextTokenClear(tag, '\"'), ' ');
		if (fieldVals)
			fieldVals[0] += ("<" + field + ">" + value + "</" + field + ">");
	}

	if (ret.IsEmpty())
		ret = defVal;

	return ret;
}

//---------------------------------------------------------------------------------------
SFString snagFieldClear(SFString& in, const SFString& field, const SFString& defVal)
{
#ifdef _DEBUG
	SFBool start = in.Contains("<"+field+">");
	SFBool stop  = in.Contains("</"+field+">");
	if (start != stop)
	{
		SFString out = in;
		out.ReplaceAll("<", "&lt;");
		out.ReplaceAll(">", "&gt;");
		printf("%s", (const char*)(SFString("<h3>'") + field + "' not found in '" + out + "'<h3><br>"));
	}
#endif
	SFString f1 = "</" + field + ">";
	SFString ret = in.Left(in.Find(f1));

	SFString f2 = "<" + field + ">";
	ret = ret.Mid(ret.Find(f2)+f2.GetLength());

	in.Replace(f2 + ret + f1, "");

	if (ret.IsEmpty())
		ret = defVal;

	return ret;
}

//---------------------------------------------------------------------------------------
SFString snagFieldClearStrip(SFString& in, const SFString& field, const SFString& defVal, const SFString& stripWhat)
{
	SFString ret = snagFieldClear(in, field, defVal);
	return StripAny(ret, stripWhat);
}

//---------------------------------------------------------------------------------------
SFString snagField(const SFString& in, const SFString& field, const SFString& defVal)
{
#ifdef _DEBUG
	SFBool start = in.Contains("<"+field+">");
	SFBool stop  = in.Contains("</"+field+">");
	if (start != stop)
	{
		SFString out = in;
		out.ReplaceAll("<", "&lt;");
		out.ReplaceAll(">", "&gt;");
		printf("%s", (const char*)(SFString("<h3>'") + field + "' not found in '" + out + "'<h3><br>"));
	}
#endif
    SFString f = "</" + field + ">";
    SFString ret = in.Left(in.Find(f));

    f.Replace("</", "<");
    ret = ret.Mid(ret.Find(f)+f.GetLength());

	if (ret.IsEmpty())
		ret = defVal;

	return ret;
}

//---------------------------------------------------------------------------------------
SFString snagEmail(const SFString& email)
{
	if (!IsValidEmail(email))
		return EMPTY;
	return "<a href=mailto:" + email + ">" + email + "</a>";
}

//---------------------------------------------------------------------------------------
SFString snagURL(const SFString& url)
{
	if (!isURL(url))
		return EMPTY;
	return "<a target=top href=" + url + ">" + url + "</a>";
}

//---------------------------------------------------------------------------------------
static SFBool isURL_base(const SFString& test, SFBool testRel=FALSE)
{
	if (test.Find("://")!=-1)
		return TRUE;
	if (testRel && test.GetLength() && (test[0] == '.' || test[0] == '/'))
		return TRUE;
	return FALSE;
}

//---------------------------------------------------------------------------------------
SFBool isURL(const SFString& test)
{
	return isURL_base(test, TRUE);
}

//---------------------------------------------------------------------------------------
SFBool isAbsolutePath(const SFString& test)
{
	if (test.IsEmpty())
		return FALSE;

	// string either starts with a '/' or.... (mostly linux)
	if (test[0] == '/' || test[0] == '\\')
		return TRUE;

    if (test.GetLength() < 2)
        return FALSE;

	// it starts with a drive letter then a colon (mostly windows)
	return (test[1] == ':');
}

//---------------------------------------------------------------------------------------
SFBool isRelativePath(const SFString& test)
{
	return (!isAbsolutePath(test));
}

//---------------------------------------------------------------------------------------
SFString toRelativePath(const SFString& root, const SFString& path)
{
	SFString ret = path;
	ret.Replace(root, "./");
	return ret;
}

const char* CHR_VALID_NAME  = "\t\n\r()<>[]{}`\\|; " "'!$^*~@" "?&#+%" ",:/=\"";
const char* CHR_ALLOW_URL   =                              "@" "?&#+%" ",:/=\"";
const char* CHR_ALLOW_EMAIL =                        "'!$^*~@" "?&#+%";

//---------------------------------------------------------------------------------------
SFString makeValidName(const SFString& inOut)
{
	SFString ret = inOut;

	// make it a valid path
	ret.ReplaceAny(CHR_VALID_NAME, "_");
	if (!ret.IsEmpty() && isdigit(ret[0]))
		ret = "_" + ret;

	return ret;
}

//---------------------------------------------------------------------------------------
SFString makeValidName_noDash(const SFString& inOut)
{
	SFString out = makeValidName(inOut);
	out.ReplaceAll("-", "_");
	return out;
}

//---------------------------------------------------------------------------------------
SFString markURLs(const SFString& input)
{
	SFString in = input;

	SFString repStr = CHR_VALID_NAME;
	repStr.ReplaceAny(CHR_ALLOW_URL, EMPTY);
	in.ReplaceAll("</",   "_");
	in.ReplaceAny(repStr, "_");

	SFString list;
	while (!in.IsEmpty())
	{
		SFString part = nextTokenClear(in, '_');

		if (isURL_base(part) && !list.Contains(part))
			list += (part + "|");

		if (IsValidEmail(part) && !list.Contains(part))
			list += (part + "|");
	}
	if (list.IsEmpty())
		return input;

	SFString out = input;
	while (!list.IsEmpty())
	{
		SFString part = nextTokenClear(list, '|');
		if (isURL_base(part))
			out.ReplaceAll(part, snagURL  (part));
		if (IsValidEmail(part))
			out.ReplaceAll(part, snagEmail(part));
		list.ReplaceAll(part + "|", EMPTY);
	}

	return out;
}

//---------------------------------------------------------------------------------------
SFBool SFString::FindExact(const SFString& search, char sep, const SFString& repables) const
{
	ASSERT(sep == '|' || sep == ';' || sep == CH_COMMA || sep == ' ');

	SFString sepStr(sep);

	// Surround the stringa with sep to handle boundary cases
	SFString tS = sepStr + *this  + sepStr;
	SFString qS = sepStr + search + sepStr; 

	// we will replace everything but the separator
	SFString replaceables = repables;
	replaceables.Replace(sepStr, EMPTY);

 	tS.ReplaceAny(replaceables, sepStr);

	return tS.Find(qS);
}

//---------------------------------------------------------------------------------------
SFBool SFString::endsWithAny(const SFString& str) const
{
	for (int i=0;i<str.GetLength();i++)
		if (endsWith(str[i]))
			return true;
	return false;
}

//---------------------------------------------------------------------------------------
SFBool SFString::startsWithAny(const SFString& str) const
{
	for (int i=0;i<str.GetLength();i++)
		if (startsWith(str[i]))
			return true;
	return false;
}

//-----------------------------------------------------------------------------------------
SFString stripHTML(const SFString& strIn)
{
	SFString out;
	SFInt32 inToken=0;

	SFString str = strIn;
	while (str.Contains("`"))
	{
		str.Replace("`","<a ");
		str.Replace("|",">");
		str.Replace("`","</a>");
	}

	char *s = (char *)(const char *)str;
	while (*s)
	{
		switch (*s)
		{
			case '<':
				inToken++;
				break;
			case '>':
				inToken--;
				break;
			case '`':
				if (inToken)
					inToken--;
				else
					inToken++;
		}

		if (!inToken && *s != '>')
		{
			if (*s == '\"')
				*s = '\'';
			out += *s;
		}
		s++;
	}

	return Strip(out, ' ');
}

//-----------------------------------------------------------------------------------------
SFString randomPassword(void)
{
	SFInt32 min = (SFInt32)'A';
	SFInt32 max = (SFInt32)'z';
	SFString ret;
	for (int i=0;i<12;i++)
	{
		char c = (char)RandomValue(min, max);
		ret += SFString(c);
	}
	ret.ReplaceAny("[\\", asString(RandomValue(0,9)));
	ret.ReplaceAny("_`",  asString(RandomValue(0,9)));
	ret.ReplaceAny("^]",  asString(RandomValue(0,9)));

	return ret;
}

//-----------------------------------------------------------------------------------------
SFString whichBadChar(const SFString& checkStr, const SFString& disallowStr)
{
	SFString badChar;
	for (int i=0;i<disallowStr.GetLength();i++)
		if (checkStr.Contains(disallowStr[i]))
			badChar = disallowStr[i];
	return badChar;
}

//-----------------------------------------------------------------------------------------
void SFString::markAtOccurenceOf(char c, SFInt32 nOcc)
{
	SFInt32 cnt=0;
	char *s = m_Values;
	while (*s)
	{
		if (*s == c)
			cnt++;
		if (cnt == nOcc)
		{
			*s = '~';
			return;
		}
		s++;
	}
}

//-----------------------------------------------------------------------------------------
int sortByStringValue(const void *rr1, const void *rr2)
{
	SFString n1 = *(SFString*)rr1;
	SFString n2 = *(SFString*)rr2;
	return (int)n1.ICompare(n2);
}

//-----------------------------------------------------------------------------------------
int compareStringValue(const void *rr1, const void *rr2)
{
	// return true of these are the same string
	return !sortByStringValue(rr1, rr2);
}
