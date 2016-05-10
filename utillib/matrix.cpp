/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 1999, 2016 by Great Hill Corporation.
 * All Rights Reserved
 *
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "matrix.h"

/*

CMatrix::CMatrix() 
{
  // identity
  for (int i=0; i<DIM; i++) 
	{
    for (int j=0; j<DIM; j++) 
		{
      mat[i][j] = ((i == j) ? 1.0 : 0.0);
    }
  }
}

CMatrix::CMatrix(const CMatrix& M)
{
  for (int i=0; i<DIM; i++) 
	{
    for (int j=0; j<DIM; j++) 
		{
      mat[i][j] = M.mat[i][j];
    }
  }
}

CMatrix::~CMatrix() 
{
}

CMatrix& CMatrix::operator=(const CMatrix& M)
{
  for (int i=0; i<DIM; i++) 
	{
    for (int j=0; j<DIM; j++) 
		{
      mat[i][j] = M.mat[i][j];
    }
  }
  return *this;
}

CMatrix IdentityMatrix(void)
{
  CMatrix M;
  return M;
}

CMatrix TranslationMatrix(const CDoublePoint& loc)
{
  CMatrix M = IdentityMatrix();
  
  M.mat[2][0] = loc.x;
  M.mat[2][1] = loc.y;
  
  return M;
}

CMatrix ScaleMatrix(double x, double y)
{
  CMatrix M = IdentityMatrix();
  
  M.mat[0][0] = x;
  M.mat[1][1] = y;
  
  return M;
}

CMatrix RotationMatrix(double angle)
{
  CMatrix M = IdentityMatrix();

  double cosine = cos(angle);
  double sine   = sin(angle);
  
  M.mat[0][0] = cosine;
  M.mat[1][0] = -sine;
  M.mat[0][1] = sine;
  M.mat[1][1] = cosine;
  
  return M;
}

CMatrix& CMatrix::operator+=(const CMatrix& M)
{
  for (int i=0; i<DIM; i++)
  {
    for (int j=0; j<DIM; j++)
    {
      mat[i][j] += M.mat[i][j];
    }
  }

  return *this;
}

CMatrix& CMatrix::operator-=(const CMatrix& M)
{
  for (int i=0; i<DIM; i++)
  {
    for (int j=0; j<DIM; j++)
    {
      mat[i][j] -= M.mat[i][j];
    }
  }

  return *this;
}

CMatrix& CMatrix::operator*=(double A)
{
  for (int i=0; i<DIM; i++)
  {
    for (int j=0; j<DIM; j++)
    {
      mat[i][j] *= A;
    }
  }

  return *this;
}

CMatrix& CMatrix::operator*= (const CMatrix& M)
{
  CMatrix ret = *this;

  int i,j,k;
  for (i=0; i < DIM; i++)
  {
    for (j=0; j<DIM; j++)
    {
      double subt = 0.0;
      for (k=0; k<DIM; k++)
      {
	      subt += (ret.mat[i][k] * M.mat[k][j]);
      }

      mat[i][j] = subt;
    }
  }

  return *this;
}

CMatrix operator+(const CMatrix& M1, const CMatrix& M2)
{
  CMatrix ret;

  for (int i=0; i<DIM; i++)
  {
    for (int j=0; j<DIM; j++)
    {
      ret.mat[i][j] = M1.mat[i][j] + M2.mat[i][j];
    }
  }

  return ret;
}

CMatrix operator- (const CMatrix& M1, const CMatrix& M2)
{
  CMatrix ret;

  for (int i=0; i<DIM; i++)
  {
    for (int j=0; j<DIM; j++)
    {
      ret.mat[i][j] = M1.mat[i][j] - M2.mat[i][j];
    }
  }

  return ret;
}

CMatrix operator* (const CMatrix& M1, const CMatrix& M2)
{
  CMatrix ret;

  for (int i=0; i<DIM; i++)
  {
    for (int j=0; j<DIM; j++)
    {
      double subt = 0.0;
      for (int k=0; k<DIM; k++)
      {
	      subt += (M1.mat[i][k] * M2.mat[k][j]);
      }

      ret.mat[i][j] = subt;
    }
  }

  return ret;
}

CDoublePoint operator* (const CMatrix& M, const CDoublePoint& v)
{
  CDoublePoint ret;
  double denom;

  ret.x = v.x * M.mat[0][0] + v.y * M.mat[1][0] + M.mat[2][0];
  ret.y = v.x * M.mat[0][1] + v.y * M.mat[1][1] + M.mat[2][1];
  denom = M.mat[0][2] + M.mat[1][2] + M.mat[2][2];

  if (denom != 1.0)
  {
    ret /= denom;
  }
  
  return ret;
}

CDoubleRect operator* (const CMatrix& M, const CDoubleRect& r)
{
  CDoubleRect ret;
  double denom;

  CDoublePoint pt;
  pt = CDoublePoint(r.left, r.top);
  pt = M * pt;
  ret.left = pt.x;
  ret.top = pt.y;

  pt = CDoublePoint(r.right, r.bottom);
  pt = M * pt;
  ret.right = pt.x;
  ret.bottom = pt.y;
  
  denom = M.mat[0][2] + M.mat[1][2] + M.mat[2][2];

  ASSERT(denom==1.0);
//  if (denom != 1.0)
//  {
//    ret /= denom;
//  }
  
  return ret;
}

CMatrix operator*(const CMatrix& M, double s)
{
  CMatrix ret;
  
  for (int i=0; i<DIM; i++)
  {
    for (int j=0; j<DIM; j++)
    {
      ret.mat[i][j] = M.mat[i][j] * s;
    }
  }

  return ret;
}

#ifdef _DEBUG
SFString CMatrix::ToString(void) const
{
	SFString str("\n");

  for (int i=0;i<DIM;i++) 
	{
    str += "\t  [";
    for (int j=0;j<DIM;j++) 
		{
      SFString str1;
			str1.Format("% 4.5f ", mat[i][j]);
			str += str1;
    }
    str += "]\n";
  }
	return str;
}
#endif

void CMatrix::Invert()
{
  CMatrix out = IdentityMatrix();

  for (int i=0; i<DIM; i++) 
  {
    if (mat[i][i] != 1.0) 
    {
      double divby = mat[i][i];
      for (int j=0; j<DIM; j++) 
      {
	      out.mat[i][j] /= divby;
        mat[i][j]     /= divby;
      }
    }
    for (int j=0; j<DIM; j++) 
    {
      if (j != i) 
      {
	      if (mat[j][i] != 0.0) 
	      {
          double mulby = mat[j][i];
	        for (int k=0; k<DIM; k++) 
	        {
	          mat[j][k]     -= (mulby * mat[i][k]);
            out.mat[j][k] -= (mulby * out.mat[i][k]);
          }
        }
      }
    }
  }

  *this = out;
}

CMatrix Invert(const CMatrix& M)
{
  CMatrix I = M;
  I.Invert();
  return I;
}

void CMatrix::Transpose()
{
  for (int i=0; i<DIM; i++) 
  {
    for (int j=i; j<DIM; j++)
    {
      if (i != j)
      {
	      double temp = mat[i][j];
	      mat[i][j] = mat[j][i];
        mat[j][i] = temp;
      }
    }
  }
}

CMatrix Transpose(const CMatrix& M)
{
  CMatrix T = M;
  T.Transpose();
  return T;
}

CMatrix ZeroMatrix(void)
{
  CMatrix M;

  for (int i=0; i<DIM; i++)
  {
    for (int j=0; j<DIM; j++)
    {
      M.mat[i][j] = 0.0;
    }
  }
  return M;
}

CMatrix MirrorX2d(void)
{
  CMatrix M = IdentityMatrix();

  M.mat[0][0] = -1.0;

  return M;
}

CMatrix MirrorY2d(void)
{
  CMatrix M = IdentityMatrix();

  M.mat[1][1] = -1.0;

  return M;
}

CMatrix RotationOnly2d(const CMatrix& M)
{
  CMatrix ret = M;
  
  ret.mat[2][0] = 0.0;
  ret.mat[2][1] = 0.0;
  
  return ret;
}

CDoublePoint RotateOnly2d(const CMatrix& M, const CDoublePoint& v)
{
  CDoublePoint ret;
  double denom;

  ret.x = v.x * M.mat[0][0] + v.y * M.mat[1][0];
  ret.y = v.x * M.mat[0][1] + v.y * M.mat[1][1];
  denom = M.mat[0][2] + M.mat[1][2] + M.mat[2][2];
  
  if (denom != 1.0)
  {
    ret /= denom;
  }

  return ret;
}
*/

