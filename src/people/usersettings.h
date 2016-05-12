#ifndef _USERSETTINGS_H
#define _USERSETTINGS_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

class CUserSettings
{
private:
	SFBool	 m_selfEdit;
	SFInt32  m_lowestPermission;
	SFInt32  m_highestPermission;
	SFInt32  m_defaultPermission;
	SFInt32  m_passwordAge;
	SFBool   m_useEmailAsID;
	SFBool   m_showDropdown;
	SFBool   m_passwordReminders;
	SFBool   m_allowGroupPerms;
	SFBool   m_singleContactDB;
	SFBool   m_allowDiffPerms;
	SFBool   m_showLastLogin;
	SFBool   m_useridRequired;
	SFBool   m_passwordRequired;
	SFBool   m_emailRequired;
	SFBool   m_nameRequired;
	SFBool   m_phoneRequired;
	SFBool   m_cellRequired;
	SFBool   m_faxRequired;
	SFBool   m_hintRequired;
	SFBool   m_resetPassword;
	SFBool   m_passwordNotSame;

public:
	CUserSettings(void)
		{
			Init();
		}

	CUserSettings(const CUserSettings& data)
		{
			Copy(data);
		}

	CUserSettings& operator=(const CUserSettings& data)
		{
			Copy(data);
			return *this;
		}

private:
	void Init(void)
		{
			m_selfEdit          = TRUE;
			m_lowestPermission  = PERMISSION_VIEW;
			m_highestPermission = PERMISSION_SUPER;
			m_defaultPermission = PERMISSION_EDIT;
			m_passwordAge       = 0;
			m_useEmailAsID      = FALSE;
			m_showDropdown      = TRUE;
			m_passwordReminders = TRUE;
			m_allowGroupPerms   = FALSE;
			m_singleContactDB   = TRUE;
			m_allowDiffPerms    = FALSE;
			m_showLastLogin     = TRUE;
			m_useridRequired    = TRUE;
			m_passwordRequired  = TRUE;
			m_emailRequired     = FALSE;
			m_nameRequired      = FALSE;
			m_phoneRequired     = FALSE;
			m_cellRequired      = FALSE;
			m_faxRequired       = FALSE;
			m_hintRequired      = FALSE;
			m_resetPassword     = FALSE;
			m_passwordNotSame   = TRUE;
		}

	void Copy(const CUserSettings& data)
		{
			m_selfEdit          = data.m_selfEdit;
			m_lowestPermission  = data.m_lowestPermission;
			m_highestPermission = data.m_highestPermission;
			m_defaultPermission = data.m_defaultPermission;
			m_passwordAge       = data.m_passwordAge;
			m_useEmailAsID      = data.m_useEmailAsID;
			m_showDropdown      = data.m_showDropdown;
			m_passwordReminders = data.m_passwordReminders;
			m_allowGroupPerms   = data.m_allowGroupPerms;
			m_singleContactDB   = data.m_singleContactDB;
			m_allowDiffPerms    = data.m_allowDiffPerms;
			m_showLastLogin     = data.m_showLastLogin;
			m_useridRequired    = data.m_useridRequired;
			m_passwordRequired  = data.m_passwordRequired;
			m_emailRequired     = data.m_emailRequired;
			m_nameRequired      = data.m_nameRequired;
			m_phoneRequired     = data.m_phoneRequired;
			m_cellRequired      = data.m_cellRequired;
			m_faxRequired       = data.m_faxRequired;
			m_hintRequired      = data.m_hintRequired;
			m_resetPassword     = data.m_resetPassword;
			m_passwordNotSame   = data.m_passwordNotSame;
		}

public:
	SFString toAscii              (SFBool prevVersion=FALSE) const;
	void     fromAscii            (const SFString& in);

