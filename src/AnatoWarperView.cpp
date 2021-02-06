
// AnatoWarperView.cpp : implementation of the CAnatoWarperView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "AnatoWarper.h"
#endif

#include "AnatoWarperDoc.h"
#include "AnatoWarperView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAnatoWarperView

IMPLEMENT_DYNCREATE(CAnatoWarperView, CView)

BEGIN_MESSAGE_MAP(CAnatoWarperView, CView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(ID_SHOWMARKS, OnShowmarks)
	ON_UPDATE_COMMAND_UI(ID_SHOWMARKS, OnUpdateShowmarks)
	ON_COMMAND(ID_FILE_OPEN_IMAGES, OnFileOpenImages)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CAnatoWarperView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CAnatoWarperView construction/destruction

CAnatoWarperView::CAnatoWarperView() noexcept
	: m_pTransform(NULL)
	, m_pInverseTransform(NULL)
{
}

CAnatoWarperView::~CAnatoWarperView()
{
}

BOOL CAnatoWarperView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CAnatoWarperView drawing

void CAnatoWarperView::OnDraw(CDC* /*pDC*/)
{
	CAnatoWarperDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}


// CAnatoWarperView printing


void CAnatoWarperView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CAnatoWarperView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CAnatoWarperView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CAnatoWarperView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CAnatoWarperView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CAnatoWarperView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CAnatoWarperView diagnostics

#ifdef _DEBUG
void CAnatoWarperView::AssertValid() const
{
	CView::AssertValid();
}

void CAnatoWarperView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAnatoWarperDoc* CAnatoWarperView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAnatoWarperDoc)));
	return (CAnatoWarperDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CAnatoWarperView message handlers

void CAnatoWarperView::OnInitialUpdate()
{
	// set the document for the views
	m_arrViews[0].SetDocument(GetDocument());
	m_arrViews[0].SetDib(GetDocument()->GetImage(ImageRole::DestinationImage));
	m_arrViews[1].SetDocument(GetDocument());
	m_arrViews[1].SetDib(GetDocument()->GetImage(ImageRole::BlendedImage));
	m_arrViews[2].SetDocument(GetDocument());
	m_arrViews[2].SetDib(GetDocument()->GetImage(ImageRole::SourceImage));

	CView::OnInitialUpdate();
}

CRect CAnatoWarperView::GetRect(int nArea)
{
	CRect rectClient;
	GetClientRect(&rectClient);
	rectClient.DeflateRect(10, 10, 10, 10);
	//rectClient.right = rectClient.Height();

	// compute the rectangle
	CRect rect(rectClient);
	rect.left = 5 + (nArea % 2) * (rectClient.Width() / 2);
	rect.right = rect.left + (rectClient.Width() / 2 - 5);
	rect.top = 5 + nArea / 2 * rectClient.Height() / 2;
	rect.bottom = rect.top + rect.Width();	// assume square image

	return rect;
}

void CAnatoWarperView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	CRect rect = GetRect(0);
	m_arrViews[0].MoveWindow(&rect);

	rect = GetRect(1);
	m_arrViews[1].MoveWindow(&rect);

	rect = GetRect(3);
	m_arrViews[2].MoveWindow(&rect);

	rect = GetRect(2);
	m_morphSlider.MoveWindow(&rect);
}

int CAnatoWarperView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect = GetRect(0);
	if (!m_arrViews[0].Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, rect, this, 0))
		return -1;

	rect = GetRect(1);
	if (!m_arrViews[1].Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, rect, this, 0))
		return -1;

	rect = GetRect(3);
	if (!m_arrViews[2].Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, rect, this, 0))
		return -1;

	if (!m_morphSlider.Create(IDD_MORPHSLIDER, this))
		return -1;

	rect = GetRect(2);
	m_morphSlider.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(),
		SWP_SHOWWINDOW);

	return 0;
}

void CAnatoWarperView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	// set the pointer to transform
	m_pTransform = GetDocument()->GetTransform();
	m_pInverseTransform = GetDocument()->GetInverseTransform();

	if (m_pTransform)
	{
		CWaitCursor* pWaitCursor = NULL;
		if (m_pTransform->GetLandmarkCount() > 10)
		{
			pWaitCursor = new CWaitCursor;
		}

		// now resample the center image
		TRACE("OnUpdate/Resample");

		// resample the images
		GetDocument()->UpdateResampled(m_morphSlider.m_morphPercent);

		GetDocument()->GetImage(ImageRole::BlendedImage)->CopyPixels(
			GetDocument()->GetImage(ImageRole::WarpedSourceImage));
		GetDocument()->GetImage(ImageRole::BlendedImage)->BlendPixels(
			GetDocument()->GetImage(ImageRole::WarpedDestinationImage), m_morphSlider.m_blendPercent);

		delete pWaitCursor;
	}

	ASSERT(m_arrViews[0].GetDib() == GetDocument()->GetImage(ImageRole::DestinationImage));
	m_arrViews[0].SetDib(GetDocument()->GetImage(ImageRole::DestinationImage));
	m_arrViews[0].SetTransform(m_pTransform, m_pInverseTransform, 0);

	ASSERT(m_arrViews[1].GetDib() == GetDocument()->GetImage(ImageRole::BlendedImage));
	m_arrViews[1].SetDib(GetDocument()->GetImage(ImageRole::BlendedImage));
	m_arrViews[1].SetTransform(m_pTransform, m_pInverseTransform, 0);

	ASSERT(m_arrViews[2].GetDib() == GetDocument()->GetImage(ImageRole::SourceImage));
	m_arrViews[2].SetDib(GetDocument()->GetImage(ImageRole::SourceImage));
	m_arrViews[2].SetTransform(m_pTransform, m_pInverseTransform, 1);

	m_morphSlider.m_pDoc = GetDocument();
	m_morphSlider.m_pWarpedView = &m_arrViews[1];

	Invalidate(FALSE);
}

void CAnatoWarperView::OnShowmarks()
{
	m_arrViews[0].m_bDrawMarks = !m_arrViews[0].m_bDrawMarks;
	m_arrViews[0].Invalidate(FALSE);
	m_arrViews[1].m_bDrawMarks = m_arrViews[0].m_bDrawMarks;
	m_arrViews[1].Invalidate(FALSE);
	m_arrViews[2].m_bDrawMarks = m_arrViews[0].m_bDrawMarks;
	m_arrViews[2].Invalidate(FALSE);
}

void CAnatoWarperView::OnUpdateShowmarks(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_arrViews[0].m_bDrawMarks ? 1 : 0);
}

void CAnatoWarperView::OnFileOpenImages()
{
	CFileDialog imageFileOpen1(TRUE, _T("bmp"), NULL, OFN_HIDEREADONLY, NULL,
		::AfxGetMainWnd());

	CFileDialog imageFileOpen2(TRUE, _T("bmp"), NULL, OFN_HIDEREADONLY, NULL,
		::AfxGetMainWnd());

	if (IDOK == imageFileOpen1.DoModal() && IDOK == imageFileOpen2.DoModal())
	{
		// open the files
		if (GetDocument()->LoadImages(imageFileOpen1.GetPathName(),
			imageFileOpen2.GetPathName()))
		{
			// set up some corner landmarks
			GetDocument()->InitCornerLandmarks();

			// invalidate everything
			Invalidate(TRUE);

			// redraw the window
			GetDocument()->UpdateAllViews(NULL);
		}
	}
}
