//////////////////////////////////////////////////////////////////////
// TPSTransform.h: interface for the CTPSTransform class.
//
// Copyright (C) 2002-2020 Derek Lane
// $Id: TPSTransform.h,v 1.3 2003/04/26 20:52:39 default Exp $
//////////////////////////////////////////////////////////////////////

#pragma once

// container vector for the landmarks
#include <vector>
using namespace std;

// vector includes
#include "VectorD.h"
#include "VectorN.h"

// matrix
#include "MatrixNxM.h"

// model object base class
#include "ModelObject.h"

//////////////////////////////////////////////////////////////////////
// class CTPSTransform
// 
// represents a basic TPS transform.  holds a collection of position
//		vector pairs and interpolates the field from those
//////////////////////////////////////////////////////////////////////
class WARPTPSLIB_API CTPSTransform : public CModelObject
{
public:
	// construction/destruction
	CTPSTransform();
	virtual ~CTPSTransform();

	// landmark accessors
	int GetLandmarkCount();
	const CVectorD<3>& GetLandmark(int nDataSet, int nIndex);
	void SetLandmark(int nDataSet, int nIndex, const CVectorD<3>& vLandmark);

	// functions to add landmark points
	int AddLandmark(const CVectorD<3>& vLandmark);
	int AddLandmark(const CVectorD<3>& vLandmark1, 
		const CVectorD<3>& vLandmark2);

	// removes all landmarks from the transform
	void RemoveAllLandmarks();

	// sets the r-param
	void SetRExponent(float r_exp) 
	{ 
		m_r_exp = r_exp; 
		m_bRecalc = TRUE;
		m_bRecalcPresample = TRUE;
	}

	void SetK(float k)
	{
		m_k = k;
		m_bRecalc = TRUE;
		m_bRecalcPresample = TRUE;
	}

	// evaluates the field at a point
	// returns the offset vector, so the mapped point can be derived by adding the position and offset
	void Eval(const CVectorD<3>& vPos, CVectorD<3>& vOffset, float percent);

	// resample pixels
	void ResampleRaw(LPBYTE pSrcPixels, LPBYTE pDstPixels, UINT bytesPerPixel, UINT width, UINT height, UINT stride, float percent);

	// resample pixels
	void ResampleRawWithField(LPBYTE pSrcPixels, LPBYTE pDstPixels, UINT bytesPerPixel, UINT width, UINT height, UINT stride, float percent);

	// helper to check that another transform is set up as the inverse of this one
	BOOL CheckInverse(CTPSTransform* pInverse);

protected:
	// recalculates the TPS from the landmarks
	void RecalcWeights();

	// used to construct the presampled vector field
	void Presample(int width, int height);

private:
	// the array of landmarks
	vector< CVectorD<3> > m_arrLandmarks[2];

	// represents the pre-sampled array
	vector< CVectorD<3> > m_presampledOffsets;
	int m_presampledWidth;
	int m_presampledHeight;

	// stores the inverse of the distance matrix
	CMatrixNxM<> m_mL_inv;

	// the final weight vectors
	CVectorN<> m_vWx;
	CVectorN<> m_vWy;

	// the radial basis exponent
	float m_r_exp;
	float m_k;

	// flag to indicate that recalculation of the TPS is needed
	BOOL m_bRecalcMatrix;
	BOOL m_bRecalc;
	BOOL m_bRecalcPresample;
};
