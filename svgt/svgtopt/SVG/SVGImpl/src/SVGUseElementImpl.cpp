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

#include "SVGUseElementImpl.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"

#include "GfxAffineTransform.h"

#include "SVGImageElementImpl.h"

_LIT( XLINKHREF, "xlink:href" );
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

CSvgUseElementImpl* CSvgUseElementImpl::NewL(  const TUint8 aElemID,
                                              CSvgDocumentImpl* aDoc )
    {
    CSvgUseElementImpl* self    = new ( ELeave ) CSvgUseElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(  aElemID );

    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

CSvgUseElementImpl* CSvgUseElementImpl::NewLC(  const TUint8 aElemID,
                                              CSvgDocumentImpl* aDoc )

	{
    CSvgUseElementImpl* self    = new ( ELeave ) CSvgUseElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL( aElemID );

    return self;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgUseElementImpl::ConstructL( const TUint8 aElemID)
	{
    CSvgElementImpl::InitializeL( aElemID );

	iSvgStyleProperties = new(ELeave) RPointerArray<CCssValue>(KCSS_MAX_ATTR);
	User::LeaveIfError( iSvgStyleProperties->Append( NULL ) );
	iSvgStyleProperties->Remove( 0 );
    iSvgTransformable = CSvgTransformableImpl::NewL();
    iSvgUriReferenceImpl = CSvgUriReferenceImpl::NewL();

	#ifdef SVG_FLOAT_BUILD
    iX = 0 ;
    iY = 0 ;
	#else
    iX.operator = ( 0 );
    iY.operator = ( 0 );
	#endif
    //iXSet = EFalse;
    //iYSet = EFalse;

	iReqAttrFlag = KAtrXlinkhref;
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgUseElementImpl::~CSvgUseElementImpl()
    {

	if ( iSvgUriReferenceImpl )
		{
		delete iSvgUriReferenceImpl;
		iSvgUriReferenceImpl = NULL;
        }

	if ( iSvgStyleProperties )
		{
		iSvgStyleProperties->Close();
		delete iSvgStyleProperties;
		iSvgStyleProperties = NULL;
		}
	if ( iUseHRef )
	    {
		delete iUseHRef;
		iUseHRef = NULL;
		}	
    }

// *******************************************************
// From SVG DOM

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgUseElementImpl::X()
    {
    return iX;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgUseElementImpl::Y()
    {
    return iY;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgUseElementImpl::Width()
    {
    return iWidth;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TFloatFixPt CSvgUseElementImpl::Height()
    {
    return iHeight;
    }

// *******************************************************
// SVG Implementation


// *******************************************************
// From MXmlElement

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgUseElementImpl::SetAttributeL( const TDesC& aName,
                                        const TDesC& aValue )
    {

	CSvgElementImpl::SetAttributeL(aName,aValue);
    // Get the path from the xlink
    _LIT( KXLink, "xlink:href" );
    if ( aName == KXLink )
        {
        iReqAttrFlag = 0;

        // STEP 1 - Get the reference element
        // If the first char is '#' then remove it
        // This is possible if coming from cXML parser and not CVG Decoder
        
        TInt pos1 = aValue.Locate('/');
        
        if ( pos1 != KErrNotFound)
        {
        	if (iOwnerDocument)
        	{
        			_LIT( KMsg, "Non-local valid URI in <use> element" );
                			((CSvgDocumentImpl*)iOwnerDocument)->SetError( KErrNotFound, KMsg, aValue );
                    		((CSvgDocumentImpl*)iOwnerDocument)->SetIsWarning( ETrue );
        	}
        }
        
        TInt pos = aValue.Locate( '#' );
        if ( pos != KErrNotFound && pos == 0 )
            {
            TPtrC ptr( &aValue[1], aValue.Length() - 1);
            
            // Maintain one more class level member for JSR
            // This member will contain the value starting with '#'
            if ( iUseHRef )
			    {
				delete iUseHRef;
				iUseHRef = NULL;
			    }
			    
		    iUseHRef = aValue.AllocL();

            if ( this->SetXlinkAttributeL( aName, ptr ) )
                {
                ireferenceEl = NULL;
                if ( iOwnerDocument )
                    {
                    ireferenceEl = (CSvgElementImpl*)((CSvgDocumentImpl*)iOwnerDocument)->GetElementById( ptr );

                    if ( ireferenceEl && ireferenceEl->ElemID() == KSvgUseElement)
                    {
                    	((CSvgUseElementImpl*)ireferenceEl)->iUseElementThatUsesThisUse = this;

                    	if ( ((CSvgUseElementImpl*)ireferenceEl)->iUseElementThatUsesThisUse == iUseElementThatUsesThisUse )
                    	{
                    		//then there are use elements in recursion
                    		// Set error in document
                			_LIT( KMsg, "URI in <use> causes recursion loop with another <use>: " );
                			((CSvgDocumentImpl*)iOwnerDocument)->SetError( KErrNotFound, KMsg, aValue );
                    		((CSvgDocumentImpl*)iOwnerDocument)->SetIsWarning( EFalse );

                    		//get rid of the loop by removing this one
                    		ireferenceEl = NULL;
                    	}
                    }


                    }
                if(ireferenceEl == NULL)
                    {
                    // Set error in document
                    if ( iOwnerDocument && ((CSvgDocumentImpl*)iOwnerDocument)->iFinishedParsing )
                        {
                        _LIT( KMsg, "Invalid URI in <use> element: " );
                        ((CSvgDocumentImpl*)iOwnerDocument)->SetError( KErrNotFound, KMsg, aValue );
                        }
                    return KErrReferencedElementNotFound; // failure in finding the referenced element
                    }
                }
            }
        else if ( pos > 0 && iOwnerDocument && ((CSvgDocumentImpl*)iOwnerDocument)->iFinishedParsing )
            {
            // Set error in document
            _LIT( KMsg, "Non-local valid URI in <use> element: " );
            ((CSvgDocumentImpl*)iOwnerDocument)->SetError( KErrNotFound, KMsg, aValue );
            }
        else if( pos == KErrNotFound )
            {
            
            // Maintain one more class level member for JSR
            // This member will contain the value starting with '#'
            if ( iUseHRef )
		        {
			    delete iUseHRef;
			    iUseHRef = NULL;
			    }
			        
		    iUseHRef = aValue.AllocL();
            TPtrC ptr( &aValue[0], aValue.Length());
            this->SetXlinkAttributeL( aName, ptr);
            }
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgUseElementImpl::SetAttributeDesL( const TInt aNameId,
                                        const TDesC& aValue )
{
    switch ( aNameId )
        {
        case KAtrXlinkhref:
            {
            iReqAttrFlag = 0;

            // STEP 1 - Get the reference element
            // If the first char is '#' then remove it
            // This is possible if coming from cXML parser and not CVG Decoder
            TInt pos1 = aValue.Locate('/');
        
        	if ( pos1 != KErrNotFound)
        	{
        		if (iOwnerDocument)
        		{
        			_LIT( KMsg, "Non-local valid URI in <use> element" );
                			((CSvgDocumentImpl*)iOwnerDocument)->SetError( KErrNotFound, KMsg, aValue );
                    		((CSvgDocumentImpl*)iOwnerDocument)->SetIsWarning( ETrue );
        		}
        	}
        
            
            TInt pos = aValue.Locate( '#' );
            if ( pos != KErrNotFound && pos == 0 )
                {

                TPtrC ptr( &aValue[1], aValue.Length() - 1);
                // Maintain one more class level member for JSR
                // This member will contain the value starting with '#'
			    if ( iUseHRef )
			       {
				   delete iUseHRef;
				   iUseHRef = NULL;
			       }
		     	iUseHRef = aValue.AllocL();

                if ( this->SetXlinkAttributeL( XLINKHREF, ptr ) )
                    {
                    ireferenceEl = NULL;
                    if ( iOwnerDocument )
                        {
                        ireferenceEl = (CSvgElementImpl*)((CSvgDocumentImpl*)iOwnerDocument)->GetElementById( ptr );

                        if (ireferenceEl && ireferenceEl->ElemID() == KSvgUseElement)
                            {
                    	    ((CSvgUseElementImpl*)ireferenceEl)->iUseElementThatUsesThisUse = this;

                        	if ( ((CSvgUseElementImpl*)ireferenceEl)->iUseElementThatUsesThisUse == iUseElementThatUsesThisUse )
                    	        {
                    	    	//then there are use elements in recursion
                    		    _LIT( KMsg, "URI in <use> causes recursion loop with another <use>: " );
                			    ((CSvgDocumentImpl*)iOwnerDocument)->SetError( KErrNotFound, KMsg, aValue );
                    		    ((CSvgDocumentImpl*)iOwnerDocument)->SetIsWarning( EFalse );

                    	     	//get rid of the loop by removing this one
                    		    ireferenceEl = NULL;
                        	    }
                            }

                        }
                if(ireferenceEl == NULL)
                    {
                    // Set error in document
                    if ( iOwnerDocument && ((CSvgDocumentImpl*)iOwnerDocument)->iFinishedParsing)
                        {
                        _LIT( KMsg, "Invalid URI in <use> element: " );
                        ((CSvgDocumentImpl*)iOwnerDocument)->SetError( KErrNotFound, KMsg, aValue );
                        }
                    return KErrReferencedElementNotFound; // failure in finding the referenced element
                    }
                }
            }
        else if ( pos > 0 && iOwnerDocument && ((CSvgDocumentImpl*)iOwnerDocument)->iFinishedParsing )
                {
                // Set error in document
                _LIT( KMsg, "Non-local valid URI in <use> element: " );
                ((CSvgDocumentImpl*)iOwnerDocument)->SetError( KErrNotFound, KMsg, aValue );
                }
        else if( pos == KErrNotFound )
                {
                // Maintain one more class level member for JSR
                // This member will contain the value starting with '#'
                if ( iUseHRef )
			        {
				    delete iUseHRef;
				    iUseHRef = NULL;
			        }
			    
		        iUseHRef = aValue.AllocL();
		        
            	TPtrC ptr( &aValue[0], aValue.Length());
            	this->SetXlinkAttributeL( XLINKHREF, ptr);
                }
            return KErrNone;
            }

        default:
            {
            return CSvgElementImpl::SetAttributeDesL( aNameId, aValue );
            }
        }
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgUseElementImpl::GetAttributeDes( const TInt aNameId, TPtrC16& aValue )
    {
 
    if ( aNameId == KAtrXlinkhref )
       	{
       	// iUseHRef contains value starting with '#'
       	if ( iUseHRef )
       	{
       		aValue.Set( *iUseHRef );
		    return KErrNone;
       	}
       	
       	if ( iSvgUriReferenceImpl )
       	{
       		aValue.Set( iSvgUriReferenceImpl->Href());
       		return KErrNone;
       	}
       
       }

    return CSvgElementImpl::GetAttributeDes( aNameId, aValue );

    }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgUseElementImpl::SetAttributeFloatL( const TInt aNameId,
                                              const TFloatFixPt aValue )
    {
    switch ( aNameId )
        {
        case KAtrX:
					iX = aValue;
					//iXSet = ETrue;
			        break;
        case KAtrY:

					iY = aValue;
					//iYSet = ETrue;
				    break;
        case KAtrWidth:
					iWidth = aValue;
			        break;

        case KAtrHeight:
					iHeight = aValue;
			        break;
        default:
        return CSvgElementImpl::SetAttributeFloatL( aNameId, aValue );
        }
    return KErrNone;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgUseElementImpl::GetAttributeFloat( const TInt aNameId,
                                             TFloatFixPt& aValue )
    {
    switch ( aNameId )
        {
        case KAtrX:
				 aValue =	iX ;
			     break;
        case KAtrY:

				aValue =	iY ;
			    break;
        case KAtrWidth:
				aValue =	iWidth ;
		        break;

        case KAtrHeight:
				 aValue = 	iHeight ;
		        break;
        default:
        return CSvgElementImpl::GetAttributeFloat( aNameId, aValue );
        }
    return KErrNone;
    }


// *******************************************************
// From MXmlElementOpt

// *******************************************************
// From CSvgElementImpl

// perform a deep clone of this object
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
MXmlElement* CSvgUseElementImpl::CloneL(MXmlElement* aParentElement)
    {

	CSvgUseElementImpl* newElement = CSvgUseElementImpl::NewL(  this->ElemID(),
										((CSvgDocumentImpl*)iOwnerDocument) );
		CleanupStack::PushL(newElement);

	newElement->iParentNode = aParentElement;
    // copy everything over
    this->CopyL(newElement);
CleanupStack::Pop();
    return newElement;
    }

//---------------------------------------------------------------------------
// perform a deep copy of this object
//
//---------------------------------------------------------------------------
void CSvgUseElementImpl::CopyL( CSvgUseElementImpl* aDestElement )
    {
    	if(aDestElement)
		{
	    // copy stuff from superclass
	    this->CSvgElementImpl::CopyL(aDestElement);

	    // copy iRectangle items special to circle
	    aDestElement->iX = this->iX;
	    aDestElement->iY = this->iY;
	    aDestElement->iHeight = this->iHeight;
	    aDestElement->iWidth = this->iWidth;

		aDestElement->ireferenceEl = this->ireferenceEl;
		aDestElement->iUseElementThatUsesThisUse = this->iUseElementThatUsesThisUse;

		 // copy the reference to idoc (CSvgDocumentImpl)
	    aDestElement->iOwnerDocument = this->iOwnerDocument;
		}
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgUseElementImpl::DrawL( CGfx2dGc* /* aGc */,
                                 CSvgElementImpl* /* aElement */ )
    {
    return ETrue;
    }

// *******************************************************
// Private



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgUseElementImpl::CSvgUseElementImpl( CSvgDocumentImpl* aDoc )
    {
	SetOwnerDocument(aDoc);
    }

/*
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgUseElementImpl::DeepClone( MXmlElement* aReferenceEl )
    {
    CSvgElementImpl*aNewElement = ( CSvgElementImpl* )
                                  aReferenceEl->FirstChild();
    while ( aNewElement )
        {
        DeepClone( aNewElement );
        aNewElement = ( CSvgElementImpl * ) aNewElement->NextSibling();
        }
    }
*/

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgUseElementImpl::SetReferenceElementL()
    {
	if( ireferenceEl != NULL )
		{
 		CSvgElementImpl* theElement = (CSvgElementImpl *)ireferenceEl->CloneL((MXmlElement*)this);
 		CleanupStack::PushL(theElement);
		this->CXmlElementImpl::AppendChildL(theElement);
		CleanupStack::Pop();

		//if reference element is a use element check for recursion loop
		if (ireferenceEl->ElemID() == KSvgUseElement)
            {
        	((CSvgUseElementImpl*)ireferenceEl)->iUseElementThatUsesThisUse = this;

        	if ( ireferenceEl == iUseElementThatUsesThisUse )
            	{
        		//then there are use elements in recursion
        		_LIT( KMsg, "URI in <use> causes recursion loop with another <use>: " );
    			((CSvgDocumentImpl*)iOwnerDocument)->SetError( KErrNotFound, KMsg, Href() );
        		((CSvgDocumentImpl*)iOwnerDocument)->SetIsWarning( EFalse );

        		//get rid of the loop by removing this one
        		ireferenceEl = NULL;
            	}
            }
        }
    }
    
    
CSvgElementImpl* CSvgUseElementImpl::GetReferenceElementL()
{
	return ireferenceEl;
}

TInt  CSvgUseElementImpl::SetRefElemById(const TDesC& aName)
    {
     ireferenceEl = ( CSvgElementImpl * )
        ((CSvgDocumentImpl*)iOwnerDocument)->GetElementById( aName );
    iReqAttrFlag = 0;

     return ( ireferenceEl == NULL ) ? KErrNotFound : KErrNone;
    }

void CSvgUseElementImpl::GetUnscaledBBox( TGfxRectangle2D& aBbox )
{
	TGfxRectangle2D lReferenceBbox;
	if (ireferenceEl != NULL)
	{
		((CSvgElementImpl*)ireferenceEl)->GetUnscaledBBox(lReferenceBbox);
	}

	// width and height
	aBbox.iX = lReferenceBbox.iX + X();
	aBbox.iY = lReferenceBbox.iY + Y();
	aBbox.iWidth = lReferenceBbox.iWidth;
	aBbox.iHeight = lReferenceBbox.iHeight;
}

TBool CSvgUseElementImpl::IsViewable( )
{
	if (ireferenceEl)
	{
		return ireferenceEl->IsViewable();
	}

	return EFalse;
}

void CSvgUseElementImpl::GetBBox( TGfxRectangle2D& aBbox )
{
	TGfxRectangle2D lReferenceBbox;
	if (ireferenceEl)
	{
		((CSvgElementImpl*)ireferenceEl)->GetBBox(lReferenceBbox);
	}

	// width and height
	aBbox.iX = lReferenceBbox.iX + X();
	aBbox.iY = lReferenceBbox.iY + Y();
	aBbox.iWidth = lReferenceBbox.iWidth;
	aBbox.iHeight = lReferenceBbox.iHeight;

}

void CSvgUseElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<use x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xlink:href=\"hmm\">", (int)iX, (int)iY, (int)iWidth, (int)iHeight/*, Href()*/);
		#endif
	}
}
// new function
void CSvgUseElementImpl::GetBoundingBox( TGfxRectangle2D& aBbox ) 
{
	TGfxRectangle2D lReferenceBbox;
	if (ireferenceEl != NULL)
	{
	TGfxAffineTransform lTempCTM = ((CSvgElementImpl*)ireferenceEl)->GetCTM();
	TGfxAffineTransform lElementCTM = ((CSvgElementImpl*)this)->GetCTM();
	MSvgTransformList*  trList;
    TGfxAffineTransform lElementTransform;
    ((CSvgElementImpl*)ireferenceEl)->GetTransform( trList);
    lElementTransform = trList->Consolidate( ((CSvgElementImpl*)ireferenceEl)->IsOverwriteTransforms() );
	lElementTransform.Concatenate(lElementCTM);
	
	((CSvgElementImpl*)ireferenceEl)->SetCTM(lElementTransform);
    //((CSvgElementImpl*)ireferenceEl)->GetBBox(lReferenceBbox);
	((CSvgElementImpl*)ireferenceEl)->GetUnscaledBBox(lReferenceBbox);
	lReferenceBbox.GetBounds(lElementTransform, lReferenceBbox);
	
    ((CSvgElementImpl*)ireferenceEl)->SetCTM(lTempCTM);
	}
	// width and height
	aBbox.iX = lReferenceBbox.iX + X();
	aBbox.iY = lReferenceBbox.iY + Y();
	aBbox.iWidth = lReferenceBbox.iWidth;
	aBbox.iHeight = lReferenceBbox.iHeight;

}
