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


#include "SVGScriptElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGEngineImpl.h"

// ---------------------------------------------------------------------------
// Two phase construction
// ---------------------------------------------------------------------------
CSvgScriptElementImpl* CSvgScriptElementImpl::NewL( TUint8 aElementId, CSvgDocumentImpl* aDocument )
    {
    CSvgScriptElementImpl*  self = new ( ELeave ) CSvgScriptElementImpl( aDocument );
    CleanupStack::PushL( self );
    self->ConstructL( aElementId );
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgScriptElementImpl::~CSvgScriptElementImpl()
    {
    delete iScript;
    delete iXLink;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
const HBufC8* CSvgScriptElementImpl::GetScript()
    {
    return iScript;
    }

// ---------------------------------------------------------------------------
// Script has "type" and "xlink:href" attributes
// ---------------------------------------------------------------------------
TInt CSvgScriptElementImpl::SetAttributeL( const TDesC& aName, const TDesC& aValue )
{
    _LIT( KXlink, "xlink:href" );
    _LIT( KType, "type" );
    if ( aName == KXlink )
    {
        delete iXLink;
        iXLink = NULL;
        iXLink = aValue.Alloc();
        FetchExternalScriptL();
    }
    else if ( aName == KType )
    {
        // ignore
    }

    return KErrNone;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgScriptElementImpl::SetScriptL( const TDesC& aScriptString )
    {
    // Already have script from xlink:href
    if ( iXLink != NULL && iScript != NULL )
        {
        return;
        }

    delete iScript;
    iScript = NULL;
    iScript = HBufC8::NewL( aScriptString.Length() );
    iScript->Des().Copy( aScriptString );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgScriptElementImpl::CSvgScriptElementImpl( CSvgDocumentImpl* aDocument )
    {
    iScript = NULL;
    iDocument = aDocument;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgScriptElementImpl::ConstructL( TUint8 aElementId )
    {
    CSvgElementImpl::InitializeL( aElementId );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgScriptElementImpl::FetchExternalScriptL()
    {
    	
  	// Presently SVGT Engine is not supporting expernal script.
  	// This requires script engine which has capability to change DOM.
  	/*
    if ( iXLink != NULL && iDocument != NULL && iDocument->Engine() != NULL )
        {
        iScript = iDocument->Engine()->FetchExternalDataL( *iXLink );
        }
    */
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgScriptElementImpl::Print( TBool aIsEncodeOn )
{
	if (!aIsEncodeOn)
	{
		#ifdef _DEBUG
		RDebug::Printf("<script xlink:href=\"hmm\">"/*, iXLink*/);
		RDebug::Printf("hmm"/*, iScript*/);
		RDebug::Printf("</script",this);
		#endif
	}
}
