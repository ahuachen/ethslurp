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
#include "function.h"

// EXISTING_CODE
// EXISTING_CODE

//---------------------------------------------------------------------------
SFString nextFunctionChunk_custom(const SFString& fieldIn, SFBool& force, const void *data)
{
	CFunctionNotify *fu = (CFunctionNotify*)data;
	const CFunction *fun = fu->getDataPtr();
	switch (tolower(fieldIn[0]))
	{
		// EXISTING_CODE
		// EXISTING_CODE
		default:
			break;
	}
	
#pragma unused(fu)
#pragma unused(fun)

	return EMPTY;
}

//---------------------------------------------------------------------------
SFBool CFunction::handleCustomFormat(CExportContext& ctx, const SFString& fmtIn, void *data) const
{
	// EXISTING_CODE
	// EXISTING_CODE
	return FALSE;
}

// EXISTING_CODE
void CFunction::parseParams(SFBool input, const SFString& contents)
{
	char *p = (char *)(const char*)contents;
	while (p && *p)
	{
		CParameter param;SFInt32 nFields=0;
		p = param.parseJson(p,nFields);
		if (nFields)
		{
			if (input)
				inputs[inputs.getCount()+1] = param;
			else
				outputs[outputs.getCount()+1] = param;
		}
	}
}
// EXISTING_CODE
