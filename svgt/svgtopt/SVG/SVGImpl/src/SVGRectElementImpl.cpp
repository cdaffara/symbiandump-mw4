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
#include "SVGRectElementImpl.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"
#include "SVGFourPointRect.h"

#include "GfxAffineTransform.h"



// ==========================================================================
// Need method description
// ==========================================================================
CSvgRectElementImpl* CSvgRectElementImpl::NewL(  const TUint8 aElemID,
												CSvgDocumentImpl* aDoc )
	{
	CSvgRectElementImpl*self    = new ( ELeave ) CSvgRectElementImpl( aDoc );
	CleanupStack::PushL( self );
	self->ConstructL(  aElemID);
	CleanupStack::Pop();

	return self;
	}

// ==========================================================================
// Need method description
// ==========================================================================
CSvgRectElementImpl* CSvgRectElementImpl::NewLC(  const TUint8 aElemID,
												 CSvgDocumentImpl* aDoc )
	{
	CSvgRectElementImpl*self    = new ( ELeave ) CSvgRectElementImpl( aDoc );
	CleanupStack::PushL( self );
	self->ConstructL(  aElemID );

	return self;
	}




// ==========================================================================
// Need method description
// ==========================================================================
CSvgRectElementImpl::~CSvgRectElementImpl()
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

// ==========================================================================
// Need method description
// ==========================================================================
TFloatFixPt CSvgRectElementImpl::X()
    {
    return iRectangle.iX;
    }

// ==========================================================================
// Need method description
// ==========================================================================
TFloatFixPt CSvgRectElementImpl::Y()
    {
    return iRectangle.iY;
    }

// ==========================================================================
// Need method description
// ==========================================================================
TFloatFixPt CSvgRectElementImpl::Width()
    {
    return iRectangle.iWidth;
    }

// ==========================================================================
// Need method description
// ==========================================================================
TFloatFixPt CSvgRectElementImpl::Height()
    {
    return iRectangle.iHeight;
    }

// ==========================================================================
// Need method description
// ==========================================================================
TFloatFixPt CSvgRectElementImpl::Rx()
    {
    return iRectangle.iArcWidth;
    }

// ==========================================================================
// Need method description
// ==========================================================================
TFloatFixPt CSvgRectElementImpl::Ry()
    {
    return iRectangle.iArcHeight;
    }

// ==========================================================================
// For JSR support
// ==========================================================================
void CSvgRectElementImpl::SetRectValues(TFloatFixPt aX, TFloatFixPt aY, TFloatFixPt aWidth, TFloatFixPt aHeight)
    {
    iRectangle.iX = aX;
    iRectangle.iY = aY;
    iRectangle.iHeight = aHeight;
    iRectangle.iWidth =  aWidth;
    }


// From MXmlElementOpt

// ==========================================================================
// Need method description
// ==========================================================================
TInt CSvgRectElementImpl::GetAttributeFloat( const TInt aNameId,
                                             TFloatFixPt& aValue )
    {
    switch ( aNameId )
        {
        case KAtrX:
        case KAtrRefX:
        aValue = X();
        break;
        case KAtrY:
        case KAtrRefY:
        aValue = Y();
        break;
        case KAtrWidth:
        aValue = Width();
        break;
        case KAtrHeight:
        aValue = Height();
        break;
		// added as a part of get and Set api update
		case KAtrRx:
		aValue = iRectangle.iArcWidth;
		break;
		case KAtrRy:
		aValue = iRectangle.iArcHeight;
		break;
        default:
        return CSvgElementImpl::GetAttributeFloat( aNameId, aValue );
        }
    return KErrNone;
    }

// ==========================================================================
// Need method description
// ==========================================================================
TInt CSvgRectElementImpl::SetAttributeFloatL( const TInt aNameId,
                                              const TFloatFixPt aValue )
    {
    switch ( aNameId )
        {
        case KAtrX:
					iRectangle.iX = aValue;
			        break;
        case KAtrY:

					iRectangle.iY = aValue;
				    break;

		case KAtrRx:
					iRectangle.iArcWidth = aValue;
					break;
		case KAtrRy:
					iRectangle.iArcHeight = aValue;
					break;


        case KAtrWidth:
					if(iReqAttrFlag == KAtrSVGRec)
						{
						iReqAttrFlag = KAtrHeight;
						}
					else
						{
						iReqAttrFlag = 0;
						
						if( WasTurnedOff() )
			                {
			                _LIT(KInline , "inline");
			                // turn it on. this means all the required attributes Are present and
			                // hence we can render it.
			                CSvgElementImpl::SetPropertyL(KCSS_ATTR_DISPLAY, KInline);
			                SetTurnOff( EFalse );
			                }
						}
					iRectangle.iWidth = aValue;
			        break;

        case KAtrHeight:
					if(iReqAttrFlag == KAtrSVGRec)
						{
						iReqAttrFlag = KAtrWidth;
						}
					else
						{
						iReqAttrFlag = 0;
						if( WasTurnedOff() )
			                {
			                _LIT(KInline , "inline");
			                // turn it on. this means all the required attributes Are present and
			                // hence we can render it.
			                CSvgElementImpl::SetPropertyL(KCSS_ATTR_DISPLAY, KInline);
			                SetTurnOff( EFalse );
			                }
						}
					iRectangle.iHeight = aValue;
			        break;
        default:
        return CSvgElementImpl::SetAttributeFloatL( aNameId, aValue );
        }
    return KErrNone;
    }


