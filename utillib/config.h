#ifndef _CONFIG_H
#define _CONFIG_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "attribute.h"
#include "database.h"
#include "list.h"

#define MAX_SERVER_VAR_LEN 4096

//-------------------------------------------------------------------------
// Config data are name/value pairs...
//-------------------------------------------------------------------------
class SFKey : public SFAttribute
{
private:

	SFBool m_commented;
	SFBool m_deleted;

public:
	SFKey()
		{
			m_commented = FALSE;
			m_deleted   = FALSE;
		}

	SFKey(const SFKey& key)
		: SFAttribute(key.m_Name, key.m_Value)
		{
			m_commented = key.m_commented;
			m_deleted   = key.m_deleted;
		}

	SFKey& operator=(const SFKey& key)
		{
			SFAttribute::operator=(key);
			m_commented = key.m_commented;
			m_deleted   = key.m_deleted;
			return *this;
		}

	SFBool isCommented  (void) const       { return m_commented; }
	SFBool isDeleted    (void) const       { return m_deleted; }

	void   setCommented (SFBool commented) { m_commented = commented; }
	void   setDeleted   (SFBool del)       { m_deleted   = del; }

	SFString ToString(void) const;
};
typedef SFList<SFKey*> SFKeyList;

inline SFBool shouldRemove(SFKey *key, const SFString& mask)
{
	if (mask.IsEmpty())
		return TRUE;
		
	if (!mask.Contains("*"))
		return key->getName() == mask;
		
	SFString m = mask;
	m.ReplaceAll("*", "");
	return key->getName().Contains(m);
}

//-------------------------------------------------------------------------
// ... hierarchically sorted by groups
//-------------------------------------------------------------------------
class SFGroup
{
private:
	SFString m_name;
	SFBool   m_commented;
	SFBool   m_deleted;
	SFBool m_temp; // may be added to config file in memory but is not written to disc

public:
	SFKeyList m_keys;

public:
	SFGroup(void) { Clear(); }
	SFGroup(const SFGroup& group)
		{
			Clear();
			Copy(group);
		}

	~SFGroup(void) { Clear(); }

	SFGroup& operator=(const SFGroup& group)
		{
			Clear();
			Copy(group);
			return *this;
		}

	SFString getName      (void) const            { return m_name; }
	SFBool   isCommented  (void) const            { return m_commented; }
	SFBool   isDeleted    (void) const            { return m_deleted; }
	SFBool   isTemp       (void) const            { return m_temp; }
	SFBool   isEmpty      (void) const;
	SFBool   isAllComments(void) const;

	void     setName      (const SFString& name) { m_name      = name; }
	void     setCommented (SFBool commented)     { m_commented = commented; }
	void     setDeleted   (SFBool deleted)       { m_deleted   = deleted; }
	void     setTemp      (SFBool t)             { m_temp      = t; }

	void     removeAllComments(void);

	SFKey   *findKey      (const SFString& keyName, SFBool incDeleted=FALSE) const;

	//----------------------------------------------------------------------------------
	void ClearKeys(const SFString& mask=nullString)
		{
			SFKeyList working;
			
			LISTPOS kPos = m_keys.GetHeadPosition();
			while (kPos)
			{
				SFKey *key = m_keys.GetNext(kPos);
				if (! shouldRemove(key, mask))
					working.AddTail(key);
				else
					delete key;
			}
			m_keys.RemoveAll();

			// Copy them back
			kPos = working.GetHeadPosition();
			while (kPos)
				m_keys.AddTail(working.GetNext(kPos));

			working.RemoveAll();
		}

	//----------------------------------------------------------------------------------
	SFKey *addKey(const SFString& keyName, const SFString& val, SFBool commented);

	SFString ToString(void) const;

private:
	void Clear(void)
		{
			m_name      = EMPTY;
			m_commented = FALSE;
			m_deleted   = FALSE;
			m_temp      = FALSE;
			ClearKeys();
		}

	void Copy(const SFGroup& group)
		{
			m_name      = group.m_name;
			m_commented = group.m_commented;
			m_deleted   = group.m_deleted;
			m_temp      = group.m_temp;
			
			ASSERT(!m_keys.GetHeadPosition()); // else we drop memory

			LISTPOS kPos = group.m_keys.GetHeadPosition();
			while (kPos)
			{
				SFKey *orig = group.m_keys.GetNext(kPos);
				ASSERT(orig);
				SFKey *copy = new SFKey(*orig);
				m_keys.AddTail(copy);
			}
		}

};
typedef SFList<SFGroup*> SFGroupList;

class CConfig;
typedef void (*DEFAULT_REMOVEFUNC)(CConfig *config, const void *data);

//-------------------------------------------------------------------------
// Config file are hierarchical groups (SFGroups) of name/value pairs (SFKeys)
//-------------------------------------------------------------------------
class CConfig : public CSharedResource
{
public:
	SFGroupList         m_groups;
	DEFAULT_REMOVEFUNC  m_removeFunc;
	const void         *m_removeData;

public:
	CConfig(DEFAULT_REMOVEFUNC func, const void *data=NULL);
	~CConfig(void) { Clear(); }

	SFBool loadFile(const SFString& filename);
	SFBool saveAndReload(void);

