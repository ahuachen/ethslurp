#ifndef _FIELDDATA_H
#define _FIELDDATA_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "list.h"
#include "html.h"
#include "permission.h"

class CConfig;

#define MODE_EDIT    1
#define MODE_VIEW    2
#define TRASH        ((SFUint32)-10)

// Do not change these values
#define E_NO_EDIT		    (1<<12)
#define E_NO_REQUIRED		(1<<13)
#define E_NO_CUSTOMIZE		(1<<14)
#define E_NO_DEFAULT		(1<<15)
#define E_NO_ACCESS_LEVEL	(1<<16)

#define NOT_A_FIELD			(E_NO_DEFAULT | E_NO_ACCESS_LEVEL | E_NO_REQUIRED )

#define LONG_STRING_LEN     256
#define MID_STRING_LEN      128
#define PHONE_STRING_LEN    32
#define USERID_STRING_LEN   64

//-------------------------------------------------------------------------
class CFieldData
{
private:
	// saved in file
	SFString m_groupName;
	SFString m_fieldName;
	SFString m_prompt;
	SFString m_defStr;
	SFBool   m_required;
	SFBool   m_export;
	SFInt32  m_minPerms;
	SFBool   m_access;
	SFString m_helpText;

	// not saved in file
	SFInt32  m_fieldID;
	SFInt32  m_defInt;
	SFInt32  m_type;
	SFInt32  m_editSize;
	SFInt32  m_maxSize;
    SFString m_value;
	SFBool   m_resolved;
	SFString m_script;
	SFBool   m_disabled;
	SFString m_problem;
	SFBool   m_hidden;
	SFBool   m_noRow;
	SFBool   m_showHidden;
	SFInt32  m_extraInt;
	SFString m_extraStr;
	SELFUNC  m_selFunc;
	void    *m_dataPtr;

public:
	CFieldData (void);

	CFieldData (const CFieldData& data)
		{
			Copy(data);
		}

	CFieldData (const SFString& group, const SFString& fieldName, const SFString& prompt, const SFString& value, SFInt32 type, SFBool required, SFInt32 fieldID);
	CFieldData (const SFString& group, const SFString& fieldName, const SFString& prompt, SFInt32 value,         SFInt32 type, SFBool required, SFInt32 fieldID)
	{ *this = CFieldData(group, fieldName, prompt, asString(value), type, required, fieldID); }

	CFieldData (const SFString& in);

	CFieldData& operator=(const CFieldData& data)
		{
			Copy(data);
			return *this;
		}

	SFBool operator==(const CFieldData& data)
		{
			if ( m_groupName   != data.m_groupName) return FALSE;
			if ( m_fieldName   != data.m_fieldName) return FALSE;
			if ( m_prompt      != data.m_prompt   ) return FALSE;
			if ( m_defStr      != data.m_defStr   ) return FALSE;
			if ( m_required    != data.m_required ) return FALSE;
			if ( m_export      != data.m_export   ) return FALSE;
			if ( m_minPerms    != data.m_minPerms ) return FALSE;
			if ( m_access      != data.m_access   ) return FALSE;
			if ( m_helpText    != data.m_helpText ) return FALSE;
			if ( m_fieldID     != data.m_fieldID  ) return FALSE;

			ASSERT( m_defInt     == data.m_defInt   );
			ASSERT( m_type       == data.m_type     );
			ASSERT( m_editSize   == data.m_editSize );
			ASSERT( m_maxSize    == data.m_maxSize  );
			ASSERT( m_value      == data.m_value    );
			ASSERT( m_resolved   == data.m_resolved );
			ASSERT( m_disabled   == data.m_disabled );
			ASSERT( m_problem    == data.m_problem  );
			ASSERT( m_noRow      == data.m_noRow    );
			//ASSERT( m_showHidden == data.m_showHidden );
			//ASSERT( m_hidden   == data.m_hidden   );
			//ASSERT( m_script   == data.m_script   );
			//ASSERT( m_extraInt == data.m_extraInt );
			//ASSERT( m_extraStr == data.m_extraStr );
			//ASSERT( m_selFunc  == data.m_selFunc  );
			//ASSERT( m_dataPtr  == data.m_dataPtr  );

			return TRUE;
		}

