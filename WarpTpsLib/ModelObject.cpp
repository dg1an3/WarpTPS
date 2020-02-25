//////////////////////////////////////////////////////////////////////
// ModelObject.cpp: implementation of the CModelObject class.
//
// Copyright (C) 1999-2020 Derek Lane
// $Id: ModelObject.cpp,v 1.3 2003/04/26 20:51:50 default Exp $
//////////////////////////////////////////////////////////////////////

// pre-compiled headers
#include "pch.h"

// utility macros
#include "UtilMacros.h"

// API declaration
#include "WarpTpsLib.h"

// the main include for the class
#include "ModelObject.h"

// debug new statement
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// model object to compile
static CModelObject* generate = new CModelObject("test");