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
#include "SVGGElementImpl.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"

#include "GfxAffineTransform.h"

#include "SVGUseElementImpl.h"
_LIT(DISPLAY, "display");
_LIT(NONE, "none");

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgGElementImpl* CSvgGElementImpl::NewL(  const TUint8 aElemID,
                                          CSvgDocumentImpl* aDoc )
    {
    CSvgGElementImpl*   self    = new ( ELeave ) CSvgGElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(  aElemID );
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgGElementImpl* CSvgGElementImpl::NewLC(  const TUint8 aElemID,
                                           CSvgDocumentImpl* aDoc )
    {
    CSvgGElementImpl*   self    = new ( ELeave ) CSvgGElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(  aElemID );

    return self;
    }




// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgGElementImpl::~CSvgGElementImpl()
    {
	if ( iSvgStyleProperties )
		{
		iSvgStyleProperties->Close();
		delete iSvgStyleProperties;
		iSvgStyleProperties = NULL;
		}

    }


// Private
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

void CSvgGElementImpl::ConstructL(  const TUint8 aElemID )
    {
    CSvgElementImpl::InitializeL( aElemID );

	iSvgStyleProperties = new(ELeave) RPointerArray<CCssValue>(KCSS_MAX_ATTR);
	User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
iSvgStyleProperties->Remove( 0 );

    iSvgTransformable = CSvgTransformableImpl::NewL();

    if (this->ElemID() >= KSvgAllGElemsSubStartIndex &&
    	this->ElemID() <= KSvgAllGElemsEndIndex )
        {
        // Set display to "none"
        // Otherwise it will be rendered
		SetAttributeL(DISPLAY, NONE);
        }
    }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

CSvgGElementImpl::CSvgGElementImpl( CSvgDocumentImpl* aDoc )
    {
	SetOwnerDocument(aDoc);
    }


// *******************************************************
// From MXmlElement

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgGElementImpl::SetAttributeL( const TDesC& aName, const TDesC& aValue )
    {
	CSvgElementImpl::SetAttributeL(aName,aValue);
    return KErrNone;
    }


// From MXmlElementOpt

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgGElementImpl::GetAttributeFloat( const TInt aNameId, TFloatFixPt& aValue )
    {
    switch ( aNameId )
        {
        case KAtrRefX:
            {
            TFloatFixPt          xmin( KMAXFLOATFIX ), x; // 0x7fff is the maximum integer in TFixPt
            CSvgElementImpl*lNewElement = ( CSvgElementImpl* ) FirstChild();
            while ( lNewElement != NULL )
                {
                lNewElement->GetAttributeFloat( KAtrRefX, x );
                if ( x < xmin )
                    xmin = x;
                lNewElement = ( CSvgElementImpl * )
                              lNewElement->NextSibling();
                }
            aValue = xmin;
            }
        break;
        case KAtrRefY:
            {
            TFloatFixPt          ymin( KMAXFLOATFIX ), y; // 0x7fff is the maximum integer in TFixPt
            CSvgElementImpl*lNewElement = ( CSvgElementImpl* ) FirstChild();
            while ( lNewElement != NULL )
                {
                lNewElement->GetAttributeFloat( KAtrRefY, y );
                if ( y < ymin )
                    ymin = y;
                lNewElement = ( CSvgElementImpl * )
                              lNewElement->NextSibling();
                }
            aValue = ymin;
            }
        break;
        default:
        return CSvgElementImpl::GetAttributeFloat( aNameId, aValue );
        }
    return KErrNone;
    }

// *******************************************************
// From CSvgElementImpl

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgGElementImpl::DrawL( CGfx2dGc* /* aGc */,
                               CSvgElementImpl* /* aElement */ )
    {

	if (	this->ElemID() >= KSvgAllGElemsStartIndex &&
			this->ElemID() <= KSvgAllGElemsEndIndex
	   )


        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }

    }


