//////////////////////////////////////////////////////////////////////
// VectorN.h: declaration and definition of the CVectorN dynamic 
//		vector template class.
//
// Copyright (C) 1999-2020 Derek G Lane
// $Id: VectorN.h,v 1.6 2003/02/26 05:52:35 default Exp $
//////////////////////////////////////////////////////////////////////

#pragma once

// base class include
#include "VectorBase.h"

// inclue the boost geometry header
#include <boost/numeric/ublas/vector.hpp>
namespace ublas = boost::numeric::ublas;

//////////////////////////////////////////////////////////////////////
// class CVectorN<TYPE>
//
// represents a dynamically sizable mathematical vector with element
//		type given
//////////////////////////////////////////////////////////////////////
template<class TYPE = REAL>
class CVectorN : public CVectorBase<TYPE>
{
public:
	// constructors / destructor
	CVectorN();
	explicit CVectorN(int nDim);
	CVectorN(const CVectorN& vFrom);
	CVectorN(const CVectorBase<TYPE>& vFrom);

	// assignment operator
	CVectorN& operator=(const CVectorN& vFrom);
	CVectorN& operator=(const CVectorBase<TYPE>& vFrom);

	// sets the dimensionality of this vector
	void SetDim(int nDim);

	// returns the boost::ublas matrix
	ublas::vector<TYPE>& as_vector()
	{
		return m_v;
	}

private:
	ublas::vector<TYPE> m_v;
};

//////////////////////////////////////////////////////////////////
// CVectorN<TYPE>::CVectorN<TYPE>
//
// default constructor
//////////////////////////////////////////////////////////////////
template<class TYPE>
CVectorN<TYPE>::CVectorN() 
{
}	// CVectorN<TYPE>::CVectorN<TYPE>


//////////////////////////////////////////////////////////////////
// CVectorN<TYPE>::CVectorN<TYPE>(int nDim) 
//
// construct an arbitrary dimensioned vector
//////////////////////////////////////////////////////////////////
template<class TYPE>
CVectorN<TYPE>::CVectorN(int nDim) 
{
	// set the dimensionality of the vector
	SetDim(nDim);

}	// CVectorN<TYPE>::CVectorN<TYPE>(int nDim) 


//////////////////////////////////////////////////////////////////
// CVectorN<TYPE>::CVectorN<TYPE>(const CVectorN<TYPE>& vFrom)
//
// copy constructor
//////////////////////////////////////////////////////////////////
template<class TYPE>
CVectorN<TYPE>::CVectorN(const CVectorN<TYPE>& vFrom)
{
	// set the dimensionality of the vector
	SetDim(vFrom.GetDim());

	// copy the elements
	(*this) = vFrom;

}	// CVectorN<TYPE>::CVectorN<TYPE>(const CVectorN<TYPE>& vFrom)


//////////////////////////////////////////////////////////////////
// CVectorN<TYPE>::CVectorN<TYPE>(const CVectorBase<TYPE>& vFrom)
//
// copy constructor
//////////////////////////////////////////////////////////////////
template<class TYPE>
CVectorN<TYPE>::CVectorN(const CVectorBase<TYPE>& vFrom)
{
	// set the dimensionality of the vector
	SetDim(vFrom.GetDim());

	// copy the elements
	(*this) = vFrom;

}	// CVectorN<TYPE>::CVectorN<TYPE>(const CVectorBase<TYPE>& vFrom)


//////////////////////////////////////////////////////////////////
// CVectorN<TYPE>::operator=
//
// assignment operator
//////////////////////////////////////////////////////////////////
template<class TYPE>
CVectorN<TYPE>& CVectorN<TYPE>::operator=(const CVectorN<TYPE>& vFrom)
{
	// set the dimensionality of the vector
	SetDim(vFrom.GetDim());

	// copy the elements
	memcpy((*this), vFrom, __min(this->GetDim(), vFrom.GetDim()) * sizeof(TYPE));

	// set remainder of elements to 0
	if (this->GetDim() > vFrom.GetDim())
	{
		memset(&(*this)[vFrom.GetDim()], 0, 
			(this->GetDim() - vFrom.GetDim()) * sizeof(TYPE));
	}

	// return a reference to this
	return (*this);

}	// CVectorN<TYPE>::operator=


//////////////////////////////////////////////////////////////////
// CVectorN<TYPE>::operator=
//
// assignment operator
//////////////////////////////////////////////////////////////////
template<class TYPE>
CVectorN<TYPE>& CVectorN<TYPE>::operator=(const CVectorBase<TYPE>& vFrom)
{
	// set the dimensionality of the vector
	SetDim(vFrom.GetDim());

	// copy the elements
	memcpy((*this), vFrom, __min(this->GetDim(), vFrom.GetDim()) * sizeof(TYPE));

	// set remainder of elements to 0
	if (this->GetDim() > vFrom.GetDim())
	{
		memset(&(*this)[vFrom.GetDim()], 0, 
			(this->GetDim() - vFrom.GetDim()) * sizeof(TYPE));
	}

	// return a reference to this
	return (*this);

}	// CVectorN<TYPE>::operator=


