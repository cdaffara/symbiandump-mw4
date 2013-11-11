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


#include "SVGLangSpaceImpl.h"


// ---------------------------------------------------------------------------
// Implements the SVGLangSpace DOM interface.
// ---------------------------------------------------------------------------
CSvgLangSpaceImpl* CSvgLangSpaceImpl::NewL()
    {
    CSvgLangSpaceImpl* self = new ( ELeave ) CSvgLangSpaceImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgLangSpaceImpl* CSvgLangSpaceImpl::NewLC()
    {
    CSvgLangSpaceImpl* self = new ( ELeave ) CSvgLangSpaceImpl();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgLangSpaceImpl::~CSvgLangSpaceImpl()
    {
	if ( iXMLLang )
		{
		delete iXMLLang;
		iXMLLang = NULL;
		}

	if ( iXMLSpace)
		{
		delete iXMLSpace;
		iXMLSpace = NULL;
		}
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgLangSpaceImpl::ConstructL()
    {
    iXMLLang = HBufC::NewL( 0 );
    iXMLSpace = HBufC::NewL( 0 );
    }


// From MSvgLangSpace

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC& CSvgLangSpaceImpl::XMLLang()
    {
    return ( const TDesC & ) *iXMLLang;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgLangSpaceImpl::SetXMLLangL( const TDesC& aXMLLang )
    {
	if ( iXMLLang )
		{
		delete iXMLLang;
		iXMLLang = NULL;
		}
    iXMLLang = aXMLLang.AllocL();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const TDesC& CSvgLangSpaceImpl::XMLSpace()
    {
    return ( const TDesC & ) *iXMLSpace;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt CSvgLangSpaceImpl::SetXMLSpaceL( const TDesC& aXMLSpace )
    {
	if ( iXMLSpace )
		{
		delete iXMLSpace;
		iXMLSpace = NULL;
		}
    iXMLSpace = aXMLSpace.AllocL();
    return KErrNone;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TBool CSvgLangSpaceImpl::SetLangSpaceAttributeL( const TDesC& aPropertyName,
                                                 const TDesC& aValue )
    {
    _LIT( KXMLLang, "xml:lang" );
    _LIT( KXMLSpace, "xml:space" );

    if ( aPropertyName == KXMLLang )
        {
        return SetXMLLangL( aValue );
        }
    else if ( aPropertyName == KXMLSpace )
        {
        return SetXMLSpaceL( aValue );
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgLangSpaceImpl* CSvgLangSpaceImpl::CloneL()
    {
    CSvgLangSpaceImpl* retValue = CSvgLangSpaceImpl::NewL();

	CleanupStack::PushL(retValue);
    // *** copy iXMLLang
    retValue->SetXMLLangL(this->XMLLang());

    // *** copy iXMLSpace
    retValue->SetXMLSpaceL(this->XMLSpace());
	CleanupStack::Pop();
    return retValue;
    }
