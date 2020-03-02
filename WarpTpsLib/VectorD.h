//////////////////////////////////////////////////////////////////////
// Vector.h: declaration and definition of the CVectorD template class.
//
// Copyright (C) 1999-2020 Derek G Lane
// $Id: VectorD.h,v 1.8 2003/04/26 20:38:19 default Exp $
//////////////////////////////////////////////////////////////////////

#pragma once

// math utilities
#include "MathUtil.h"

// base class include
#include "VectorBase.h"

//////////////////////////////////////////////////////////////////////
// class CVectorD<DIM, TYPE>
//
// represents a mathematical vector with dimension and type given
//////////////////////////////////////////////////////////////////////
template<int DIM = 4, class TYPE = double>
class CVectorD : public CVectorBase<TYPE>
{
public:
	// constructors / destructor
	CVectorD();
	CVectorD(TYPE x);
	CVectorD(TYPE x, TYPE y);
	CVectorD(TYPE x, TYPE y, TYPE z);
	CVectorD(TYPE x, TYPE y, TYPE z, TYPE w);
	CVectorD(const CVectorD& vFrom);
	explicit CVectorD(const CVectorBase<TYPE>& vFrom);
#ifdef __AFX_H__
	CVectorD(const CPoint& pt);
#endif
	~CVectorD();

	// assignment
	CVectorD& operator=(const CVectorD& vFrom);

	// conversion
#ifdef __AFX_H__
	operator CPoint() const;
#endif

	// tests for approximate equality using the EPS defined at the 
	//		top of this file
	BOOL IsApproxEqual(const CVectorD& v, TYPE epsilon = DEFAULT_EPSILON) const;

	// assignment operators -- requires proper type for strict
	//		compile-time check
	CVectorD& operator+=(const CVectorD& vRight);
	CVectorD& operator-=(const CVectorD& vRight);

	typedef bg::model::point<TYPE, DIM, bg::cs::cartesian> Point_t;
	Point_t& point() const { return const_cast<Point_t&>(m_point);  }

private:
	// the vector's elements
	Point_t m_point;
};


// use with Point_t
const int X = 0;
const int Y = 1;
const int Z = 2;
typedef CVectorD<3, REAL>::Point_t Point_t;


//////////////////////////////////////////////////////////////////
// CVectorD<DIM, TYPE>::CVectorD<DIM, TYPE>() 
//
// default constructor
//////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
CVectorD<DIM, TYPE>::CVectorD() 
{
	// set the elements pointer to the static array 
	//		and initialize the dimension
	SetElements(DIM, const_cast<TYPE*>(&this->point().get<0>()), FALSE);

	// clear to all zeros
	memset((*this), 0, DIM * sizeof(TYPE));

}	// CVectorD<DIM, TYPE>::CVectorD<DIM, TYPE>() 


//////////////////////////////////////////////////////////////////
// CVectorD<DIM, TYPE>::CVectorD<DIM, TYPE>(TYPE x) 
//
// construct from one element
//////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
CVectorD<DIM, TYPE>::CVectorD(TYPE x) 
{
	// set the elements pointer to the static array 
	//		and initialize the dimension
	SetElements(DIM, const_cast<TYPE*>(&this->point().get<0>()), FALSE);

	// set the given elements
	(*this)[0] = x;

	// clear to all zeros
	if (DIM > 1)
	{
		memset(&(*this)[1], 0, (DIM-1) * sizeof(TYPE));
	}

}	// CVectorD<DIM, TYPE>::CVectorD<DIM, TYPE>(TYPE x) 


//////////////////////////////////////////////////////////////////
// CVectorD<DIM, TYPE>::CVectorD<DIM, TYPE>(TYPE x, TYPE y) 
//
// construct from two elements
//////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
CVectorD<DIM, TYPE>::CVectorD(TYPE x, TYPE y) 
{
	// set the elements pointer to the static array 
	//		and initialize the dimension
	SetElements(DIM, const_cast<TYPE*>(&this->point().get<0>()), FALSE);

	// set the given elements
	(*this)[0] = x;
	(*this)[1] = y;

	// clear to all zeros
	if (DIM > 2)
	{
		memset(&(*this)[2], 0, (DIM-2) * sizeof(TYPE));
	}

}	// CVectorD<DIM, TYPE>::CVectorD<DIM, TYPE>(TYPE x, TYPE y) 


//////////////////////////////////////////////////////////////////
// CVectorD<DIM, TYPE>::CVectorD<DIM, TYPE>(TYPE x, TYPE y, TYPE z) 
//
// construct from three elements
//////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
CVectorD<DIM, TYPE>::CVectorD(TYPE x, TYPE y, TYPE z) 
{
	// set the elements pointer to the static array 
	//		and initialize the dimension
	SetElements(DIM, const_cast<TYPE*>(&this->point().get<0>()), FALSE);

	// set the given elements
	(*this)[0] = x;
	(*this)[1] = y;
	(*this)[2] = z;

	// clear to all zeros
	if (DIM > 3)
	{
		memset(&(*this)[3], 0, (DIM-3) * sizeof(TYPE));
	}

}	// CVectorD<DIM, TYPE>::CVectorD<DIM, TYPE>(TYPE x, TYPE y, TYPE z) 


