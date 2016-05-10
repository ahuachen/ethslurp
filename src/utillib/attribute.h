#ifndef _ATTRIBUTE_H
#define _ATTRIBUTE_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

//-------------------------------------------------------------------------
// A simple name/value pair
//-------------------------------------------------------------------------
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
