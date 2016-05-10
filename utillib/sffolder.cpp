/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "paths.h"

//----------------------------------------------------------------------------------
SFFolder::SFFolder(const SFString& dir) : SFFile(dir)
{
	type = 2;
	Init();
	ListFiles();
}

//----------------------------------------------------------------------------------
SFFolder::SFFolder(const SFFolder& folder) : SFFile(folder)
{
	Init();
	Copy(folder);
}

//----------------------------------------------------------------------------------
SFFolder& SFFolder::operator=(const SFFolder& folder)
{
	SFFile::operator=(folder);
	Clear();
	Copy(folder);
	return *this;
}

//----------------------------------------------------------------------------------
SFFolder::~SFFolder(void)
{
	Clear();
}

//----------------------------------------------------------------------------------
void SFFolder::AddFolder(const SFString& folderName)
{
	SFFolder *folder = new SFFolder(input+"/"+folderName);
	folder->type = 2;
	fileList.AddTail(folder);
}

//----------------------------------------------------------------------------------
void SFFolder::AddFile(const SFString& fileName)
{
	SFFile *file = new SFFile(input+"/"+fileName);
	file->type = 1;
	fileList.AddTail(file);
}

//----------------------------------------------------------------------------------
void SFFolder::Report(CExportContext &ctx)
{
	ctx << "\n";

	SFFile::Report(ctx);

	LISTPOS fPos = fileList.GetHeadPosition();
	while (fPos)
	{
		SFFile *file = fileList.GetNext(fPos);
		file->Report(ctx);
	}
}

//----------------------------------------------------------------------------------
void SFFolder::Init(void)
{
}

//----------------------------------------------------------------------------------
void SFFolder::Clear(void)
{
	LISTPOS fPos = fileList.GetHeadPosition();
	while (fPos)
	{
		SFFile *file = fileList.GetNext(fPos);
		delete file;
	}
	fileList.RemoveAll();
}

//----------------------------------------------------------------------------------
void SFFolder::Copy(const SFFolder& folder)
{
	Clear();

	LISTPOS fPos = folder.fileList.GetHeadPosition();
	while (fPos)
	{
		SFFile *file = folder.fileList.GetNext(fPos);
		ASSERT(file);
		SFFile *copy = new SFFile(*file);
		fileList.AddTail(copy);
	}
}

//----------------------------------------------------------------------------------
void SFFolder::ListFiles(void)
{
	{
		SFInt32 nFiles = 0;
		SFos::listFiles(nFiles, NULL, input + "/*");
		if (nFiles)
		{
			SFString *files = new SFString[nFiles];
			SFos::listFiles(nFiles, files, input + "/*");

			for (int i=0;i<nFiles;i++)
				if (files[i] != "." && files[i] != "..")
					AddFile(files[i]);

			delete [] files;
		}
	}
	{
		SFInt32 nFolders = 0;
		SFos::listFolders(nFolders, NULL, input + "/*");
		if (nFolders)
		{
			SFString *folders = new SFString[nFolders];
			SFos::listFolders(nFolders, folders, input + "/*");

			for (int i=0;i<nFolders;i++)
				if (folders[i] != "." && folders[i] != "..")
					AddFolder(folders[i]);

			delete [] folders;
		}
	}
}

