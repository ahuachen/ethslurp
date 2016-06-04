#ifndef _OPTIONS_H_
#define _OPTIONS_H_
/*--------------------------------------------------------------------------------
 * Copyright 2016 - Great Hill Corporation.
 --------------------------------------------------------------------------------*/

//--------------------------------------------------------------------------------
class COptions
{
public:
	SFBool   slurp;
	SFBool   prettyPrint;
	SFBool   rerun;
	SFBool   incomeOnly;
	SFBool   expenseOnly;
	SFInt32  firstBlock;
	SFInt32  lastBlock;
	SFTime   firstDate;
	SFTime   lastDate;
	SFBool   openFile;
	SFString addr;
	SFInt32  maxTransactions;
	SFInt32  pageSize;
	SFString exportFormat;
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
