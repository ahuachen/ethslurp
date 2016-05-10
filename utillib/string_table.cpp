/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "sfstring.h"
#include "list.h"
#include "database.h"
#include "dates.h"
#include "string_table.h"

//--------------------------------------------------------------------------------------
class CIndexedString
{
	static int nextID;

public:
	SFInt32  id;
	SFString name;
	SFString string;

	CIndexedString (void)                                 { Init(BAD_NUMBER, EMPTY, EMPTY); }
	CIndexedString (const SFString& n, const SFString& v) { nextID++; Init(nextID, n, v); }
	CIndexedString (const CIndexedString& copy)           { Copy(copy); }
	CIndexedString& operator=(const CIndexedString& copy) { Copy(copy); return *this; }

public:
	void Init(SFInt32 idIn, const SFString& n , const SFString& v)
		{
			name   = n;
			string = v;
			id     = idIn;

			string.ReplaceAll("\\n", '\n');
			string.ReplaceAll("\\t", '\t');
		}
	void Copy(const CIndexedString& copy)
		{
			name   = copy.name;
			string = copy.string;
			id     = copy.id;
		}
};
typedef SFArrayBase<CIndexedString> CStringTableArray;

//--------------------------------------------------------------------------------------
class CStringTable
{
public:
	CStringTableArray m_table;
	SFInt32           m_nStrings;
	SFInt32          *m_indexTable;
	char             *m_strings;
	char             *m_buffer;
};
static CStringTable theStringTable;

//--------------------------------------------------------------------------------------
#define HEADER_FILE       (UTESTING_SOURCE + "lib_strings/strings.h")
#define TEXT_FILE         (UTESTING_SOURCE + "lib_strings/strings.txt")
#define STRING_FILEREPORT (UTESTING_SOURCE + "lib_strings/compile_report.txt")
#define STRING_FILENAME   SFString("./strings.bin")
#define FIRST_INDEX       1000

//--------------------------------------------------------------------------------------
int CIndexedString::nextID = FIRST_INDEX;

//--------------------------------------------------------------------------------------
static void reportMissingFiles(void)
{
const char* IDS_MSG_NOSTRINGFILE =
"<br>\n"
"The program could not find its string file.  Quitting.<br>\n"
"Source Filename: [SOURCE]<br>\n"
"String Filename: [FILENAME]<br>\n"
"Working Folder:  [FOLDER]<br>\n";

	SFString msg = LoadStringGH(IDS_MSG_NOSTRINGFILE);
	msg.Replace("[SOURCE]",   TEXT_FILE);
	msg.Replace("[FILENAME]", STRING_FILENAME);
	msg.Replace("[FOLDER]",   getCWD());
	fprintf(stderr, "%s", (const char*)msg);
	fflush(stderr);
}

//--------------------------------------------------------------------------------------
#ifdef BUILDABLE
static void LoadAsciiFile(void)
{
	SFString buffer;
	SFString textFile = TEXT_FILE;
	if (!SFos::fileExists(textFile))
	{
		reportMissingFiles();
		exit(0);
	}

	if (asciiFileToBuffer(textFile, &buffer))
	{
		SFString curString = snagFieldClear(buffer, "string");
		while (!curString.IsEmpty())
		{
			SFString name  = snagFieldClear(curString, "name");
			SFString value = snagFieldClear(curString, "value");
			value.ReplaceAll("\n", EMPTY);
			value.ReplaceAll("\t", EMPTY);
			if (value.IsEmpty())
			{
				printf("%s", (const char*)("\nBad data file at name: '" + name + "' value: '" + value + "' - exiting\n"));
				printf("\n\n\t\tTO FIX THIS EDIT THE STRINGS.TXT FILE\n\n\n");
				exit(0);
			}

			theStringTable.m_table.addValue(CIndexedString(name, value));
			curString = snagFieldClear(buffer, "string");
		}
	}
}