//////////////////////////////////////////////////////////////////
// CVectorD<DIM, TYPE>::CVectorD<DIM, TYPE>(TYPE x, TYPE y, TYPE z, TYPE w) 
//
// construct from four elements
//////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
CVectorD<DIM, TYPE>::CVectorD(TYPE x, TYPE y, TYPE z, TYPE w) 
{ 
	// set the elements pointer to the static array 
	//		and initialize the dimension
	SetElements(DIM, const_cast<TYPE*>(&this->point().get<0>()), FALSE);

	// set the given elements
	(*this)[0] = x;
	(*this)[1] = y;
	(*this)[2] = z;
	(*this)[3] = w;

	// clear to all zeros
	if (DIM > 4)
	{
		memset(&(*this)[4], 0, (DIM-4) * sizeof(TYPE));
	}

}	// CVectorD<DIM, TYPE>::CVectorD<DIM, TYPE>(TYPE x, TYPE y, TYPE z, TYPE w) 


//////////////////////////////////////////////////////////////////
// CVectorD<DIM, TYPE>::CVectorD<DIM, TYPE>(const CVectorD<DIM, TYPE>& vFrom) 
//
// copy constructor
//////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
CVectorD<DIM, TYPE>::CVectorD(const CVectorD<DIM, TYPE>& vFrom) 
{
	// set the elements pointer to the static array 
	//		and initialize the dimension
	SetElements(DIM, const_cast<TYPE*>(&this->point().get<0>()), FALSE);

	// assign to copy 
	(*this) = vFrom;

}	// CVectorD<DIM, TYPE>::CVectorD<DIM, TYPE>(const CVectorD<DIM, TYPE>& vFrom) 


//////////////////////////////////////////////////////////////////
// CVectorD<DIM, TYPE>::CVectorD<DIM, TYPE>(const CVectorBase<TYPE>& vFrom) 
//
// copy constructor from base class object
//////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
CVectorD<DIM, TYPE>::CVectorD(const CVectorBase<TYPE>& vFrom) 
{
	// set the elements pointer to the static array 
	//		and initialize the dimension
	SetElements(DIM, const_cast<TYPE*>(&this->point().get<0>()), FALSE);

	// copy the elements
	memcpy((*this), vFrom, __min(this->GetDim(), vFrom.GetDim()) * sizeof(TYPE));

	// set remainder of elements to 0
	if (this->GetDim() > vFrom.GetDim())
	{
		memset(&(*this)[vFrom.GetDim()], 0, 
			(this->GetDim() - vFrom.GetDim()) * sizeof(TYPE));
	}

}	// CVectorD<DIM, TYPE>::CVectorD<DIM, TYPE>(const CVectorBase<TYPE>& vFrom) 


#ifdef __AFX_H__
//////////////////////////////////////////////////////////////////
// CVectorD<DIM, TYPE>::CVectorD<DIM, TYPE>(const CPoint& pt)
//
// construct from a windows CPoint
//////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
CVectorD<DIM, TYPE>::CVectorD(const CPoint& pt)
{
	// set the elements pointer to the static array 
	//		and initialize the dimension
	SetElements(DIM, const_cast<TYPE*>(&this->point().get<0>()), FALSE);

	// set the given elements
	(*this)[0] = pt.x;
	(*this)[1] = pt.y;

	// clear to all zeros
	if (DIM > 2)
	{
		memset(&(*this)[2], 0, (DIM - 2) * sizeof(TYPE));
	}

}	// CVectorD<DIM, TYPE>::CVectorD<DIM, TYPE>(const CPoint& pt)
#endif


//////////////////////////////////////////////////////////////////
// CVectorD<DIM, TYPE>::~CVectorD<DIM, TYPE>()
//
// destructor
//////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
CVectorD<DIM, TYPE>::~CVectorD()
{
	// ensure no monkey business has occurred
	ASSERT(this->m_pElements == const_cast<TYPE*>(&this->point().get<0>()));

	// ensure we don't delete anything
	this->m_pElements = NULL;

}	// CVectorD<DIM, TYPE>::~CVectorD<DIM, TYPE>()


#ifdef __AFX_H__
//////////////////////////////////////////////////////////////////
// CVectorD<DIM, TYPE>::operator CPoint() const
//
// Windows CPoint conversion 
//////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
CVectorD<DIM, TYPE>::operator CPoint() const
{
	return CPoint((int)(*this)[0], (int)(*this)[1]);

}	// CVectorD<DIM, TYPE>::operator CPoint() const
#endif


//////////////////////////////////////////////////////////////////
// CVectorD<DIM, TYPE>& CVectorD<DIM, TYPE>::operator=
//
// assignment operator
//////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
CVectorD<DIM, TYPE>& CVectorD<DIM, TYPE>::operator=(const CVectorD<DIM, TYPE>& vFrom)
{
	// NOTE: this ASSERT would work, were it not for the terrible down-cast
	//		that occurs in CMatrixD::operator[]
	// ASSERT(m_pElements == &m_arrElements[0]);

	// copy the elements
	memcpy((*this), vFrom, this->GetDim() * sizeof(TYPE));

	// return a reference to this
	return (*this);

}	// CVectorD<DIM, TYPE>& CVectorD<DIM, TYPE>::operator=


