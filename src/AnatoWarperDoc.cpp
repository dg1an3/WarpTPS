
// AnatoWarperDoc.cpp : implementation of the CAnatoWarperDoc class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "AnatoWarper.h"
#endif

#include "AnatoWarperDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAnatoWarperDoc

IMPLEMENT_DYNCREATE(CAnatoWarperDoc, CDocument)

BEGIN_MESSAGE_MAP(CAnatoWarperDoc, CDocument)
END_MESSAGE_MAP()


// CAnatoWarperDoc construction/destruction

CAnatoWarperDoc::CAnatoWarperDoc() noexcept
{
	for (int nAt = 0; nAt < 5; nAt++)
	{
		// initialize each image
		m_pImage[nAt] = NULL;
	}
}

CAnatoWarperDoc::~CAnatoWarperDoc()
{
	for (int nAt = 0; nAt < 5; nAt++)
	{
		// delete the existing images
		delete m_pImage[nAt];
		m_pImage[nAt] = NULL;
	}
}

BOOL CAnatoWarperDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

#ifdef PRELOAD_TEST_IMAGES
	if (!LoadImages(_T("E:\\github\\dg1an3\\warptps\\TestData\\001.bmp"), _T("E:\\github\\dg1an3\\warptps\\TestData\\002.bmp")))
#else
	if (!LoadImages(NULL, NULL))
#endif
	{
		return FALSE;
	}

	// add some landmark points
	InitCornerLandmarks();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CWarpTPSDoc commands

BOOL CAnatoWarperDoc::LoadImages(LPCTSTR pszFilename1, LPCTSTR pszFilename2)
{
	for (int nAt = 0; nAt < 5; nAt++)
	{
		// delete the existing images
		delete m_pImage[nAt];
		m_pImage[nAt] = NULL;
	}

	// load the first image
	m_pImage[ImageRole::DestinationImage] = new CDib();
	m_pImage[ImageRole::WarpedDestinationImage] = new CDib();
	m_pImage[ImageRole::WarpedSourceImage] = new CDib();
	m_pImage[ImageRole::SourceImage] = new CDib();
	m_pImage[ImageRole::BlendedImage] = new CDib();

	if (NULL != pszFilename1 && NULL != pszFilename2)
	{
		BOOL bResult = m_pImage[ImageRole::DestinationImage]->Load(pszFilename1);
		if (!bResult)
		{
			goto ERROR_CLEANUP;
		}

		// load the warped image (initializes the size)
		bResult = m_pImage[ImageRole::WarpedSourceImage]->Load(pszFilename2);
		if (!bResult)
		{
			goto ERROR_CLEANUP;
		}

		bResult = m_pImage[ImageRole::SourceImage]->Load(pszFilename2);
		if (!bResult)
		{
			goto ERROR_CLEANUP;
		}

		// load the warped image (initializes the size)
		bResult = m_pImage[ImageRole::WarpedDestinationImage]->Load(pszFilename1);
		if (!bResult)
		{
			goto ERROR_CLEANUP;
		}

		// load the blended image (initializes size and pixel format)
		bResult = m_pImage[ImageRole::BlendedImage]->Load(pszFilename1);
		if (!bResult)
		{
			goto ERROR_CLEANUP;
		}
	}
	else
	{
		BOOL bResult = m_pImage[ImageRole::DestinationImage]->CreateFromResource(IDB_HEDGY);
		if (!bResult)
		{
			goto ERROR_CLEANUP;
		}

		// load the warped image (initializes the size)
		bResult = m_pImage[ImageRole::WarpedSourceImage]->CreateFromResource(IDB_GRUMPY);
		if (!bResult)
		{
			goto ERROR_CLEANUP;
		}

		bResult = m_pImage[ImageRole::SourceImage]->CreateFromResource(IDB_GRUMPY);
		if (!bResult)
		{
			goto ERROR_CLEANUP;
		}

		// load the warped image (initializes the size)
		bResult = m_pImage[ImageRole::WarpedDestinationImage]->CreateFromResource(IDB_HEDGY);
		if (!bResult)
		{
			goto ERROR_CLEANUP;
		}

		// load the blended image (initializes size and pixel format)
		bResult = m_pImage[ImageRole::BlendedImage]->CreateFromResource(IDB_GRUMPY);
		if (!bResult)
		{
			goto ERROR_CLEANUP;
		}
	}

	// check the size of the images
	if (m_pImage[ImageRole::DestinationImage]->GetSize()
		!= m_pImage[ImageRole::SourceImage]->GetSize())
	{
		::AfxMessageBox(_T("Images must have same dimensions"));
		goto ERROR_CLEANUP;
	}

	// ensures the initial update sets things up
	SendInitialUpdate();

	// redraw the window
	UpdateAllViews(NULL);

	// success
	return TRUE;

ERROR_CLEANUP:
	// delete any loaded images
	for (int nAt = 0; nAt < 5; nAt++)
	{
		// delete the existing images
		delete m_pImage[nAt];
		m_pImage[nAt] = NULL;
	}

	// and return FALSE
	return FALSE;
}

CDib* CAnatoWarperDoc::GetImage(int nIndex)
{
	return m_pImage[nIndex];
}