	SFBool operator!=(const CFieldData& data)
		{
			return !operator==(data);
		}

	~CFieldData(void)
		{
		}

	//--------------------------------------------------------------------------------
	void   setSizes (void);

	SFString getFieldName   (void) const
	{
		if (m_fieldName.Contains('|'))
			return m_fieldName.Left(m_fieldName.Find("|"));
		return m_fieldName;
	}

	SFString getControl   (void) const;

	SFString getGroupName (void) const { return m_groupName; }
	SFString getPrompt    (void) const { return m_prompt;    }
	SFString getDefStr    (void) const { return m_defStr;    }
	SFInt32  getDefInt    (void) const { return m_defInt;    }
	SFBool   isRequired   (void) const { return m_required;  }
	SFInt32  getFieldType (void) const { return m_type;      }
	SFInt32  getMinPerms  (void) const { return m_minPerms;  }
	SFInt32  getEditSize  (void) const { return m_editSize;  }
	SFInt32  getMaxSize   (void) const { return m_maxSize;   }
	SFString getValue     (void) const { return m_value;     }
	SFInt32  getValueI    (void) const { return atoi((const char *)m_value); }
	SFBool   isExport     (void) const { return m_export;    }
	SFBool   getAccess    (void) const { return m_access;    }
	SFString getHelpText  (void) const { return m_helpText;  }
	SFInt32  getFieldID   (void) const { return m_fieldID;   }
	SFBool   isResolved   (void) const { return m_resolved;  }
	SFString getScript    (void) const { return m_script;    }
	SELFUNC  getSelFunc   (void) const { return m_selFunc;   }
	void    *getDataPtr   (void) const { return m_dataPtr;   }
	SFBool   getNoRow     (void) const { return m_noRow;     }
	SFBool   getShowHidden(void) const { return m_showHidden;}
	SFString getProblem   (void) const { return m_problem;   }
	SFBool   isDisabled   (void) const { return m_disabled;  }
	SFInt32  getExtraInt  (void) const { return m_extraInt;  }
	SFString getExtraStr  (void) const { return m_extraStr;  }
	SFBool   isHidden     (void) const { return m_hidden;    }

	SFBool   isFieldVisible(SFInt32 userPerm) const;

	void     setFieldName (const SFString& fieldName) { m_fieldName = fieldName;    }
	void     setGroupName (const SFString& groupName) { m_groupName = groupName;    }
	void     setPrompt    (const SFString& prompt)    { m_prompt    = prompt;       }
	void     setRequired  (SFBool required)           { m_required  = required;     }
	void     setMinPerms  (SFInt32 minPerm)           { m_minPerms  = minPerm;      }
	void     setValue     (const SFString& val)       { m_value     = val;          }
	void     setValue     (SFInt32 val)               { m_value     = asString(val);}
	void     setExport    (SFBool exp)                { m_export    = exp;          }
	void     setAccess    (SFBool priv)               { m_access    = priv;         }
	void     setHelpText  (const SFString& ht)        { m_helpText  = ht;           }
	void     setResolved  (SFBool on)                 { m_resolved  = on;           }
	void     setDisabled  (SFBool disable)            { m_disabled  = disable;      }
	void     setHidden    (SFBool hide)               { m_hidden    = hide;         }
	void     setSelFunc   (SELFUNC func)              { m_selFunc   = func;         }
	void     setDataPtr   (void *data)                { m_dataPtr   = data;         }
	void     setNoRow     (SFBool noRow)              { m_noRow     = noRow;        }
	void     setShowHidden(SFBool showHidden)         { m_showHidden= showHidden;   }
	void     setEditSize  (SFInt32 edit)              { m_editSize  = edit;         }
	void     setMaxsize   (SFInt32 max)               { m_maxSize   = max;          }
	void     setExtraInt  (SFInt32 ext)               { m_extraInt  = ext;          }
	void     setExtraStr  (const SFString& ext)       { m_extraStr  = ext;          }
	void     setScript    (const SFString& scr)       { m_script    = scr;          }
	void     setFieldID   (SFInt32 id);
	void     setFieldType (SFInt32 type)
		{
			m_type = type;
			setSizes();
		}