	SFBool keyExists(const SFString& group, const SFString& key)
	{
		SFGroup *grp = findGroup(group);
		if (grp)
			return (SFBool)(grp->findKey(key) != NULL);
		return FALSE;
	}
	
	SFGroup *findGroup(const SFString& group) const
		{
			LISTPOS gPos = m_groups.GetHeadPosition();
			while (gPos)
			{
				SFGroup *grp = m_groups.GetNext(gPos);
				if (!grp->isDeleted() && grp->getName() == group)
					return grp;
			}
			return NULL;
		}

	SFKey *findKey(const SFString& group, const SFString& key) const
		{
			SFGroup *grp = findGroup(group);
			if (grp)
				return grp->findKey(key);
			return NULL;
		}

	SFGroup *addGroup(const SFString& group, SFBool commented)
		{
			ASSERT(!findGroup(group));
			SFGroup *newGroup = new SFGroup;
			newGroup->setCommented(commented);
			newGroup->setName(group);
			m_groups.AddTail(newGroup);
			return newGroup;
		}

	SFKey *addKey(const SFString& group, const SFString& key, const SFString& val, SFBool commented)
		{
			SFGroup *grp = findGroup(group);
			ASSERT(grp);
			return grp->addKey(key, val, commented);
		}

	SFBool   getVersion (CVersion& version) const;
	CVersion getVersion (void) const;

	SFBool copyGroup     (const SFString& newName, SFGroup *fromGroup);
	SFBool copyKey       (const SFString& newName, SFGroup *fromGroup, const SFString& keyname);
	void   deleteGroup   (const SFString& groupName);
	void   removeGroup   (const SFString& groupName);
	void   deleteKey     (const SFString& groupName, const SFString& keyName);
	void   unDeleteKey   (const SFString& groupName, const SFString& keyName);
	void   makeGroupTemp (const SFString& groupName);
	void   negateKey     (const SFString& group, const SFString& name);
	void   renameKey     (const SFString& group, const SFString& oldName, const SFString& newName);
	void   moveKey       (const SFString& oldGroup, const SFString& oldName, const SFString& newGroup, const SFString& newName, CConfig *dest=NULL);
	void   moveKey       (const SFString& in, CConfig *config2=NULL, SFBool negate=FALSE);
	void   moveGroup     (const SFString& oldName, const SFString& newName, CConfig *dest);
	void   moveGroup     (const SFString& oldName, CConfig *dest)           { moveGroup(oldName, oldName, dest); }
	void   moveGroup     (const SFString& oldName, const SFString& newName) { moveGroup(oldName, newName, this); }
	void   deleteDefault (const SFString& group, const SFString& key, const SFString& def);
	void   deleteDefault (const SFString& group, const SFString& key, SFInt32 def) { deleteDefault(group, key, asString(def)); }
	SFBool groupHidden   (const SFString& group) const;

public:
	SFString GetProfileStringGH(const SFString& group, const SFString& key, const SFString& def) const;
	SFInt32  GetProfileIntGH   (const SFString& group, const SFString& key, SFInt32 def) const
		{
			SFString ret = GetProfileStringGH(group, key, asString(def));
			return atoi((const char *)ret);
		}
	SFBool   GetProfileBoolGH(const SFString& group, const SFString& key, SFInt32 def) const
		{
			SFString ret = GetProfileStringGH(group, key, asString(def));
			if (ret % "true")  return TRUE;
			if (ret % "false") return FALSE;
			return atoi((const char *)ret);
		}

	void SetProfileString(const SFString& group, const SFString& key, const SFString& value)
		{
			SFBool commented = key.startsWith(';');

			SFGroup *grp = findGroup(group);
			if (!grp)
			{
				addGroup(group, FALSE);
				addKey(group, key, value, commented);
			} else
			{
				SFKey *k = grp->findKey(key);
				if (k)
				{
					k->setCommented(commented);
					k->setValue(value);
				} else
				{
					addKey(group, key, value, commented);
				}
			}
		}

	void SetProfileInt(const SFString& group, const SFString& key, SFInt32 value)
		{
			SetProfileString(group, key, asString(value));
		}

	void     mergeConfig(CConfig *configIn);
	SFBool   writeFile  (const SFString& version);
	SFString ToString   (void) const;

private:
			 CConfig   (void);
	         CConfig   (const CConfig& config);
	CConfig& operator= (const CConfig& config);
	void Clear(void)
		{
			LISTPOS gPos = m_groups.GetHeadPosition();
			while (gPos)
			{
				SFGroup *group = m_groups.GetNext(gPos);
				delete group;
			}
			m_groups.RemoveAll();
		}
};

//-------------------------------------------------------------------------
class CConfigReadOnly : public CConfig
{
public:
	CConfigReadOnly(void) : CConfig(NULL, NULL) {}
};

extern SFString getValueFromConfigFile (const SFString& filename, const SFString& groupName, const SFString& stringName, const SFString& def);
extern SFString getPathFromConfigFile  (const SFString& filename, const SFString& stringName, const SFString& def);

//---------------------------------------------------------------------------------------
inline SFBool CConfig::groupHidden(const SFString& group) const
{
	return GetProfileIntGH("HIDDEN", toLower(group), FALSE);
}
#endif
