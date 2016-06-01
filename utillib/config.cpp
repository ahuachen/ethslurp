/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "sfos.h"
#include "config.h"
#include "exportcontext.h"

CConfig::CConfig(DEFAULT_REMOVEFUNC func, const void *data)
{
	m_removeFunc = func;
	m_removeData = data;
}

CVersion CConfig::getVersion(void) const
{
	CVersion version;
	getVersion(version);
	return version;
}

SFBool CConfig::getVersion(CVersion& version) const
{
	version.Reset();

	SFBool ret = FALSE;
	SFGroup *group = findGroup("CONFIGFILE");
	if (group)
	{
		// newer versions of this file have the 'file version= line as does the user.dat file
		LISTPOS kPos = group->m_keys.GetHeadPosition();
		while (kPos)
		{
			SFKey *key = group->m_keys.GetNext(kPos);
			if (!version.isSet())
				version.setVersionStr(key->getName());
			if (version.isSet())
				return TRUE;
		}
	}

	if (SFos::fileExists(m_filename))
		version.setVersion(1,0,1);
	else
		version.setVersion(1,0,0);

	return ret;
}

SFBool CConfig::copyGroup(const SFString& newName, SFGroup *fromGroup)
{
	if (!fromGroup)
		return FALSE;

	ASSERT(!fromGroup->isDeleted());
	LISTPOS kPos = fromGroup->m_keys.GetHeadPosition();
	while (kPos)
	{
		SFKey *fromKey = fromGroup->m_keys.GetNext(kPos);
		ASSERT(fromKey);
		SetProfileString(newName, fromKey->getName(), fromKey->getValue()); // Adds
	}
	return TRUE;
}

SFBool CConfig::copyKey(const SFString& newName, SFGroup *fromGroup, const SFString& keyName)
{
	if (fromGroup)
	{
		SFKey *k = fromGroup->findKey(keyName);
		if (k)
		{
			SetProfileString(fromGroup->getName(), newName, k->getValue());
			return TRUE;
		}
	}
	return FALSE;
}

SFKey *SFGroup::addKey(const SFString& keyName, const SFString& val, SFBool commented)
{
	ASSERT(!findKey(keyName)||commented);
	SFKey *key = new SFKey;
	key->setCommented(commented);
	key->setName(keyName);
	key->setValue(val);
	m_keys.AddTail(key);
	return key;
}

void CConfig::deleteGroup(const SFString& groupName)
{
	SFGroup *group = findGroup(groupName);
	if (group)
		group->setDeleted(TRUE);
}

void CConfig::removeGroup(const SFString& groupName)
{
	SFGroup *group = findGroup(groupName);
	if (group)
	{
		group->setName("REMOVED_"+asString(RandomValue(1,1000)));
		group->setDeleted(TRUE);
	}
}

void CConfig::makeGroupTemp(const SFString& groupName)
{
	SFGroup *group = findGroup(groupName);
	if (group)
		group->setTemp(TRUE);
}

void CConfig::deleteKey(const SFString& groupName, const SFString& keyName)
{
	SFGroup *group = findGroup(groupName);
	if (group)
	{
		SFKey *key = group->findKey(keyName);
		if (key)
			key->setDeleted(TRUE);
	}
}

void CConfig::unDeleteKey(const SFString& groupName, const SFString& keyName)
{
	SFGroup *group = findGroup(groupName);
	if (group)
	{
		SFKey *key = group->findKey(keyName);
		if (key)
			key->setDeleted(FALSE);
	}
}

static void addVersionGroup(CConfig *cfg, const SFString& version)
{
	// Let's not make it worse by creating another one - this file is messed up so let them call for help
	if (cfg->findGroup("CONFIGFILE"))
		return;
		
	cfg->WriteLine("[CONFIGFILE]\n");
	cfg->WriteLine(";------------------------------------------------------------------------------\n");
	cfg->WriteLine("; Copyright (c) by Great Hill Corporation. All Rights Reserved.\n");
	cfg->WriteLine("; file version=" + version + "\n");
	cfg->WriteLine(";------------------------------------------------------------------------------\n");
}

