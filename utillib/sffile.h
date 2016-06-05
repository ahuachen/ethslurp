#ifndef _H_FILECLASS
#define _H_FILECLASS
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
#include "exportcontext.h"

//----------------------------------------------------------------------------------
class SFFile
{
	SFString path;
	SFString fileName;
	SFString input;
	SFInt32  type;
	SFInt32  size;

public:
	SFFile(void);
	SFFile(const SFString& fileName);
	SFFile(const SFFile& file);
	SFFile& operator=(const SFFile& file);
	virtual ~SFFile(void);
	SFString getPath(void) const;
	SFString getFilename(void) const;
	SFString getInput(void) const;
	SFInt32  getSize(void) const;
	SFInt32  getType(void) const;
	void setType(SFInt32 t);
	virtual void Report(CExportContext& ctx);
	SFString getFullPath(void) const;

private:
	void Init(void);
	void Clear(void);
	void Copy(const SFFile& file);

	friend class SFFolder;
};

//--------------------------------------------------------------------------------
inline SFString getHomeFolder(void)
{
	struct passwd *pw = getpwuid(getuid());
	return SFString(pw->pw_dir)+"/";
}

#endif
