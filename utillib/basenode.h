#ifndef __NODE_H3D
#define __NODE_H3D
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

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
