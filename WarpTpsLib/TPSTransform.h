//////////////////////////////////////////////////////////////////////
// TPSTransform.h: interface for the CTPSTransform class.
//
// Copyright (C) 2002-2020 Derek Lane
// $Id: TPSTransform.h,v 1.3 2003/04/26 20:52:39 default Exp $
//////////////////////////////////////////////////////////////////////

#pragma once

// vector includes
#include "VectorD.h"

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

	typedef std::tuple<CVectorD<3>::Point_t, CVectorD<3>::Point_t> LandmarkTuple_t;
	const LandmarkTuple_t CTPSTransform::GetLandmarkTuple(int nIndex);

	template<int DATASET>
	void SetLandmark(int nIndex, const CVectorD<3>& vLandmark);

	void SetLandmarkTuple(int nIndex, CTPSTransform::LandmarkTuple_t);

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
	void Eval(const CVectorD<3>::Point_t& vPos, CVectorD<3>::Point_t& vOffset, float percent);

	// resample pixels
	void ResampleRaw(LPBYTE pSrcPixels, LPBYTE pDstPixels, UINT bytesPerPixel, UINT width, UINT height, UINT stride, float percent);

	// resample pixels
	void ResampleRawWithField(LPBYTE pSrcPixels, LPBYTE pDstPixels, UINT bytesPerPixel, UINT width, UINT height, UINT stride, float percent);

protected:
	// recalculates the TPS from the landmarks
	void RecalcWeights();

	// used to construct the presampled vector field
	void Presample(int width, int height);

