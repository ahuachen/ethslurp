#ifndef _EDITABLE_OBJECT_BASE_H
#define _EDITABLE_OBJECT_BASE_H
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
class CFieldList;

//-------------------------------------------------------------------------
// CEditableObject - WARNING! -- if the format of this class is changed you
// have ruined backward compatibility.  Do not change the number of functions
// either because this might break the format of the underlying data.
//
// See peoplelib/editableobject.h for more info on this class.  This is done
// because we moved this file into the utillib tree at a late date.
//-------------------------------------------------------------------------
class CEditableObject
{
public:
				      CEditableObject     (void);
	virtual          ~CEditableObject     (void);

	virtual SFString  getFieldValueByName (const SFString& fieldName, const SFString& def=nullString) const;
	virtual	void      setFieldValueByName (const SFString& fieldName, const SFString& value);

	// do not remove the next line - the size of this object depends on this being here
	virtual SFString  unusedFunction      (SFInt32 id, const SFString& def) const { return def; }
	// do not remove the previous line - the size of this object depends on this being here

	virtual	SFInt32   getFieldID          (const SFString& fieldName) const = 0;

	virtual SFString  getFieldValueByID   (SFInt32 id,                const SFString& def=nullString) const = 0;
	virtual	void      setFieldValueByID   (SFInt32 id,                const SFString& value) = 0;

	virtual	SFString  toAscii             (const SFString& format) const = 0;
	virtual	void      fromAscii           (const SFString& in, const CVersion& version) = 0;
};
// CEditableObject - WARNING! -- if the format of this class is changed you
// have ruined backward compatibility.  Do not change the number of functions
// either because this might break the format of the underlying data.

//--------------------------------------------------------------------------------------------------------------
inline SFString CEditableObject::getFieldValueByName (const SFString& fieldName, const SFString& def) const
{
	return getFieldValueByID(getFieldID(fieldName), def);
}

//--------------------------------------------------------------------------------------------------------------
inline void CEditableObject::setFieldValueByName (const SFString& fieldName, const SFString& value)
{
	setFieldValueByID(getFieldID(fieldName), value);
}

//--------------------------------------------------------------------------------------------------------------
class CServerBase;
typedef SFString (*NEXTCHUNKFUNC) (const SFString& fieldIn, SFBool& forceShow, const void *data);

//--------------------------------------------------------------------------------------------------------------
SFString getNextChunk(SFString& fmtOut, NEXTCHUNKFUNC func, const void *data);

//--------------------------------------------------------------------------------------------------------------
class CServerBase
{
public:
	SFString m_theCookie;
	SFString m_finalScript;
	SFString m_helperStr;
	SFBool   m_isEditing;


					 CServerBase            (void);
    virtual       	~CServerBase            (void) {}

	virtual SFString getCookieString        (const SFString& name, const SFString& defVal=nullString) const;
	virtual SFInt32  getCookieInt32         (const SFString& name, SFInt32 defVal) const;

	virtual void     setCookieString        (const SFString& name, const SFString& value, SFInt32 nMonths=2);
	virtual void     setCookieInt32         (const SFString& name, SFInt32 value);

	virtual void     setSessionCookieString (const SFString& name, const SFString& value);
	virtual void     setSessionCookieInt32  (const SFString& name, SFInt32 value);

	virtual void     updateCookie           (const SFString& name, const SFString& newVal);
	virtual void     expireCookie           (const SFString& name);

		    SFString getTheCookie           (void) const;

    virtual SFString getStringBase          (const SFString& fieldName, const SFString& defVal) const;

private:
	        void     establishCookie        (void);
	virtual SFString formatCookie           (const SFString& name, const SFString& value) const = 0;
	virtual SFString parseCookie            (const SFString& name, const SFString& defVal) const = 0;
};

//---------------------------------------------------------------------------
inline SFInt32 CServerBase::getCookieInt32(const SFString& name, SFInt32 defVal) const
{
	return toLong(getCookieString(name, asString(defVal)));
}

//-------------------------------------------------------------------
inline void CServerBase::setCookieInt32(const SFString& name, SFInt32 value)
{
	setCookieString(name, asString(value));
}

//-------------------------------------------------------------------
inline void CServerBase::setSessionCookieString(const SFString& name, const SFString& value)
{
	setCookieString(name, value, 0);
}

//-------------------------------------------------------------------
inline void CServerBase::setSessionCookieInt32(const SFString& name, SFInt32 value)
{
	setSessionCookieString(name, asString(value));
}

//-------------------------------------------------------------------
inline void CServerBase::expireCookie(const SFString& name)
{
	SFString newCookie = "<script>ExpireCookie(" + asciiQuote(name) + ");</script>";
	if (!m_finalScript.Contains(newCookie))
		m_finalScript += newCookie;
	updateCookie(name, EMPTY);
}

#endif