	void     setProblem   (const SFString& prob);
	void     setLabel(const SFString& label)                  
		{
			m_script = EMPTY;
			addScript("<label>" + label + "</label>");
		}
	void     addScript(const SFString& script)
		{
			m_script += script;
		}
	void     setDefStr    (const SFString& defStr)
		{
			m_defStr = defStr;
			m_defInt = atoi((const char *)m_defStr);
		}

	SFString toAscii      (void) const;

	SFBool Customize(const CConfig *config);

private:
	void Init(void)
		{
//	SFString m_groupName;
//	SFString m_fieldName;
//	SFString m_prompt;
//	SFString m_defStr;
//	SFString m_value;
			m_type       = 0;
			m_editSize   = 0;
			m_maxSize    = 0;
			m_defInt     = 0;
			m_required   = FALSE;
			m_export     = FALSE;
			m_minPerms   = PERMISSION_NONE; // every field is visible to every user by default
			m_access     = FALSE;
// SFString m_helpText;
			m_fieldID    = NOT_A_FIELD;
			m_resolved   = FALSE;
			m_disabled   = FALSE;
			m_hidden     = FALSE;
//	SFString m_script;
//  SFString m_problem;
			m_selFunc    = NULL;
			m_dataPtr    = NULL;
			m_noRow      = FALSE;
			m_showHidden = TRUE;
			m_extraInt   = 0;
//  SFString m_extraStr;
	}

	void Copy(const CFieldData& data)
		{
			m_groupName = data.m_groupName;
			m_fieldName = data.m_fieldName;
			m_type      = data.m_type;
			m_editSize  = data.m_editSize;
			m_maxSize   = data.m_maxSize;
			m_prompt    = data.m_prompt;
			m_defStr    = data.m_defStr;
			m_defInt    = data.m_defInt;
			m_required  = data.m_required;
			m_minPerms  = data.m_minPerms;
			m_value     = data.m_value;
			m_export    = data.m_export;
			m_access    = data.m_access;
			m_helpText  = data.m_helpText;
			m_fieldID   = data.m_fieldID;
			m_resolved  = data.m_resolved;
			m_script    = data.m_script;
			m_disabled  = data.m_disabled;
			m_problem   = data.m_problem;
			m_hidden    = data.m_hidden;
			m_selFunc   = data.m_selFunc;
			m_dataPtr   = data.m_dataPtr;
			m_noRow     = data.m_noRow;
			m_showHidden= data.m_showHidden;
			m_extraInt  = data.m_extraInt;
			m_extraStr  = data.m_extraStr;
		}
};

//-------------------------------------------------------------------------
class CFieldList : public SFList<CFieldData*>
{
private:
	SFString    m_group;

	// Hide these to force the use of the filtered accessors
	CFieldData *GetNext(LISTPOS& rPosition) const;
	CFieldData *GetHeadPosition(void) const;

public:
	                  CFieldList           (void) : SFList<CFieldData*>() {}
	      void        Release              (void) const;
	      LISTPOS     GetFirstItem         (const SFString& filter=nullString) const;
	      CFieldData *GetNextItem          (LISTPOS& rPosition) const;

	const CFieldData *getFieldByID         (SFInt32 id) const;
	const CFieldData *getFieldByName       (const SFString& name) const;
};

#undef E_PENDING // windows defines this but we need it for something more useful

