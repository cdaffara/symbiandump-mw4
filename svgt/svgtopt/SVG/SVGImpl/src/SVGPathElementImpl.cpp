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

#include "SVGPathElementImpl.h"
#include "GfxGeneralPath.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"
#include "SVGPathDataParser.h"

_LIT( D, "d" );

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

CSvgPathElementImpl* CSvgPathElementImpl::NewL(  const TUint8 aElemID,
                                                CSvgDocumentImpl* aDoc )
    {
    CSvgPathElementImpl*self    = new ( ELeave ) CSvgPathElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL( aElemID );
    CleanupStack::Pop();

    return self;
    }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgPathElementImpl::~CSvgPathElementImpl()
    {
    if(iShape)
		{
		delete iShape;
		iShape = NULL;
        }
	if(iSvgStyleProperties)
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
TInt CSvgPathElementImpl::SetAttributeL( const TDesC& aName,
                                         const TDesC& aValue )
    {


	CSvgElementImpl::SetAttributeL(aName,aValue);

    if ( aName == D )
        {
        iReqAttrFlag = 0;
        TSvgPathDataParser::ParsePathData( aValue, iShape );

        // verify path starts with move command
        RArray<TUint32>* typeArray = iShape->PointTypeArray();
        if ( !typeArray || ( typeArray->Count() > 0 && (*typeArray)[0] != EGfxSegMoveTo ) )
            {
            if ( iOwnerDocument )
                {
                _LIT( msg1, "Missing 'move to segment' in <path>" );
                _LIT(msg2, "");
                ((CSvgDocumentImpl*)iOwnerDocument)->SetError( KErrNotFound, msg1, msg2 );
                }
            }
        }

	if (( ((CSvgElementImpl*)iParentNode) != NULL && ((CSvgElementImpl*)iParentNode)->ElemID() == KSvgGlyphElement ) && ( aName == D) )
	    {
       ((CSvgElementImpl*)iParentNode)->SetAttributeL(aName,aValue);
	    }

    return KErrNone;
    }

// From MXmlElementOpt

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgPathElementImpl::GetAttributeFloat( const TInt aNameId,
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
TInt CSvgPathElementImpl::GetAttributePath( const TInt aNameId,
                                            CGfxGeneralPath*& aValue )
    {
    if ( aNameId == KAtrData )
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
TInt CSvgPathElementImpl::SetAttributePathL( const TInt aNameId,
                                             CGfxGeneralPath* aValue )
    {
    if ( aNameId == KAtrData )
        {
        // copy aValue into iShape not just reassign pointers - that will cause memory leak
        // copy when CGfxGeneralPath implements a copy method
        iShape->SetPointTypeArrayL( aValue->PointTypeArray() );
        iShape->SetPointCoordsArrayL( aValue->PointCoordsArrayAll() );
        if( WasTurnedOff() )
			{
			// turn it on. this means all the required attributes Are present and
			// hence we can render it.
			_LIT(KInline , "inline");
			CSvgElementImpl::SetPropertyL(KCSS_ATTR_DISPLAY, KInline);
			SetTurnOff( EFalse );
			}
        
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
TInt CSvgPathElementImpl::SetAttributePathRef( const TInt aNameId,
                                              CGfxGeneralPath*&  aValue  )
    {
    if ( aNameId == KAtrData )
        {

	    if(iShape)
			{
			delete iShape;
			iShape= NULL;
			}
		iShape= aValue;
		
		if (( ((CSvgElementImpl*)iParentNode) != NULL && ((CSvgElementImpl*)iParentNode)->ElemID() == KSvgGlyphElement ) && ( aNameId == KAtrD) )
		    {
            ((CSvgElementImpl*)iParentNode)->SetPathAttribute(aNameId,aValue);
		    }
		
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
TBool CSvgPathElementImpl::DrawL( CGfx2dGc* aGc, CSvgElementImpl* aElement )
    {
    this->DrawShapeL( aGc, *iShape, aElement );
    return ETrue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgPathElementImpl::ConstructL(  const TUint8 aElemID )
    {
    CSvgElementImpl::InitializeL( aElemID );

    iSvgStyleProperties = new(ELeave) RPointerArray<CCssValue>(KCSS_MAX_ATTR);
    User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
    iSvgStyleProperties->Remove( 0 );
    iSvgTransformable = CSvgTransformableImpl::NewL();

    iShape = CGfxGeneralPath::NewL();
	iReqAttrFlag=KSVG_PATH_ELEMFLAG;

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgPathElementImpl::CSvgPathElementImpl( CSvgDocumentImpl* aDoc )
    {
	SetOwnerDocument(aDoc);
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgPathElementImpl::GetBBox( TGfxRectangle2D& aBbox )
    {

    if (!iShape || iShape->PointTypeArray()->Count() == 0)
    {
    	// The D attribute has not been set yet
    	return;
    }

    iShape->GetBounds( GetCTM(), aBbox );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgPathElementImpl::GetUnscaledBBox( TGfxRectangle2D& aBbox )
    {
    	if (!iShape || iShape->PointTypeArray()->Count() == 0)
    	{
    	// The D attribute has not been set yet
    	return;
    	}

    TGfxAffineTransform identityTx;
    iShape->GetBounds( identityTx, aBbox );
    }

// ---------------------------------------------------------------------------
// perform a deep clone of this object
// ---------------------------------------------------------------------------
MXmlElement* CSvgPathElementImpl::CloneL(MXmlElement* aParentElement)
    {
    // create new polyline
	CSvgPathElementImpl* newElement = CSvgPathElementImpl::NewL( this->ElemID(), ((CSvgDocumentImpl*)iOwnerDocument) );

	CleanupStack::PushL(newElement);
	newElement->iParentNode = aParentElement;
    // copy everything over
    this->CopyL(newElement);
	CleanupStack::Pop();
    return newElement;
    }

// ---------------------------------------------------------------------------
// perform a deep copy of this object
// ---------------------------------------------------------------------------
void CSvgPathElementImpl::CopyL( CSvgPathElementImpl* aDestElement )
    {
    if(aDestElement)
	    {
	    // copy stuff from superclass
	    this->CSvgElementImpl::CopyL(aDestElement);

	    // copy iShape
        RArray<TUint32>* lTypes = this->iShape->PointTypeArray();
        RArray<TFloatFixPt>* lCoords = this->iShape->PointCoordsArrayAll();
        aDestElement->iShape->SetPointTypeArrayL(lTypes);
        aDestElement->iShape->SetPointCoordsArrayL(lCoords);

	    // copy the reference to idoc (CSvgDocumentImpl)
	    aDestElement->iOwnerDocument = this->iOwnerDocument;
	    }

    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CGfxGeneralPath* CSvgPathElementImpl::GetPathAttribute(TInt aAttributeId)
{
	if(aAttributeId == KAtrD)
		{
		return iShape;
		}
	return NULL;
}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgPathElementImpl::SetPathAttribute(TInt aAttributeId, CGfxGeneralPath* aPathHandle)
{
	if(aAttributeId == KAtrD)
		{
		if(iShape != aPathHandle)
			{
			delete iShape;
			iShape = aPathHandle;
			}
		}
}

void CSvgPathElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
	#ifdef _DEBUG
	RArray<TUint32>* lTypes = this->iShape->PointTypeArray();
	RArray<TFloatFixPt>* lCoords = this->iShape->PointCoordsArrayAll();

	if (lTypes->Count() != lCoords->Count())
	{
		RDebug::Printf("Invalid path points and types didnt match", this);
		return;
	}

	RDebug::Printf("<path d=\"", this);
	
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
