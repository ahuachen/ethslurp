#ifndef _SFSTRING_H
#define _SFSTRING_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "basetypes.h"

extern char nullString[];

extern const char* CHR_VALID_NAME;
extern const char* CHR_ALLOW_URL;
extern const char* CHR_ALLOW_EMAIL;

/*----------------------------------------------------------------------
CLASS
	SFString

	A string class.

DESCRIPTION
	<ul>
	This is a basic C++ string class supporting most of the functionality
	available for strings.
	</ul>

NOTES
	<ul>
	All SFString class instances refer to the 'nullString' object
	by default.
	</ul>

EXAMPLE CODE
	<pre>
	// Use SFStrings as you would any string class
	SFString string = "This is a string";
	// Find returns -1 if search string is not found.
	ASSERT(string.Find(".")==-1);

	// Add a period.
	string += ".";
	// Now it is found.
	ASSERT(string.Find(".")!=-1);

	// Assignment
	SFString str1 = string;
	// Equality (many other operators are available also).
	ASSERT(str1 == string);

	// Use 'nullString' or IsEmpty to test for empty strings
	SFString defaultString;
	ASSERT(defaultString == nullString);
	ASSERT(defaultString.IsEmpty());
	</pre>

MEMBERS
----------------------------------------------------------------------*/
class SFString
{
private:
	SFString  (long) {};
	const SFString& operator=(long);

protected:
  //<doc>------------------------------------------------------------
  // <dd>The actual characters for this string.
	// [default:] NULL
	//
	char    *m_Values;

  //<doc>------------------------------------------------------------
  // <dd>The number of characters in the string including trailing '\0'.
	// [default:] 0
	//
	SFInt32  m_nValues;

  //<doc>------------------------------------------------------------
  // <dd>The size of the string buffer (usually the same as m_nValues)
	// [default:] 0
	//
	SFInt32  m_buffSize;

public:
  //<doc>------------------------------------------------------------
  // <dd>Default constructor.
	//
	SFString  (void);

  //<doc>------------------------------------------------------------
  // <dd>Copy constructor.
	//
	SFString  (const SFString& str);

  //<doc>------------------------------------------------------------
  // <dd>Destructor.
	//
	~SFString (void);

  //<doc>------------------------------------------------------------
  // <dd>Constructor.
	//
	// [in] ch: A character to use to initialize the string.
	// [in] reps: The number of times to repeat the character.
	//
	SFString  (char ch, SFInt32 reps=1);

  //<doc>------------------------------------------------------------
  // <dd>Constructor.
	//
	// [in] str: A charater string to promote to this SFString.
	// [in] start: The starting position from which to start copying (defaults to 0).
	// [in] len: The number of characters to copy starting at start (defaults to rest of string).
	//
	SFString  (const char *str, SFInt32 start=0, SFInt32 len=-1);

  //<doc>------------------------------------------------------------
  // <dd>Clear string memory and reset the string (same as default construction).
	//
	void            Clear         (void);

  //<doc>------------------------------------------------------------
  // <dd>Assignment operator from another SFString.
	//
	const SFString& operator=     (const SFString& str);

		//<doc>------------------------------------------------------------
  // <dd>Assignment operator from a character.
	//
	const SFString& operator=     (char ch);

  //<doc>------------------------------------------------------------
  // <dd>Assignment operator from a character string.
	//
	const SFString& operator=     (const char *str);

  //<doc>------------------------------------------------------------
  // <dd>Append operator from an SFString.
	//
	const SFString& operator+=    (const SFString& str);

  //<doc>------------------------------------------------------------
  // <dd>Append operator from a character.
	//
	const SFString& operator+=    (char ch);

  //<doc>------------------------------------------------------------
  // <dd>Append operator from a character string.
	//
	const SFString& operator+=    (const char *str);

  //<doc>------------------------------------------------------------
  // <dd>Access operator for a particular character.
	//
	// [in] index: The character to return.
	//
	char            GetAt         (SFInt32 index) const;

  //<doc>------------------------------------------------------------
  // <dd>Set a particular character
	//
	// [in] index: The character to set.
	// [in] ch: The character data.
	//
	void            SetAt         (SFInt32 index, char ch);

  //<doc>------------------------------------------------------------
  // <dd>Access operator for a particular character (same as GetAt).
	//
	// [in] index: The character to return.
	//
	char            operator[]    (int index) const;

