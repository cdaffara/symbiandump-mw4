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


#if !defined(__E32DEBUG_H__)
#include <e32debug.h>
#endif

#include "SVGIntCssValueImpl.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CIntCssValueImpl::~CIntCssValueImpl()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CIntCssValueImpl::SetValueL( const TDesC& aValue )
    {

    TInt32 tInt;
    TLex tLex( aValue );
    if ( tLex.Val( tInt ) == KErrNone )
        {
        iValue = tInt;
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CIntCssValueImpl::SetValueL( const TInt& aValue )
    {
    iValue = aValue;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CIntCssValueImpl::Value()
    {
    return iValue;
    }

// *******************************************************
// Private

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CIntCssValueImpl::CIntCssValueImpl(  )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CIntCssValueImpl::IsEqual( CCssValue* aValue )
{
	if (((CIntCssValueImpl*)aValue)->iValue == iValue)
	{
		return ETrue;
	}

	return EFalse;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CIntCssValueImpl::Print( )
{
#ifdef _DEBUG
	RDebug::Printf("%d", iValue);
#endif
}
