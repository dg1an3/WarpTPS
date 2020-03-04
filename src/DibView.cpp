// DibView.cpp : implementation file
//

#include "stdafx.h"
#include "DibView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDibView

CDibView::CDibView()
	: m_pDoc(NULL),
		m_pDib(NULL),
		m_pTransform(NULL),
		m_pInverseTransform(NULL),
		m_nDataSet(-1),
		m_nDraggingLandmark(-1),
		m_bDrawMarks(TRUE)
{
}

CDibView::~CDibView()
{
}

// sets the document that is being displayed by this CDibView
void CDibView::SetDocument(CDocument *pDoc)
{
	m_pDoc = pDoc;
}

// sets the DIB to be displayed
void CDibView::SetDib(CDib *pDib)
{
	m_pDib = pDib;
}

// sets the transform to be managed
void CDibView::SetTransform(CTPSTransform *pForwardTransform, CTPSTransform *pInverseTransform, int nDataSet)
{
	m_pTransform = pForwardTransform;
	m_pInverseTransform = pInverseTransform;
	m_nDataSet = nDataSet;
}

BEGIN_MESSAGE_MAP(CDibView, CWnd)
	//{{AFX_MSG_MAP(CDibView)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDibView message handlers

static COLORREF g_arrColors[] = 
{
	RGB(255, 128, 255),
	RGB(255,   0, 255),
	RGB(255, 255,   0),
	RGB(  0, 255, 255),
	RGB(255,   0,   0),
	RGB(  0, 255,   0),
	RGB(  0,   0, 255),
	RGB(128, 255, 128),
};

void CDibView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (m_pDib)
	{
		CRect rectSrc(0, 0, m_pDib->GetSize().cx, m_pDib->GetSize().cy);
		CRect rectDst = GetDstRect();
		
		// draw the image
		m_pDib->Draw(dc, &rectDst, &rectSrc,
		    FALSE, /*bUseDrawDib*/
			NULL, 
			FALSE /*bForeground*/
			);

		// draw the landmarks
		if (m_nDataSet >= 0 && m_bDrawMarks)
		{
			CBrush *pOldBrush = (CBrush *) dc.SelectStockObject(HOLLOW_BRUSH);

			for (int nAt = 0; nAt < m_pTransform->GetLandmarkCount(); nAt++)
			{	
				int nColorCount = sizeof(g_arrColors) / sizeof(COLORREF);
				CPen pen(PS_SOLID, 1, g_arrColors[nAt % nColorCount]);
				CPen *pOldPen = (CPen *) dc.SelectObject(&pen);

				CPoint pt;
				switch (m_nDataSet)
				{
				case 0:
					pt = Image2Client(m_pTransform->GetLandmark<0>(nAt));
					break;
				case 1:
					pt = Image2Client(m_pTransform->GetLandmark<1>(nAt));
					break;
				default:
					throw invalid_argument("data set must be 0 or 1");
				}					

				// dc.Ellipse(pt.x - 5, pt.y - 5, pt.x + 6, pt.y + 6);

				dc.MoveTo(pt.x - 5, pt.y);
				dc.LineTo(pt.x + 6, pt.y);

				dc.MoveTo(pt.x, pt.y - 5);
				dc.LineTo(pt.x, pt.y + 6);

				dc.SelectObject(pOldPen);
			}

			dc.SelectObject(pOldBrush);
		}
	}
	
	// Do not call CWnd::OnPaint() for painting messages
}

