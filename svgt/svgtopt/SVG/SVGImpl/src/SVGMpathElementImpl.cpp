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

#include "SVGMpathElementImpl.h"
#include "GfxGeneralPath.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"

_LIT( XLINKHREF, "xlink:href" );

// ---------------------------------------------------------------------------
// Two phase construction
// ---------------------------------------------------------------------------


CSvgMpathElementImpl* CSvgMpathElementImpl::NewL(  const TUint8 aElemID,
                                                  CSvgDocumentImpl* aDoc )
    {
    CSvgMpathElementImpl*   self    = new ( ELeave )
                                      CSvgMpathElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(  aElemID );
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgMpathElementImpl* CSvgMpathElementImpl::NewLC(  const TUint8 aElemID,
                                                   CSvgDocumentImpl* aDoc )
    {
    CSvgMpathElementImpl*   self    = new ( ELeave )
                                      CSvgMpathElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL( aElemID );

    return self;
    }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgMpathElementImpl::~CSvgMpathElementImpl()
    {
    if ( iSvgUriReferenceImpl )
        {
        delete iSvgUriReferenceImpl;
        iSvgUriReferenceImpl = NULL;
        }
    if ( iParsedHref )
    {
    	delete iParsedHref;
    	iParsedHref = NULL;
    }

    }

// *******************************************************
// From MXmlElement


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgMpathElementImpl::SetAttributeL( const TDesC& aName,
                                          const TDesC& aValue )
    {
    if ( this->SetXlinkAttributeL( aName, aValue ) )
        {
        // Get the path from the xlink
        TBuf<10>    lTmpXlink( XLINKHREF );
        if ( aName == lTmpXlink )
            {
            if( iReqAttrFlag == KAtrXlinkhref)
                {
                iReqAttrFlag =0;
                }
            CSvgElementImpl*targetEl    = NULL;
            TInt            pos         = KErrNotFound;
            // If the first char is '#' then remove it
            // This is possible if coming from cXML parser and not CVG Decoder

            pos = aValue.Locate( '#' );
            if ( pos != KErrNotFound && pos == 0 )
                {
                iParsedHref = HBufC::NewL( aValue.Length() );
                TPtr    tPtr    = iParsedHref->Des();
                tPtr.Copy( aValue );
                tPtr.Delete( pos, 1 );
                targetEl = ( CSvgElementImpl * ) ((CSvgDocumentImpl*)iOwnerDocument)->GetElementById( tPtr );
                }
            else
                {
                return pos;
                }

			if (targetEl)
			{
            	targetEl->GetAttributePath( KAtrData, iShape );
			}
			else
			{
				iNeedTarget = ETrue;
			}

            if ( iShape )
                {
                CSvgElementImpl*parentEl    = ( CSvgElementImpl* )
                                              ParentNode();
                if ( parentEl )
                    {
                    parentEl->SetAttributePathL( KAtrData, iShape );
                    }
                }
            }

        return KErrNone;
        }


    CSvgElementImpl::SetAttributeL(aName,aValue);

    return KErrNone;
    }

// From MXmlElementOpt

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgMpathElementImpl::GetAttributePath( const TInt aNameId,
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
TInt CSvgMpathElementImpl::SetAttributePathL( const TInt aNameId,
                                              CGfxGeneralPath* /* aValue */ )
    {
    if ( aNameId == KAtrData )
        {
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
TBool CSvgMpathElementImpl::DrawL( CGfx2dGc* /*aGc*/, CSvgElementImpl* /*aElement*/ )
    {

    return EFalse;

    /*
    if (iNeedTarget)
    {
    	CSvgElementImpl* targetEl = NULL;

    	iNeedTarget = EFalse;

    	if ( iParsedHref && iParsedHref->Length() > 0)
    	{
        	TPtr    tPtr    = iParsedHref->Des();
        	targetEl = ( CSvgElementImpl * ) ((CSvgDocumentImpl*)iOwnerDocument)->GetElementById( tPtr );

			if (targetEl)
			{
           		targetEl->GetAttributePath( KAtrData, iShape );
			}
			else
			{
				iNeedTarget = ETrue;
			}
		}
    }

    this->DrawShapeL( aGc, *iShape, aElement );
    return EFalse;
    */
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

void CSvgMpathElementImpl::ConstructL(  const TUint8 aElemID )
    {
    CSvgElementImpl::InitializeL(  aElemID );
    iReqAttrFlag = KAtrXlinkhref;

    iSvgUriReferenceImpl = CSvgUriReferenceImpl::NewL();
    iParsedHref = NULL;
    iNeedTarget = EFalse;
    }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

CSvgMpathElementImpl::CSvgMpathElementImpl( CSvgDocumentImpl* aDoc )
    {
    SetOwnerDocument(aDoc);
    }


// ---------------------------------------------------------------------------
// perform a deep clone of this object
// ---------------------------------------------------------------------------
MXmlElement* CSvgMpathElementImpl::CloneL(MXmlElement* aParentElement)
    {
    // create new polyline
    CSvgMpathElementImpl* newElement = CSvgMpathElementImpl::NewL(  this->ElemID(), ((CSvgDocumentImpl*)iOwnerDocument) );
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
void CSvgMpathElementImpl::CopyL( CSvgMpathElementImpl* aDestElement )
    {
    if(aDestElement)
		{
	    // copy stuff from superclass
	    this->CSvgElementImpl::CopyL(aDestElement);
        // copy iShape
        aDestElement->iShape->SetPointTypeArrayL( iShape->PointTypeArray() );
        aDestElement->iShape->SetPointCoordsArrayL( iShape->PointCoordsArrayAll() );
        // copy the reference to idoc (CSvgDocumentImpl)
	    aDestElement->iOwnerDocument = this->iOwnerDocument;
		}
    }

// ---------------------------------------------------------------------------
// perform a deep copy of this object
// ---------------------------------------------------------------------------
void CSvgMpathElementImpl::SetAttributePathByIdL(const TDesC& aId)
    {
    CSvgElementImpl*targetEl    = NULL;
    targetEl = ( CSvgElementImpl * ) ((CSvgDocumentImpl*)iOwnerDocument)->GetElementById( aId );
    targetEl->GetAttributePath( KAtrData, iShape );

    if ( iShape )
        {
        CSvgElementImpl*parentEl    = ( CSvgElementImpl* )
                                       ParentNode();
        if ( parentEl )
           {
            parentEl->SetAttributePathL( KAtrData, iShape );
           }
        }
    }

void CSvgMpathElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
	#ifdef _DEBUG
	RDebug::Printf("<mpath xlink:href=\"hmm\">"/*, Href()*/);
	#endif
	}
}
