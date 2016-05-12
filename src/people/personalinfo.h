#ifndef _PERSONALINFO_H
#define _PERSONALINFO_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

class CPersonalInfo
{
private:
	SFString m_spouse;
	SFString m_children;
	SFString m_gender;
	SFTime   m_birthdate;
	SFTime   m_anniversary;

public:
	CPersonalInfo(void)
		{
			Init();
		}

	CPersonalInfo(const CPersonalInfo& data)
		{
			Copy(data);
		}

	CPersonalInfo& operator=(const CPersonalInfo& data)
		{
			Copy(data);
			return *this;
		}

	SFString getSpouse      (void) const { return m_spouse; }
	SFString getChildren    (void) const { return m_children; }
	SFString getGender      (void) const { return m_gender; }
	SFTime   getBirthdate   (void) const { return m_birthdate; }
	SFTime   getAnniversary (void) const { return m_anniversary; }

	void setSpouse      (const SFString& spouse)      { m_spouse = spouse; }
	void setChildren    (const SFString& children)    { m_children = children; }
	void setGender      (const SFString& gender)      { m_gender = gender; }
	void setBirthdate   (const SFTime  & birthdate)   { m_birthdate = birthdate; }
	void setAnniversary (const SFTime  & anniversary) { m_anniversary = anniversary; }

private:
	void Init(void)
		{
//			m_spouse      = "";
//			m_children    = "";
//			m_gender      = "";
			m_birthdate   = earliestDate;
			m_anniversary = earliestDate;
		}

	void Copy(const CPersonalInfo& data)
		{
			m_spouse      = data.m_spouse;
			m_children    = data.m_children;
			m_gender      = data.m_gender;
			m_birthdate   = data.m_birthdate;
			m_anniversary = data.m_anniversary;
		}

	friend class CPerson;
};

#endif
