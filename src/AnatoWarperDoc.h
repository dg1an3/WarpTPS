
// AnatoWarperDoc.h : interface of the CAnatoWarperDoc class
//


#pragma once

#include "Dib.h"

// enum used to access images on the doc
enum ImageRole
{
	DestinationImage = 0,
	WarpedDestinationImage = 3,
	SourceImage = 2,
	WarpedSourceImage = 1,
	BlendedImage = 4,
};


class CAnatoWarperDoc : public CDocument
{
protected: // create from serialization only
	CAnatoWarperDoc() noexcept;
	DECLARE_DYNCREATE(CAnatoWarperDoc)

	// Attributes
public:

	// get one of the images: 0 for first, 
	//		2 for second, 
	//		1 for warped second
	CDib* GetImage(int nIndex);

	// get the transform object
	CTPSTransform* GetTransform();

	// get the transform object
	CTPSTransform* GetInverseTransform();

	// updates both resampled images
	void UpdateResampled(float forwardPercent);

	// Operations
public:
	// loads the two specified images
	BOOL LoadImages(LPCTSTR pszFilename1, LPCTSTR pszFilename2);

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	void InitCornerLandmarks();
	virtual ~CAnatoWarperDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CTPSTransform m_transform;
	CTPSTransform m_inversetransform;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

private:
	// stores the images as:
	//		0 : first image (destination)
	//		1 : warped image (warped source image)
	//		2 : second image (source)
	//		3 : inverse warped image (warped destination image)
	//		4 : blended image
	CDib* m_pImage[5];
};
