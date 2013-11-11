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
#include "SVGLineElementImpl.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"

#include "GfxAffineTransform.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgLineElementImpl* CSvgLineElementImpl::NewL(  const TUint8 aElemID,
												CSvgDocumentImpl* aDoc )
	{
	CSvgLineElementImpl*self    = new ( ELeave ) CSvgLineElementImpl( aDoc );
	CleanupStack::PushL( self );
	self->ConstructL(  aElemID );
	CleanupStack::Pop();

	return self;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgLineElementImpl* CSvgLineElementImpl::NewLC(  const TUint8 aElemID,
												 CSvgDocumentImpl* aDoc )
	{
	CSvgLineElementImpl*self    = new ( ELeave ) CSvgLineElementImpl( aDoc );
	CleanupStack::PushL( self );
	self->ConstructL(  aElemID );

	return self;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgLineElementImpl::ConstructL(  const TUint8 aElemID )
	{
	CSvgElementImpl::InitializeL(  aElemID );

		iSvgStyleProperties = new(ELeave) RPointerArray<CCssValue>(KCSS_MAX_ATTR);
	User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
	iSvgStyleProperties->Remove( 0 );
    iSvgTransformable = CSvgTransformableImpl::NewL();
	}



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgLineElementImpl::CSvgLineElementImpl( CSvgDocumentImpl* aDoc ) : iLine( 0,
                                                                            0,
                                                                            0,
                                                                            0 )
    {
	SetOwnerDocument(aDoc);
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgLineElementImpl::~CSvgLineElementImpl()
    {
	if ( iSvgStyleProperties )
		{
		iSvgStyleProperties->Close();
		delete iSvgStyleProperties;
		iSvgStyleProperties = NULL;
		}
    }



// *******************************************************
// From MXmlElement

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgLineElementImpl::SetAttributeL( const TDesC& aName,
                                         const TDesC& aValue )
    {

	CSvgElementImpl::SetAttributeL(aName,aValue);
    return KErrNone;
    }


// From MXmlElementOpt

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgLineElementImpl::GetAttributeFloat( const TInt aNameId,
                                             TFloatFixPt& aValue )
    {
    switch ( aNameId )
        {
        case KAtrX1:
        aValue = iLine.iX1;
        break;
        case KAtrY1:
        aValue = iLine.iY1;
        break;
        case KAtrX2:
        aValue = iLine.iX2;
        break;
        case KAtrY2:
        aValue = iLine.iY2;
        break;
        default:
        return CSvgElementImpl::GetAttributeFloat( aNameId, aValue );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgLineElementImpl::SetAttributeFloatL( const TInt aNameId,
                                              const TFloatFixPt aValue )
    {
    switch ( aNameId )
        {
        case KAtrX1:
        iLine.iX1 = aValue;
        // SetX1(aValue);
        break;
        case KAtrY1:
        iLine.iY1 = aValue;
        // SetY1(aValue);
        break;
        case KAtrX2 :
        iLine.iX2 = aValue;
        //SetX1(aValue);
        break;
        case KAtrY2 :
        iLine.iY2 = aValue;
        //SetY2(aValue);
        break;
        default:
        return CSvgElementImpl::SetAttributeFloatL( aNameId, aValue );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgLineElementImpl::DrawL( CGfx2dGc* aGc, CSvgElementImpl* aElement )
    {
    
    //line isnt a line at all two identical points
    if (iLine.iX1 == iLine.iX2 && iLine.iY1 == iLine.iY2)
    return ETrue;
    
    this->DrawShapeL( aGc, iLine, aElement );
    return ETrue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgLineElementImpl::GetBBox( TGfxRectangle2D& aBbox )
    {
    iLine.GetBounds( GetCTM(), aBbox );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgLineElementImpl::GetUnscaledBBox( TGfxRectangle2D& aBbox )
    {
    TGfxAffineTransform identityTx;
    iLine.GetBounds( identityTx, aBbox );
    }

// ---------------------------------------------------------------------------
// perform a deep clone of this object
// ---------------------------------------------------------------------------

MXmlElement* CSvgLineElementImpl::CloneL(MXmlElement* aParentElement)
    {
    // create new circle
    CSvgLineElementImpl* newElement = CSvgLineElementImpl::NewL(  this->ElemID(), ((CSvgDocumentImpl*)iOwnerDocument));

    CleanupStack::PushL(newElement);

	newElement->iParentNode = aParentElement;

    // copy everything over
    this->CopyL(newElement);
	CleanupStack::Pop();
    return newElement;
    }

// perform a deep copy of this object
void CSvgLineElementImpl::CopyL( CSvgLineElementImpl* aDestElement )
    {
    if(aDestElement)
		{
	    // copy stuff from superclass
	    this->CSvgElementImpl::CopyL(aDestElement);

	    // copy iLine items special to circle
	    aDestElement->iLine.iX1 = this->iLine.iX1;
	    aDestElement->iLine.iX2 = this->iLine.iX2;
	    aDestElement->iLine.iY1 = this->iLine.iY1;
	    aDestElement->iLine.iY2 = this->iLine.iY2;

	    // copy the reference to idoc (CSvgDocumentImpl)
	    aDestElement->iOwnerDocument = this->iOwnerDocument;
		}



    }

void CSvgLineElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\">", (int)iLine.iX1, (int)iLine.iX2, (int)iLine.iY1, (int)iLine.iY2);
		#endif
	}
}