//////////////////////////////////////////////////////////////////
// CVectorD<DIM, TYPE>::IsApproxEqual
//
// tests for approximate equality using the given epsilon
//////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
BOOL CVectorD<DIM, TYPE>::IsApproxEqual(const CVectorD<DIM, TYPE>& v,
	TYPE epsilon) const
{
	// form the difference vector
	CVectorD<DIM, TYPE> vDiff(*this);
	vDiff -= v;

	return (vDiff.GetLength() < epsilon);

}	// CVectorD<DIM, TYPE>::IsApproxEqual


//////////////////////////////////////////////////////////////////
// CVectorD<DIM, TYPE>::operator+=
//
// in-place vector addition; returns a reference to this
//////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
CVectorD<DIM, TYPE>& CVectorD<DIM, TYPE>::operator+=(const CVectorD<DIM, TYPE>& vRight)
{
	// use the base-class operator+=
	CVectorBase<TYPE>::operator+=((const CVectorBase<TYPE>&)vRight);

	return (*this);

}	// CVectorD<DIM, TYPE>::operator+=


//////////////////////////////////////////////////////////////////
// CVectorD<DIM, TYPE>::operator-=
//
// in-place vector subtraction; returns a reference to this
//////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
CVectorD<DIM, TYPE>& CVectorD<DIM, TYPE>::operator-=(const CVectorD<DIM, TYPE>& vRight)
{
	// use the base-class operator-=
	CVectorBase<TYPE>::operator-=((const CVectorBase<TYPE>&)vRight);

	return (*this);

}	// CVectorD<DIM, TYPE>::operator-=


//////////////////////////////////////////////////////////////////////
// operator==(const CVectorD, const CVectorD)
//
// friend function to provide equality comparison for vectors.
// use IsApproxEqual for approximate equality.
//////////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
inline bool operator==(const CVectorD<DIM, TYPE>& vLeft, 
					   const CVectorD<DIM, TYPE>& vRight)
{
	// use the base-class operator==
	return operator==((const CVectorBase<TYPE>&) vLeft,
		(const CVectorBase<TYPE>&) vRight);

}	// operator==(const CVectorD, const CVectorD)


//////////////////////////////////////////////////////////////////////
// operator!=(const CVectorD, const CVectorD)
//
// friend function to provide inequality comparison for vectors.
// use !IsApproxEqual for approximate inequality.
//////////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
inline bool operator!=(const CVectorD<DIM, TYPE>& vLeft, 
					   const CVectorD<DIM, TYPE>& vRight)
{
	// use the base-class operator!=
	return operator!=((const CVectorBase<TYPE>&) vLeft,
		(const CVectorBase<TYPE>&) vRight);

}	// operator!=(const CVectorD, const CVectorD)


//////////////////////////////////////////////////////////////////////
// operator+(const CVectorD, const CVectorD)
//
// friend function to add two vectors, returning the sum as a new 
//		vector
//////////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
inline CVectorD<DIM, TYPE> operator+(const CVectorD<DIM, TYPE>& vLeft, 
							  const CVectorD<DIM, TYPE>& vRight)
{
	CVectorD<DIM, TYPE> vSum = vLeft;
	vSum += vRight;

	return vSum;

}	// operator+(const CVectorD, const CVectorD)

	
//////////////////////////////////////////////////////////////////////
// operator-(const CVectorD, const CVectorD)
//
// friend function to subtract one vector from another, returning 
//		the difference as a new vector
//////////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
inline CVectorD<DIM, TYPE> operator-(const CVectorD<DIM, TYPE>& vLeft, 
							  const CVectorD<DIM, TYPE>& vRight)
{
	CVectorD<DIM, TYPE> vSum = vLeft;
	vSum -= vRight;

	return vSum;

}	// operator-(const CVectorD, const CVectorD)

#ifdef __AFX_H__
//////////////////////////////////////////////////////////////////////
// operator<<(CArchive &ar, CVectorD)
//
// CArchive serialization of a vector
//////////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
CArchive& operator<<(CArchive &ar, CVectorD<DIM, TYPE> v)
{
	for (int nAt = 0; nAt < DIM; nAt++)
	{
		ar << v[nAt];
	}

	return ar;

}	// operator<<(CArchive &ar, CVectorD)


//////////////////////////////////////////////////////////////////////
// operator>>(CArchive &ar, CVectorD)
//
// CArchive de-serialization of a vector
//////////////////////////////////////////////////////////////////////
template<int DIM, class TYPE>
CArchive& operator>>(CArchive &ar, CVectorD<DIM, TYPE>& v)
{
	for (int nAt = 0; nAt < DIM; nAt++)
	{
		ar >> v[nAt];
	}

	return ar;

}	// operator>>(CArchive &ar, CVectorD)
#endif
