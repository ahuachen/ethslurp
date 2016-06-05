#ifndef manage_manage_h
#define manage_manage_h
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
#include "utillib.h"
#include "ethlib.h"
#include "options.h"
#include "slurp.h"
#include "transaction.h"

//---------------------------------------------------------------------------------------------------
class CSlurperApp
{
private:
	CVersion  version;
	CConfig   config;
	CWebAPI   api;

public:
	CSlurp    theAccount;

              CSlurperApp (void) : config(NULL) { };
             ~CSlurperApp (void) {  }

	SFBool    Initialize  (COptions& options, SFString& message);
	SFBool    Slurp       (COptions& options, SFString& message);
	SFBool    Filter      (COptions& options, SFString& message);
	SFBool    Display     (COptions& options, SFString& message);

private:
	void      buildDisplayStrings (COptions& options);
	SFString  getFormatString     (COptions& options, const SFString& name);
};

//--------------------------------------------------------------------------------
extern SFString getHomeFolder(void);

//---------------------------------------------------------------------------------------------------
extern SFBool establishFolders(CConfig& config, const SFString& vers);
extern void   findBlockRange  (const SFString& contents, SFInt32& minBlock, SFInt32& maxBlock);

//---------------------------------------------------------------------------------------------------
extern SFString configPath(const SFString& part=EMPTY);
extern SFString cachePath (const SFString& part=EMPTY);

//--------------------------------------------------------------------------------
extern SFBool verbose;
extern SFBool isTesting;
extern CFileExportContext  outScreen;
extern CFileExportContext& outErr;

#endif
