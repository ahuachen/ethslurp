#ifndef _PROTECTED_OBJECT_H
#define _PROTECTED_OBJECT_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------------------
class CPerson;

//-----------------------------------------------------------------------------------------
class CProtectedObject
{
private:
	SFString m_name;
	SFString m_group;
	SFString m_objectid;
	SFInt32  m_defPerm;
	SFInt32  m_minPerm;
	SFInt32  m_fallBack;
	SFBool   m_deleted;
	SFBool   m_hidden;

public:
	CProtectedObject(void)
		{
			Init();
		}
	CProtectedObject(const CProtectedObject& data)
		{
			Copy(data);
		}
	CProtectedObject& operator=(const CProtectedObject& data)
		{
			Copy(data);
			return *this;
		}

	SFString getName        (void) const { return m_name;     }
	SFString getGroup       (void) const { return m_group;    }
	SFString getObjectID    (void) const { return m_objectid; }
	SFInt32  getDefaultPerm (void) const { return m_defPerm;  }
	SFInt32  getMinPerm     (void) const { return m_minPerm;  }
	SFInt32  getFallback    (void) const { return m_fallBack; }
	SFBool   isDeleted      (void) const { return m_deleted;  }
	SFBool   isHidden       (void) const { return m_hidden;   }
	SFString getNameWithGrp (void) const;

	void     setName        (const SFString& name)  { m_name     = name;    }
	void     setGroup       (const SFString& group) { m_group    = group;   }
	void     setObjectID    (const SFString& loc)   { m_objectid = loc;     }
	void     setDefaultPerm (SFInt32 perm)          { m_defPerm  = perm;    }
	void     setMinPerm     (SFInt32 perm)          { m_minPerm  = perm;    }
	void     setFallback    (SFInt32 perm)          { m_fallBack = perm;    }
	void     setDeleted     (SFBool del)            { m_deleted  = del;     }
	void     setHidden      (SFBool hide)           { m_hidden   = hide;    }

	SFString toAscii(void) const
		{
			return
				m_name               + ";" + m_objectid           + ";" + asString(m_defPerm)  + ";" + 
				asString(m_minPerm)  + ";" + asString(m_fallBack) + ";" + asString(m_deleted)  + ";" + 
				asString(m_hidden)   + ";" + m_group;
		}

private:
	void Init(void)
		{
//			m_name       = EMPTY;
//			m_group      = EMPTY;
//			m_objectid   = EMPTY;
			m_defPerm    = PERMISSION_VIEW;
			m_minPerm    = PERMISSION_NONE;
			m_fallBack   = PERMISSION_VIEW;
			m_deleted    = FALSE;
			m_hidden     = FALSE;
		}

	void Copy(const CProtectedObject& data)
		{
			m_name     = data.m_name;
			m_group    = data.m_group;
			m_objectid = data.m_objectid;
			m_defPerm  = data.m_defPerm;
			m_minPerm  = data.m_minPerm;
			m_fallBack = data.m_fallBack;
			m_deleted  = data.m_deleted;
			m_hidden   = data.m_hidden;
		}
};

extern int sortByObjectName(const void *rr1, const void *rr2);

#define DELETED_ALSO  (1<<1)
#define HIDDEN_ALSO   (1<<2)

#endif
