//////////////////////////////////////////////////////////////////////
// MatrixBase.inl : implementation of the CMatrixBase class.
//
// Copyright (C) 2002-2020 Derek G Lane
// $Id: MatrixBase.inl,v 1.5 2003/03/01 22:27:19 default Exp $
// U.S. Patent Pending
//////////////////////////////////////////////////////////////////////

#pragma once

// valarray helper class
#undef min
#undef max

#include <valarray>
using namespace std;

// class declaration
#include "MatrixBase.h"

// CVectorN
#include "VectorN.h"

// helper macros
#define SQR(a) ((a)*(a))
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

// maximum iterations for SVD
#define MAX_ITER 30

// constant for finding the pivot during a matrix inversion
#define MAX_TO_PIVOT (1.0)


//////////////////////////////////////////////////////////////////////
// pythag
//
// computes the c side of a triangle from a and b
//////////////////////////////////////////////////////////////////////
template<class TYPE>
TYPE pythag(TYPE a, TYPE b)
{
	TYPE absa = (TYPE) fabs(a);
	TYPE absb = (TYPE) fabs(b);

	if (absa > absb)
	{
		return absa * (TYPE) sqrt(1.0 + SQR(absb / absa));
	}
	else
	{
		return (absb == 0.0 ? 0.0 : absb * (TYPE) sqrt(1.0 + SQR(absa / absb)));
	}

}	// pythag


//////////////////////////////////////////////////////////////////////
// CMatrixBase<TYPE>::SetElements
//
// sets the elements of the matrix
//////////////////////////////////////////////////////////////////////
template<class TYPE>
void CMatrixBase<TYPE>::SetElements(int nCols, int nRows, 
									TYPE *pElements, BOOL bFreeElements)
{
	// delete previous data
	if (m_bFreeElements && NULL != m_pElements)
	{
		delete [] m_pElements;
		m_pElements = NULL;
	}

	if (nCols != m_nCols 
		&& NULL != m_pColumns)
	{
		delete [] m_pColumns;
		m_pColumns = NULL;
	}

	m_nCols = nCols;
	m_nRows = nRows;

	m_pElements = pElements;

	// set up the column vectors
	if (0 != m_nCols)
	{
		if (m_pColumns == NULL)
		{
			// allocate column vectors
			m_pColumns = new CVectorN<TYPE>[GetCols()];
		}

		// initialize the column vectors and the pointers
		for (int nAt = 0; nAt < GetCols(); nAt++)
		{
			// initialize the column vector
			m_pColumns[nAt].SetElements(m_nRows, &m_pElements[nAt * GetRows()],
				FALSE);
		}
	}

	m_bFreeElements = bFreeElements;

}	// CMatrixBase<TYPE>::SetElements


//////////////////////////////////////////////////////////////////////
// CMatrixBase<TYPE>::Transpose
//
// transposes the matrix
//////////////////////////////////////////////////////////////////////
template<class TYPE>
void CMatrixBase<TYPE>::Transpose()
{
	// allocate the elements of the transposed matrix
	TYPE *pElements = new TYPE[GetRows() * GetCols()];

	{
		// make the transposed matrix
		CMatrixBase<TYPE> mTranspose;
		mTranspose.SetElements(GetRows(), GetCols(), pElements, FALSE);
		
		for (int nCol = 0; nCol < GetCols(); nCol++)
		{
			for (int nRow = 0; nRow < GetRows(); nRow++)
			{
				mTranspose[nRow][nCol] = (*this)[nCol][nRow];
			}
		}
	}

	// set the elements of this to the transposed elements
	SetElements(GetRows(), GetCols(), pElements, TRUE);

}	// CMatrixBase<TYPE>::Transpose