  //<doc>------------------------------------------------------------
  // <dd>Casts the string to a const char *
	//
	operator        const char *  (void) const;

  //<doc>------------------------------------------------------------
  // <dd>Casts the string to a char *
	//
	operator        char *  (void);

  //<doc>------------------------------------------------------------
  // <dd>Returns the character pointer data member for direct manipulation.
	//
	char           *GetBuffer     (SFInt32 size=-1);

  //<doc>------------------------------------------------------------
  // <dd>Releases the character buffer and resizes the array.
	//
	void            ReleaseBuffer (void);

  //<doc>------------------------------------------------------------
  // <dd>Return the length of this string.
	//
	SFInt32         GetLength        (void) const;

  //<doc>------------------------------------------------------------
  // <dd>Returns TRUE if this string is empty (i.e. GetLength() == 0).
	//
	SFBool          IsEmpty       (void) const;

  //<doc>------------------------------------------------------------
  // <dd>Case sensitive compare this string to the given string.  Returns -1 if 'str' is less than, 0 if equal and 1 if 'str' is greater than this string.
	//
	// [in] str: The string to compare.
	//
	SFInt32         Compare       (const char *str) const;

  //<doc>------------------------------------------------------------
  // <dd>Case in-sensitive compare this string to the given string.  Returns -1 if 'str' is less than, 0 if equal and 1 if 'str' is greater than this string.
	//
	// [in] str: The string to compare.
	//
	SFInt32         ICompare      (const char *str) const;

  //<doc>------------------------------------------------------------
  // <dd>Extract a substring from the middle of this string.
	//
	// [in] first: The starting character to extract.
	// [in] len: The number of characters to extract from start.
	//
	SFString        Mid           (SFInt32 first, SFInt32 len) const;

  //<doc>------------------------------------------------------------
  // <dd>Extract a substring from the middle of this string to the end.
	//
	// [in] first: The starting character to extract.
	//
	SFString        Mid           (SFInt32 first) const;

  //<doc>------------------------------------------------------------
  // <dd>Extract the leftmost 'len' characters from this string.
	//
	// [in] len: The number of characters to extract.
	//
	SFString        Left          (SFInt32 len) const;

  //<doc>------------------------------------------------------------
  // <dd>Extract the rightmost 'len' characters from this string.
	//
	// [in] len: The number of characters to extract.
	//
	SFString        Right         (SFInt32 len) const;

  //<doc>------------------------------------------------------------
  // <dd>Pad string on both sides with spaces to keep the string centered
	//
	// [in] len: Total width of resulting centered string
	//
	SFString        Center        (SFInt32 width) const;

  //<doc>------------------------------------------------------------
  // <dd>Convert this string to all upper case.
	//
	void            MakeUpper       (void);

  //<doc>------------------------------------------------------------
  // <dd>Convert this string to all lower case.
	//
	void            MakeLower       (void);

  //<doc>------------------------------------------------------------
  // <dd>Convert this string to all upper case.
	//
	void            MakeProper      (void);

  //<doc>------------------------------------------------------------
  // <dd>Reverse this string.
	//
	void            Reverse       (void);

  //<doc>------------------------------------------------------------
  // <dd>Same as sprintf from 'c' runtime library
	//
	void Format(const char *lpszFormat, ...) const;

	//<doc>------------------------------------------------------------
	// <dd>Hash the string into an index in the range (0-max)
	//
	SFInt32 Hash(SFInt32 max) const;

	//<doc>------------------------------------------------------------
	// <dd>Search and replace stuff
	//
	SFInt32  Find            (char ch) const;
	SFInt32  Find            (const char *search) const;
	SFInt32  FindI           (const char *search) const;

	SFBool   Contains        (char search) const;
	SFBool   Contains        (const SFString& search) const;
	SFBool   ContainsI       (const SFString& search) const;
	SFBool   ContainsAll     (const SFString& search) const;
	SFBool   ContainsAny     (const SFString& search) const;

	SFString Substitute      (const SFString& what, const SFString& with) const;

	void     Replace         (const SFString& what, const SFString& with);
	void     ReplaceI        (const SFString& what, const SFString& with);

	void     ReplaceAll      (      char      what,       char      with);
	void     ReplaceAll      (const SFString& what, const SFString& with);
	void     ReplaceAllI     (const SFString& what, const SFString& with);

