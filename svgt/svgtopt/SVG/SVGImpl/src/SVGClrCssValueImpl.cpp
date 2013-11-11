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


#include "SVGClrCssValueImpl.h"
#include "SVGElementImpl.h"


// *******************************************************
// Constructor/deconstructor


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CClrCssValueImpl::~CClrCssValueImpl()
    {
        if(iValue)
            {
            delete iValue;
            iValue = NULL;
            }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

TSvgColor* CClrCssValueImpl::Value()
    {
    return iValue;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CClrCssValueImpl::SetValueL( const TDesC& aValue )
    {

    if(iValue)
        {
        delete iValue;
        iValue=NULL;
        }

    _LIT( KNone, "none" );
    _LIT( KCurrentColor, "currentColor" );

    TUint32 tempVal;
    if ( aValue == KNone )
        {
        tempVal = KGfxColorNull;
        }
    else if ( aValue == KCurrentColor )
        {
        tempVal = KSvgCurrentColor;
        }
    else
        {
        TSvgColor tColor(KGfxColorNull);
        if( tColor.GetStringL( aValue,tempVal) != EFalse )
            {
            }
        else
            {
            tempVal = 0;
            }

        }

    iValue = new (ELeave) TSvgColor(tempVal);
    return KErrNone;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CClrCssValueImpl::SetValueL( const TInt& aValue )
    {
    if(iValue)
        {
        delete iValue;
        iValue=NULL;
        }

    iValue = new (ELeave)TSvgColor (aValue);
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

void CClrCssValueImpl::SetL(CClrCssValueImpl* aValueToMimic)
{
	CloneRGBValueL(aValueToMimic->iValue->GetColor());
}

// *******************************************************
// Private


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CClrCssValueImpl::CClrCssValueImpl(  ):iValue(NULL)
    {

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CClrCssValueImpl::CloneRGBValueL(const TInt& aValue)
    {
    if ( iValue )
        {
        delete ( TSvgColor * ) iValue;
        iValue = NULL;
        }

    iValue = new ( ELeave ) TSvgColor( (TUint32)aValue );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CClrCssValueImpl::IsEqual( CCssValue* aValue )
{
	if (((CClrCssValueImpl*)aValue)->iValue == iValue)
	{
		return ETrue;
	}

	return EFalse;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CClrCssValueImpl::Print()
{
#ifdef _DEBUG
	if (iValue)
	RDebug::Printf("%x", iValue->GetColor());
#endif
}
