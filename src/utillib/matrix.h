#ifndef __MATRIX2D_H
#define __MATRIX2D_H
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/

/*

#include "point.h"
#include "rect.h"
#include "list.h"

#define DIM 3

//---------------------------------------------------------------------------------
class CMatrix
{
public:
  double mat[DIM][DIM];
  
                      CMatrix           (void);
                      CMatrix           (const CMatrix& mat);
                     ~CMatrix           (void);

          CMatrix&    operator=         (const CMatrix& mat);

          // Assignment operators
          CMatrix&    operator+=        (const CMatrix& m);
          CMatrix&    operator-=        (const CMatrix& m);
          CMatrix&    operator*=        (const CMatrix& m);
          CMatrix&    operator*=        (double s);
  
  friend  CMatrix     IdentityMatrix    (void);
  friend  CMatrix     TranslationMatrix (const CDoublePoint& loc);
  friend  CMatrix     RotationMatrix    (double angle);
  friend  CMatrix     ScaleMatrix       (double X, double Y);

  // Binary operators
  friend  CMatrix     operator+         (const CMatrix& A, const CMatrix& B);
  friend  CMatrix     operator-         (const CMatrix& A, const CMatrix& B);
  friend  CMatrix     operator*         (const CMatrix& A, double B);
  friend  CMatrix     operator*         (const CMatrix& A, const CMatrix& B);

  friend class CDoublePoint operator*   (const CMatrix& M, const CDoublePoint& v);
  friend class CDoubleRect  operator*   (const CMatrix& M, const CDoubleRect& r);
  friend class CPolygonInfo Transform   (const CMatrix& M, const CDoubleRect& r);

	SFString ToString(void) const;
};

//------------------------------------------------------------------------
typedef SFStack<CMatrix *> CMatrixStack;
*/

#endif
