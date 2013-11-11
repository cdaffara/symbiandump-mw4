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
#include "SVGCircleElementImpl.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"

#include "GfxAffineTransform.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgCircleElementImpl* CSvgCircleElementImpl::NewL(  const TUint8 aElemID,
                                                    CSvgDocumentImpl* aDoc )
    {
    CSvgCircleElementImpl*  self    = new ( ELeave )
                                      CSvgCircleElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL( aElemID );
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgCircleElementImpl* CSvgCircleElementImpl::NewLC(  const TUint8 aElemID,
                                                     CSvgDocumentImpl* aDoc )
    {
    CSvgCircleElementImpl*  self    = new ( ELeave )
                                      CSvgCircleElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(  aElemID );

    return self;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgCircleElementImpl::ConstructL(  const TUint8 aElemID )
    {
    CSvgElementImpl::InitializeL(  aElemID );

    iSvgStyleProperties = new(ELeave) RPointerArray<CCssValue>(KCSS_MAX_ATTR);
     User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
    iSvgStyleProperties->Remove( 0 );

    iSvgTransformable = CSvgTransformableImpl::NewL();

	iReqAttrFlag=KSVG_CIRCLE_ELEMFLAG;

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgCircleElementImpl::~CSvgCircleElementImpl()
    {

	if ( iSvgStyleProperties )
		{
		iSvgStyleProperties->Close();
		delete iSvgStyleProperties;
		iSvgStyleProperties = NULL;
		}
    }

// *******************************************************
// From SVG DOM

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgCircleElementImpl::Cx()
    {
	#ifdef SVG_FLOAT_BUILD
		return iCircle.iX + (iCircle.iWidth * TFloatFixPt( 0.5f ) );
	#else
	return iCircle.iX + (iCircle.iWidth >> 1);
	#endif

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

TFloatFixPt CSvgCircleElementImpl::Cy()
    {
	#ifdef SVG_FLOAT_BUILD
    return iCircle.iY + (iCircle.iHeight * .5f);
	#else
    return iCircle.iY + (iCircle.iHeight >> 1);
	#endif
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

TFloatFixPt CSvgCircleElementImpl::R()
    {
	#ifdef SVG_FLOAT_BUILD
    return iCircle.iWidth * .5f;
	#else
    return iCircle.iWidth >> 1;
	#endif
    }

// *******************************************************
// SVG Implementation

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

void CSvgCircleElementImpl::SetCx( TFloatFixPt aCx )
    {
	#ifdef SVG_FLOAT_BUILD
    iCircle.iX = aCx - (iCircle.iWidth * .5f);
	#else
    iCircle.iX = aCx - (iCircle.iWidth >> 1);
	#endif
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

void CSvgCircleElementImpl::SetCy( TFloatFixPt aCy )
    {
	#ifdef SVG_FLOAT_BUILD
    iCircle.iY = aCy - (iCircle.iHeight * .5f);
	#else
    iCircle.iY = aCy - (iCircle.iHeight >> 1);
	#endif

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

void CSvgCircleElementImpl::SetR( TFloatFixPt aR )
    {
	#ifdef SVG_FLOAT_BUILD
    if (aR < TFloatFixPt(0))
        {
        aR = 0;
        }
    iCircle.iX += (iCircle.iWidth * TFloatFixPt(.5f)) - aR;
    iCircle.iWidth = aR * TFloatFixPt(2.0f);
    // Both x and width will be changed.
    iCircle.iY += (iCircle.iHeight * TFloatFixPt(.5f)) - aR;
    iCircle.iHeight = aR * TFloatFixPt(2.0f);
	#else
    if (aR < TFloatFixPt(0, ETrue))
        {
        aR = TFloatFixPt(0, ETrue);
        }
    iCircle.iX += (iCircle.iWidth >> 1) - aR;
    iCircle.iWidth = aR << 1;
    // Both x and width will be changed.
    iCircle.iY += (iCircle.iHeight >> 1) - aR;
    iCircle.iHeight = aR << 1;
	#endif

    }

// *******************************************************
// From MXmlElement

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgCircleElementImpl::SetAttributeL( const TDesC& aName,
                                           const TDesC& aValue )
    {
	CSvgElementImpl::SetAttributeL(aName,aValue);
    return KErrNone;
    }


// From MXmlElementOpt

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgCircleElementImpl::GetAttributeFloat( const TInt aNameId,
                                               TFloatFixPt& aValue )
    {
    switch ( aNameId )
        {
        case KAtrCx:
        case KAtrRefX:
        aValue = Cx();
        break;
        case KAtrCy:
        case KAtrRefY:
        aValue = Cy();
        break;
        case KAtrRadius:
        aValue = R();
        break;
        default:
        return CSvgElementImpl::GetAttributeFloat( aNameId, aValue );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgCircleElementImpl::SetAttributeFloatL( const TInt aNameId,
                                                const TFloatFixPt aValue )
    {
    switch ( aNameId )
        {
        case KAtrCx:
        SetCx( aValue );
        break;
        case KAtrCy:
        SetCy( aValue );
        break;
        case KAtrRadius:
		SetR( aValue );
		iReqAttrFlag = 0;
		if ( WasTurnedOff() )
        {
        _LIT(KInline , "inline");
        // turn it on. this means all the required attributes Are present and
        // hence we can render it.
        CSvgElementImpl::SetPropertyL(KCSS_ATTR_DISPLAY, KInline);
        SetTurnOff( EFalse );
        }
		break;
        default:
        return CSvgElementImpl::SetAttributeFloatL( aNameId, aValue );
        }
    return KErrNone;
    }

// *******************************************************
// From CSvgElementImpl

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgCircleElementImpl::DrawL( CGfx2dGc* aGc, CSvgElementImpl* aElement )
    {
	TFloatFixPt lValue;
	this->GetAttributeFloat(KAtrRadius,lValue);
	if (lValue.iValue <= 0)
		{
		return ETrue;
		}
    this->DrawShapeL( aGc, iCircle, aElement );
    return ETrue;
    }

// *******************************************************
// Private



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

CSvgCircleElementImpl::CSvgCircleElementImpl( CSvgDocumentImpl* aDoc ) : iCircle( 0,
                                                                                  0,
                                                                                  0,
                                                                                  0 )
    {
	SetOwnerDocument(aDoc);
    }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgCircleElementImpl::GetBBox( TGfxRectangle2D& aBbox )
    {
    iCircle.GetBounds( GetCTM(), aBbox );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgCircleElementImpl::GetUnscaledBBox( TGfxRectangle2D& aBbox )
    {
    TGfxAffineTransform identityTx;
    iCircle.GetBounds( identityTx, aBbox );
    }

// perform a deep clone of this object
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
MXmlElement* CSvgCircleElementImpl::CloneL(MXmlElement* aParentElement)
    {

	CSvgCircleElementImpl* newElement = CSvgCircleElementImpl::NewL( this->ElemID(),
																	 ((CSvgDocumentImpl*)iOwnerDocument) );


	CleanupStack::PushL(newElement);
	newElement->iParentNode = aParentElement;

    // copy everything over
    this->CopyL(newElement);
    CleanupStack::Pop();

    return newElement;
    }

// perform a deep copy of this object
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgCircleElementImpl::CopyL( CSvgCircleElementImpl* aDestElement )
    {
   if(aDestElement)
	   	{
	    // copy iCircle items special to circle
	    aDestElement->iCircle.iX = this->iCircle.iX;
	    aDestElement->iCircle.iY = this->iCircle.iY;
	    aDestElement->iCircle.iWidth = this->iCircle.iWidth;
	    aDestElement->iCircle.iHeight = this->iCircle.iHeight;

	    // copy the reference to idoc (CSvgDocumentImpl)

	    aDestElement->iOwnerDocument = this->iOwnerDocument;

		// copy stuff from superclass
	    this->CSvgElementImpl::CopyL(aDestElement);
	    }

    }

void CSvgCircleElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<circle x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\">", (int)iCircle.iX, (int)iCircle.iY, (int)iCircle.iWidth, (int)iCircle.iHeight);
		#endif
	}
}