	void     ReplaceAny      (const SFString& list, const SFString& with);
	void     ReplaceAnyI     (const SFString& list, const SFString& with);

	SFBool   FindExact       (const SFString& search, char sep, const SFString& replaceables=CHR_VALID_NAME) const;
	SFBool   FindExactI      (const SFString& search, char sep, const SFString& replaceables=CHR_VALID_NAME) const;

	SFBool   ContainsExact   (const SFString& search, char sep, const SFString& replaceables=CHR_VALID_NAME) const;
	SFBool   ContainsExactI  (const SFString& search, char sep, const SFString& replaceables=CHR_VALID_NAME) const;

	void     ReplaceExact    (const SFString& what, const SFString& with, char sep, const SFString& replaceables=CHR_VALID_NAME);
	void     ReplaceExactI   (const SFString& what, const SFString& with, char sep, const SFString& replaceables=CHR_VALID_NAME);

	void     ReplaceAllExact (const SFString& what, const SFString& with, char sep, const SFString& replaceables=CHR_VALID_NAME);
	void     ReplaceAllExactI(const SFString& what, const SFString& with, char sep, const SFString& replaceables=CHR_VALID_NAME);

	void     ReplaceAnyExact (const SFString& list, const SFString& with, char sep, const SFString& replaceables=CHR_VALID_NAME);
	void     ReplaceAnyExactI(const SFString& list, const SFString& with, char sep, const SFString& replaceables=CHR_VALID_NAME);

	SFInt32  ReverseFind     (char ch) const;
	void     ReplaceReverse  (const SFString& what, const SFString& with);

	//<doc>------------------------------------------------------------
  // <dd>Return str2 concatinated to str1.
	//
	friend SFString operator+(const SFString& str1, const SFString& str2);

  //<doc>------------------------------------------------------------
  // <dd>Return ch concatinated to str.
	//
	friend SFString operator+(const SFString& str,  char ch);

  //<doc>------------------------------------------------------------
  // <dd>Return val (as a string) concatinated to str.
	//
	friend SFString operator+(const SFString& str,  long val);

  //<doc>------------------------------------------------------------
  // <dd>Return str concatinated to ch.
	//
	friend SFString operator+(char ch,              const SFString& str);

  //<doc>------------------------------------------------------------
  // <dd>Return str2 concatinated to str1.
	//
	friend SFString operator+(const SFString& str1, const char *str2);

  //<doc>------------------------------------------------------------
  // <dd>Return str2 concatinated to str1.
	//
	friend SFString operator+(const char *str1,     const SFString& str2);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if the two strings are the same.
	//
	friend SFBool   operator== (const SFString& str1, const SFString& str2);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if the two strings are the same.
	//
	friend SFBool   operator== (const SFString& str1, const char *str2);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if the two strings are the same.
	//
	friend SFBool   operator== (const char *str1, const SFString& str2);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if the two strings are the same.
	//
	friend SFBool   operator== (const SFString& str1, char ch);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if the two strings are the same.
	//
	friend SFBool   operator== (char ch, const SFString& str2);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if the two strings are not the same.
	//
	friend SFBool   operator!= (const SFString& str1, const SFString& str2);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if the two strings are not the same.
	//
	friend SFBool   operator!= (const SFString& str1, const char *str2);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if the two strings are not the same.
	//
	friend SFBool   operator!= (const char *str1, const SFString& str2);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if str1 is strictly less than str2.
	//
	friend SFBool   operator<  (const SFString& str1, const SFString& str2);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if str1 is strictly less than str2.
	//
	friend SFBool   operator<  (const SFString& str1, const char *str2);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if str1 is strictly less than str2.
	//
	friend SFBool   operator<  (const char *str1, const SFString& str2);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if str1 is strictly greater than str2.
	//
	friend SFBool   operator>  (const SFString& str1, const SFString& str2);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if str1 is strictly greater than str2.
	//
	friend SFBool   operator>  (const SFString& str1, const char *str2);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if str1 is strictly greater than str2.
	//
	friend SFBool   operator>  (const char *str1, const SFString& str2);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if str1 is less than or equal to str2.
	//
	friend SFBool   operator<= (const SFString& str1, const SFString& str2);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if str1 is less than or equal to str2.
	//
	friend SFBool   operator<= (const SFString& str1, const char *str2);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if str1 is less than or equal to str2.
	//
	friend SFBool   operator<= (const char *str1, const SFString& str2);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if str1 is greater than or equal to str2.
	//
	friend SFBool   operator>= (const SFString& str1, const SFString& str2);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if str1 is greater than or equal to str2.
	//
	friend SFBool   operator>= (const SFString& str1, const char *str2);