//////////////////////////////////////////////////////////////////
// CVectorN<TYPE>::SetDim
//
// sets the dimensionality of this vector
//////////////////////////////////////////////////////////////////
template<class TYPE>
void CVectorN<TYPE>::SetDim(int nDim)
{
	// do nothing if the dim is already correct
	if (m_nDim != nDim)
	{
		m_v.resize(nDim);
		m_pElements = &m_v.data()[0];
		m_nDim = nDim;
		m_bFreeElements = false;
	}

}	// CVectorN<TYPE>::SetDim


//////////////////////////////////////////////////////////////////////
// operator+(const CVectorN, const CVectorN)
//
// friend function to add two vectors, returning the sum as a new 
//		vector
//////////////////////////////////////////////////////////////////////
template<class TYPE>
inline CVectorN<TYPE> operator+(const CVectorN<TYPE>& vLeft, 
							  const CVectorN<TYPE>& vRight)
{
	// copy left vector to intermediate sum value
	CVectorN<TYPE> vSum(vLeft);

	// add the right vector
	vSum += vRight;

	// return formed sum value
	return vSum;

}	// operator+(const CVectorN, const CVectorN)


//////////////////////////////////////////////////////////////////////
// operator-(const CVectorN, const CVectorN)
//
// friend function to subtract one vector from another, returning 
//		the difference as a new vector
//////////////////////////////////////////////////////////////////////
template<class TYPE>
inline CVectorN<TYPE> operator-(const CVectorN<TYPE>& vLeft, 
							  const CVectorN<TYPE>& vRight)
{
	// copy left vector to intermediate difference value
	CVectorN<TYPE> vDiff(vLeft);

	// subtract the right vector
	vDiff -= vRight;

	// return formed difference value
	return vDiff;

}	// operator-(const CVectorN, const CVectorN)


//////////////////////////////////////////////////////////////////////
// operator*(const CVectorN, const CVectorN)
//
// friend function for vector inner product
//////////////////////////////////////////////////////////////////////
template<class TYPE>
inline TYPE operator*(const CVectorN<TYPE>& vLeft, 
							  const CVectorN<TYPE>& vRight)
{
	return operator*((const CVectorBase<TYPE>&) vLeft,
		(const CVectorBase<TYPE>&) vRight);

}	// operator*(const CVectorN, const CVectorN)


//////////////////////////////////////////////////////////////////////
// operator*(TYPE scalar, const CVectorN)
//
// friend function for scalar multiplication of a vector
//////////////////////////////////////////////////////////////////////
template<class TYPE>
inline CVectorN<TYPE> operator*(const TYPE& scalar, 
							  const CVectorN<TYPE>& vRight)
{
	// copy vector to intermediate product value
	CVectorN<TYPE> vProd(vRight);

	// in-place scalar multiplication
	vProd *= scalar;

	// return the result
	return vProd;

}	// operator*(TYPE scalar, const CVectorN)


//////////////////////////////////////////////////////////////////////
// operator*(const CVectorN, TYPE scalar)
//
// friend function for scalar multiplication of a vector
//////////////////////////////////////////////////////////////////////
template<class TYPE>
inline CVectorN<TYPE> operator*(const CVectorN<TYPE>& vLeft, 
							  const TYPE& scalar)
{
	// copy vector to intermediate product value
	CVectorN<TYPE> vProd(vLeft);

	// in-place scalar multiplication
	vProd *= scalar;

	// return the result
	return vProd;

}	// operator*(const CVectorN, TYPE scalar)


#ifdef __AFX_H__

//////////////////////////////////////////////////////////////////////
// function operator<<(CArchive &ar, CVector)
//
// CArchive serialization of a vector
//////////////////////////////////////////////////////////////////////
template<class TYPE>
CArchive& operator<<(CArchive &ar, CVectorN<TYPE> v)
{
	// store the dimension first
	ar << v.GetDim();

	// store the elements
	for (int nAt = 0; nAt < v.GetDim(); nAt++)
	{
		ar << v[nAt];
	}

	return ar;
}

//////////////////////////////////////////////////////////////////////
// function operator>>(CArchive &ar, CVector)
//
// CArchive de-serialization of a vector
//////////////////////////////////////////////////////////////////////
template<class TYPE>
CArchive& operator>>(CArchive &ar, CVectorN<TYPE>& v)
{
	// retrieve the dimension first
	int nDim;
	ar >> nDim;
	v.SetDim(nDim);

	// retrieve the elements
	for (int nAt = 0; nAt < v.GetDim(); nAt++)
	{
		ar >> v[nAt];
	}

	return ar;
}

#endif	// __AFX_H__
