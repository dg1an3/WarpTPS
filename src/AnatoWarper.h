
// AnatoWarper.h : main header file for the AnatoWarper application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CAnatoWarperApp:
// See AnatoWarper.cpp for the implementation of this class
//

class CAnatoWarperApp : public CWinAppEx
{
public:
	CAnatoWarperApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CAnatoWarperApp theApp;