  //<doc>------------------------------------------------------------
  // <dd>Return TRUE if str1 is greater than or equal to str2.
	//
	friend SFBool   operator>= (const char *str1, const SFString& str2);

  //<nodoc>------------------------------------------------------------
  // <dd>Extract a substring from this string (used by Mid, Left and Right).
	//
	// [in] first: The starting character to extract.
	// [in] len: The number of characters to extract from start.
	//
	SFString        Extract       (long first, long len) const;

  //<nodoc>------------------------------------------------------------
  // <dd>Grow or shrink the buffer to new length.  Returns the size of the new buffer.
	//
	// [in] nLen: The new length of the string buffer.
	//
	SFInt32         ResizeBuffer  (SFInt32 nLen);

	SFBool          endsWith      (char ch) const;
	SFBool          startsWith    (char ch) const;
	SFBool          endsWithAny   (const SFString& str) const;
	SFBool          startsWithAny (const SFString& str) const;

	SFBool          endsWith      (const SFString& str) const;
	SFBool          startsWith    (const SFString& str) const;

	SFBool          LoadString(SFInt32 nID);
	void			markAtOccurenceOf(char c, SFInt32 nOcc);
	
protected:
  //<nodoc>------------------------------------------------------------
  // <dd>Reinitialize the string.
	//
	void            Initialize    (void);

private:
  //<nodoc>------------------------------------------------------------
  // <dd>Support for Format method
	//
	void	FormatV	(const char *lpszFormat, va_list argList) const;
};

#define SPACER        SFString("&nbsp;")
#define BIG_SPACE     (SPACER+SPACER+SPACER+SPACER)

#define CH_SEMI       ';'
#define CH_TAB        '\t'
#define CH_BAR        '|'
#define CH_COMMA      ','

#define SEP_SEMI      SFString(CH_SEMI)
#define SEP_TAB       SFString(CH_TAB)
#define SEP_BAR       SFString(CH_BAR)
#define SEP_COMMA     SFString(CH_COMMA)
#define SEP_PLUS      SFString("+")
#define SEP_SPACE     SFString(" ")

#define NEVER_TIMEOUT 50000

extern SFString unquoteLine         (char *buff, char delim, const SFString& replace);
extern SFString rotateStr           (const SFString& in, SFBool  timed,   SFInt32 serr, SFBool oldVerion);
extern SFString unRotateStr         (const SFString& ii, SFInt32 timeout, SFInt32 serr, SFBool oldVersion);
extern SFBool   IsValidEmail        (const SFString& email);
extern SFString makeValidName       (const SFString& input);
extern SFString makeValidName_noDash(const SFString& input);
extern SFString snagEmail           (const SFString& email);
extern SFString snagURL             (const SFString& url);
extern SFBool   isURL               (const SFString& test);
extern SFBool   isAbsolutePath      (const SFString& test);
extern SFBool   isRelativePath      (const SFString& test);
extern SFString toRelativePath      (const SFString& root, const SFString& path);
extern SFString markURLs            (const SFString& input);

inline SFBool isWhiteSpace(char c)
{
	return (c == '\0' || c == ' ' || c == '\n' || c == '\r' || c == '\t');
}

inline SFBool isNumeric(const SFString& str)
{
	SFString test = str;
	// replace all valid characters
	test.ReplaceAny("+-0123456789", "");
	// if everything was numeric (i.e. everything was replaced
	// with "") then this is numeric
	return !test.GetLength();
}

//----------------------------------------------------------
inline const SFString& SFString::operator=(const char *str)
{
	*this = SFString(str);
	return *this;
}

inline const SFString& SFString::operator=(char ch)
{
	*this = SFString(ch);
	return *this;
}

inline const SFString& SFString::operator+=(const char *str)
{
	return operator+=(SFString(str));
}

inline const SFString& SFString::operator+=(char ch)
{
	return operator+=(SFString(ch));
}

inline SFInt32 SFString::GetLength() const
{
	return m_nValues;
}

