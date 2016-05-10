/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "rect.h"

void CDoubleRect::operator|=(const CDoubleRect& rect)
{
	top    = MIN(top,    rect.top);
	left   = MIN(left,   rect.left);
	bottom = MAX(bottom, rect.bottom);
	right  = MAX(right,  rect.right);
}

CDoubleRect ShapeFromPercents(const CDoubleRect base, const CDoubleRect& pcts)
{
  CDoublePoint trPct(pcts.left,  pcts.top);
  CDoublePoint scPct(pcts.right-pcts.left, pcts.bottom-pcts.top);
  
  double w = base.Width();
  double h = base.Height();

  CDoublePoint tr(w * trPct.x, h * trPct.y);

  CDoubleRect rect = base;

  rect *= scPct;
	rect += tr;
	
	return rect;
}
