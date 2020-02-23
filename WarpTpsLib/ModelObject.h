//////////////////////////////////////////////////////////////////////
// ModelObject.h: interface for the CModelObject class.
//
// Copyright (C) 1999-2020 Derek Lane
// $Id: ModelObject.h,v 1.3 2003/04/26 20:51:38 default Exp $
//////////////////////////////////////////////////////////////////////

#pragma once


//////////////////////////////////////////////////////////////////////
// class CModelObject
// 
// a model object:
//		1) fires change events observable
//		2) has a name
//		3) possibly has children
//		4) can be serialized
//////////////////////////////////////////////////////////////////////
class WARPTPSLIB_API CModelObject // : public CObject
{
public:
	// constructors/destructors
	CModelObject(const CString& strName = _T(""));
	virtual ~CModelObject();

	// the given name for this model object
	const CString& GetName() const;
	void SetName(const CString& strName);

	// collection of the children of this model object
	int GetChildCount() const;
	CModelObject *GetChildAt(int nIndex);
	int AddChild(CModelObject *pObject);



protected:
	// the name of the object
	CString m_strName;

	// the model object's children
	CArray<CModelObject*> m_arrChildren;

	// reference count for the object
	DWORD m_dwRefCount;
};