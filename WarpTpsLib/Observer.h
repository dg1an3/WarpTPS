//////////////////////////////////////////////////////////////////////
// Observer.h: interface for the CObservableEvent class.
//
// Copyright (C) 1999-2001
// $Id: Observer.h,v 1.2 2002/06/03 05:36:59 default Exp $
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBSERVER_H__AAA9A381_F0B7_11D4_9E39_00B0D0609AB0__INCLUDED_)
#define AFX_OBSERVER_H__AAA9A381_F0B7_11D4_9E39_00B0D0609AB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// forward declaration of the CObservableEvent class
//////////////////////////////////////////////////////////////////////
class CObservableEvent;

//////////////////////////////////////////////////////////////////////
// defines the ChangeFunction which is called when a change occurs
//////////////////////////////////////////////////////////////////////
typedef void (CObject::*ListenerFunction)(CObservableEvent *, void *);
typedef ListenerFunction ChangeFunction;

//////////////////////////////////////////////////////////////////////
// class CObservableEvent
// 
// a CObservableObject fires change events that can be processed by 
// an observer
//////////////////////////////////////////////////////////////////////
class CObservableEvent : public CObject
{
public:
	// creates an event for the parent object
	CObservableEvent(CObject *pParent = NULL);

	// includes dynamic type information
	DECLARE_DYNAMIC(CObservableEvent)

	// returns the parent of this event
	CObject *GetParent();

	// accessors for the observer list
	void AddObserver(CObject *pObserver, ListenerFunction func) const;
	void RemoveObserver(CObject *pObserver, ListenerFunction func) const;

	// called to fire a change
	void Fire(void *pValue = NULL);

private:
	// the parent object of this event
	CObject *m_pParent;

	// the array of observers
	mutable CArray<CObject *, CObject *> m_arrObservers;

	// the array of member functions
	mutable CArray<ChangeFunction, ChangeFunction> m_arrFunctions;
};

// typedef for compatibility
typedef CObservableEvent CObservableObject;


//////////////////////////////////////////////////////////////////////
// template function AddObserver
// 
// type-safe function to add an observer to a CObservableObject
//////////////////////////////////////////////////////////////////////
template<class OBSERVER_TYPE>
void AddObserver(CObservableEvent *pObservable, 
				 OBSERVER_TYPE *pObserver, 
				 void (OBSERVER_TYPE::*func)(CObservableEvent *, void *))
{
	pObservable->AddObserver(pObserver, (ChangeFunction) func);
}

//////////////////////////////////////////////////////////////////////
// template function RemoveObserver
// 
// type-safe function to remove an observer from a CObservableObject
//////////////////////////////////////////////////////////////////////
template<class OBSERVER_TYPE>
void RemoveObserver(CObservableEvent *pObservable, 
					OBSERVER_TYPE *pObserver, 
					void (OBSERVER_TYPE::*func)(CObservableEvent *, void *))
{
	pObservable->RemoveObserver(pObserver, (ChangeFunction) func);
}

#endif // !defined(AFX_OBSERVER_H__AAA9A381_F0B7_11D4_9E39_00B0D0609AB0__INCLUDED_)