private:
	// the array of landmarks
	vector<tuple<CVectorD<3>, CVectorD<3>>> m_arrLandmarkTuples;

	// represents the pre-sampled array
	vector<CVectorD<3>::Point_t> m_presampledOffsets;
	int m_presampledWidth;
	int m_presampledHeight;

	// stores the inverse of the distance matrix
	ublas::matrix<REAL> m_mL_inv;

	// the final weight vectors
	ublas::vector<REAL> m_vWx;
	ublas::vector<REAL> m_vWy;

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
inline double distance_function(const CVectorD<3>::Point_t& vL1, const CVectorD<3>::Point_t& vL2, const REAL k = 1.0, REAL r_exp = 2.0)
{
	// compute the euclidean distance
	auto diff = vL1;
	bg::subtract_point(diff, vL2);
	double r = sqrt(diff.get<X>()*diff.get<X>() + diff.get<Y>()*diff.get<Y>());

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
// CTPSTransform::GetLandmarkTuple
// returns a landmark tuple
//////////////////////////////////////////////////////////////////////
inline const CTPSTransform::LandmarkTuple_t
	CTPSTransform::GetLandmarkTuple(int nIndex)
{
	CVectorD<3, REAL> vL0, vL1;
	std::tie(vL0, vL1) = m_arrLandmarkTuples[nIndex];
	return std::make_tuple(vL0.point(), vL1.point());
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

inline void CTPSTransform::SetLandmarkTuple(int nIndex, CTPSTransform::LandmarkTuple_t toTuple)
{
	Point_t pt0, pt1;
	std::tie(pt0, pt1) = toTuple;
	CVectorD<3, REAL> v0(pt0.get<X>(), pt0.get<Y>(), pt0.get<Z>());
	SetLandmark<0>(nIndex, v0);
	CVectorD<3, REAL> v1(pt1.get<X>(), pt1.get<Y>(), pt1.get<Z>());
	SetLandmark<1>(nIndex, v1);
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
inline void CTPSTransform::Eval(const CVectorD<3>::Point_t& vPos, CVectorD<3>::Point_t& vOffset, float percent)
{
	// ensure vOffset is initialized to zero
	bg::assign_zero(vOffset);

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
		CVectorD<3, REAL>::Point_t vL0;
		std::tie(vL0, ignore) = GetLandmarkTuple(nAt);

		// distance to the first landmark
		double d = distance_function(vPos, vL0, m_k, m_r_exp);

		// add weight vector displacements
		CVectorD<3,REAL>::Point_t displacement(m_vWx(nAt), m_vWy(nAt));
		bg::multiply_value(displacement, d * percent);
		bg::add_point(vOffset, displacement);
	}

	// add the affine displacements
	bg::add_point(vOffset, CVectorD<3, REAL>::Point_t(m_vWx(n + 0), m_vWy(n + 0)));

	CVectorD<3, REAL>::Point_t weightx(m_vWx(n + 1), m_vWy(n + 1));
	bg::multiply_value(weightx, vPos.get<X>());
	bg::add_point(vOffset, weightx);

	CVectorD<3, REAL>::Point_t weighty(m_vWx(n + 2), m_vWy(n + 2));
	bg::multiply_value(weighty, vPos.get<Y>());
	bg::add_point(vOffset, weighty);
}


//////////////////////////////////////////////////////////////////////
// CTPSTransform::Resample
// 
// resamples a source image through the transform
//////////////////////////////////////////////////////////////////////
inline void CTPSTransform::Presample(int width, int height)
{
	if (width != m_presampledWidth
		|| height != m_presampledHeight) {
		m_presampledWidth = width;
		m_presampledHeight = height;
		m_presampledOffsets.resize(width * height);
		m_bRecalcPresample = TRUE;
	}

	if (m_bRecalcPresample) {
		// position of the destination
		CVectorD<3>::Point_t vDstPos(0.0, 0.0, 0.0);

		// for each pixel in the image
		for (int dstAtY = 0; vDstPos.get<Y>() < height; dstAtY++) {
			bg::set<X>(vDstPos, 0.0);
			for (int dstAtX = 0; vDstPos.get<X>() < width; dstAtX++) {
				Eval(vDstPos, m_presampledOffsets[dstAtY * m_presampledWidth + dstAtX], 1.0);
				bg::set<X>(vDstPos, vDstPos.get<X>() + 1.0);
			}
			bg::set<Y>(vDstPos, vDstPos.get<Y>() + 1.0);
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
	// see if a recalc is needed
	if (m_bRecalc) {

		// recalculate the weight vectors
		RecalcWeights();
	}

	// position of the destination
	CVectorD<3> vDstPos;

	// position of the source
	CVectorD<3> vOffset;

	// for each pixel in the image
	for (vDstPos[1] = 0.0; vDstPos[1] < height; vDstPos[1] += 1.0)
	{
		for (vDstPos[0] = 0.0; vDstPos[0] < width; vDstPos[0] += 1.0)
		{
			Eval(vDstPos.point(), vOffset.point(), percent);
			CVectorD<3>::Point_t vSrcPos(vDstPos.point());
			bg::add_point(vSrcPos, vOffset.point());

			// compute the destination position
			int nDstY = height - (int)vDstPos[1] - 1;
			int nDstIndex = bytesPerPixel * (int)vDstPos[0]
				+ nDstY * stride;

			// bounds check source position
			int nSrcY = height - (int)floor(vSrcPos.get<Y>() + 0.5) - 1;
			if (vSrcPos.get<X>() >= 0.0 && vSrcPos.get<X>() < width
				&& nSrcY >= 0 && nSrcY < (int) height)
			{
				// compute the positions
				int nSrcIndex = bytesPerPixel * (int)floor(vSrcPos.get<X>() + 0.5)
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
	if (m_bRecalc) {

		// recalculate the weight vectors
		RecalcWeights();
	}

	// if there are no landmarks, then no presampled vector field
	if (m_bRecalcPresample
		|| m_presampledWidth != width
		|| m_presampledHeight != height) {

		Presample(width, height);
	}

	CVectorD<3>::Point_t vSrcPos(0.0, 0.0, 0.0);
	for (int dstAtY = 0; dstAtY < (int)height; dstAtY++) {
		for (int dstAtX = 0; dstAtX < (int)width; dstAtX++) {
			// compute the destination position
			int nDstY = height - dstAtY - 1;
			int nDstIndex = bytesPerPixel * dstAtX
				+ nDstY * stride;

			const CVectorD<3>::Point_t& offset = m_presampledOffsets[dstAtY * m_presampledWidth + dstAtX];
			bg::set<0>(vSrcPos, dstAtX + ((double)percent) * offset.get<0>());
			bg::set<1>(vSrcPos, dstAtY + ((double)percent) * offset.get<1>());

			// bounds check source position
			int nSrcY = height - (int)floor(vSrcPos.get<1>() + 0.5) - 1;
			if (vSrcPos.get<0>() >= 0.0 && vSrcPos.get<0>() < width
				&& nSrcY >= 0 && nSrcY < (int) height) {
				// compute the positions
				int nSrcIndex = bytesPerPixel * (int)floor(vSrcPos.get<0>() + 0.5)
					+ nSrcY * stride;

				// and resample
				memcpy(&pDstPixels[nDstIndex], &pSrcPixels[nSrcIndex],
					bytesPerPixel);
			} else {
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
						distance_function(GetLandmark<0>(nAtRow).point(),
							GetLandmark<0>(nAtCol).point(), m_k, m_r_exp);
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
		m_mL_inv.resize(n + 3, n + 3);
		invert(mL, m_mL_inv);

		m_bRecalcMatrix = FALSE;
	}

	// compute the x- and y-direction "heights"
	ublas::vector<REAL> vHx(n + 3);
	ublas::vector<REAL> vHy(n + 3);

	int nAtLandmark = 0;
	for (; nAtLandmark < n; nAtLandmark++) {
		CVectorD<3, REAL>::Point_t vL0, vL1;
		std::tie(vL0, vL1) = GetLandmarkTuple(nAtLandmark);
		vHx(nAtLandmark) = vL1.get<X>() - vL0.get<X>();
		vHy(nAtLandmark) = vL1.get<Y>() - vL0.get<Y>();
	}
	for (; nAtLandmark < n + 3; nAtLandmark++) {
		vHx(nAtLandmark) = 0.0;
		vHy(nAtLandmark) = 0.0;
	}

	m_vWx = ublas::prod(m_mL_inv, vHx);
	m_vWy = ublas::prod(m_mL_inv, vHy);

	// unset flag
	m_bRecalc = FALSE;
}
