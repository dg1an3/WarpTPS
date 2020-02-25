// WarpTpsLib.h : main header file for the WarpTpsLib DLL
//

#pragma once

#include "resource.h"		// main symbols

#ifdef WARPTPSLIB_EXPORTS
#define WARPTPSLIB_API __declspec(dllexport)
#else
#define WARPTPSLIB_API __declspec(dllimport)
#endif

#include "TPSTransform.h"

