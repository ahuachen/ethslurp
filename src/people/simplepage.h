#ifndef _SIMPLEFORM_H_
#define _SIMPLEFORM_H_
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

#include "utillib.h"

//--------------------------------------------------------------------------------------------
enum pageStatus
{
	ST_UPPER=0,
	ST_LOWER=1,
};

//-----------------------------------------------------------------------------------------
class CSimplePage;
typedef void (*PAGERENDERFUNC)(CSimplePage *page);
typedef int  (*CALLBACKFUNC)  (CSimplePage *page, SFString& html);

//-----------------------------------------------------------------------------------------
class CSimplePage
{
public:
	SFString cmd;
	SFString strings;
	SFString title;
	SFString subTitle;
	SFString outerBody;
	SFString tailMsg;
	SFString loginMsg;

	SFString rowFmt;
	SFString rowData;
	SFString innerHead;
	SFString innerBody;
	SFString innerTail;

	SFString pageType;
	SFString fieldList;
	SFInt32  nFields;

	PAGERENDERFUNC func;
	CALLBACKFUNC   callBk_display;

	          CSimplePage     (const SFString& cmdIn);
	void      Initialize      (const SFString& cmdIn, SFBool isConfirmed);
	void      readStrings     (const SFString& cmdIn);
	SFBool    loginNeeded     (void);
	SFString  getCode         (void);

    void      closeForm       (const SFString& newStat);

    SFString  statusMsg       (pageStatus isUpper);
    void      setStatusMsg    (pageStatus isUpper, const SFString& status);
    SFString  status          (SFInt32 isGet=TRUE, pageStatus stType=ST_LOWER, const SFString& newMsg=nullString);

	SFString  displayPage     (void);

private:
	          CSimplePage     (void) {};
		      CSimplePage     (const CSimplePage& page) {};
};

#endif