inline SFBool SFString::IsEmpty() const
{
	return !GetLength();
}

inline char SFString::operator[](int index) const
{
	return GetAt(index);
}

inline SFString::operator const char *() const
{
	return m_Values;
}

inline SFString::operator char *()
{
	return m_Values;
}

//----------------------------------------------------------
// Friends and family
inline SFString operator+(const SFString& str1, const char *str2)
{
	return operator+(str1, SFString(str2));
}

inline SFString operator+(const SFString& str1, long val)
{
	char str[10];
	sprintf(str, "%ld", val);
	return operator+(str1, SFString(str));
}

inline SFString operator+(const char *str1, const SFString& str2)
{
	return operator+(SFString(str1), str2);
}

inline SFString operator+(const SFString& str,  char ch)
{
	return operator+(str, SFString(ch));
}

inline SFBool SFString::endsWith(char ch) const
{
	if (IsEmpty())
		return FALSE;
	return (GetAt(GetLength()-1) == ch);
}

inline SFBool SFString::startsWith(char ch) const
{
	if (IsEmpty())
		return FALSE;
	return (GetAt(0) == ch);
}

inline SFBool SFString::endsWith(const SFString& str) const
{
	if (IsEmpty())
		return FALSE;
	return (Right(str.GetLength()) == str);
}

inline SFBool SFString::startsWith(const SFString& str) const
{
	if (IsEmpty())
		return FALSE;
	return (Left(str.GetLength()) == str);
}

//----------------------------------------------------------
// comparison friends
inline SFBool operator==(const SFString& str1, const SFString& str2)
{
	return str1.Compare(str2) == 0;
}

inline SFBool operator==(const SFString& str1, const char *str2)
{
	return str1.Compare(str2) == 0;
}

inline SFBool operator==(const char *str1, const SFString& str2)
{
	return str2.Compare(str1) == 0;
}

inline SFBool operator==(const SFString& str1, char ch)
{
	return str1.Compare(SFString(ch)) == 0;
}

inline SFBool operator==(char ch, const SFString& str2)
{
	return str2.Compare(SFString(ch)) == 0;
}

inline SFBool operator!=(const SFString& str1, const SFString& str2)
{
	return str1.Compare(str2) != 0;
}

inline SFBool operator!=(const SFString& str1, const char *str2)
{
	return str1.Compare(str2) != 0;
}

inline SFBool operator!=(const char *str1, const SFString& str2)
{
	return str2.Compare(str1) != 0;
}

inline SFBool operator<(const SFString& str1, const SFString& str2)
{
	return str1.Compare(str2) < 0;
}

inline SFBool operator<(const SFString& str1, const char *str2)
{
	return str1.Compare(str2) < 0;
}

inline SFBool operator<(const char *str1, const SFString& str2)
{
	return str2.Compare(str1) > 0;
}

inline SFBool operator>(const SFString& str1, const SFString& str2)
{
	return str1.Compare(str2) > 0;
}

inline SFBool operator>(const SFString& str1, const char *str2)
{
	return str1.Compare(str2) > 0;
}

inline SFBool operator>(const char *str1, const SFString& str2)
{
	return str2.Compare(str1) < 0;
}

inline SFBool operator<=(const SFString& str1, const SFString& str2)
{
	return str1.Compare(str2) <= 0;
}

inline SFBool operator<=(const SFString& str1, const char *str2)
{
	return str1.Compare(str2) <= 0;
}

inline SFBool operator<=(const char *str1, const SFString& str2)
{
	return str2.Compare(str1) >= 0;
}

inline SFBool operator>=(const SFString& str1, const SFString& str2)
{
	return str1.Compare(str2) >= 0;
}

inline SFBool operator>=(const SFString& str1, const char *str2)
{
	return str1.Compare(str2) >= 0;
}

inline SFBool operator>=(const char *str1, const SFString& str2)
{
	return str2.Compare(str1) <= 0;
}

//----------------------------------------------------------------
// Approximately equal -- ie. ignore case
// These are undocumented but handy
inline SFBool operator%(const SFString& str1, const SFString& str2)
{
	return str1.ICompare(str2) == 0;
}

inline SFBool operator%(const SFString& str1, const char *str2)
{
	return str1.ICompare(str2) == 0;
}

inline SFBool operator%(const char *str1, const SFString& str2)
{
	return str2.ICompare(str1) == 0;
}

