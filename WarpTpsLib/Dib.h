////////////////////////////////////////////////////////////////
// PixieLib(TM) Copyright 1997-1999 Paul DiLascia
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
//
#ifndef _DIB_H
#define _DIB_H

#ifndef _INC_VFW
#include <vfw.h>
#endif

#define DLLFUNC
#define DLLCLASS

// tell linker to look in Video for Windows
#pragma comment(linker, "/defaultlib:vfw32.lib")

// get dimensions of a bitmap
extern DLLFUNC CSize PLGetBitmapSize(CBitmap* pBitmap);

// draw bitmap on dc -- BION, there's no function in Windows to do this!
extern DLLFUNC BOOL  PLDrawBitmap(CDC& dc, CBitmap* pBitmap,
	const CRect* rcDst=NULL, const CRect* rcSrc=NULL, DWORD dwRop=SRCCOPY);

////////////////
// CDib implements Device Independent Bitmaps as a form of CBitmap. 
//
class DLLCLASS CDib : public CBitmap {
protected:
	BITMAP	m_bm;		// stored for speed
	CPalette m_pal;	// palette
	HDRAWDIB m_hdd;	// for DrawDib

public:
	DECLARE_DYNAMIC(CDib)
	CDib();
	~CDib();

	// helper to make a copy
	void CopyPixels(CDib* from);
	void BlendPixels(CDib* from, float percent);

	CSize	GetSize() { return CSize(m_bm.bmWidth, m_bm.bmHeight); }
	BOOL Attach(HGDIOBJ hbm);
	BOOL Load(LPCTSTR szPathName);
	BOOL Load(HINSTANCE hInst, LPCTSTR lpResourceName);
	BOOL Load(HINSTANCE hInst, UINT uID)
		{ return Load(hInst, MAKEINTRESOURCE(uID)); }

	// Universal Draw function can use DrawDib or not.
	BOOL Draw(CDC& dc, const CRect* rcDst=NULL, const CRect* rcSrc=NULL,
		BOOL bUseDrawDib=TRUE, CPalette* pPal=NULL, BOOL bForeground=FALSE);

	BOOL DeleteObject();
	BOOL CreatePalette(CPalette& pal);
	CPalette* GetPalette()  { return &m_pal; }

	UINT GetColorTable(RGBQUAD* colorTab, UINT nColors);

	// returns a void * to the pixel data for the image
	void *GetDIBits();

	// returns the size of the bitmap
	CSize GetBitmapSize()
	{
		return CSize(m_bm.bmWidth, m_bm.bmHeight);
	}
};

#endif // _DIB_H