//--------------------------------------------------------------------------------------
static void SaveAllFiles()
{
	CSharedResource lock;

	SFString stringFile = STRING_FILENAME;
	SFos::removeFile(stringFile); // gets removed anyway but its OK
	if (lock.Lock(stringFile, binaryWriteCreate, LOCK_CREATE)) // wait for lock, create file for writing
	{
		SFInt32 nStrings = theStringTable.m_table.getCount();

		lock.Write(&nStrings, sizeof(SFInt32), 1);

		SFInt32 offset = sizeof(SFInt32);
		int i=0;
		for (i=0;i<nStrings;i++)
		{
			SFInt32 id = theStringTable.m_table[i].id;
			lock.Write(&id,     sizeof(SFInt32), 1);
			lock.Write(&offset, sizeof(SFInt32), 1);
			offset += (theStringTable.m_table[i].string.GetLength() + sizeof(SFInt32) + 1);
		}

		for (i=0;i<nStrings;i++)
		{
			lock.Write(theStringTable.m_table[i].string);
			char zero = '\0';
			lock.Write(&zero, sizeof(char), 1);
		}

		lock.Release();
	}

#if defined(DO_HEADER) && defined(BUILDABLE)
	SFos::removeFile(HEADER_FILE);
	if (lock.Lock(HEADER_FILE, asciiWriteCreate, LOCK_CREATE)) // wait for lock, create file for writing
	{
		lock.WriteLine("// THIS FILE IS GENERATED AUTOMATICALLY - DO NOT EDIT!\n");
		lock.WriteLine("#ifndef _STRING_RESOURCES_H_\n");
		lock.WriteLine("#define _STRING_RESOURCES_H_\n");
		lock.WriteLine("/*-------------------------------------------------------------------------\n");
		lock.WriteLine(" * This source code is confidential proprietary information which is\n");
		lock.WriteLine(" * Copyright (c) 1999, 2016 by Great Hill Corporation.\n");
		lock.WriteLine(" * All Rights Reserved\n");
		lock.WriteLine(" *\n");
		lock.WriteLine(" *------------------------------------------------------------------------*/\n");
		lock.WriteLine("\n");

		lock.WriteLine(SFString("#define ") + padRight("IDS_FIRST_STRING", 35) + padLeft(asString(FIRST_INDEX), 5) + "\n");

		for (int i=0;i<theStringTable.m_table.getCount();i++)
			lock.WriteLine(SFString("#define ") + padRight(theStringTable.m_table[i].name, 35) + padLeft(asString(theStringTable.m_table[i].id), 5) + "\n");

		CIndexedString last("last", "last");
		lock.WriteLine(SFString("#define ") + padRight("IDS_LAST_STRING", 35) + padLeft(asString(last.id), 5) + "\n");

		lock.WriteLine("\n");
		lock.WriteLine("#endif\n");

		lock.Release();
	}
#endif
	printf("%s", "Strings file updated\n");
}
#endif