inline SFBool operator%(const SFString& str1, char ch)
{
	return str1.ICompare(SFString(ch)) == 0;
}

inline SFBool operator%(char ch, const SFString& str2)
{
	return str2.ICompare(SFString(ch)) == 0;
}

inline SFInt32 countOf(char c, const SFString& str)
{
	SFInt32 len = str.GetLength();
	SFInt32 i=0, cnt=0;
	while (i<len)
	{
		if (str.GetAt(i) == c)
			cnt++;
		i++;
	}
	return cnt;
}

inline SFString asHex8(SFInt32 val)
{
	char tmp[20];
	sprintf(tmp, "%08x", (unsigned int)val);
	return tmp;
}

inline SFString asHex(SFInt32 val)
{
	ASSERT(val >=0 && val < 256);

	char tmp[20];
	sprintf(tmp, "%02x", (unsigned int)(char)val);
	SFString ret = tmp;
	return ret.Right(2);
}

#define WIERD_NUM (-20206)

inline SFString asString(SFInt32 i, SFInt32 def=WIERD_NUM)
{
	if (i == def && def != WIERD_NUM)
		return nullString;
	char ret[50];
	sprintf(ret, "%ld", i);
	return SFString(ret);
}

inline SFString asStringU(SFUint32 i, SFUint32 def=(SFUint32)WIERD_NUM)
{
	if (i == def && def != (SFUint32)WIERD_NUM)
		return nullString;
	char ret[50];
	sprintf(ret, "%lu", i);
	return SFString(ret);
}

inline SFString asBitmap(unsigned long value)
{
	SFString ret;
	for (int i=31;i>-1;i--)
	{
		SFBool isOn = (value & (1<<i));
		if (isOn) 
			ret += "1";
		else
			ret += "0";
	}
	return ret;
}

inline SFString nextToken(SFString& line, char delim)
{
	SFString ret;
	
	SFInt32 find = line.Find(delim);
	if (find!=-1)
	{
		ret  = line.Left(find);
		line = line.Mid(find+1);
	} else if (!line.IsEmpty())
		ret  = line;
	
	return ret;
}

inline SFString nextTokenClear(SFString& line, char delim)
{
	SFString ret;
	
	SFInt32 find = line.Find(delim);
	if (find!=-1)
	{
		ret  = line.Left(find);
		line = line.Mid(find+1);
	} else if (!line.IsEmpty())
	{
		ret  = line;
		line = "";
	}
	
	return ret;
}

inline SFInt32 nextTokenClearI(SFString& line, char delim)
{
	return toLong(nextTokenClear(line, delim));
}

#define EMPTY SFString("")

SFString RemoveOne(const SFString& path, char delim);

// Remove trailing and leading char c's
inline SFString StripTrailing(const SFString& str, char c)
{
	SFString ret = str;

	while (ret.endsWith(c))
		ret = ret.Left(ret.GetLength()-1);

	return ret;
}

inline SFString StripLeading(const SFString& str, char c)
{
	SFString ret = str;

	while (ret.startsWith(c))
		ret = ret.Mid(1);

	return ret;
}

inline SFString Strip(const SFString& str, char c)
{
	return StripTrailing(StripLeading(str, c), c);
}

inline SFString StripAny(const SFString& str, const SFString& any)
{
	SFString ret = str;
	while (ret.endsWithAny(any) || ret.startsWithAny(any))
	{
		for (int i=0;i<any.GetLength();i++)
			ret = Strip(ret, any[i]);
	}
	return ret;
}

inline SFString NormalizePath(const SFString& path)
{
	SFString ret = path;
	if (!ret.endsWith('/'))
		ret += "/";

	ret.ReplaceAll("\\", "/");
	ret.ReplaceAll("//", "/");

	return ret;
}

inline SFString padRight(const SFString& str, SFInt32 len, char p=' ')
{
	if (len > str.GetLength())
		return str + SFString(p, len-str.GetLength());

	return str.Left(len);
}

inline SFString padLeft(const SFString& str, SFInt32 len, char p=' ')
{
	if (len > str.GetLength())
		return SFString(p, len-str.GetLength()) + str;

	return str.Left(len);
}
#define padNum2(n) padLeft(asString((n)),2).Substitute(" ", "0")
#define padNum3(n) padLeft(asString((n)),3).Substitute(" ", "0")
#define padNum4(n) padLeft(asString((n)),4).Substitute(" ", "0")
#define padNum5(n) padLeft(asString((n)),5).Substitute(" ", "0")