CTPSTransform* CAnatoWarperDoc::GetTransform()
{
	return &m_transform;
}

CTPSTransform* CAnatoWarperDoc::GetInverseTransform()
{
	return &m_inversetransform;
}

void Resample(CTPSTransform* xform, CDib* pImageSrc, CDib* pImageDst, float percentage)
{
	// get the number of bytes-per-pixel for each
	BITMAP srcBitmap;
	LONG bytesFilled = pImageSrc->GetBitmap(&srcBitmap);
	if (bytesFilled == 0)
	{
		return;
	}

	int nSrcBytesPerPixel = (srcBitmap.bmBitsPixel + 7) / 8;

	// ensure the scan line width is on a LONG boundary
	int nSrcWidthBytes = 4 * ((srcBitmap.bmWidth * nSrcBytesPerPixel + 3) / 4);

	BITMAP dstBitmap;
	bytesFilled = pImageDst->GetBitmap(&dstBitmap);
	if (bytesFilled == 0)
	{
		return;
	}
	
	int nDstBytesPerPixel = (dstBitmap.bmBitsPixel + 7) / 8;

	// ensure the scan line width is on a LONG boundary
	int nDstWidthBytes = 4 * ((dstBitmap.bmWidth * nDstBytesPerPixel + 3) / 4);

	// should have same pixel format
	ASSERT(nDstBytesPerPixel == nSrcBytesPerPixel);
	ASSERT(nDstWidthBytes == nSrcWidthBytes);

	// get the size of the source image
	CSize srcSize = pImageSrc->GetSize();
	CSize dstSize = pImageDst->GetSize();

	// retrieve the source and destination pixels
	LPBYTE pSrcPixels = (LPBYTE)pImageSrc->GetDIBits();
	LPBYTE pDstPixels = (LPBYTE)pImageDst->GetDIBits();

	TRACE("Resampling...\n");
	xform->ResampleRawWithField(pSrcPixels, pDstPixels, nSrcBytesPerPixel, srcSize.cx, srcSize.cy, nSrcWidthBytes, percentage);
}

void CAnatoWarperDoc::UpdateResampled(float forwardPercent)
{
	Resample(GetTransform(), m_pImage[ImageRole::SourceImage], m_pImage[ImageRole::WarpedSourceImage], forwardPercent);
	Resample(GetInverseTransform(), m_pImage[ImageRole::DestinationImage], m_pImage[ImageRole::WarpedDestinationImage], 1.0f - forwardPercent);
}

void CAnatoWarperDoc::InitCornerLandmarks()
{
	// remove all landmarks from the transform
	GetTransform()->RemoveAllLandmarks();
	GetInverseTransform()->RemoveAllLandmarks();

	// add a landmark at <0.0, 0.0>
	GetTransform()->AddLandmark(CVectorD<3>(0.0, 0.0));
	GetInverseTransform()->AddLandmark(CVectorD<3>(0.0, 0.0));

	// compute the other four corners
	CSize size1 = GetImage(ImageRole::DestinationImage)->GetSize();
	CSize size2 = GetImage(ImageRole::SourceImage)->GetSize();

	// add the other four corners
	GetTransform()->AddLandmark(CVectorD<3>(0.0, size1.cy),
		CVectorD<3>(0.0, size2.cy));
	GetTransform()->AddLandmark(CVectorD<3>(size1.cx, size1.cy),
		CVectorD<3>(size2.cx, size2.cy));
	GetTransform()->AddLandmark(CVectorD<3>(size1.cx, 0.0),
		CVectorD<3>(size2.cx, 0.0));

	GetInverseTransform()->AddLandmark(CVectorD<3>(0.0, size2.cy),
		CVectorD<3>(0.0, size1.cy));
	GetInverseTransform()->AddLandmark(CVectorD<3>(size2.cx, size2.cy),
		CVectorD<3>(size1.cx, size1.cy));
	GetInverseTransform()->AddLandmark(CVectorD<3>(size2.cx, 0.0),
		CVectorD<3>(size1.cx, 0.0));

	// resample the images
	UpdateResampled(1.0);
}

void CAnatoWarperDoc::DeleteContents()
{
	m_transform.RemoveAllLandmarks();
	m_inversetransform.RemoveAllLandmarks();
	CDocument::DeleteContents();
}



// CAnatoWarperDoc serialization

void CAnatoWarperDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_transform.GetLandmarkCount();
		for (int nAt = 0; nAt < m_transform.GetLandmarkCount(); nAt++)
		{
			ar << m_transform.GetLandmark<0>(nAt);
			ar << m_transform.GetLandmark<1>(nAt);
		}
	}
	else
	{
		int nCount = 0;
		ar >> nCount;
		for (int nAt = 0; nAt < nCount; nAt++)
		{
			CVectorD<3> vL1, vL2;
			ar >> vL1;
			ar >> vL2;
			m_transform.AddLandmark(vL1, vL2);
			m_inversetransform.AddLandmark(vL2, vL1);
		}
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CAnatoWarperDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CAnatoWarperDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CAnatoWarperDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CAnatoWarperDoc diagnostics

#ifdef _DEBUG
void CAnatoWarperDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAnatoWarperDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CAnatoWarperDoc commands
