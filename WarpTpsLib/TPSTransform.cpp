//////////////////////////////////////////////////////////////////////
// TPSTransform.cpp: implementation of the CTPSTransform class.
//
// Copyright (C) 2002-2020 Derek Lane
// $Id: TPSTransform.cpp,v 1.3 2003/04/26 20:52:46 default Exp $
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "WarpTpsLib.h"

#include "MatrixNxM.h"
#include "MatrixBase.inl"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CTPSTransform* pGenerateTransform = new CTPSTransform();