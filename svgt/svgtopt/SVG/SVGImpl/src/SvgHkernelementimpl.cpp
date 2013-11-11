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


#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif
#include "SvgHkernelementimpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"

#include "GfxAffineTransform.h"


#ifdef SVG_FONTS_INCLUDE

// ---------------------------------------------------------------------------
// Two phase construction
// ---------------------------------------------------------------------------
CSvgHkernElementImpl* CSvgHkernElementImpl::NewL(  const TUint8 aElemID,
                                                CSvgDocumentImpl* aDoc )
    {
    CSvgHkernElementImpl*self    = new ( ELeave ) CSvgHkernElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL( aElemID);
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgHkernElementImpl* CSvgHkernElementImpl::NewLC(  const TUint8 aElemID,
                                                 CSvgDocumentImpl* aDoc )
    {
    CSvgHkernElementImpl*self    = new ( ELeave ) CSvgHkernElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(  aElemID );

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgHkernElementImpl::ConstructL(  const TUint8 aElemID)
    {
    CSvgElementImpl::InitializeL( aElemID );

	#ifdef SVG_FLOAT_BUILD
    iK = TFloatFixPt(0.0f);
	#else
    iK = TFloatFixPt( 0 ,ETrue);
	#endif

    iG1 = HBufC::NewL( 0 );
    iG2 = HBufC::NewL( 0 );
    iU1 = HBufC::NewL( 0 );
    iU2 = HBufC::NewL( 0 );

    iReqAttrFlag=KSVG_HKERN_ELEMFLAG;

    }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgHkernElementImpl::~CSvgHkernElementImpl()
    {
    if ( iG1 )
        {
        delete iG1;
        iG1 = NULL;
        }

    if ( iG2 )
        {
        delete iG2;
        iG2 = NULL;
        }

    if ( iU1 )
        {
        delete iU1;
        iU1 = NULL;
        }

    if (iU2)
        {
        delete iU2;
        iU2 = NULL;
        }
    }

// ===================================
// From SVG DOM
// ===================================

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgHkernElementImpl::GetK()
    {
    return iK;
    }


// ===================================
// From MXmlElement
// ===================================

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgHkernElementImpl::SetAttributeL( const TDesC& aName,
                                         const TDesC& aValue )
    {



    CSvgElementImpl::SetAttributeL(aName,aValue);

    return KErrNone;
    }


TInt  CSvgHkernElementImpl::SetAttributeDesL( const TInt aNameId, const TDesC& aValue )
{
    switch (aNameId)
        {
        case KAtrG1:
                        delete iG1;
                        iG1 = NULL;
                        iG1 = aValue.AllocL();
                        break;
        case KAtrG2:
                        delete iG2;
                        iG2 = NULL;
                        iG2 = aValue.AllocL();
                        break;
        case KAtrU1:
                        delete iU1;
                        iU1 = NULL;
                        iU1 = aValue.AllocL();
                        break;
        case KAtrU2:
                        delete iU2;
                        iU2 = NULL;
                        iU2 = aValue.AllocL();
                        break;
        case KAtrK:
                        {
                        TLex    lString ( aValue );
                        TReal32 lVal;
                        if( lString.Val( lVal, '.' ) != KErrNone )
                            lVal = 0;
                        iK = (TFloatFixPt) lVal;
                        iReqAttrFlag = 0;
                        break;
                        }
        default:        return CSvgElementImpl::SetAttributeDesL( aNameId, aValue );

        }
    return KErrNone;
}


// From MXmlElementOpt
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgHkernElementImpl::GetAttributeFloat( const TInt aNameId,
                                             TFloatFixPt& aValue )
    {
    switch ( aNameId )
        {
        case KAtrK:
        aValue = GetK();
        break;
        default:
        return CSvgElementImpl::GetAttributeFloat( aNameId, aValue );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgHkernElementImpl::SetAttributeFloatL( const TInt aNameId,
                                             const TFloatFixPt aValue )
    {
    switch ( aNameId )
        {
        case KAtrK:
        iK= aValue;
        break;
        default:
        return CSvgElementImpl::SetAttributeFloatL( aNameId, aValue );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgHkernElementImpl::GetAttributeDes( const TInt aNameId, TPtrC16& aValue )
    {
    switch ( aNameId )
        {
        case KAtrG1:
        if ( iG1->Length() > 0 )
            {
            aValue.Set( *iG1 );
            break;
            }
        else
            return KErrNoAttribute;
        case KAtrG2:
        if ( iG2->Length() > 0 )
            {
            aValue.Set( *iG2 );
            break;
            }
        else
            return KErrNoAttribute;
        case KAtrU1:
        if ( iU1->Length() > 0 )
            {
            aValue.Set( *iU1 );
            break;
            }
        else
            return KErrNoAttribute;
        case KAtrU2:
        if ( iU2->Length() > 0 )
            {
            aValue.Set( *iU2 );
            break;
            }
        else
            return KErrNoAttribute;

        default:
        return CSvgElementImpl::GetAttributeDes( aNameId, aValue );
        }
    return KErrNone;
    }


// ===================================
// From CSvgElementImpl
// ===================================

// ---------------------------------------------------------------------------
// perform a deep clone of this object
// ---------------------------------------------------------------------------
MXmlElement* CSvgHkernElementImpl::CloneL(MXmlElement*)
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgHkernElementImpl::DrawL( CGfx2dGc* /* aGc */,
                                  CSvgElementImpl* /* aElement */ )
    {
    return EFalse;
    }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

CSvgHkernElementImpl::CSvgHkernElementImpl( CSvgDocumentImpl* aDoc )
    {
    SetOwnerDocument(aDoc);
    }

void CSvgHkernElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<hkern u1=\"hmm\" u2=\"hmm\" g1=\"hmm\" g2=\"hmm\" k=\"%d\" \\>", /*iU1->Des(), iU2->Des(), iG1->Des(), iG2->Des(),*/ (int)iK);
		#endif
	}
}

#endif    //ifdef SVG_FONTS_INCLUDE
