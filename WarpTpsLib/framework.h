#pragma once

#include "targetver.h"


#ifdef __AFX_H__
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <afxwin.h>         // MFC core and standard components
#else
typedef int BOOL;
#define TRUE 1
#define FALSE 0

typedef unsigned char BYTE;
typedef BYTE * LPBYTE;
typedef unsigned int  UINT;
#define ASSERT assert
#endif
