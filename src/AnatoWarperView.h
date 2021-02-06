
// AnatoWarperView.h : interface of the CAnatoWarperView class
//

#pragma once

#include <DibView.h>	// Added by ClassView
#include <MorphSlider.h>

class CAnatoWarperView : public CView
{
protected: // create from serialization only
	CAnatoWarperView() noexcept;
	DECLARE_DYNCREATE(CAnatoWarperView)

// Attributes
public:
	CAnatoWarperDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

// Implementation
public:
	// returns the rectangle for the three views
	CRect GetRect(int nArea);

	// the subordinate views
	CDibView m_arrViews[3];

	MorphSlider m_morphSlider;

	// the transform for the view
	CTPSTransform* m_pTransform;

	// the inverse transform for the view
	CTPSTransform* m_pInverseTransform;

	// destructor
	virtual ~CAnatoWarperView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowmarks();
	afx_msg void OnUpdateShowmarks(CCmdUI* pCmdUI);
	afx_msg void OnFileOpenImages();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AnatoWarperView.cpp
inline CAnatoWarperDoc* CAnatoWarperView::GetDocument() const
   { return reinterpret_cast<CAnatoWarperDoc*>(m_pDocument); }
#endif

