#ifndef _VERSION_H
#define _VERSION_H
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
// Applications must define these items
#define FILE_MAJOR_VERSION ((SFInt32)0)
#define FILE_MINOR_VERSION ((SFInt32)9)
#define FILE_BUILD_VERSION ((SFInt32)0)

// Syntactic sugar
#define VERSION_MAJOR  FILE_MAJOR_VERSION
#define VERSION_MINOR  FILE_MINOR_VERSION
#define VERSION_BUILD  FILE_BUILD_VERSION

//-------------------------------------------------------------------------
// Stores version info and does simple calculations
//-------------------------------------------------------------------------
class CVersion
{
private:
	SFInt32   m_Major;
	SFInt32   m_Minor;
	SFInt32   m_Build;

public:
	          CVersion     (void);
	          CVersion     (const CVersion& version);

//	         ~CVersion     (void);

	CVersion& operator=    (const CVersion& version);

	void      Reset        (void);
	SFBool    isSet        (void) const;

	void      setVersionStr(const SFString& line);
	void      setVersion   (SFInt32 m1, SFInt32 m2, SFInt32 m3);

	SFBool    isVersion    (SFInt32 qMaj, SFInt32 qMin, SFInt32 qBuild) const;
	SFBool    eqLater      (SFInt32 qMaj, SFInt32 qMin, SFInt32 qBuild) const;
	SFBool    later        (SFInt32 qMaj, SFInt32 qMin, SFInt32 qBuild) const;
	SFBool    eqEarlier    (SFInt32 qMaj, SFInt32 qMin, SFInt32 qBuild) const;
	SFBool    earlier      (SFInt32 qMaj, SFInt32 qMin, SFInt32 qBuild) const;

	SFBool    isVersion    (const CVersion& version) const;
	SFBool    eqLater      (const CVersion& version) const;
	SFBool    later        (const CVersion& version) const;
	SFBool    eqEarlier    (const CVersion& version) const;
	SFBool    earlier      (const CVersion& version) const;

	SFString  toString     (char sep=':') const;
	SFInt32   toInt32      (void) const;
};

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline void CVersion::Reset(void)
{
	m_Major = m_Minor = m_Build = FALSE;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool CVersion::isSet(void) const
{
	return !isVersion(0,0,0);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline void CVersion::setVersion(SFInt32 m1, SFInt32 m2, SFInt32 m3)
{
	m_Major = m1;
	m_Minor = m2;
	m_Build = m3;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool CVersion::isVersion(SFInt32 qMaj, SFInt32 qMin, SFInt32 qBuild) const
{
	return (qMaj == m_Major && qMin == m_Minor && qBuild == m_Build);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool CVersion::isVersion(const CVersion& version) const
{
	ASSERT(isSet());
	return isVersion(version.m_Major, version.m_Minor, version.m_Build);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool CVersion::eqLater(SFInt32 qMaj, SFInt32 qMin, SFInt32 qBuild) const
{
	ASSERT(isSet());
	ASSERT(qMaj    < 100 && qMin    < 100 && qBuild  < 100);
	ASSERT(m_Major < 100 && m_Minor < 100 && m_Build < 100);

	SFInt32 q = qMaj    * 10000 + qMin    * 100 + qBuild;
	SFInt32 t = m_Major * 10000 + m_Minor * 100 + m_Build;

	return (t >= q);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool CVersion::eqLater(const CVersion& version) const
{
	return eqLater(version.m_Major, version.m_Minor, version.m_Build);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool CVersion::later(SFInt32 qMaj, SFInt32 qMin, SFInt32 qBuild) const
{
	ASSERT(isSet());
	ASSERT(qMaj    < 100 && qMin    < 100 && qBuild  < 100);
	ASSERT(m_Major < 100 && m_Minor < 100 && m_Build < 100);

	SFInt32 q = qMaj    * 10000 + qMin    * 100 + qBuild;
	SFInt32 t = m_Major * 10000 + m_Minor * 100 + m_Build;

	return (t > q);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool CVersion::later(const CVersion& version) const
{
	return later(version.m_Major, version.m_Minor, version.m_Build);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool CVersion::eqEarlier(SFInt32 qMaj, SFInt32 qMin, SFInt32 qBuild) const
{
	return !later(qMaj, qMin, qBuild);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool CVersion::eqEarlier(const CVersion& version) const
{
	return !later(version);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool CVersion::earlier(SFInt32 qMaj, SFInt32 qMin, SFInt32 qBuild) const
{
	return !eqLater(qMaj, qMin, qBuild);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline SFBool CVersion::earlier(const CVersion& version) const
{
	return !eqLater(version);
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline CVersion laterOf(const CVersion& one, const CVersion& two)
{
	if (one.later(two))
		return one;
	else if (two.later(one))
		return two;
	return one;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
inline CVersion earlierOf(const CVersion& one, const CVersion& two)
{
	if (one.earlier(two))
		return one;
	else if (two.earlier(one))
		return two;
	return one;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
extern SFString getVersionString(char sep=':', SFBool incBuild=FALSE);
extern void     getCompileDate      (SFInt32& day, SFInt32& month, SFInt32& year, SFInt32& hour, SFInt32& min, SFInt32& sec);
extern void     getCompileDate_Base (const SFString& date, const SFString& time, SFInt32& day, SFInt32& month, SFInt32& year, SFInt32& hour, SFInt32& min, SFInt32& sec);
extern SFString getCompileStamp     (const SFString& date, const SFString& time);

#endif