void CDibView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (NULL != m_pTransform)
	{
		// find the landmark that we are dragging
		for (int nAt = 0; nAt < m_pTransform->GetLandmarkCount(); nAt++)
		{
			CPoint ptLandmark;
			switch (m_nDataSet)
			{
			case 0:
				ptLandmark = Image2Client(m_pTransform->GetLandmark<0>(nAt));
				break;
			case 1:
				ptLandmark = Image2Client(m_pTransform->GetLandmark<1>(nAt));
				break;
			default:
				throw invalid_argument("data set must be 0 or 1");
			}

			CSize size = point - ptLandmark;

			if (-5 < size.cx && size.cx < 5
				&& -5 < size.cy && size.cy < 5)
			{
				m_nDraggingLandmark = nAt;
			}
		}

		// create a new point
		if (-1 == m_nDraggingLandmark)
		{
			int nOldSize = m_pTransform->GetLandmarkCount();
			m_nDraggingLandmark = m_pTransform->AddLandmark(Client2Image(point));
			int nNewSize = m_pTransform->GetLandmarkCount();
			ASSERT(nOldSize+1 == nNewSize);

			// add the inverse landmark
			int inverseLandmark = m_pInverseTransform->AddLandmark(Client2Image(point));

			// check that the inverse landmark is at the same position
			ASSERT(m_nDraggingLandmark == inverseLandmark);

			// redraw the segment the warping
			Invalidate(FALSE);
		}

		// store the mouse point
		m_ptPrev = point;
	}
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CDibView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (-1 != m_nDraggingLandmark)
	{
		Point_t forwardl0, forwardl1;
		std::tie(forwardl0, forwardl1) = m_pTransform->GetLandmarkTuple(m_nDraggingLandmark);
		Point_t reversel0, reversel1;
		std::tie(reversel0, reversel1) = m_pInverseTransform->GetLandmarkTuple(m_nDraggingLandmark);
		switch (m_nDataSet)
		{
		case 0:
			bg::add_point(forwardl0, Client2Image(point).point());
			bg::subtract_point(forwardl0, Client2Image(m_ptPrev).point());

			bg::add_point(reversel1, Client2Image(point).point());
			bg::subtract_point(reversel1, Client2Image(m_ptPrev).point());
			break;
		case 1:
			bg::add_point(reversel0, Client2Image(point).point());
			bg::subtract_point(reversel0, Client2Image(m_ptPrev).point());

			bg::add_point(forwardl1, Client2Image(point).point());
			bg::subtract_point(forwardl1, Client2Image(m_ptPrev).point());
			break;
		default:
			throw new invalid_argument("dataset must be 0 or 1");
		}
		m_pTransform->SetLandmarkTuple(m_nDraggingLandmark, std::make_tuple(forwardl0, forwardl1));
		m_pInverseTransform->SetLandmarkTuple(m_nDraggingLandmark, std::make_tuple(reversel0, reversel1));

		Invalidate(FALSE);

		// store the mouse point
		m_ptPrev = point;
	}
	
	CWnd::OnMouseMove(nFlags, point);
}

BOOL CheckInverse(CTPSTransform* pForward, CTPSTransform* pInverse)
{
	// now check to ensure the offsets at each landmark is correct
	for (int nAtLandmark = 0; nAtLandmark < pForward->GetLandmarkCount(); nAtLandmark++)
	{
		const CVectorD<3>& vL0 = pForward->GetLandmark<0>(nAtLandmark);
		const CVectorD<3>& vL0_other = pInverse->GetLandmark<1>(nAtLandmark);
		if (!vL0.IsApproxEqual(vL0_other))
			return FALSE;

		const CVectorD<3>& vL1 = pForward->GetLandmark<1>(nAtLandmark);
		const CVectorD<3>& vL1_other = pInverse->GetLandmark<0>(nAtLandmark);
		if (!vL1.IsApproxEqual(vL1_other))
			return FALSE;
	}

	return TRUE;
}

void CDibView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_nDraggingLandmark = -1;

	// Check if both inverse and forward are consistent
	ASSERT(CheckInverse(m_pTransform, m_pInverseTransform));
	ASSERT(CheckInverse(m_pInverseTransform, m_pTransform));

	if (m_pDoc)
	{
		m_pDoc->UpdateAllViews(NULL);
	}
	
	CWnd::OnLButtonUp(nFlags, point);
}

CVectorD<3> CDibView::Client2Image(const CPoint &ptClient)
{
	// source (DIB) image size
	CRect rectSrc(0, 0, m_pDib->GetSize().cx, m_pDib->GetSize().cy);

	// destination (window) image size
	CRect rectDst = GetDstRect();

	// ratio of source / destination heights
	REAL ratio = (REAL) rectSrc.Height() / (REAL) rectDst.Height();

	// offset to center and scale
	CVectorD<3> vImage(ptClient);
	vImage -= CVectorD<3>(rectDst.CenterPoint());
	vImage[0] *= ratio;
	vImage[1] *= ratio;
	vImage[2] *= ratio;
	vImage += CVectorD<3>(rectSrc.CenterPoint());

	return vImage;
}

CPoint CDibView::Image2Client(const CVectorD<3>&vImage)
{
	// source (DIB) image size
	CRect rectSrc(0, 0, m_pDib->GetSize().cx, m_pDib->GetSize().cy);

	// destination (window) image size
	CRect rectDst = GetDstRect();

	// ratio of destination / source heights
	REAL ratio = (REAL) rectDst.Height() / (REAL) rectSrc.Height();

	// offset to center and scale
	CPoint ptClient(vImage - CVectorD<3>(rectSrc.CenterPoint()));
	ptClient.x = (int)(ratio * (REAL) ptClient.x); 
	ptClient.y = (int)(ratio * (REAL) ptClient.y);
	ptClient += rectDst.CenterPoint();

	return ptClient;
}

CRect CDibView::GetDstRect()
{
	CRect rectDst;
	GetClientRect(&rectDst);

	CRect rectSrc(0, 0, m_pDib->GetSize().cx, m_pDib->GetSize().cy);

	// set the client rectangle to the same aspect ratio
	int nClientWidth = rectDst.Height() * rectSrc.Width() 
		/ rectSrc.Height();
	CPoint ptCenter = rectDst.CenterPoint();
	rectDst.left = ptCenter.x - nClientWidth / 2;
	rectDst.right = ptCenter.x + nClientWidth / 2;

	return rectDst;
}
