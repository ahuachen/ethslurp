#ifndef _POSTALADDRESS_H
#define _POSTALADDRESS_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

class CPostalInfo
{
private:
	SFString m_address1;
	SFString m_address2;
	SFString m_city;
	SFString m_region;
	SFString m_postalcode;
	SFString m_country;
	SFString m_phone;
	SFString m_fax;
	SFString m_cell;
	SFString m_pager;
	SFString m_webpage;
	SFString m_email;

public:
	CPostalInfo(void)
		{
			Init();
		}
	CPostalInfo(const CPostalInfo& data)
		{
			Copy(data);
		}
	CPostalInfo& operator=(const CPostalInfo& data)
		{
			Copy(data);
			return *this;
		}

	SFString getFullAddress (void) const;
	SFString getDownloadAddr(void) const;
	SFString getAddress1    (void) const { return m_address1; }
	SFString getAddress2    (void) const { return m_address2; }
	SFString getCity        (void) const { return m_city; }
	SFString getRegion      (void) const { return m_region; }
	SFString getPostalCode  (void) const { return m_postalcode; }
	SFString getLocale      (void) const;
	SFString getCountry     (void) const { return m_country; }
	SFString getPhone       (void) const { return m_phone; }
	SFString getFax         (void) const { return m_fax; }
	SFString getCell        (void) const { return m_cell; }
	SFString getPager       (void) const { return m_pager; }
	SFString getWebPage     (void) const { return m_webpage; }
	SFString getEmail       (void) const { return m_email; }

	void setAddress1        (const SFString& address1)   { m_address1 = address1; }
	void setAddress2        (const SFString& address2)   { m_address2 = address2; }
	void setCity            (const SFString& city)       { m_city = city; }
	void setRegion          (const SFString& region)     { m_region = region; }
	void setPostalCode      (const SFString& postalcode) { m_postalcode = postalcode; }
	void setCountry         (const SFString& country)    { m_country = country; }
	void setPhone           (const SFString& phone)      { m_phone = phone; }
	void setFax             (const SFString& fax)        { m_fax = fax; }
	void setCell            (const SFString& cell)       { m_cell = cell; }
	void setPager           (const SFString& pager)      { m_pager = pager; }
	void setWebPage         (const SFString& webpage)    { m_webpage = webpage; }
	void setEmail           (const SFString& email)      { m_email = email; }

private:
	void Init(void)
		{
//			m_address1   = "";
//			m_address2   = "";
//			m_city       = "";
//			m_region     = "";
//			m_postalcode = "";
//			m_country    = "";
//			m_phone      = "";
//			m_fax        = "";
//			m_cell       = "";
//			m_pager      = "";
//			m_webpage    = "";
//			m_email      = "";
		}

	void Copy(const CPostalInfo& data)
		{
			m_address1   = data.m_address1;
			m_address2   = data.m_address2;
			m_city       = data.m_city;
			m_region     = data.m_region;
			m_postalcode = data.m_postalcode;
			m_country    = data.m_country;
			m_phone      = data.m_phone;
			m_fax        = data.m_fax;
			m_cell       = data.m_cell;
			m_pager      = data.m_pager;
			m_webpage    = data.m_webpage;
			m_email      = data.m_email;
		}
};

#endif