SFBool CConfig::writeFile(const SFString& version)
{
	if (m_removeFunc)
		(m_removeFunc)(this, m_removeData);

	CVersion tVersion;
	
	if (Lock(m_filename, asciiWriteCreate, LOCK_CREATE)) // wait for lock, delete existing contents, open for writing
	{
		ASSERT(isOpen());

		Seek(0, SEEK_SET);

		if (!getVersion(tVersion)) // Add version group for future upgrades if not found
			addVersionGroup(this, version);

		SFBool first = TRUE;
		
		LISTPOS gPos = m_groups.GetHeadPosition();
		while (gPos)
		{
			CStringExportContext ctx;

			SFGroup *group = m_groups.GetNext(gPos);
			ASSERT(group);
			if (!group->isDeleted() && !group->isTemp() && !group->isEmpty())
			{
				if (!first)
					ctx << "\n";
				first = FALSE;

				if (group->isCommented())
					ctx << ";";
				ctx << "[" << group->getName() << "]\n";

				LISTPOS kPos = group->m_keys.GetHeadPosition();
				while (kPos)
				{
					SFKey *key = group->m_keys.GetNext(kPos);
					ASSERT(key);
					if (!key->isDeleted())
					{
						SFString name = key->getName();
						SFString equals = "=";
						SFString val = key->getValue();
						if (key->isCommented())
						{
							name = ";" + name;
							if (val.IsEmpty())
								equals="";
						
							if (name.Find("Version 1.0.1") != -1 || 
									name.Find("file version") != -1 ||
									name.Find("file_version") != -1)
								name = "; file version=" + version;
						}

						// hack warning: we used to store a Windows specific code - turn it into valid HTML
						val.ReplaceAll("�",    "&#169;");
						val.ReplaceAll("\xa9", "&#169;");

						ctx << name << equals << val << "\n";
					}
				}

				WriteLine(ctx.str);
				ctx.str = "";
			}
		}
		Release();
	} else
	{
		// could not open file?
		LockFailure();
		return FALSE;
	}

	return TRUE;
}

//--------------------------------------------------------------------------------
SFString CConfig::ToString(void) const
{
	CStringExportContext ctx;

	LISTPOS gPos = m_groups.GetHeadPosition();
	while (gPos)
	{
		SFGroup *group = m_groups.GetNext(gPos);
		if (!group->isDeleted() && !group->isTemp())
		{
			// the first time trough don't skip a line, otherwise yes
			if (!ctx.str.IsEmpty())
				ctx << "\n";
			ctx << group->ToString();
		}
	}

	return ctx.str;
}

//--------------------------------------------------------------------------------
SFBool SFGroup::isEmpty(void) const
{
	LISTPOS kPos = m_keys.GetHeadPosition();
	while (kPos)
	{
		SFKey *key = m_keys.GetNext(kPos);
		if (!key->isDeleted())
			return FALSE;
	}
	return TRUE;
}

//--------------------------------------------------------------------------------
void SFGroup::removeAllComments(void)
{
	LISTPOS kPos = m_keys.GetHeadPosition();
	while (kPos)
	{
		SFKey *key = m_keys.GetNext(kPos);
		if (key->isCommented())
			key->setDeleted(TRUE);
	}
}

//--------------------------------------------------------------------------------
SFBool SFGroup::isAllComments(void) const
{
	LISTPOS kPos = m_keys.GetHeadPosition();
	while (kPos)
	{
		SFKey *key = m_keys.GetNext(kPos);
		if (!key->isDeleted() && !key->isCommented())
			return FALSE;
	}
	return TRUE;
}

//--------------------------------------------------------------------------------
SFString SFGroup::ToString(void) const
{
	CStringExportContext ctx;

	if (isCommented())
		ctx << ";";
	ctx << "[" << getName() << "]" << "\n";

	LISTPOS kPos = m_keys.GetHeadPosition();
	while (kPos)
	{
		SFKey *key = m_keys.GetNext(kPos);
		if (!key->isDeleted())
			ctx << key->ToString();
	}

	return ctx.str;
}

//--------------------------------------------------------------------------------
SFString SFKey::ToString(void) const
{
	CStringExportContext ctx;
	if (isCommented())
		ctx << ";";

	ctx << getName();
	
	if (!isCommented() || !getValue().IsEmpty()) // draw '=' only if not commented or value is not empty
		ctx << "=" << getValue();

	ctx << "\n";

	return ctx.str;
}

//----------------------------------------------------------------
// Read a one time line from a config file in a particular group
//----------------------------------------------------------------
SFString getValueFromConfigFile(const SFString& filename, const SFString& groupName, const SFString& stringName, const SFString& def)
{
	// the file better exists or we are in trouble
	if (!SFos::fileExists(filename))
		return def;

	// Note: if groupName is EMPTY then won't match in a file that
	// that has groups except for values that are not contained in a group
	SFString currentGroup;
	
	SFString value = def;

	CSharedResource lock;
	if (lock.Lock(filename, asciiReadOnly, LOCK_NOWAIT)) // do not wait for lock - read only file
	{
		ASSERT(lock.isOpen());
		char buff[4096];
		while (lock.ReadLine(buff, 4096))
		{
			if (buff[0] != ';') // Commented?
			{
				SFString line = buff;
				line.Replace("\n","");
				line.Replace("\r","");

				if (line.Find("[") != -1 && line.Find("]") != -1)
				{
					currentGroup = line;
					currentGroup.ReplaceAny("][", "");
				}
				
				if (groupName.IsEmpty() || currentGroup % groupName)
				{
					SFInt32 find = line.Find(stringName + "=");
					if (find != -1)
					{
						value = line.Mid(find + stringName.GetLength() + 1);
						lock.Release();
						return value;
					}
				}
			}
		}
		lock.Release();
	} else
	{
		// ...and not report any errors since errors will have already been reported by InstallCheck
		//lock.LockFailure();
	}

	return value;
}

