#ifndef _H_UNIQUELIST
#define _H_UNIQUELIST
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

//----------------------------------------------------------------------------------------
template<class TYPE>
class SFUniqueList : public SFList<TYPE>
{
	SORTINGFUNC   sortFunc;
	DUPLICATEFUNC compFunc;
public:
	SFUniqueList(SORTINGFUNC sFunc, DUPLICATEFUNC cFunc)
		{
			sortFunc = sFunc;
			compFunc = cFunc;
		}
	SFBool AddItemUnique(TYPE item)
		{
			return SFList<TYPE>::AddSorted(item, sortFunc, compFunc);
		}
};

//----------------------------------------------------------------------------------------
typedef SFUniqueList<SFString*>  SFUniqueStringList;

#endif
