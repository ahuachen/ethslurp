#if 1
#ifndef _OPTIONS_H_
#define _OPTIONS_H_
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
class COptions
{
public:
	SFBool   slurp;
	SFBool   prettyPrint;
	SFBool   rerun;
	SFBool   incomeOnly;
	SFBool   expenseOnly;
	SFInt32  firstBlock2Read;
	SFInt32  lastBlock2Read;
	SFTime   firstDate;
	SFString funcFilter;
	SFTime   lastDate;
	SFBool   openFile;
	SFBool   cmdFile;
	SFString addr;
	SFInt32  maxTransactions;
	SFInt32  pageSize;
	SFString exportFormat;
	SFString name;
	SFString archiveFile;
	SFBool   wantsArchive;
	FILE    *output; // for use when -a is on

	        COptions       (void);
	       ~COptions       (void);
	SFInt32 parseArguments (SFInt32 nArgs, const SFString *args);
};

//--------------------------------------------------------------------------------
class CParams
{
public:
	SFString  shortName;
	SFString  longName;
	SFString  description;
	CParams( const SFString& name, const SFString& descr );
};

//--------------------------------------------------------------------------------
extern int      usage        (const SFString& errMsg=nullString);
extern SFString options     (void);
extern SFString descriptions(void);
extern SFString purpose     (void);

//--------------------------------------------------------------------------------
extern int      sortParams  (const void *c1, const void *c2);
extern SFString expandOption(SFString& arg);

//--------------------------------------------------------------------------------
extern CParams params[];
extern SFInt32 nParams;

#endif
#else
#ifndef _OPTIONS_H_
#define _OPTIONS_H_
class COptions
{
public:
	SFBool   count;
	SFBool   sum;
	SFString file;

	        COptions       (void);
	       ~COptions       (void);
	SFInt32 parseArguments (SFInt32 nArgs, const SFString *args);
};

//--------------------------------------------------------------------------------
class CParams
{
public:
	SFString  shortName;
	SFString  longName;
	SFString  description;
	CParams( const SFString& name, const SFString& descr );
};

//--------------------------------------------------------------------------------
extern int      usage        (const SFString& errMsg=nullString);
extern SFString options     (void);
extern SFString descriptions(void);
extern SFString purpose     (void);

//--------------------------------------------------------------------------------
extern CParams params[];
extern SFInt32 nParams;

#endif
#endif