// *******************************************************
// From MXmlElement


// ==========================================================================
// Need method description
// ==========================================================================
TInt CSvgRectElementImpl::SetAttributeL( const TDesC& aName,
                                         const TDesC& aValue )
    {

	CSvgElementImpl::SetAttributeL(aName,aValue);
    return KErrNone;
    }

// ==========================================================================
// Need method description
// ==========================================================================
TBool CSvgRectElementImpl::DrawL( CGfx2dGc* aGc, CSvgElementImpl* aElement )
    {
	if (iRectangle.iWidth.iValue <= 0 || iRectangle.iHeight.iValue <= 0 )
		{
		return ETrue;
		}
    DrawShapeL( aGc, iRectangle, aElement );
    return ETrue;
    }


// ==========================================================================
// Need method description
// ==========================================================================
void CSvgRectElementImpl::ConstructL(  const TUint8 aElemID )
	{
	CSvgElementImpl::InitializeL( aElemID );

	 iSvgStyleProperties = new(ELeave) RPointerArray<CCssValue>(KCSS_MAX_ATTR );

	 User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
     iSvgStyleProperties->Remove( 0 );

    iSvgTransformable = CSvgTransformableImpl::NewL();

	iReqAttrFlag=KSVG_RECT_ELEMFLAG;

	}



// ==========================================================================
// Need method description
// ==========================================================================

CSvgRectElementImpl::CSvgRectElementImpl( CSvgDocumentImpl* aDoc ) : iRectangle()
    {
	SetOwnerDocument(aDoc);
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgRectElementImpl::GetFourPointBBox(TSvgFourPointRect& aFourPointBbox)
{
	const TGfxAffineTransform& ctm = GetCTM();

	TSvgFourPointRect lFourPointBox(iRectangle.iX, iRectangle.iY, iRectangle.iWidth, iRectangle.iHeight);

    TGfxPoint2D lPoint1, lPoint2, lPoint3, lPoint4;

    lFourPointBox.GetPoints(lPoint1, lPoint2, lPoint3, lPoint4);

    ctm.Transform(&lPoint1, &lPoint1, 1);
    ctm.Transform(&lPoint2, &lPoint2, 1);
    ctm.Transform(&lPoint3, &lPoint3, 1);
    ctm.Transform(&lPoint4, &lPoint4, 1);

    aFourPointBbox.SetRectPoints(lPoint1, lPoint2, lPoint3, lPoint4);
}

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgRectElementImpl::GetBBox( TGfxRectangle2D& aBbox )
    {
    iRectangle.GetBounds( GetCTM(), aBbox );
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgRectElementImpl::GetUnscaledBBox( TGfxRectangle2D& aBbox )
    {
    TGfxAffineTransform identityTx;
    iRectangle.GetBounds( identityTx, aBbox );
    }

// ==========================================================================
// perform a deep clone of this object
// ==========================================================================
MXmlElement* CSvgRectElementImpl::CloneL(MXmlElement* aParentElement)
    {
    // create new rectangle
	CSvgRectElementImpl* newElement = CSvgRectElementImpl::NewL(  this->ElemID(), ((CSvgDocumentImpl*)iOwnerDocument) );

	CleanupStack::PushL(newElement);
	newElement->iParentNode = aParentElement;
    // copy everything over
    this->CopyL(newElement);
	CleanupStack::Pop();
    return newElement;
    }

// ==========================================================================
// perform a deep copy of this object
// ==========================================================================
void CSvgRectElementImpl::CopyL( CSvgRectElementImpl* aDestElement )
    {
    if(aDestElement)
		{
	    // copy stuff from superclass
	    this->CSvgElementImpl::CopyL(aDestElement);

	    // copy iRectangle items special to circle
	    aDestElement->iRectangle.iX = this->iRectangle.iX;
	    aDestElement->iRectangle.iY = this->iRectangle.iY;
	    aDestElement->iRectangle.iHeight = this->iRectangle.iHeight;
	    aDestElement->iRectangle.iWidth = this->iRectangle.iWidth;
	    aDestElement->iRectangle.iArcHeight = this->iRectangle.iArcHeight;
	    aDestElement->iRectangle.iArcWidth = this->iRectangle.iArcWidth;

	    // copy the reference to idoc (CSvgDocumentImpl)
	    aDestElement->iOwnerDocument = this->iOwnerDocument;
		}

    }

void CSvgRectElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" rx=\"%d\" ry=\"%d\">", (int)iRectangle.iX, (int)iRectangle.iY, (int)iRectangle.iWidth, (int)iRectangle.iHeight, (int)iRectangle.iArcWidth, (int)iRectangle.iArcHeight);
		#endif
	}
}
