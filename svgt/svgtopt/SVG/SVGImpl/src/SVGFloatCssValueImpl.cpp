/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Implementation source file
 *
*/


#ifdef _DEBUG
#if !defined(__E32DEBUG_H__)
#include <e32debug.h>
#endif
#endif
#include "SVGFloatCssValueImpl.h"




// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CFloatCssValueImpl::CFloatCssValueImpl()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CFloatCssValueImpl::CFloatCssValueImpl( float aValue )
{
    iValue = aValue;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CFloatCssValueImpl::~CFloatCssValueImpl()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CFloatCssValueImpl::Value()
    {
    return iValue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CFloatCssValueImpl::SetValueL( const TDesC& aValue )
    {

    TReal32 tReal;
    TLex tLex( aValue );
    if ( tLex.Val( tReal, '.' ) == KErrNone )
        {
        iValue = ( TFloatFixPt ) tReal;
        }
    return KErrNone;
    }

TInt CFloatCssValueImpl::SetValueL( const TFloatFixPt& aValue )
    {
    iValue = aValue;
    return KErrNone;
    }


// *******************************************************
// Private


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CFloatCssValueImpl::IsEqual( CCssValue* aValue )
{
	if (((CFloatCssValueImpl*)aValue)->iValue == iValue)
	{
		return ETrue;
	}

	return EFalse;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CFloatCssValueImpl::Print()
{
	#ifdef _DEBUG
	RDebug::Printf("%d", (int)iValue);
	#endif
}