//---------------------------------------------------------------------------------------
SFString getPathFromConfigFile(const SFString& filename, const SFString& stringName, const SFString& def)
{
	SFString ret = getValueFromConfigFile(filename, EMPTY, stringName, def);
	if (!ret.IsEmpty())
		ret = NormalizePath(ret);
	return ret;
}

//---------------------------------------------------------------------------------------
SFString CConfig::GetProfileStringGH(const SFString& group, const SFString& key, const SFString& def) const
{
	SFString ret = def;
	SFGroup *grp = findGroup(group);
	if (grp)
	{
		SFKey *k = grp->findKey(key);
		if (k && !k->isCommented())
			ret = k->getValue();
	}

//DEBUG_ALL_CONFIG_CALLS
//#define DEBUG_ALL_CONFIG_CALLS 1
#undef DEBUG_ALL_CONFIG_CALLS
#ifdef DEBUG_ALL_CONFIG_CALLS
	static CSharedResource allCalls;
	if (!allCalls.isOpen())
	{
		allCalls.Lock("/allcalls.txt", asciiWriteCreate, LOCK_NOWAIT);
		allCalls.WriteLine("Filename\tGroup\tKey\tDefault\tValue\n");
	}
	if (allCalls.isOpen())
	{
		SFString file = m_filename;
		if (m_filename.Contains("calweb.ini"))    file = "c";
		if (m_filename.Contains("eventedit.dat")) file = "e";
		if (m_filename.Contains("todoedit.dat"))  file = "t";
		SFString deff = def;
		deff.ReplaceAny("\r\n", EMPTY);
		SFString rett = ret;
		rett.ReplaceAny("\r\n", EMPTY);
		allCalls.WriteLine(file + "\t" + group + "\t" + key + "\t" + deff + "\t" + rett + "\n");
	}
#endif

	return ret;
}

//---------------------------------------------------------------------------------------
void CConfig::negateKey(const SFString& group, const SFString& name)
{
	SFKey *key = findKey(group, name);
	if (key)
		key->setValue(asString(!toLong(key->getValue())));
}

//---------------------------------------------------------------------------------------
void CConfig::renameKey(const SFString& group, const SFString& oldName, const SFString& newName)
{
	SFKey *key = findKey(group, oldName);
	if (key)
		key->setName(newName);
}

//---------------------------------------------------------------------------------------
void CConfig::moveKey(const SFString& oldGroup, const SFString& oldName, const SFString& newGroup, const SFString& newName, CConfig *dest)
{
	if (!dest)
		dest = this;
	ASSERT(dest);

	SFKey *key = findKey(oldGroup, oldName);
	if (key)
	{
		// do not overwrite if its already present
		if (!dest->findKey(newGroup, newName))
			dest->SetProfileString(newGroup, newName, key->getValue());

		if (oldGroup+oldName != newGroup+newName)
			deleteKey(oldGroup, oldName);
	}
}

//---------------------------------------------------------------------------------------
void CConfig::moveKey(const SFString& in, CConfig *dest, SFBool negate)
{
	SFString str = in;

	str.Reverse();
	SFString def = Strip(nextTokenClear(str, '|'), ' ');
	def.Reverse();
	str.Reverse();
	str.ReplaceAll(" ", EMPTY);

	SFString oldGroup = nextTokenClear(str, '|');
	SFString oldName  = nextTokenClear(str, '|');
	SFString newGroup = nextTokenClear(str, '|');
	SFString newName  = nextTokenClear(str, '|');

	if (!dest)
		dest = this;

	if (negate)
		negateKey(oldGroup, oldName);
	moveKey(oldGroup, oldName, newGroup, newName, dest);
}

//---------------------------------------------------------------------------------------
void CConfig::moveGroup(const SFString& groupName, const SFString& newName, CConfig *dest)
{
	SFGroup *group = findGroup(groupName);
	if (group)
	{
		group->setName(newName);

		LISTPOS kPos = group->m_keys.GetHeadPosition();
		while (kPos)
		{
			SFKey *key = group->m_keys.GetNext(kPos);
			if (!key->isDeleted())
				dest->SetProfileString(groupName, key->getName(), key->getValue());
		}

		deleteGroup(groupName);
	}
}

