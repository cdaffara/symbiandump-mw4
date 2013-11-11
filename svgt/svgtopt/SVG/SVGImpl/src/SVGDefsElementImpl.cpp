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
#include "SVGDefsElementImpl.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"

#include "GfxAffineTransform.h"

// *******************************************************
// Constructor/deconstructor

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgDefsElementImpl* CSvgDefsElementImpl::NewL( const TUint8 aElemID,
                                                CSvgDocumentImpl* aDoc )
    {
    CSvgDefsElementImpl*self    = new ( ELeave ) CSvgDefsElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL( aElemID );
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgDefsElementImpl* CSvgDefsElementImpl::NewLC( const TUint8 aElemID,
                                                 CSvgDocumentImpl* aDoc )
    {
    CSvgDefsElementImpl*self    = new ( ELeave ) CSvgDefsElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL( aElemID );

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgDefsElementImpl::~CSvgDefsElementImpl()
    {
    }

// *******************************************************
// From MXmlElement

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgDefsElementImpl::SetAttributeL( const TDesC& aName,
                                         const TDesC& aValue )
    {

    if ( this->SetLangSpaceAttributeL( aName, aValue ) )
        {
        return KErrNone;
        }



    if ( this->SetTransform( aName, aValue ) )
        {
        return KErrNone;
        }

    if ( this->SetTestAttributeL( aName, aValue ) )
        {
        return KErrNone;
        }

    if ( SetIdandXmlbaseL( aName, aValue ) )
        {
        return KErrNone;
        }

    return KErrNone;
    }


// From MXmlElementOpt

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgDefsElementImpl::GetAttributeFloat( const TInt aNameId,
                                             TFloatFixPt& aValue )
    {
    switch ( aNameId )
        {
        case KAtrRefX:
            {
            TFloatFixPt          xmin( KMAXFLOATFIX ), x; // 0x7fff is the maximum integer in TFixPt
            CSvgElementImpl* lNewElement = ( CSvgElementImpl* ) FirstChild();
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
MXmlElement* CSvgDefsElementImpl::CloneL(MXmlElement*)
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgDefsElementImpl::DrawL( CGfx2dGc*  /* aGc */,
                                  CSvgElementImpl* /* aElement */ )
    {
    return EFalse;
    }


// *******************************************************
// Private

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgDefsElementImpl::ConstructL( const TUint8 aElemID )
    {
    CSvgElementImpl::InitializeL( aElemID );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgDefsElementImpl::CSvgDefsElementImpl( CSvgDocumentImpl* aDoc )
    {
    	SetOwnerDocument( aDoc );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgDefsElementImpl::Print( TBool aIsEncodeOn )
{	
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<defs>");
		//something in def here
		RDebug::Printf("</defs>");
		#endif
	}
}
