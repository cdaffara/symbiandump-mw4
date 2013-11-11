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
#include "SVGEllipseElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"

#include "GfxAffineTransform.h"
_LIT(KInline , "inline");

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgEllipseElementImpl* CSvgEllipseElementImpl::NewL(  const TUint8 aElemID,
                                                      CSvgDocumentImpl* aDoc )
    {
    CSvgEllipseElementImpl* self    = new ( ELeave )
                                      CSvgEllipseElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(  aElemID );
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgEllipseElementImpl* CSvgEllipseElementImpl::NewLC(  const TUint8 aElemID,
                                                       CSvgDocumentImpl* aDoc )
    {
    CSvgEllipseElementImpl* self    = new ( ELeave )
                                      CSvgEllipseElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(  aElemID );

    return self;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgEllipseElementImpl::ConstructL( const TUint8 aElemID )
    {
    CSvgElementImpl::InitializeL( aElemID );
     iSvgStyleProperties = new(ELeave) RPointerArray<CCssValue>(KCSS_MAX_ATTR);
     User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
     iSvgStyleProperties->Remove( 0 );



    iSvgTransformable = CSvgTransformableImpl::NewL();
    //iSvgTests = CSvgTestsImpl::NewL();

	iReqAttrFlag=KSVG_ELLIPSE_ELEMFLAG;

    }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgEllipseElementImpl::~CSvgEllipseElementImpl()
    {

	if( iSvgStyleProperties )
		{
		iSvgStyleProperties->Close();
		delete iSvgStyleProperties;
		iSvgStyleProperties = NULL;
		}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

TFloatFixPt CSvgEllipseElementImpl::Rx()
    {
	#ifdef SVG_FLOAT_BUILD
    return iEllipse.iWidth * .5f;
	#else
    return iEllipse.iWidth >> 1;
	#endif
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgEllipseElementImpl::Ry()
    {
	#ifdef SVG_FLOAT_BUILD
    return iEllipse.iHeight * .5f;
	#else
    return iEllipse.iHeight >> 1;
	#endif

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgEllipseElementImpl::Cx()
    {
	#ifdef SVG_FLOAT_BUILD
    return iEllipse.iX + (iEllipse.iWidth * .5f);
	#else
    return iEllipse.iX + (iEllipse.iWidth >> 1);
	#endif
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgEllipseElementImpl::Cy()
    {
	#ifdef SVG_FLOAT_BUILD
    return iEllipse.iY + (iEllipse.iHeight * .5f);
	#else

    return iEllipse.iY + (iEllipse.iHeight >> 1);
	#endif

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgEllipseElementImpl::SetRx( TFloatFixPt aRx )
    {
	#ifdef SVG_FLOAT_BUILD
    // Both x and width will be changed.
    iEllipse.iX += (iEllipse.iWidth * .5f) - aRx;
    iEllipse.iWidth = aRx * TFloatFixPt(2);
	#else
    // Both x and width will be changed.
    iEllipse.iX += (iEllipse.iWidth >> 1) - aRx;
    iEllipse.iWidth = aRx << 1;
	#endif
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgEllipseElementImpl::SetRy( TFloatFixPt aRy )
    {
	#ifdef SVG_FLOAT_BUILD
    // Both x and width will be changed.
    iEllipse.iY += (iEllipse.iHeight * .5f) - aRy;
    iEllipse.iHeight = aRy * TFloatFixPt(2);
	#else
    // Both x and width will be changed.
    iEllipse.iY += (iEllipse.iHeight >> 1) - aRy;
    iEllipse.iHeight = aRy << 1;
	#endif
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgEllipseElementImpl::SetCx( TFloatFixPt aCx )
    {
	#ifdef SVG_FLOAT_BUILD
    iEllipse.iX = aCx - (iEllipse.iWidth * .5f);
	#else
    iEllipse.iX = aCx - (iEllipse.iWidth >> 1);
	#endif
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgEllipseElementImpl::SetCy( TFloatFixPt aCy )
    {
	#ifdef SVG_FLOAT_BUILD
    iEllipse.iY = aCy - (iEllipse.iHeight * .5f);
	#else

    iEllipse.iY = aCy - (iEllipse.iHeight >> 1);
	#endif

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgEllipseElementImpl::SetAttributeL( const TDesC& aName,
                                            const TDesC& aValue )
    {

	CSvgElementImpl::SetAttributeL(aName,aValue);


    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgEllipseElementImpl::GetAttributeFloat( const TInt aNameId,
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
        case KAtrRx:
            aValue = Rx();
        break;
        case KAtrRy:
            aValue = Ry();
        break;
        default:
            return CSvgElementImpl::GetAttributeFloat( aNameId, aValue );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgEllipseElementImpl::SetAttributeFloatL( const TInt aNameId,
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

        case KAtrRx:
		if(iReqAttrFlag == KAtrSVGElp)
			{
			iReqAttrFlag = KAtrRy;
			}
		else
			{
			iReqAttrFlag = 0;
    			if (  WasTurnedOff() )
                {
                // turn it on. this means all the required attributes Are present and
                // hence we can render it.
                CSvgElementImpl::SetPropertyL(KCSS_ATTR_DISPLAY, KInline);
                SetTurnOff( EFalse );
                }
			}

		SetRx( aValue );


        break;
        case KAtrRy:
		if(iReqAttrFlag == KAtrSVGElp)
			{
			iReqAttrFlag = KAtrRx;
			}
		else
			{
			iReqAttrFlag = 0;
			    if ( WasTurnedOff() )
                {
                // turn it on. this means all the required attributes Are present and
                // hence we can render it.
                CSvgElementImpl::SetPropertyL(KCSS_ATTR_DISPLAY, KInline);
                SetTurnOff( EFalse );
                }
			}

        SetRy( aValue );
        break;
        default:
            return CSvgElementImpl::SetAttributeFloatL( aNameId, aValue );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgEllipseElementImpl::DrawL( CGfx2dGc* aGc, CSvgElementImpl* aElement )
    {
    // If either of width or height is zero no need to draw the ellipse.
    if(iEllipse.iWidth.iValue && iEllipse.iHeight.iValue)
        {
        DrawShapeL( aGc, iEllipse, aElement );
        }
    return ETrue;
    }




// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

CSvgEllipseElementImpl::CSvgEllipseElementImpl( CSvgDocumentImpl* aDoc ) : iEllipse( 0,
                                                                                     0,
                                                                                     0,
                                                                                     0 )
    {
	SetOwnerDocument(aDoc);
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgEllipseElementImpl::GetBBox( TGfxRectangle2D& aBbox )
    {
    iEllipse.GetBounds( GetCTM(), aBbox );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgEllipseElementImpl::GetUnscaledBBox( TGfxRectangle2D& aBbox )
    {
    TGfxAffineTransform identityTx;
    iEllipse.GetBounds( identityTx, aBbox );
    }

// ---------------------------------------------------------------------------
//
// perform a deep clone of this object
// ---------------------------------------------------------------------------
MXmlElement* CSvgEllipseElementImpl::CloneL(MXmlElement* aParentElement)
    {
    // create new ellipse

	CSvgEllipseElementImpl* newElement = CSvgEllipseElementImpl::NewL(  this->ElemID(), ((CSvgDocumentImpl*)iOwnerDocument) );

		CleanupStack::PushL(newElement);
	newElement->iParentNode = aParentElement;

    // copy everything over
    CopyL(newElement);
	CleanupStack::Pop();
    return newElement;
    }

// ---------------------------------------------------------------------------
//
// perform a deep clone of this object
// ---------------------------------------------------------------------------
void CSvgEllipseElementImpl::CopyL( CSvgEllipseElementImpl* aDestElement )
    {
    if(aDestElement)
	    {

	    // copy stuff from superclass
	    CSvgElementImpl::CopyL(aDestElement);

	    // copy iEllipse items special to circle
	    aDestElement->iEllipse.iX = iEllipse.iX;
	    aDestElement->iEllipse.iY = iEllipse.iY;
	    aDestElement->iEllipse.iHeight = iEllipse.iHeight;
	    aDestElement->iEllipse.iWidth = iEllipse.iWidth;

	    // copy the reference to idoc (CSvgDocumentImpl)
	    aDestElement->iOwnerDocument = iOwnerDocument;
	    }
    }

void CSvgEllipseElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<ellipse x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\">", (int)iEllipse.iX, (int)iEllipse.iY, (int)iEllipse.iWidth, (int)iEllipse.iHeight);
		#endif
	}
}
