#pragma once

#ifdef USE_MFC
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <afxwin.h>         // MFC core and standard components
#else
typedef bool BOOL;
typedef unsigned char BYTE;
typedef BYTE * LPBYTE;
typedef unsigned int  UINT;
#define TRUE true
#define FALSE false

#include <assert.h>
#define ASSERT assert
#endif