//---------------------------------------------------------------------------------------
void CConfig::deleteDefault(const SFString& group, const SFString& keyName, const SFString& def)
{
	// even if commented
	SFGroup *grp = findGroup(group);
	if (grp)
	{
		SFKey *key = grp->findKey(keyName, TRUE);
		if (key && key->getValue() == def)
			key->setDeleted(TRUE);
	}
}

//---------------------------------------------------------------------------------------
SFKey *SFGroup::findKey(const SFString& keyName, SFBool incDeleted) const
{
	LISTPOS kPos = m_keys.GetHeadPosition();
	while (kPos)
	{
		SFKey *key = m_keys.GetNext(kPos);
		if (key->isCommented() && !incDeleted)
		{
			SFString search = "_" + keyName + "=";
			SFString name   = key->getName();
			name.ReplaceAll(" ", "_");
			if (name.Find(search)!=-1)
				return key;
			search.Replace("=", "");
			if (name.Find(search)!=-1)
				return key;
		} else
		{
			SFString name = key->getName();
			if (incDeleted)
				name.Replace("=", EMPTY);
			if (name == keyName)
				return key;
		}
	}
	return NULL;
}

//---------------------------------------------------------------------------------------
void CConfig::mergeConfig(CConfig *configIn)
{
	LISTPOS gPos = configIn->m_groups.GetHeadPosition();
	while (gPos)
	{
		SFGroup *group = configIn->m_groups.GetNext(gPos);
		if (group->getName() != "CONFIGFILE")
		{
			LISTPOS kPos = group->m_keys.GetHeadPosition();
			while (kPos)
			{
				SFKey *key = group->m_keys.GetNext(kPos);
				SetProfileString(group->getName(), key->getName(), key->getValue());

				// The key may be deleted so we want to make sure its not delete
				// in the resulting file if it is present in the source file
				unDeleteKey(group->getName(), key->getName());
			}
		}
	}
}

//---------------------------------------------------------------------------------------
SFBool CConfig::saveAndReload(void)
{
	writeFile("");
	return loadFile(m_filename);
}

//---------------------------------------------------------------------------------------
SFBool CConfig::loadFile(const SFString& filename)
{
	Clear();
	
	SFString curGroup;
	if (Lock(filename, asciiReadOnly, LOCK_WAIT)) // wait for lock before opening for read
	{
		char buff[MAX_SERVER_VAR_LEN];
		while (ReadLine(buff, MAX_SERVER_VAR_LEN))
		{
			SFString line = buff;

			if (line == "xxx\n")
			{
				// debugging -- put this line first in the config file to not
				// use any config data.
				return TRUE;
			}

			line.Replace("\n","");
			line.Replace("\r","");

			SFInt32 len = line.GetLength();
			if (len)
			{
				
				SFBool commented = FALSE;
				if (line[0] == ';')
				{
					commented = TRUE;
					line = line.Mid(1);
					len--;
				}

				// start of a group tag?
				if (len && line[0] == '[')
				{
					// a robust extraction of stuff
					// between '[' and ']'
					int start = (int)line.Find('[');
					start++;
					int end = (int)line.ReverseFind(']');
					end--;
					line = line.Extract( start, end );

					addGroup(line, commented);
					curGroup = line;
				} else if (len)
				{
					if (commented)
					{
						addKey(curGroup, line, "", commented);
					} else
					{
						SFInt32 find = line.Find("=");
						// may be a blank line or a comment
						if (find!=-1)
						{
							SFString key = line.Left(find);
							SFString val = line.Mid(find+1);
							addKey(curGroup, key, val, commented);
						}
					}
				}
			}
		}

		Release();

		SFGroup  *sources[20];
		SFString  names  [20];
		SFInt32 nCopies = 0;
		LISTPOS gPos = m_groups.GetHeadPosition();
		while (gPos)
		{
			SFGroup *grp = m_groups.GetNext(gPos);
			if (grp->getName().Contains("|"))
			{
				SFString name = grp->getName();
				grp->setName(nextTokenClear(name, '|')); // rename the first one
				while (nCopies < 20 && !name.IsEmpty()) // copy the rest
				{
					sources[nCopies]   = grp;
					names  [nCopies++] = nextTokenClear(name, '|');
				}
			}
		}
		for (int i=0;i<nCopies;i++)
			copyGroup(names[i], sources[i]);

		return TRUE;
	}

	return FALSE;
}
