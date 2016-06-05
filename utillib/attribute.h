#ifndef _ATTRIBUTE_H
#define _ATTRIBUTE_H
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
class SFAttribute
{
protected:

	SFString    m_Name;
	SFString    m_Value;

public:
	            SFAttribute (void);
	            SFAttribute (const SFAttribute& attr);
	            SFAttribute (const SFString& n, const SFString& v);

//	           ~SFAttribute (void)

	SFAttribute& operator=  (const SFAttribute& attr);

	SFBool      operator== (const SFAttribute& attr) const
		{
			if (m_Name  != attr.m_Name) return FALSE;
			if (m_Value != attr.m_Value) return FALSE;
			return TRUE;
		}

	SFBool      operator!= (const SFAttribute& attr) const
		{
			return !operator==(attr);
		}

	SFString    getName    (void) const;
	SFString    getValue   (void) const;

	void        setName    (const SFString& n);
	void        setValue   (const SFString& v);

	void        set        (const SFString& n, const SFString& v);
};

inline SFString SFAttribute::getName(void) const
{
	return m_Name;
}

inline SFString SFAttribute::getValue(void) const
{
	return m_Value;
}

inline void SFAttribute::setName(const SFString& n)
{
	m_Name = n;
}

inline void SFAttribute::setValue(const SFString& v)
{
	m_Value = v;
}

inline void SFAttribute::set(const SFString& n, const SFString& v)
{
	m_Name  = n;
	m_Value = v;
}

extern int sortByAttributeName        (const void *rr1, const void *rr2);
extern int sortByAttributeValue       (const void *rr1, const void *rr2);
extern int sortByAttributeNameReverse (const void *rr1, const void *rr2);
extern int sortByAttributeValueReverse(const void *rr1, const void *rr2);
extern int sortByAttributeIntReverse  (const void *rr1, const void *rr2);

#endif
