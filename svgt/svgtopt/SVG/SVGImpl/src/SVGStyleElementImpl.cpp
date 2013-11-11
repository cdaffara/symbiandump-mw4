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
#include "SVGStyleElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"

#include "GfxAffineTransform.h"

// *******************************************************
// Constructor/deconstructor


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgStyleElementImpl* CSvgStyleElementImpl::NewL(  const TUint8 aElemID,
                                                  CSvgDocumentImpl* aDoc )
    {
    CSvgStyleElementImpl*   self    = new ( ELeave )
                                      CSvgStyleElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(  aElemID );
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgStyleElementImpl* CSvgStyleElementImpl::NewLC(  const TUint8 aElemID,
                                                   CSvgDocumentImpl* aDoc )
    {
    CSvgStyleElementImpl*   self    = new ( ELeave )
                                      CSvgStyleElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL(  aElemID );

    return self;
    }




// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgStyleElementImpl::~CSvgStyleElementImpl()
    {
	if ( iStyle )
		{
		delete iStyle;
		iStyle =  NULL;
		}

	if ( iType )
		{
	    delete iType;
		iType = NULL;
		}

	if ( iMedia )
		{
		delete iMedia;
		iMedia = NULL;
		}

	if ( iTitle)
		{
		delete iTitle;
		iTitle = NULL;
		}
    }

// *******************************************************
// SVG Implementation
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgStyleElementImpl::SetTypeL( const TDesC& aValue )
    {
	if ( iType )
		{
		delete iType;
	    iType = NULL;
		}
    iType = aValue.AllocL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgStyleElementImpl::SetMediaL( const TDesC& aValue )
    {
	if ( iMedia )
		{
		delete iMedia;
		iMedia = NULL;
		}
    iMedia = aValue.AllocL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgStyleElementImpl::SetTitleL( const TDesC& aValue )
    {
	if ( iTitle )
		{
		delete iTitle;
		iTitle = NULL;
		}
    iTitle = aValue.AllocL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgStyleElementImpl::SetStyleL( const TDesC& aValue )
    {
	if ( iStyle )
		{
		delete iStyle;
		iStyle = NULL;
		}
    iStyle = aValue.AllocL();
    }


// *******************************************************
// From MXmlElement

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgStyleElementImpl::SetAttributeL( const TDesC& aName,
                                          const TDesC& aValue )
    {
    if ( SetIdandXmlbaseL( aName, aValue ) )
        {
        return KErrNone;
        }

    if ( this->SetLangSpaceAttributeL( aName, aValue ) )
        {
        return KErrNone;
        }

    // Set type
    _LIT( KType, "type" );
    _LIT( KMedia, "media" );
    _LIT( KTitle, "title" );

    if ( aName == KType )
        {
        SetTypeL( aValue );
        }
    // Set media
    else if ( aName == KMedia )
        {
        SetMediaL( aValue );
        }
    // Set title
    else if ( aName == KTitle )
        {
        SetTitleL( aValue );
        }
    else if ( aName == KTitle )
        {
        SetStyleL( aValue );
        }

    return KErrNone;
    }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt  CSvgStyleElementImpl::SetAttributeDesL( const TInt aNameId,
											   const TDesC& aValue )
	{

	switch (aNameId)
		{
		case KAtrType:
				     delete iType;
                     iType = NULL;
					 iType = aValue.AllocL();
					 break;
		case KAtrMedia:
					 delete iMedia;
					 iMedia = NULL;
				     iMedia = aValue.AllocL();
				     break;
		case KAtrTitle:
					 delete iTitle;
					 iTitle = NULL;
			         iTitle = aValue.AllocL();
						break;

		default:
				return CSvgElementImpl::SetAttributeDesL( aNameId, aValue );

		}
	return KErrNone;
	}

// From MXmlElementOpt

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgStyleElementImpl::GetAttributeFloat( const TInt aNameId,
                                              TFloatFixPt& aValue )
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

// perform a deep clone of this object
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
MXmlElement* CSvgStyleElementImpl::CloneL(MXmlElement*)
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgStyleElementImpl::DrawL( CGfx2dGc* /* aGc */,
                                   CSvgElementImpl* /* aElement */ )
    {
    return EFalse;
    }


// *******************************************************
// Private

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgStyleElementImpl::ConstructL(  const TUint8 aElemID )
	{
	CSvgElementImpl::InitializeL( aElemID );
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

CSvgStyleElementImpl::CSvgStyleElementImpl( CSvgDocumentImpl* aDoc )
    {
	SetOwnerDocument(aDoc);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgStyleElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<style type=\"hmm\" media=\"hmm\" title=\"hmm\">"/*, iType, iMedia, iTitle*/);
		#endif
	}
}
