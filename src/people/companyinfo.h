#ifndef _COMPANYINFO_H
#define _COMPANYINFO_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

class CCompanyInfo
{
private:
	SFString m_company;
	SFString m_position;
	SFString m_dept;
	SFString m_office;
	SFTime   m_doh;

public:
	CCompanyInfo(void)
		{
			Init();
		}

	CCompanyInfo(const CCompanyInfo& data)
		{
			Copy(data);
		}

	CCompanyInfo& operator=(const CCompanyInfo& data)
		{
			Copy(data);
			return *this;
		}

	SFString getCompany  (void) const { return m_company; }
	SFString getPosition (void) const { return m_position; }
	SFString getDept     (void) const { return m_dept; }
	SFString getOffice   (void) const { return m_office; }
	SFTime   getDOH      (void) const { return m_doh; }

	void setCompany  (const SFString& name)     { m_company = name; }
	void setPosition (const SFString& position) { m_position = position; }
	void setDept     (const SFString& dept)     { m_dept = dept; }
	void setOffice   (const SFString& office)   { m_office = office; }
	void setDOH      (const SFTime  & doh)      { m_doh = doh; }

private:
	void Init(void)
		{
//			m_company   = "";
//			m_position  = "";
//			m_dept      = "";
//			m_office    = "";
			m_doh       = earliestDate;
		}

	void Copy(const CCompanyInfo& data)
		{
			m_company   = data.m_company;
			m_position  = data.m_position;
			m_dept      = data.m_dept;
			m_office    = data.m_office;
			m_doh       = data.m_doh;
		}

	friend class CPerson;
};

#endif