#define E_NO_EDIT		  (1<<12)
#define E_NO_REQUIRED	  (1<<13)
#define E_NO_CUSTOMIZE	  (1<<14)
#define E_NO_DEFAULT	  (1<<15)
#define E_NO_ACCESS_LEVEL (1<<16)
#define E_NO_ROW          (1<<17)
#define E_371_BIT		  (1<<20) // field was added at version 3.7.1
#define E_RESERVED_1      (1<<21) // field was added at future upgrade
#define E_RESERVED_2      (1<<22) // field was added at future upgrade
#define E_RESERVED_3      (1<<23) // field was added at future upgrade
#define E_RESERVED_4      (1<<24) // field was added at future upgrade
#define TYPE_USER         (1<<25)
#define TYPE_GROUP        (1<<26)
#define TYPE_CONTACT      (1<<27)
#define TYPE_USER_453     (1<<28)

#define NOT_A_FIELD		  (E_NO_DEFAULT | E_NO_ACCESS_LEVEL | E_NO_REQUIRED )
#define E_EXPORT_ONLY     (E_NO_DEFAULT | E_NO_ACCESS_LEVEL | E_NO_REQUIRED | E_NO_EDIT)
#define E_INVISIBLE_FIELD (E_NO_DEFAULT | E_NO_ACCESS_LEVEL | E_NO_REQUIRED | E_NO_CUSTOMIZE | E_NO_EDIT )

typedef SFArrayBase<CFieldData> CFieldArray;

#define TS_LABEL            (1<<10)
#define TS_SORT             (1<<11)
#define TS_DATE             (1<<12)
#define TS_SEARCH           (1<<13)
#define TS_ARRAY            (1<<14)

//-------------------------------------------------------------------------
// Field types
#define T_NONE              (  0 | TS_LABEL  )
#define T_PROMPT            (  1 | TS_LABEL  )
#define T_LABEL             (  2 | TS_LABEL  )
#define T_BUTTON            (  3 | TS_LABEL  )

#define T_DATE              (  4 | TS_DATE   | TS_SORT)
#define T_TIME              (  5 | TS_DATE   | TS_SORT)

#define T_USERID            (  6 | TS_SEARCH | TS_SORT)
#define T_MULTILINE         (  7 | TS_SEARCH | TS_SORT)
#define T_DROPDOWN          (  8 | TS_SEARCH | TS_SORT)
#define T_PHONE             (  9 | TS_SEARCH | TS_SORT)
#define T_TEXT              ( 10 | TS_SEARCH | TS_SORT)
#define T_EMAIL             ( 11 | TS_SEARCH | TS_SORT)
#define T_HALFTEXT          ( 12 | TS_SEARCH | TS_SORT)
#define T_SHORTTEXT         ( 13 | TS_SEARCH | TS_SORT)
#define T_NUMBER            ( 14 | TS_SEARCH | TS_SORT)
#define T_NOTES             ( 15 | TS_SEARCH | TS_SORT)
#define T_ICONLIST          ( 16 | TS_SEARCH | TS_SORT)
#define T_ONOFF             ( 17 | TS_SEARCH | TS_SORT)
#define T_CHECKSRCH         ( 18 | TS_SEARCH | TS_SORT)
#define T_SELSEARCH         ( 19 | TS_SEARCH | TS_SORT)

#define T_COLOR             ( 20 )
#define T_SELECTION         ( 21 )
#define T_CHECKBOX          ( 22 )
#define T_RADIO             ( 23 )
#define T_PERMISSION        ( 24 )
#define T_PASSWORD          ( 25 )
#define T_LONGPASSWORD      ( 26 )
#define T_CONTROL           ( 27 )
#define T_SCRIPT            ( 28 )
#define T_FILEUPLOAD        ( 29 )

//-------------------------------------------------------------------------
inline SFBool isLabelType(SFInt32 type)
{
	return (type & TS_LABEL);
}

//-------------------------------------------------------------------------
inline SFBool isDateType(SFInt32 type)
{
	return (type & TS_DATE);
}

