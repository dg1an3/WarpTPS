//////////////////////////////////////////////////////////////////////
// TPSTransform.h: interface for the CTPSTransform class.
//
// Copyright (C) 2002-2020 Derek Lane
// $Id: TPSTransform.h,v 1.3 2003/04/26 20:52:39 default Exp $
//////////////////////////////////////////////////////////////////////

#pragma once

// container vector for the landmarks
#include <vector>
#include <tuple>


// vector includes
#include "VectorD.h"
#include "VectorN.h"

// matrix
#include "MatrixNxM.h"
#include "MatrixBase.inl"

// model object base class
#include "ModelObject.h"

//////////////////////////////////////////////////////////////////////
// class CTPSTransform
// 
// represents a basic TPS transform.  holds a collection of position
//		vector pairs and interpolates the field from those
//////////////////////////////////////////////////////////////////////
class CTPSTransform : public CModelObject
{
public:
	// construction/destruction
	CTPSTransform();
	virtual ~CTPSTransform();

	// landmark accessors
	int GetLandmarkCount();

	template<int DATASET>
	const CVectorD<3>& GetLandmark(int nIndex);

	template<int DATASET>
	void SetLandmark(int nIndex, const CVectorD<3>& vLandmark);

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
	vector<tuple<CVectorD<3>, CVectorD<3>>> m_arrLandmarkTuples;

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



//////////////////////////////////////////////////////////////////////
// distance_function
// 
// returns the log-distance squared betweeen two landmarks
//////////////////////////////////////////////////////////////////////
inline double distance_function(const CVectorD<3>& vL1, const CVectorD<3>& vL2, const REAL k = 1.0, REAL r_exp = 2.0)
{
	// compute the euclidean distance
	double r = (vL1 - vL2).GetLength();

#ifdef SQUARE_ONLY
	// pass through the log-squared
	return (r > 0.0) ? (k * r * r * log(r)) : 0.0;
#else
	return (r > 0.0) ? (k * pow(r, r_exp) * log(r)) : 0.0;
#endif
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CTPSTransform::CTPSTransform
// 
// constructs a CTPSTransform object with the given name
//////////////////////////////////////////////////////////////////////
inline CTPSTransform::CTPSTransform()
	: m_bRecalcMatrix(TRUE)
	, m_bRecalc(TRUE)
	, m_bRecalcPresample(TRUE)
	, m_presampledWidth(0)
	, m_presampledHeight(0)
	, m_k(1.0)
	, m_r_exp(2.0)
{
}

//////////////////////////////////////////////////////////////////////
// CTPSTransform::CTPSTransform
// 
// destroys the TPSTransform
//////////////////////////////////////////////////////////////////////
inline CTPSTransform::~CTPSTransform()
{
}

//////////////////////////////////////////////////////////////////////
// CTPSTransform::GetLandmarkCount
// 
// returns the number of defined landmarks for the TPS
//////////////////////////////////////////////////////////////////////
inline int CTPSTransform::GetLandmarkCount()
{
	return (int)m_arrLandmarkTuples.size();
}

//////////////////////////////////////////////////////////////////////
// CTPSTransform::GetLandmark
// 
// returns a landmark
//////////////////////////////////////////////////////////////////////
template<int DATASET>
inline const CVectorD<3>& CTPSTransform::GetLandmark(int nIndex)
{
	return std::get<DATASET>(m_arrLandmarkTuples[nIndex]);
}

//////////////////////////////////////////////////////////////////////
// CTPSTransform::SetLandmark
// 
// constructs a CTPSTransform object with the given name
//////////////////////////////////////////////////////////////////////
template<int DATASET>
inline void CTPSTransform::SetLandmark(int nIndex, const CVectorD<3>& vLandmark)
{
	std::get<DATASET>(m_arrLandmarkTuples[nIndex]) = vLandmark;

	// only recalc matrix if it is dataset 0
	if (DATASET == 0)
	{
		m_bRecalcMatrix = TRUE;
	}

	// set the flag to indicate recalculation is needed
	m_bRecalc = TRUE;
	m_bRecalcPresample = TRUE;

}

//////////////////////////////////////////////////////////////////////
// CTPSTransform::AddLandmark
// 
// adds a new landmark to the TPS
//////////////////////////////////////////////////////////////////////
inline int CTPSTransform::AddLandmark(const CVectorD<3>& vLandmark)
{
	// return the index of the new landmark
	return AddLandmark(vLandmark, vLandmark);
}

//////////////////////////////////////////////////////////////////////
// CTPSTransform::AddLandmark
// 
// adds a new landmark to the TPS
//////////////////////////////////////////////////////////////////////
inline int CTPSTransform::AddLandmark(const CVectorD<3>& vLandmark1,
	const CVectorD<3>& vLandmark2)
{
	// add the landmark as a tuple
	m_arrLandmarkTuples.push_back(std::make_tuple(vLandmark1, vLandmark2));

	// set the flag to indicate recalculation is needed
	m_bRecalcMatrix = TRUE;
	m_bRecalc = TRUE;
	m_bRecalcPresample = TRUE;

	// return the index of the new landmark
	return GetLandmarkCount() - 1;
}

//////////////////////////////////////////////////////////////////////
// CTPSTransform::RemoveAllLandmarks
// 
// removes all landmarks from the transform
//////////////////////////////////////////////////////////////////////
inline void CTPSTransform::RemoveAllLandmarks()
{
	m_arrLandmarkTuples.clear();

	m_bRecalcMatrix = TRUE;
	m_bRecalc = TRUE;
	m_bRecalcPresample = TRUE;
}

//////////////////////////////////////////////////////////////////////
// CTPSTransform::Eval
// 
// evaluates the vector field at a point
//////////////////////////////////////////////////////////////////////
inline void CTPSTransform::Eval(const CVectorD<3>& vPos, CVectorD<3>& vOffset, float percent)
{
	// TODO: should vOffset be initialized to zero?
	// start with transformed equal to input
	// vPosTrans = vPos;

	// don't compute without at least three landmarks
	auto n = GetLandmarkCount();
	if (n < 3)
	{
		return;
	}

	// see if a recalc is needed
	if (m_bRecalc)
	{
		// recalculate the weight vectors
		RecalcWeights();
	}

	// add the weight vector displacements
	for (int nAt = 0; nAt < n; nAt++)
	{
		// distance to the first landmark
		double d = distance_function(vPos, GetLandmark<0>(nAt), m_k, m_r_exp);

		// add weight vector displacements
		Point3d_t displacement(m_vWx[nAt], m_vWy[nAt]);
		bg::multiply_value(displacement, d * percent);
		bg::add_point(vOffset.point(), displacement);
	}

	// add the affine displacements
	bg::add_point(vOffset.point(), Point3d_t(m_vWx[n + 0], m_vWy[n + 0]));

	Point3d_t weightx(m_vWx[n + 1], m_vWy[n + 1]);
	bg::multiply_value(weightx, vPos[0]);
	bg::add_point(vOffset.point(), weightx);

	Point3d_t weighty(m_vWx[n + 2], m_vWy[n + 2]);
	bg::multiply_value(weighty, vPos[1]);
	bg::add_point(vOffset.point(), weighty);
}


//////////////////////////////////////////////////////////////////////
// CTPSTransform::Resample
// 
// resamples a source image through the transform
//////////////////////////////////////////////////////////////////////
inline void CTPSTransform::Presample(int width, int height)
{
	if (width != m_presampledWidth
		|| height != m_presampledHeight)
	{
		m_presampledWidth = width;
		m_presampledHeight = height;
		m_presampledOffsets.resize(width * height);
		m_bRecalcPresample = TRUE;
	}

	CVectorD<3> vZero;
	for (int nAt = 0; nAt < (int)m_presampledOffsets.size(); nAt++)
		m_presampledOffsets[nAt] = vZero;

	if (m_bRecalcPresample)
	{
		// position of the destination
		CVectorD<3> vDstPos;

		// position of the source
		CVectorD<3> vSrcPos;

		// for each pixel in the image
		int dstAtY = 0;
		for (dstAtY = 0, vDstPos[1] = 0.0; vDstPos[1] < height; dstAtY++, vDstPos[1] += 1.0)
		{
			int dstAtX = 0;
			for (dstAtX = 0, vDstPos[0] = 0.0; vDstPos[0] < width; dstAtX++, vDstPos[0] += 1.0)
			{
				CVectorD<3>& vOffset = m_presampledOffsets[dstAtY * m_presampledWidth + dstAtX];
				Eval(vDstPos, vOffset, 1.0);

				// remove initial position to leave true offset
				// vSrcPos -= vDstPos;
			}
		}

		m_bRecalcPresample = FALSE;
	}
}

//
inline void CTPSTransform::ResampleRaw(LPBYTE pSrcPixels, LPBYTE pDstPixels,
	UINT bytesPerPixel,
	UINT width,
	UINT height,
	UINT stride,
	float percent)
{
	// position of the destination
	CVectorD<3> vDstPos;

	// position of the source
	CVectorD<3> vOffset;
	CVectorD<3> vSrcPos;

	// for each pixel in the image
	for (vDstPos[1] = 0.0; vDstPos[1] < height; vDstPos[1] += 1.0)
	{
		for (vDstPos[0] = 0.0; vDstPos[0] < width; vDstPos[0] += 1.0)
		{
			Eval(vDstPos, vOffset, percent);
			vSrcPos = vDstPos + vOffset;

			// compute the destination position
			int nDstY = height - (int)vDstPos[1] - 1;
			int nDstIndex = bytesPerPixel * (int)vDstPos[0]
				+ nDstY * stride;

			// bounds check source position
			int nSrcY = height - (int)floor(vSrcPos[1] + 0.5) - 1;
			if (vSrcPos[0] >= 0.0 && vSrcPos[0] < width
				&& nSrcY >= 0 && nSrcY < (int) height)
			{
				// compute the positions
				int nSrcIndex = bytesPerPixel * (int)floor(vSrcPos[0] + 0.5)
					+ nSrcY * stride;

				// and resample
				memcpy(&pDstPixels[nDstIndex], &pSrcPixels[nSrcIndex],
					bytesPerPixel);
			}
			else
			{
				// set to zero
				memset(&pDstPixels[nDstIndex], 0, bytesPerPixel);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CTPSTransform::ResampleRawWithField
//
// resamples raw pixels using the presampled vector field
//////////////////////////////////////////////////////////////////////
inline void CTPSTransform::ResampleRawWithField(LPBYTE pSrcPixels, LPBYTE pDstPixels,
	UINT bytesPerPixel,
	UINT width,
	UINT height,
	UINT stride,
	float percent)
{
	// see if a recalc is needed
	if (m_bRecalc)
	{
		// recalculate the weight vectors
		RecalcWeights();
	}

	// if there are no landmarks, then no presampled vector field
	if (m_bRecalcPresample
		|| m_presampledWidth != width
		|| m_presampledHeight != height)
	{
		Presample(width, height);
	}

	CVectorD<3> vSrcPos;
	for (int dstAtY = 0; dstAtY < (int)height; dstAtY++)
	{
		for (int dstAtX = 0; dstAtX < (int)width; dstAtX++)
		{
			// compute the destination position
			int nDstY = height - dstAtY - 1;
			int nDstIndex = bytesPerPixel * dstAtX
				+ nDstY * stride;

			vSrcPos[0] = dstAtX;
			vSrcPos[1] = dstAtY;
			vSrcPos += ((double)percent) * m_presampledOffsets[dstAtY * m_presampledWidth + dstAtX];

			// bounds check source position
			int nSrcY = height - (int)floor(vSrcPos[1] + 0.5) - 1;
			if (vSrcPos[0] >= 0.0 && vSrcPos[0] < width
				&& nSrcY >= 0 && nSrcY < (int) height)
			{
				// compute the positions
				int nSrcIndex = bytesPerPixel * (int)floor(vSrcPos[0] + 0.5)
					+ nSrcY * stride;

				// and resample
				memcpy(&pDstPixels[nDstIndex], &pSrcPixels[nSrcIndex],
					bytesPerPixel);
			}
			else
			{
				// set to zero
				memset(&pDstPixels[nDstIndex], 0, bytesPerPixel);
			}
		}
	}

}


//////////////////////////////////////////////////////////////////////
// CTPSTransform::RecalcWeights
// 
// recalcs the TPS weights from the landmarks
//////////////////////////////////////////////////////////////////////
inline void CTPSTransform::RecalcWeights()
{
	auto n = GetLandmarkCount();
	// don't compute without at least three landmarks
	if (n < 3) {
		return;
	}

	if (m_bRecalcMatrix) {
		// stores the L matrix
		ublas::matrix<REAL> mL(n + 3, n + 3);

		// iterate over the rows and columns of the L matrix
		for (int nAtRow = 0; nAtRow < n; nAtRow++) {
			for (int nAtCol = 0; nAtCol < n; nAtCol++) {				
				if (nAtRow != nAtCol) {	// are we off-diagonal?
					// populate the K part of the matrix
					mL(nAtCol,nAtRow) =
						distance_function(GetLandmark<0>(nAtRow),
							GetLandmark<0>(nAtCol), m_k, m_r_exp);
				} else {				// zeros on the diagonal
					mL(nAtCol, nAtRow) = 0.0;
				}

				// populate the Q^T part of the matrix
				mL(nAtCol, n + 0) = 1.0;
				mL(nAtCol, n + 1) = GetLandmark<0>(nAtCol)[0];
				mL(nAtCol, n + 2) = GetLandmark<0>(nAtCol)[1];
			}

			// populate the Q part of the matrix
			mL(n + 0, nAtRow) = 1.0;
			mL(n + 1, nAtRow) = GetLandmark<0>(nAtRow)[0];
			mL(n + 2, nAtRow) = GetLandmark<0>(nAtRow)[1];
		}

		// fill the lower-right 3x3 with zeros
		for (int nAtRow = n; nAtRow < n + 3; nAtRow++) {
			for (int nAtCol = n; nAtCol < n + 3; nAtCol++) {
				mL(nAtCol, nAtRow) = 0.0;
			}
		}

		// form the inverse of L
		m_mL_inv.Reshape(n + 3, n + 3);
		invert(mL, m_mL_inv.as_matrix());

		m_bRecalcMatrix = FALSE;
	}

	// compute the x- and y-direction "heights"
	CVectorN<> vHx(n + 3);
	CVectorN<> vHy(n + 3);
	for (int nAtLandmark = 0; nAtLandmark < n; nAtLandmark++) {
		vHx[nAtLandmark] = GetLandmark<1>(nAtLandmark)[0]
			- GetLandmark<0>(nAtLandmark)[0];

		vHy[nAtLandmark] = GetLandmark<1>(nAtLandmark)[1]
			- GetLandmark<0>(nAtLandmark)[1];
	}

	auto vWx = ublas::prod(m_mL_inv.as_matrix(), vHx.as_vector());
	auto vWy = ublas::prod(m_mL_inv.as_matrix(), vHy.as_vector());

	// compute the weight vectors
	m_vWx.SetDim(vHx.GetDim());
	m_vWy.SetDim(vHy.GetDim());
#if COMPARE_UBLAS
	m_vWx = m_mL_inv * vHx;
	m_vWy = m_mL_inv * vHy;

	// std::stringstream ss; ss << vWx << " == " << m_vWx << endl; auto vWx_str = ss.str();
	for (int at = 0; at < n + 3; at++) {
		ASSERT(vWx(at) == m_vWx[at]);
		ASSERT(vWy(at) == m_vWy[at]);
	}
#else
	std::copy(vWx.begin(), vWx.end(), m_vWx.as_vector().begin());
	std::copy(vWy.begin(), vWy.end(), m_vWy.as_vector().begin());
#endif

	// unset flag
	m_bRecalc = FALSE;

#ifdef _DEBUG
	// now check to ensure the offsets at each landmark is correct
	for (int nAtLandmark = 0; nAtLandmark < n; nAtLandmark++) {
		const CVectorD<3>& vL0 = GetLandmark<0>(nAtLandmark);
		const CVectorD<3>& vL1 = GetLandmark<1>(nAtLandmark);

		CVectorD<3> vOffset;
		Eval(vL0, vOffset, 1.0);
		CVectorD<3> vL0_xform = vL0 + vOffset;

		ASSERT(vL0_xform.IsApproxEqual(vL1));
	}
#endif
}

//////////////////////////////////////////////////////////////////////
// CTPSTransform::RecalcWeights
// 
// checks that the two transforms are inverses as per the landmarks
//////////////////////////////////////////////////////////////////////
BOOL
inline CTPSTransform::CheckInverse(CTPSTransform* pInverse)
{
	// now check to ensure the offsets at each landmark is correct
	for (int nAtLandmark = 0; nAtLandmark < GetLandmarkCount(); nAtLandmark++)
	{
		const CVectorD<3>& vL0 = GetLandmark<0>(nAtLandmark);
		const CVectorD<3>& vL0_other = pInverse->GetLandmark<1>(nAtLandmark);
		if (!vL0.IsApproxEqual(vL0_other))
			return FALSE;

		const CVectorD<3>& vL1 = GetLandmark<1>(nAtLandmark);
		const CVectorD<3>& vL1_other = pInverse->GetLandmark<0>(nAtLandmark);
		if (!vL1.IsApproxEqual(vL1_other))
			return FALSE;
	}

	return TRUE;
}