inline SFString padCenter(const SFString& str, SFInt32 len, char p=' ')
{
	if (len > str.GetLength())
	{
		SFInt32 padding = (len-str.GetLength()) / 2;
		return SFString(p, padding) + str + SFString(p, padding);
	}

	return str.Left(len);
}

inline int sortByStringValueReverse(const void *rr1, const void *rr2)
{
	SFString n1 = *(SFString*)rr1;
	SFString n2 = *(SFString*)rr2;
	return (int)n2.ICompare(n1);
}

inline int sortByStringLength(const void *rr1, const void *rr2)
{
	SFString n1 = *(SFString*)rr1;
	SFString n2 = *(SFString*)rr2;
	return (int)(n1.GetLength() - n2.GetLength());
}

inline int sortByStringLengthReverse(const void *rr1, const void *rr2)
{
	SFString n1 = *(SFString*)rr1;
	SFString n2 = *(SFString*)rr2;
	return (int)(n2.GetLength() - n1.GetLength());
}

inline int compareStringLength(const void *rr1, const void *rr2)
{
	// return true of these are the same string
	return !sortByStringLength(rr1, rr2);
}

inline int sortByInt32(const void *rr1, const void *rr2)
{
	SFInt32 n1 = *(SFInt32*)rr1;
	SFInt32 n2 = *(SFInt32*)rr2;
	return (int)(n1 - n2);
}

inline int sortByInt32Reverse(const void *rr1, const void *rr2)
{
	SFInt32 n1 = *(SFInt32*)rr1;
	SFInt32 n2 = *(SFInt32*)rr2;
	return (int)(n2 - n1);
}

inline SFString encode(const SFString& in)
{
	SFString out = in;

	// This one must be first - do not move
	out.ReplaceAll ("%",  "%25");

	out.ReplaceAll ("\t", "%09");

	out.ReplaceAll (" ",  "%20");
	out.ReplaceAll ("!",  "%21");
	out.ReplaceAll ("\"", "%22");
	out.ReplaceAll ("#",  "%23");
	out.ReplaceAll ("$",  "%24");
	out.ReplaceAll ("&",  "%26");
	out.ReplaceAll ("'",  "%27");
	out.ReplaceAll ("(",  "%28");
	out.ReplaceAll (")",  "%29");
	out.ReplaceAll ("+",  "%2B");
	out.ReplaceAll (",",  "%2C");

	out.ReplaceAll (":",  "%3A");
	out.ReplaceAll (";",  "%3B");
	out.ReplaceAll ("<",  "%3C");
	out.ReplaceAll ("=",  "%3D");
	out.ReplaceAll (">",  "%3E");
	out.ReplaceAll ("?",  "%3F");

	out.ReplaceAll ("[",  "%5B");
	out.ReplaceAll ("\\", "%5C");
	out.ReplaceAll ("]",  "%5D");
	out.ReplaceAll ("^",  "%5E");

	out.ReplaceAll ("`",  "%60");

	out.ReplaceAll ("{",  "%7B");
	out.ReplaceAll ("|",  "%7C");
	out.ReplaceAll ("}",  "%7D");
	out.ReplaceAll ("~",  "%7E");
	return out;
}

inline SFString unencode(const SFString& in)
{
	SFString out = in;

	out.ReplaceAll ("%09", "\t");
	out.ReplaceAll ("%20", " " );
	out.ReplaceAll ("%21", "!" );
	out.ReplaceAll ("%22", "\"");
	out.ReplaceAll ("%23", "#" );
	out.ReplaceAll ("%24", "$" );
	out.ReplaceAll ("%25", "%" );
	out.ReplaceAll ("%26", "&" );
	out.ReplaceAll ("%27", "'" );
	out.ReplaceAll ("%28", "(" );
	out.ReplaceAll ("%29", ")" );
	out.ReplaceAll ("%2B", "+" );
	out.ReplaceAll ("%2C", "," );
	out.ReplaceAll ("%3A", ":" );
	out.ReplaceAll ("%3B", ";" );
	out.ReplaceAll ("%3C", "<" );
	out.ReplaceAll ("%3D", "=" );
	out.ReplaceAll ("%3E", ">" );
	out.ReplaceAll ("%3F", "?" );
	out.ReplaceAll ("%5B", "[" );
	out.ReplaceAll ("%5C", "\\");
	out.ReplaceAll ("%5D", "]" );
	out.ReplaceAll ("%5E", "^" );
	out.ReplaceAll ("%60", "`" );
	out.ReplaceAll ("%7B", "{" );
	out.ReplaceAll ("%7C", "|" );
	out.ReplaceAll ("%7D", "}" );
	out.ReplaceAll ("%7E", "~" );

	return out;
}