	SFBool   getSelfEdit          (void) const { return m_selfEdit;          }
	SFInt32  getLowestPermission  (void) const { return m_lowestPermission;  }
	SFInt32  getHighestPermission (void) const { return m_highestPermission; }
	SFInt32  getDefaultPermission (void) const { return m_defaultPermission; }
	SFInt32  getPasswordAge       (void) const { return m_passwordAge;       }
	SFBool   getUseEmailAsID      (void) const { return m_useEmailAsID;      }
	SFBool   getShowDropdown      (void) const;
	SFBool   getPasswordReminders (void) const { return m_passwordReminders; }
	SFBool   getAllowGroupPerms   (void) const { return m_allowGroupPerms;   }
	SFBool   getSingleContactDB   (void) const { return m_singleContactDB;   }
	SFBool   getAllowDiffPerms    (void) const { return m_allowDiffPerms;    }
	SFBool   getShowLastLogin     (void) const { return m_showLastLogin;    }
	SFBool   getUseridReq         (void) const { return m_useridRequired;    }
	SFBool   getPasswordReq       (void) const { return m_passwordRequired;  }
	SFBool   getEmailReq          (void) const { return m_emailRequired;     }
	SFBool   getNameReq           (void) const { return m_nameRequired;      }
	SFBool   getPhoneReq          (void) const { return m_phoneRequired;     }
	SFBool   getCellReq           (void) const { return m_cellRequired;      }
	SFBool   getFaxReq            (void) const { return m_faxRequired;       }
	SFBool   getHintReq           (void) const { return m_hintRequired;      }
	SFBool   getResetPassword     (void) const { return m_resetPassword;     }
	SFBool   getPasswordNotSame   (void) const { return m_passwordNotSame;   }

	void setSelfEdit          (SFBool   selfedit)         { m_selfEdit          = selfedit;   }
	void setLowestPermission  (SFInt32  lowestpe)         { m_lowestPermission  = lowestpe;   }
	void setHighestPermission (SFInt32  highestp)         { m_highestPermission = highestp;   }
	void setDefaultPermission (SFInt32  defaultp)         { m_defaultPermission = defaultp;   }
	void setPasswordAge       (SFInt32  password)         { m_passwordAge       = password;   }
	void setUseEmailAsID      (SFBool   useemail)         { m_useEmailAsID      = useemail;   }
	void setShowDropdown      (SFBool   showdrop)         { m_showDropdown      = showdrop;   }
	void setPasswordReminders (SFBool   passremind)       { m_passwordReminders = passremind; }
	void setAllowGroupPerms   (SFBool   groupPerm)        { m_allowGroupPerms   = groupPerm;  }
	void setSingleContactDB   (SFBool   singleDB)         { m_singleContactDB   = singleDB;   }
	void setAllowDiffPerms    (SFBool   allowdif)         { m_allowDiffPerms    = allowdif;   }
	void setShowLastLogin     (SFBool   show)             { m_showLastLogin     = show;       }
	void setUseridReq         (SFBool   useridre)         { m_useridRequired    = useridre;   }
	void setPasswordReq       (SFBool   password)         { m_passwordRequired  = password;   }
	void setEmailReq          (SFBool   emailreq)         { m_emailRequired     = emailreq;   }
	void setNameReq           (SFBool   namereq)          { m_nameRequired      = namereq;    }
	void setPhoneReq          (SFBool   phonereq)         { m_phoneRequired     = phonereq;   }
	void setCellReq           (SFBool   cellreq)          { m_cellRequired      = cellreq;    }
	void setFaxReq            (SFBool   faxreq)           { m_faxRequired       = faxreq;     }
	void setHintReq           (SFBool   hintreq)          { m_hintRequired      = hintreq;    }
	void setResetPassword     (SFBool   reset)            { m_resetPassword     = reset;      }
	void setPasswordNotSame   (SFBool   notSame)          { m_passwordNotSame   = notSame;    }

	void setFieldValue(const CFieldData *field, const SFString& value);
};

#endif
