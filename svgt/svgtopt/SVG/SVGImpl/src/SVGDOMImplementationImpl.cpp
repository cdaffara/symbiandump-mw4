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


#include "SVGDOMImplementationImpl.h"
#include "SVGDocumentImpl.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
MXmlDocument* CSvgDOMImplementationImpl::CreateDocumentL( const TDesC& /* aNamespaceUri */,
                                                          const TDesC& /* aQualifiedName*/ /*,
                                                          CXmlDocumentType* aDocType*/ )
    {
    return ( MXmlDocument * ) CSvgDocumentImpl::NewL(iSvgBitmapFontProvider);
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgDOMImplementationImpl* CSvgDOMImplementationImpl::NewL(CSvgBitmapFontProvider *aSvgBitmapFontProvider)
    {
    CSvgDOMImplementationImpl* self = new ( ELeave )
                                      CSvgDOMImplementationImpl(aSvgBitmapFontProvider);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgDOMImplementationImpl* CSvgDOMImplementationImpl::NewLC(CSvgBitmapFontProvider *aSvgBitmapFontProvider)
    {
    CSvgDOMImplementationImpl* self = new ( ELeave )
                                      CSvgDOMImplementationImpl(aSvgBitmapFontProvider);
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CSvgDOMImplementationImpl::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgDOMImplementationImpl::CSvgDOMImplementationImpl(CSvgBitmapFontProvider *aSvgBitmapFontProvider): iSvgBitmapFontProvider(aSvgBitmapFontProvider)
    {
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
CSvgDOMImplementationImpl::~CSvgDOMImplementationImpl()
    {
    }