//----------------------------------------------------------------------------------------------------
inline SFString nextTokenClearQuotes(SFString& s, char c)
{
        SFString ret = nextTokenClear(s, c);
        ret.ReplaceAll("\"", "");
        return ret;
}

//----------------------------------------------------------------------------------------
extern SFString nextTokenClearReverse(SFString& str, char token);
extern SFString nextTokenClearDecrypt(SFString& contents, char token);

//-------------------------------------------------------------------------------------------------------------
extern SFString snagFieldClearEx   (      SFString& in, const SFString& tagName, const SFString& defVal=nullString, SFString *fieldVals=NULL);
extern SFString snagFieldClear     (      SFString& in, const SFString& tagName, const SFString& defVal=nullString);
extern SFString snagFieldClearStrip(      SFString& in, const SFString& tagName, const SFString& defVal=nullString, const SFString& stripWhat="\t\r\n ");
extern SFString snagField          (const SFString& in, const SFString& tagName, const SFString& defVal=nullString);
extern SFString snagFieldStrip     (const SFString& in, const SFString& tagName, const SFString& defVal=nullString, const SFString& stripWhat="\t\r\n ");

//-------------------------------------------------------------------------------------------------------------
extern SFInt32  snagFieldClearI    (      SFString& in, const SFString& tagName, const SFString& defVal=nullString);

extern SFString stripHTML          (const SFString& strIn);
extern SFString randomPassword     (void);

#define asciiQuote(s) SFString("\"") + s + SFString("\"")

//-------------------------------------------------------------------------------------------------------------
inline SFInt32  snagFieldClearI(SFString& in, const SFString& tagName, const SFString& defVal)
{
	return toLong(snagFieldClear(in, tagName, defVal));
}

//-------------------------------------------------------------------------------------------------------------
inline SFString toLower(const SFString& in)
{
	SFString ret = in;
	ret.MakeLower();
	return ret;
}

//-------------------------------------------------------------------------------------------------------------
inline SFString toUpper(const SFString& in)
{
	SFString ret = in;
	ret.MakeUpper();
	return ret;
}

//-------------------------------------------------------------------------------------------------------------
inline SFString toProper(const SFString& in)
{
	SFString ret = in;
	ret.MakeProper();
	ret.ReplaceAll("_", " ");
	return ret;
}

//---------------------------------------------------------------------------------------
inline SFInt32 stringFromBinaryBuffer(char *s, SFString& ret)
{
	ASSERT(s);
	SFInt32 len = *(SFInt32*)s;
	ASSERT(len < 4096);
	s += sizeof(SFInt32);

	ret = SFString(s, 0, len);
	return sizeof(SFInt32) + len;
}

#define STRIP_DROP(sss) ((sss % LoadStringGH("No Value") || sss % "select...") ? EMPTY : sss);
#define truncPad(str, size) (size == BAD_NUMBER ? str : padRight(str.Left(size), size))

//-----------------------------------------------------------------------------------------
inline SFString Trim(const SFString& str)
{
	return Strip(str, ' ');
}

//-----------------------------------------------------------------------------------------
inline SFString TrimAt(const SFString& str, const SFString& findStr)
{
	if (!str.Contains(findStr))
		return str;
	return str.Left(str.Find(findStr));
}

#define sendOSXNotification(mmsg, ttitle) SFos::doCommand(SFString("/usr/bin/terminal-notifier -message \"")+mmsg+"\" -title \""+ttitle+"\"")

//-----------------------------------------------------------------------------------------
extern SFString whichBadChar(const SFString& checkStr, const SFString& disallowStr=";| \n\t\r");
extern int sortByStringValue(const void *rr1, const void *rr2);
extern int compareStringValue(const void *rr1, const void *rr2);
#define getString(a) EMPTY

#endif
