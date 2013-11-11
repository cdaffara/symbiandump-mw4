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
#include "SVGPolylineElementImpl.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"
#include "SVGPathDataParser.h"

#include "GfxAffineTransform.h"

_LIT(POINTS, "points");
_LIT(INLINE, "inline");
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgPolylineElementImpl* CSvgPolylineElementImpl::NewL(  const TUint8 aElemID,
														CSvgDocumentImpl* aDoc )
	{
	CSvgPolylineElementImpl*self    = new ( ELeave )
									  CSvgPolylineElementImpl( aDoc );
	CleanupStack::PushL( self );
	self->ConstructL(  aElemID );
	CleanupStack::Pop();

	return self;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgPolylineElementImpl* CSvgPolylineElementImpl::NewLC(  const TUint8 aElemID,
														 CSvgDocumentImpl* aDoc )
	{
	CSvgPolylineElementImpl*self    = new ( ELeave )
									  CSvgPolylineElementImpl( aDoc );
	CleanupStack::PushL( self );
	self->ConstructL(  aElemID );

	return self;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgPolylineElementImpl::ConstructL(  const TUint8 aElemID )
	{
	CSvgElementImpl::InitializeL(  aElemID );

	 iSvgStyleProperties = new(ELeave) RPointerArray<CCssValue>(KCSS_MAX_ATTR);
	 User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
	 iSvgStyleProperties->Remove( 0 );
    iSvgTransformable = CSvgTransformableImpl::NewL();

	iShape = CGfxGeneralPath::NewL();
	iReqAttrFlag=KSVG_POLYLINE_ELEMFLAG;

	}





// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgPolylineElementImpl::~CSvgPolylineElementImpl()
    {
	if ( iShape )
		{
		delete iShape;
		iShape = NULL;
        }
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
TInt CSvgPolylineElementImpl::SetAttributeL( const TDesC& aName,
                                             const TDesC& aValue )
    {


	CSvgElementImpl::SetAttributeL(aName,aValue);

	if( aName == POINTS )
		{

		iReqAttrFlag = 0;
		TSvgPathDataParser::ParsePointDataL( aValue,
															 iShape );
				// Polyline does not close a path.
		if(this->ElemID() == KSvgPolygonElement )
			{
			iShape->ClosePathL();
			}
		}

	return KErrNone;
    }

// From MXmlElementOpt

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgPolylineElementImpl::GetAttributeFloat( const TInt aNameId,
                                                 TFloatFixPt& aValue )
    {
    switch ( aNameId )
        {
        case KAtrRefX:
        aValue = iShape->PointCoordsArray()[0];
        break;
        case KAtrRefY:
        aValue = iShape->PointCoordsArray()[1];
        break;
        default:
        return CSvgElementImpl::GetAttributeFloat( aNameId, aValue );
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgPolylineElementImpl::GetAttributePath( const TInt aNameId,
                                                CGfxGeneralPath*& aValue )
    {
    if ( aNameId == KAtrPoints )
        {
        aValue = iShape;
        }
    else
        {
        aValue = NULL;
        return KErrNoAttribute;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgPolylineElementImpl::SetAttributePathL( const TInt aNameId,
                                                 CGfxGeneralPath* aValue )
    {
    if ( aNameId == KAtrPoints ) //.
        {
        // We need to copy aValue into iShape
        // We should not just reassign pointers - that will cause memory leak
        // We will do the copy when CGfxGeneralPath implements a copy method
        iShape->SetPointTypeArrayL( aValue->PointTypeArray() );
        iShape->SetPointCoordsArrayL( aValue->PointCoordsArrayAll() );
		if( WasTurnedOff() )
			{
			// turn it on. this means all the required attributes Are present and
			// hence we can render it.
			CSvgElementImpl::SetPropertyL(KCSS_ATTR_DISPLAY, INLINE);
			SetTurnOff( EFalse );
			}
        }
    else
        {
        return KErrNoAttribute;
        }
	if(this->ElemID() == KSvgPolygonElement )
			{
			iShape->ClosePathL();
			}
    return KErrNone;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgPolylineElementImpl::SetAttributePathRef( const TInt aNameId,
                                              CGfxGeneralPath*&  aValue  )
    {
    if ( aNameId == KAtrPoints )
        {

		if(iShape)
			{
			delete iShape;
			iShape= NULL;
			}
		iShape= aValue;

        }
    else
        {
        return KErrNoAttribute;
        }

    return KErrNone;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgPolylineElementImpl::DrawL( CGfx2dGc* aGc, CSvgElementImpl* aElement )
    {
    this->DrawShapeL( aGc, *iShape, aElement );

    return ETrue;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgPolylineElementImpl::CSvgPolylineElementImpl( CSvgDocumentImpl* aDoc )
    {
	SetOwnerDocument(aDoc);
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgPolylineElementImpl::GetBBox( TGfxRectangle2D& aBbox )
    {
    iShape->GetBounds( GetCTM(), aBbox );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgPolylineElementImpl::GetUnscaledBBox( TGfxRectangle2D& aBbox )
    {
    TGfxAffineTransform identityTx;
    iShape->GetBounds( identityTx, aBbox );
    }

// ---------------------------------------------------------------------------
// perform a deep clone of this object
// ---------------------------------------------------------------------------
MXmlElement* CSvgPolylineElementImpl::CloneL(MXmlElement* aParentElement)
    {
    // create new polyline
    CSvgPolylineElementImpl* newElement = CSvgPolylineElementImpl::NewL( this->ElemID(), ((CSvgDocumentImpl*)iOwnerDocument) );

	CleanupStack::PushL(newElement);
	newElement->iParentNode = aParentElement;
    // copy everything over
    CopyL(newElement);
	CleanupStack::Pop();
    return newElement;
    }

// ---------------------------------------------------------------------------
// perform a deep copy of this object
// ---------------------------------------------------------------------------
void CSvgPolylineElementImpl::CopyL( CSvgPolylineElementImpl* aDestElement )
    {
    if(aDestElement)
		{
	    // copy stuff from superclass
	    CSvgElementImpl::CopyL(aDestElement);

	    // copy iShape
        RArray<TUint32>* lTypes = iShape->PointTypeArray();
        RArray<TFloatFixPt>* lCoords = iShape->PointCoordsArrayAll();
        aDestElement->iShape->SetPointTypeArrayL(lTypes);
        aDestElement->iShape->SetPointCoordsArrayL(lCoords);

        // copy the reference to idoc (CSvgDocumentImpl)
	    aDestElement->iOwnerDocument = iOwnerDocument;
		}

    }

void CSvgPolylineElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
	#ifdef _DEBUG
	RArray<TUint32>* lTypes = this->iShape->PointTypeArray();
	RArray<TFloatFixPt>* lCoords = this->iShape->PointCoordsArrayAll();

	if (lTypes->Count() != lCoords->Count())
	{
		RDebug::Printf("Invalid polyline points and types didnt match", this);
		return;
	}

	RDebug::Printf("<polyline points=\"", this);

	TInt lTypesCnt = lTypes->Count();
	for (TInt i=0; i < lTypesCnt; i++)
	{
		RDebug::Printf("%c ", lTypes->operator[]( i ));
		RDebug::Printf("%d ", (int)(lCoords->operator[]( i )));
	}

	RDebug::Printf(" z\">", this);

	#endif
	}
}

