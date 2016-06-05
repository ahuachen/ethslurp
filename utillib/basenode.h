#ifndef __NODE_H3D
#define __NODE_H3D
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
#include "runtimeclass.h"

//----------------------------------------------------------------------------
class SFArchive;

//----------------------------------------------------------------------------
class CBaseNode
{
private:
	SFInt32 m_refCount;
	SFBool  m_deleted;
	SFInt32 m_schema;
	SFBool  m_showing;

public:

	                CBaseNode       (void);
	virtual        ~CBaseNode       (void);

	        void    Reference       (void);
	        SFInt32 Dereference     (void);
	        SFBool  isReferenced    (void) const;
			SFBool  isDeleted       (void) const;
			void    setDeleted      (SFBool val);
			SFInt32 getSchema       (void) const;
			void    setSchema       (SFInt32 val);
			SFBool  isShowing       (void) const;
			void    setShowing      (SFInt32 val);

	virtual SFBool  isKindOf        (const ghRuntimeClass* pClass) const;

	DECLARE_NODE_BASE(CBaseNode)

	virtual SFInt32  getHandle      (void) const = 0;
	virtual SFString getValueByName (const SFString& fieldName) const = 0;
	virtual SFBool   setValueByName (const SFString& fieldName, const SFString& fieldValue) = 0;
	virtual void     Serialize      (SFArchive& archive) = 0;
	virtual SFString Format         (const SFString& fmtIn) const = 0;

protected:
			void	 Init      		(void);
			void	 Copy      		(const CBaseNode& bn);
};

//----------------------------------------------------------------------------
#undef ghDELETE
#define ghDELETE(_nodE) \
	if ((_nodE)) \
		if ((_nodE)->Dereference()) \
			delete (_nodE); \
	(_nodE) = NULL;

#endif