//--------------------------------------------------------------------------------------
static void ReadBinaryFile(void)
{
	SFString stringFile = STRING_FILENAME;
	if (!theStringTable.m_buffer)
	{
#ifdef BUILDABLE
		SFString textFile = TEXT_FILE;
		
		SFTime binDate   = earliestDate;
		SFTime asciiDate = earliestDate;
        
		SFos::fileLastModifyDate(stringFile, &binDate);
		SFos::fileLastModifyDate(textFile,   &asciiDate);

		// Debugging
		//asciiDate = latestDate;
		if (asciiDate >= binDate)
		{
			LoadAsciiFile();
			SaveAllFiles();
			printf("%ld strings read into %s (size %d)<br>\n", theStringTable.m_table.getCount(), (const char *)stringFile, SFos::fileSize(stringFile));
		}
#endif
		SFInt32 nChars=0;
		if (binaryFileToBuffer(stringFile, nChars, NULL))
		{
			theStringTable.m_buffer = new char[nChars + 100]; // safty factor
			if (binaryFileToBuffer(stringFile, nChars, theStringTable.m_buffer))
			{
				char *ptr = theStringTable.m_buffer;

				theStringTable.m_nStrings = *(SFInt32 *)ptr;
				ptr += sizeof(SFInt32);

				theStringTable.m_indexTable = (SFInt32 *)ptr;
				ptr += (2 * sizeof(SFInt32) * theStringTable.m_nStrings);

				theStringTable.m_strings = ptr;
			}
		}

#ifdef BUILDABLE
		if (asciiDate >= binDate)
		{
const char* STR_STRINGFILE_REPORT=
"String file date: [DATE1]\n"
"Compile date:     [DATE2]\n\n"
"nStrings:         [NSTRINGS]\n";

			SFString report = LoadStringGH(STR_STRINGFILE_REPORT);
			report.Replace("[DATE1]", asciiDate.Format(FMT_DEFAULT));
			report.Replace("[DATE2]", Now().Format(FMT_DEFAULT));
			report.Replace("[NSTRINGS]", asString(theStringTable.m_nStrings));

			CSharedResource lock;
			if (lock.Lock(STRING_FILEREPORT, asciiWriteCreate, LOCK_CREATE)) // wait for lock, create file for writing
			{
				lock.WriteLine(report);
				for (int i=FIRST_INDEX+1;i<FIRST_INDEX+theStringTable.m_table.getCount()+1;i++)
				{
					SFString str = LoadStringGH(i);
					str.ReplaceAll("\n", "|");
					lock.WriteLine("str[" + padLeft(asString(i),5) + "]: ");
					while (!str.IsEmpty())
					{
						lock.WriteLine(str.Left(80) + "\n");
						str = str.Mid(80,5000);
						if (!str.IsEmpty())
							lock.WriteLine("\t\t");
					}
				}
				lock.WriteLine("\n");
				lock.Release();
			}
		}
#endif
	}

	if (theStringTable.m_indexTable == NULL)
	{
		reportMissingFiles();
		exit(0);
	}
}

//--------------------------------------------------------------------------------------
void clearStrings(void)
{
	if (theStringTable.m_buffer)
		delete [] theStringTable.m_buffer;
	theStringTable.m_buffer = NULL;
}

//--------------------------------------------------------------------------------------
SFString LoadStringGH(SFInt32 id)
{
	ReadBinaryFile();
	SFInt32 index = (id - (FIRST_INDEX+1)) * 2;

#ifdef BUILDABLE
#ifdef TESTING
	if (g_unitTesting)
#endif
	{
		SFInt32 *lastIndex = (SFInt32 *)theStringTable.m_strings;
		lastIndex -= 2;
		ASSERT(id > FIRST_INDEX && id <= *lastIndex);

		if (id < FIRST_INDEX)
		{
			printf("\nIndex %ld not found in table\n", id);
			printf("\n\n\t\tREFRESH DEFINED STRINGS\n\n\n");
			exit(0);

		} else if (id < 0)
		{
			printf("\nIndex %ld smaller than 0\n", id);
			printf("\n\n\t\tREFRESH DEFINED STRINGS\n\n\n");
			exit(0);

		} else if (id > *lastIndex)
		{
			printf("\nIndex %ld larger than lastIndex: %ld\n", id, *lastIndex);
			printf("\n\n\t\tREFRESH DEFINED STRINGS\n\n\n");
			exit(0);

		} else if (theStringTable.m_indexTable[index] != id)
		{
			printf("\nIndex %ld not equal to indexTable[%ld]\n", id, index);
			printf("\n\n\t\tRECOMPILE STRING_TABLE.CPP WITH DO_HEADER ENABLED\n\n\n");
			exit(0);
		}
	}
#endif
	ASSERT(theStringTable.m_indexTable[index] == id);

	// Layout of binary file
	//
	// nStrings
	// [INDEX_TABLE]
	// [STRING]
	//
	// where
	//
	// [INDEX_TABLE] is
	//
	// nString copies of 
	//
	// [ID] [OFFSET]

	return SFString((char *)(theStringTable.m_strings+theStringTable.m_indexTable[index+1]));
}