//-------------------------------------------------------------------------
inline SFBool isSearchable(SFInt32 type)
{
	return (type & TS_SEARCH);
}

//-------------------------------------------------------------------------
inline SFBool isSortable(SFInt32 type)
{
	return (type & TS_SORT);
}

//-------------------------------------------------------------------------
inline SFBool isCheckbox(SFInt32 type)
{
	return (type == T_CHECKSRCH || type == T_CHECKBOX);
}

//-------------------------------------------------------------------------
inline SFBool isSelection(SFInt32 type)
{
	return (type == T_SELSEARCH || type == T_SELECTION);
}

//-------------------------------------------------------------------------
#define FIELD_IDS                 0
#define FIELD_TYPES               1
#define FIELD_SIZES               2
#define FIELD_EDITSIZES           3

//-------------------------------------------------------------------------
// access levels (do not change these PUBLIC/PRIVATE are aliases for TRUE/FALSE in field->m_access
#define ACCESS_PUBLIC             (0x0)
#define ACCESS_PRIVATE            (0x1)
#define ACCESS_PRIVATEWHENMERGED  (0x2)
#define ACCESS_GROUPONLY          (0x4)

//-------------------------------------------------------------------------
// access levels
#define ALLOW_ALL                 0
#define ALLOW_LOGGED              1
#define ALLOW_OWNER               2
#define ALLOW_NONE                3

//-------------------------------------------------------------------------
// sort order
#define SORTORDER_REVCHRON        0
#define SORTORDER_CHRON           1

//-------------------------------------------------------------------------
// owner modification
#define OWNERCHANGE_NOONE         SFString("none")
#define OWNERCHANGE_ANYONE        SFString("anyone")
#define OWNERCHANGE_ADMINS        SFString("admins")
#define OWNERCHANGE_SUPER         SFString("super")

//--------------------------------------------------------
extern SFString getSelectString     (CSelectionData *data);
extern SFString getSelectString     (SELFUNC func, const SFString& name, const SFString& selected, SFInt32 disabled, const SFString& onChange, const void *dataPtr, SFInt32 extraInt);

extern SFString getRadioString      (const SFString& name, const SFString& label, SFInt32 val, SFBool checked, SFInt32 disabled=FALSE, const SFString& onChange=nullString);
extern SFString getCheckboxString   (const SFString& name, SFBool val, const SFString& prompt=nullString, const SFString& onClickStr=nullString, SFBool disabled=FALSE);

extern SFBool   onOffChoose         (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool   sortOrderChoose     (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);

extern SFBool   alignChoose         (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool   relSizeChoose       (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool   borderChoose        (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool   colorChoose         (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool   allowChangeChoose   (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);

extern SFBool   fontChoose          (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool   iconChoose          (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool   reminderChoose      (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool   permChoose          (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool   specPermChoose      (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool   durationChooseNoMax (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool   durationChooseMax   (SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool   prefixChoose		(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool   suffixChoose		(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool   regionChoose		(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool   countryChoose		(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);
extern SFBool   genderChoose		(SFInt32& nVals, SFAttribute *attrs, CSelectionData *data);

extern SFString getReminderString   (SFInt32 minutes);
extern SFString getTimeSelectString (const SFString& name, const SFTime& date, SFBool disabled, SFInt32 step);

//-------------------------------------------------------------------------
extern SFString ALTERNATE_FIELDMAP;
extern SFString ALTERNATE_FIELDMAP2;
extern SFString ALTERNATE_FIELDMAP3;

//-------------------------------------------------------------------------
inline void CFieldData::setFieldID(SFInt32 id)
{
	m_fieldID = id;
}

//-------------------------------------------------------------------------
typedef SFInt32 (*OTHER_ID_FUNC) (const SFString& token);
extern  void      setOtherIDFunc (OTHER_ID_FUNC func);

extern const char* IDS_WRD_RESET;

#endif
