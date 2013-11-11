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

#include "SVGVectorCssValueImpl.h"
#include "SVGPointLexer.h"




// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CVectorCssValueImpl::~CVectorCssValueImpl()
    {
    if ( iValue )
        {
        iValue->Reset();
        delete iValue;
        iValue = NULL;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CVectorCssValueImpl::Value( CArrayFix<TFloatFixPt>*& aArray )
    {
    aArray = iValue;

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CArrayFix<TFloatFixPt>* CVectorCssValueImpl::Value( )
    {
    return iValue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CVectorCssValueImpl::SetValueL( const TDesC& aValue )
    {

    if ( iValue )
        {
        iValue->Reset();
        delete iValue;
        iValue = NULL;
        }
    iValue = new ( ELeave ) CArrayFixFlat<TFloatFixPt>( 1 );


    TSvgPointLexer svgplex( aValue );

    while ( !svgplex.IsDone() )
        {
        TChar com = ',';
        TFloatFixPt dashf;
        TInt32 rslt = svgplex.GetNext( com, dashf );
        if ( rslt != KErrNone )
            {
            break;
            }
        iValue->AppendL( dashf );
        }
        svgplex.Cleanup();

    return KErrNone;
    }


// *******************************************************
// Private

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CVectorCssValueImpl::CVectorCssValueImpl():iValue( NULL )
    {
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CVectorCssValueImpl::CVectorCssValueImpl(CArrayFix<TFloatFixPt>* aValue  ):iValue( aValue )
    {
    }

// ---------------------------------------------------------------------------
// function for encoding
// ---------------------------------------------------------------------------
void  CVectorCssValueImpl::SetValueL( CArrayFix<TFloatFixPt>*& aValue )
    {
     if ( iValue )
        {
        iValue->Reset();
        delete iValue;
        iValue = NULL;
        }

    iValue= aValue;
    }

// ---------------------------------------------------------------------------
// function for encoding
// ---------------------------------------------------------------------------
void  CVectorCssValueImpl::CloneValueL( CArrayFix<TFloatFixPt>*& aValue )
    {
     if ( iValue )
        {
        iValue->Reset();
        delete iValue;
        iValue = NULL;
        }

	iValue = new ( ELeave ) CArrayFixFlat<TFloatFixPt>( 1 );
	
	TInt valCnt = aValue->Count();
	for (TInt i=0; i < valCnt; i++)
	{
		iValue->AppendL( aValue->At(i) );
	}

    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CVectorCssValueImpl::IsEqual( CCssValue* aValue )
{
	if (((CVectorCssValueImpl*)aValue)->iValue == iValue)
	{
		return ETrue;
	}

	return EFalse;
}

void CVectorCssValueImpl::Print()
{
	#ifdef _DEBUG
	
	TInt valCnt = iValue->Count();
	for (TInt i=0; i < valCnt; i++)
	{
		RDebug::Printf("%d, ", (int)(iValue->At(i)));
	}
	#endif
}
