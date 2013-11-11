/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CUpnpTerminalModeIcon class implementation.
*
*/

// INCLUDE FILES
#include "upnpterminalmodeicon.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "upnpterminalmodeiconTraces.h"
#endif

// ============================ MEMBER FUNCTIONS ===================================

// ---------------------------------------------------------------------------------
// CUpnpTerminalModeIcon::NewL
// Two-phased constructor.
// @param aPath     icon path
// @param aWidth    icon width
// @param aHeight   icon height
// @param aDepth    icon depth
// @param aMimeType icon mime type
// ---------------------------------------------------------------------------------
//
EXPORT_C CUpnpTerminalModeIcon* CUpnpTerminalModeIcon::NewL( const TDesC16& aPath, const TInt aWidth, 
                                     const TInt aHeight, const TInt aDepth, const TDesC8& aMimeType )
    {
    OstTraceFunctionEntry0( CUPNPTERMINALMODEICON_NEWL_ENTRY );
    CUpnpTerminalModeIcon* self = new (ELeave) CUpnpTerminalModeIcon( aWidth,aHeight,aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aPath,aMimeType );
    CleanupStack::Pop( self );
    OstTraceFunctionExit0( CUPNPTERMINALMODEICON_NEWL_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------------
// CUpnpTerminalModeIcon::CUpnpTerminalModeIcon
// C++ default constructor can NOT contain any code, that
// might leave.
// @param aWidth    icon width
// @param aHeight   icon height
// @param aDepth    icon depth
// ---------------------------------------------------------------------------------
//
CUpnpTerminalModeIcon::CUpnpTerminalModeIcon( const TInt aWidth,const TInt aHeight,
    const TInt aDepth ):iIconWidth(aWidth),iIconHeight(aHeight),iIconDepth(aDepth)
    {

    }

// -------------------------------------------------------------------------------------
// CUpnpTerminalModeIcon::ConstructL
// Symbian 2nd phase constructor can leave.
// @param aPath     icon path
// @param aMimeType icon mime type
// ---------------------------------------------------------------------------------
//
void CUpnpTerminalModeIcon::ConstructL( const TDesC16& aPath,const TDesC8& aMimeType )
    {
    OstTraceFunctionEntry0( CUPNPTERMINALMODEICON_CONSTRUCTL_ENTRY );
    iFilename = aPath.AllocL();
    iIconMimeType = aMimeType.AllocL();
    OstTraceFunctionExit0( CUPNPTERMINALMODEICON_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTerminalModeIcon::~CUpnpTerminalModeIcon
// Destructor
// ---------------------------------------------------------------------------------
//
CUpnpTerminalModeIcon::~CUpnpTerminalModeIcon()
    {
    OstTraceFunctionEntry0( CUPNPTERMINALMODEICON_CUPNPTERMINALMODEICON_ENTRY );
    delete iFilename;
    delete iIconMimeType;
    OstTraceFunctionExit0( CUPNPTERMINALMODEICON_CUPNPTERMINALMODEICON_EXIT );
    }

// End of File
