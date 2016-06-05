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
#include "basetypes.h"

#include "sffile.h"

//----------------------------------------------------------------------------------
SFFile::SFFile(void)
{
	Init();
}

//----------------------------------------------------------------------------------
SFFile::SFFile(const SFString& fnIn)
{
	Init();
	input = fnIn;

	SFString fn = fnIn;
	if (!fn.startsWith('/') && !fn.startsWith('.') && !fn.startsWith('~'))
		fn = "./" + fn; // assume cwd
	fn.Replace("../", getCWD()+"../");
	fn.Replace("./",  getCWD());
	fn.Replace("~/",  getHomeFolder());

	if (fn.endsWith('/'))
	{
		path = fn;
		fileName = EMPTY;

	} else
	{
		path = fn.Left(fn.ReverseFind('/')+1);
		fileName = fn;
		fileName.Replace(path, EMPTY);
	}
}

//----------------------------------------------------------------------------------
SFFile::SFFile(const SFFile& file)
{
	Copy(file);
}

//----------------------------------------------------------------------------------
SFFile& SFFile::operator=(const SFFile& file)
{
	Clear();
	Copy(file);
	return *this;
}

//----------------------------------------------------------------------------------
SFFile::~SFFile(void)
{
}

//----------------------------------------------------------------------------------
SFString SFFile::getInput(void) const
{
	return input;
}

//----------------------------------------------------------------------------------
SFString SFFile::getFilename(void) const
{
	return fileName;
}

//----------------------------------------------------------------------------------
SFString SFFile::getPath(void) const
{
	return path;
}

//----------------------------------------------------------------------------------
SFInt32 SFFile::getSize(void) const
{
	if (size == -1)
		((SFFile*)this)->size = SFos::fileSize(input);
	return size;
}

//----------------------------------------------------------------------------------
SFInt32 SFFile::getType(void) const
{
	if (type == -1)
		((SFFile*)this)->type = 1;
	return type;
}

//----------------------------------------------------------------------------------
void SFFile::setType(SFInt32 t)
{
	type = t;
}

//----------------------------------------------------------------------------------
void SFFile::Report(CExportContext& ctx)
{
	SFString fn = getInput().Mid(getInput().ReverseFind('/')+1);
	if (type == 2)
		ctx << fn << "--------------------------------------------\n";
	else
		ctx << fn << "\t" << getSize() << "\t" << getType() << "\n";
}

//----------------------------------------------------------------------------------
void SFFile::Init(void)
{
	type = -1;
	size = -1;
}

//----------------------------------------------------------------------------------
void SFFile::Clear(void)
{
	path = fileName = input = EMPTY;
	type = size = 0;
}

//----------------------------------------------------------------------------------
void SFFile::Copy(const SFFile& file)
{
	path     = file.path;
	fileName = file.fileName;
	input    = file.input;
	type     = file.type;
	size     = file.size;
}

//----------------------------------------------------------------------------------
SFString SFFile::getFullPath(void) const
{
	return (path+fileName).Substitute("//","/");
}
