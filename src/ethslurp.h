#ifndef _ETHSLURP_H_
#define _ETHSLURP_H_
/*--------------------------------------------------------------------------------
 * Copyright 2016 - Great Hill Corporation.
 --------------------------------------------------------------------------------*/

#include "manage.h"

class BlockData
{
public:
	SFString blockHash;
	SFString blockNumber;
	SFString contractAddress;
	SFString cumulativeGasUsed;
	SFString from;
	SFString gas;
	SFString gasPrice;
	SFString gasUsed;
	SFString hash;
	SFString input;
	SFString nonce;
	SFString timeStamp;
	SFString to;
	SFString transactionIndex;
	SFString value;
	BlockData(void) {}
	BlockData(const SFString& str);
};

#endif