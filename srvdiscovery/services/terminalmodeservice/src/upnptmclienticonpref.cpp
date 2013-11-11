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
* Description: CUpnpTmClientIconPref class implementation.
*
*/

// INCLUDE FILES
#include "upnptmclienticonpref.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "upnptmclienticonprefTraces.h"
#endif

// Literal 
_LIT8(KBmpMimeType,    "image/bmp");


// ============================ MEMBER FUNCTIONS ===================================

// ---------------------------------------------------------------------------------
// CUpnpTmClientIconPref::NewL
// Two-phased constructor.
// @param aProfileId  ProfileID of the client profile.
// ---------------------------------------------------------------------------------
//
EXPORT_C CUpnpTmClientIconPref* CUpnpTmClientIconPref::NewL( TUint aProfileId )
    {
    OstTraceFunctionEntry0( CUPNPTMCLIENTICONPREF_NEWL_ENTRY );
    CUpnpTmClientIconPref* self = new (ELeave) CUpnpTmClientIconPref(aProfileId);    
    CleanupStack::PushL(self);
    self->ConstructL( );  
    CleanupStack::Pop(self);
    OstTraceFunctionExit0( CUPNPTMCLIENTICONPREF_NEWL_EXIT );
    return self;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmClientIconPref::CUpnpTmClientIconPref
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------------
//
CUpnpTmClientIconPref::CUpnpTmClientIconPref( TUint aProfileId ):iClientProfileId(aProfileId),
    iIconWidth(KErrNone),iIconHeight(KErrNone),iIconDepth(KDefaultDepthValue)
   {
   
   }

// ---------------------------------------------------------------------------------
// CUpnpTmClientIconPref::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------------
//
void CUpnpTmClientIconPref::ConstructL( )
    {
    OstTraceFunctionEntry0( CUPNPTMCLIENTICONPREF_CONSTRUCTL_ENTRY );
    iIconMimeType = KBmpMimeType().AllocL();
    OstTraceFunctionExit0( CUPNPTMCLIENTICONPREF_CONSTRUCTL_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmClientIconPref::~CUpnpTmClientIconPref
// Destructor
// ---------------------------------------------------------------------------------
//
CUpnpTmClientIconPref::~CUpnpTmClientIconPref()
    {
    OstTraceFunctionEntry0( CUPNPTMCLIENTICONPREF_CUPNPTMCLIENTICONPREF_ENTRY );
    delete iIconMimeType;
    OstTraceFunctionExit0( CUPNPTMCLIENTICONPREF_CUPNPTMCLIENTICONPREF_EXIT );
    }

// ---------------------------------------------------------------------------------
// CUpnpTmClientIconPref::SetMimeTypeL
// Method is used to provide the preferred mime type of the client.
// @param aMimeType mime type of the icon
// ---------------------------------------------------------------------------------
//
EXPORT_C void CUpnpTmClientIconPref::SetMimeTypeL( const TDesC8& aMimeType )
   {
   OstTraceFunctionEntry0( CUPNPTMCLIENTICONPREF_SETMIMETYPEL_ENTRY );
   delete iIconMimeType;
   iIconMimeType = NULL;
   iIconMimeType = aMimeType.AllocL();
   OstTraceFunctionExit0( CUPNPTMCLIENTICONPREF_SETMIMETYPEL_EXIT );
   }


// End of File
