/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  NVG Decoder source file
 *
*/
#include "FloatFixPt.h"

//==============================floatfixpt functions============================
// ==========================================================================
// Constructor, default to zero
// ==========================================================================
TFloatFixPt::TFloatFixPt() : iValue(0)
    {
    }

// ==========================================================================
// Constructor
// ==========================================================================

TFloatFixPt::TFloatFixPt(TInt aVal)
    {
    iValue = (aVal << KFixPtFrac);
    }
// ==========================================================================
// Constructor
// ==========================================================================
 TFloatFixPt::TFloatFixPt(TInt32 aVal)
    {
    iValue = (aVal << KFixPtFrac);
    }
 TFloatFixPt::TFloatFixPt(TInt aVal, TBool /*a*/)
    {
    iValue = aVal;
    }
// ==========================================================================
// Constructor
// ==========================================================================
 TFloatFixPt::TFloatFixPt(TReal32 aVal)
{
    iValue = svgScalarFromFloat(aVal);
}

// ==========================================================================
// Assignment operator
// ==========================================================================
 TFloatFixPt& TFloatFixPt::operator=(TReal32 aVal)
{
    iValue = svgScalarFromFloat(aVal);
    return *this;
}

 void TFloatFixPt::copyfloatfix(TInt aVal)
{
	iValue = aVal;
}
// ==========================================================================
// Round to the nearest whole number
// ==========================================================================
 TInt32 TFloatFixPt::Round()
    {
    if (0 != (iValue & 0x8000))
        return (iValue >> KFixPtFrac) + 1;
    else
        return (iValue >> KFixPtFrac);
    }
// ===================================================================
// Addition operation
// ==========================================================================
 TFloatFixPt TFloatFixPt::operator+(const TFloatFixPt& aVal) const
    {
    return TFloatFixPt(iValue + aVal.iValue, ETrue);
    }
//==============================floatfixpt functions============================

 TFloatFixPt::operator TReal32() const
{
    return ((TReal32) iValue) / KFixPtFracVal;

}
// ==========================================================================
// Get 32-bit representation
// ==========================================================================
 TInt32 TFloatFixPt::RawData()
    {
    return iValue;
    }    
 //--------------------------------EndOfFile------------------------------------
