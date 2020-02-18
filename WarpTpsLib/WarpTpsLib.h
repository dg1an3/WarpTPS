// WarpTpsLib.h : main header file for the WarpTpsLib DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CWarpTpsLibApp
// See WarpTpsLib.cpp for the implementation of this class
//

class CWarpTpsLibApp : public CWinApp
{
public:
	CWarpTpsLibApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