// *******************************************************
// Utilities
// Get group bounding box
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgGElementImpl::GetGroupBounding( TGfxRectangle2D& aBbox,
                                         CSvgElementImpl* aStartElement )
    {
    TFloatFixPt  xMin( KMAXFLOATFIX ), yMin( KMAXFLOATFIX ), xMax( KMINFLOATFIX ), yMax( KMINFLOATFIX );

    GetGroupBoundingInternal( xMin, yMin, xMax, yMax, aStartElement );
    aBbox.iX = xMin;
    aBbox.iY = yMin;
    aBbox.iWidth = xMax - xMin;
    aBbox.iHeight = yMax - yMin;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgGElementImpl::GetGroupBoundingInternal( TFloatFixPt& aXmin,
                                                 TFloatFixPt& aYmin,
                                                 TFloatFixPt& aXmax,
                                                 TFloatFixPt& aYmax,
                                                 CSvgElementImpl* aStartElement )
    {
   	TInt    position= aStartElement->ElemID();
	if(  position > KSvgNoBoundBoxElemsEndIndex )

        {
        TFloatFixPt          zero    ( 0 );
        TGfxRectangle2D bbox;
        TInt32 displayValue;
        aStartElement->GetBBox( bbox );

        TRAP_IGNORE( aStartElement->GetAttributeIntL( KCSS_ATTR_DISPLAY, displayValue ) );

    	// Ignore area-less elements and dispay="none" elements
        if ( (bbox.iWidth > zero || bbox.iHeight > zero) && displayValue !=  KDisplayEnumNone)
        {
            if ( bbox.iX < aXmin )
                aXmin = bbox.iX;
            if ( bbox.iY < aYmin )
                aYmin = bbox.iY;
            if ( bbox.iX + bbox.iWidth > aXmax )
                aXmax = bbox.iX + bbox.iWidth;
            if ( bbox.iY + bbox.iHeight > aYmax )
                aYmax = bbox.iY + bbox.iHeight;
            }
        }

    CSvgElementImpl*element = ( CSvgElementImpl* )
                              aStartElement->FirstChild();
    while ( element != NULL )
        {
        GetGroupBoundingInternal( aXmin, aYmin, aXmax, aYmax, element );
        element = ( CSvgElementImpl * ) element->NextSibling();
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgGElementImpl::GetGroupBoundingUnscaled( TGfxRectangle2D& aBbox,
                                                 CSvgElementImpl* aStartElement )
    {
    TFloatFixPt  xMin( KMAXFLOATFIX ), yMin( KMAXFLOATFIX ), xMax( KMINFLOATFIX ), yMax( KMINFLOATFIX );

    GetGroupBoundingInternalUnscaled( xMin, yMin, xMax, yMax, aStartElement );
    aBbox.iX = xMin;
    aBbox.iY = yMin;
    aBbox.iWidth = xMax - xMin;
    aBbox.iHeight = yMax - yMin;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgGElementImpl::GetGroupBoundingInternalUnscaled( TFloatFixPt& aXmin,
                                                         TFloatFixPt& aYmin,
                                                         TFloatFixPt& aXmax,
                                                         TFloatFixPt& aYmax,
                                                         CSvgElementImpl* aStartElement )
{
    TInt    position= aStartElement->ElemID();
    if(  position > KSvgNoBoundBoxElemsEndIndex )

        {
        TFloatFixPt          zero    ( 0 );
        TGfxRectangle2D bbox;
        TInt32 displayValue;

        //solving the case where a <g><rect transform="translate(10,10)"></g>
        //the rect elements transform must be reflected on the G
        //but we dont want the scaled bbox of the rect with the entire CTM scaling

        if( aStartElement->IsTransformable() )
            {
            MSvgTransformList*  trList;

        	TGfxAffineTransform lElementTransform;

            aStartElement->GetTransform( trList );
            lElementTransform = trList->Consolidate( aStartElement->IsOverwriteTransforms() );

            TGfxAffineTransform lTempCTM = aStartElement->GetCTM();

            //use only the transform on the child element and not the entire CTM
            aStartElement->SetCTM( lElementTransform );
			if (KSvgUseElement == (( CSvgElementImpl* )aStartElement)->ElemID())
            {
              ((CSvgUseElementImpl*)aStartElement)->GetBoundingBox( bbox);
            }
            else
            	aStartElement->GetBBox( bbox );

            //set the original entire CTM back on to the element
            aStartElement->SetCTM(lTempCTM);

            }

        TRAP_IGNORE( aStartElement->GetAttributeIntL( KCSS_ATTR_DISPLAY, displayValue ) );
		
		//TBool iDisplayNoneFlag  = aStartElement->IsInherited(KCSS_ATTR_DISPLAY);
		// if the display attribute is inherited, consider it for bounding box calculation
        if ( (bbox.iWidth > zero || bbox.iHeight > zero) && ( displayValue !=  KDisplayEnumNone ) )    // Ignore area-less elements
        {
            if ( bbox.iX < aXmin )
                aXmin = bbox.iX;
            if ( bbox.iY < aYmin )
                aYmin = bbox.iY;
            if ( bbox.iX + bbox.iWidth > aXmax )
                aXmax = bbox.iX + bbox.iWidth;
            if ( bbox.iY + bbox.iHeight > aYmax )
                aYmax = bbox.iY + bbox.iHeight;
            }
        }
      if (KSvgUseElement == (( CSvgElementImpl* )aStartElement)->ElemID()) 
      return;

    CSvgElementImpl*element = ( CSvgElementImpl* )
                              aStartElement->FirstChild();
    TFloatFixPt tempXMin ;
    TFloatFixPt tempYMin ;
    TFloatFixPt tempXMax ;
    TFloatFixPt tempYMax ;
    
    while ( element != NULL )
    {
                    
        //Apply group level transformation if the group is not the original element. 
        if( KSvgGElement == element->ElemID() )
        {
 			TFloatFixPt  xMin( KMAXFLOATFIX ), yMin( KMAXFLOATFIX ), xMax( KMINFLOATFIX ), yMax( KMINFLOATFIX );
        	
        	tempXMin = KMAXFLOATFIX ;
        	tempYMin = KMAXFLOATFIX ;
        	tempXMax = KMINFLOATFIX ;
        	tempYMax = KMINFLOATFIX ;
        	
        	GetGroupBoundingInternalUnscaled( tempXMin, tempYMin, tempXMax, tempYMax, element );	
           	if((tempXMax > tempXMin)&&(tempYMax > tempYMin)) 
           	    {
           	    TGfxRectangle2D rect(tempXMin, tempYMin, tempXMax - tempXMin, tempYMax - tempYMin);
        	
        	    MSvgTransformList*  trList;

        	    TGfxAffineTransform lElementTransform;

                element->GetTransform( trList );
                lElementTransform = trList->Consolidate( element->IsOverwriteTransforms() );

        	    rect.GetBounds(lElementTransform, rect);
        	
        	    tempXMin = rect.iX ;
        	    tempYMin = rect.iY ;
        	    tempXMax = rect.iX + rect.iWidth ;
        	    tempYMax = rect.iY + rect.iHeight ; 
        	
        	    if( tempXMin < aXmin )
        		    aXmin = tempXMin ;
        	    if( tempYMin < aYmin )
        		    aYmin = tempYMin ;
        	    if( tempXMax > aXmax )
        		    aXmax = tempXMax ;
        	    if( tempYMax > aYmax )
        		    aYmax = tempYMax ;
           	    }
        }
        else
        {
        	GetGroupBoundingInternalUnscaled( aXmin, aYmin, aXmax, aYmax, element );	
        }
        
        element = ( CSvgElementImpl * ) element->NextSibling();
    }
        
}

// perform a deep clone of this object
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
MXmlElement* CSvgGElementImpl::CloneL(MXmlElement* aParentElement)
    {
    // create new g element
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
	CSvgGElementImpl* newElement = CSvgGElementImpl::NewL(  this->ElemID(), ((CSvgDocumentImpl*)iOwnerDocument) );


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
void CSvgGElementImpl::CopyL( CSvgGElementImpl* aDestElement )
    {
    if(aDestElement)
	    {
	    // copy stuff from superclass
	    this->CSvgElementImpl::CopyL(aDestElement);
	    // copy the reference to idoc (CSvgDocumentImpl)
	    aDestElement->iOwnerDocument = this->iOwnerDocument;
	    }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgGElementImpl::GetBBox( TGfxRectangle2D& aBbox )
    {

    // if there are not any children then the bbox should be NULL
    if (!HasChildNodes())
    {
    	return;
    }

    CSvgGElementImpl::GetGroupBounding( aBbox, this );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgGElementImpl::GetUnscaledBBox( TGfxRectangle2D& aBbox )
    {

    // if there are not any children then the bbox should be NULL
    if (!HasChildNodes())
    {
    	return;
    }

    CSvgGElementImpl::GetGroupBoundingUnscaled( aBbox, this );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgGElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<g>");
		#endif
	}
}
