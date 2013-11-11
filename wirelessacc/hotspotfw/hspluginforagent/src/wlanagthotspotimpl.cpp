/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of Agent-hotspot frame work interaction plugin
*
*/



#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "wlanagthotspotimpl.h"
#include "am_debug.h"

/**
* Pairs ECom implementation UIDs with a pointer to the instantiation 
* method for that implementation. Required for all ECom implementation
* collections.
*/
const TImplementationProxy ImplementationTable[] =
    {
        {{0x1028309d}, reinterpret_cast<TProxyNewLPtr>(CWlanAgtHotSpotImpl::NewL)}
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWlanAgtHotSpotImpl::CWlanAgtHotSpotImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWlanAgtHotSpotImpl::CWlanAgtHotSpotImpl( )
    {
    //LOGPRINT((_L("CWlanAgtHotSpotImpl::CWlanAgtHotSpotImpl()" )))
    DEBUG( "CWlanAgtHotSpotImpl::CWlanAgtHotSpotImpl()" );
    }

// -----------------------------------------------------------------------------
// CWlanAgtHotSpotImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWlanAgtHotSpotImpl::ConstructL()
    {
    //LOGPRINT((_L("CWlanAgtHotSpotImpl::ConstructL()" )))
    DEBUG( "CWlanAgtHotSpotImpl::ConstructL()" );

    User::LeaveIfError( iHSServer.Connect() );
    }

// -----------------------------------------------------------------------------
// CWlanAgtHotSpotImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWlanAgtHotSpotImpl* CWlanAgtHotSpotImpl::NewL( )
    {
    DEBUG("CWlanAgtHotSpotImpl::NewL()");
    CWlanAgtHotSpotImpl* self = new( ELeave ) CWlanAgtHotSpotImpl( );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CWlanAgtHotSpotImpl::~CWlanAgtHotSpotImpl()
    {
    //LOGPRINT((_L("CWlanAgtHotSpotImpl::~CWlanAgtHotSpotImpl()" )))
    DEBUG( "CWlanAgtHotSpotImpl::~CWlanAgtHotSpotImpl()" );
    iHSServer.Close();
    }


// -----------------------------------------------------------------------------
// CWlanAgtHotSpotImpl::HotSpotStart
// Sends Start request for an iAP to the HotSpot server
// -----------------------------------------------------------------------------
//
void CWlanAgtHotSpotImpl::HotSpotStart( const TUint aIapId, TRequestStatus& aStatus )
    {
    //LOGPRINT((_L("CWlanAgtHotSpotImpl::HotSpotStart()" )))
    DEBUG( "CWlanAgtHotSpotImpl::HotSpotStart()" );
    iHSServer.Start( aIapId, aStatus );
    }
	
// -----------------------------------------------------------------------------
// CWlanAgtHotSpotImpl::HotSpotStartAgain
// Sends StartAgain request for an iAP to the HotSpot server
// -----------------------------------------------------------------------------
//
void CWlanAgtHotSpotImpl::HotSpotStartAgain( const TUint aIapId, TRequestStatus& aStatus )
    {
    //LOGPRINT((_L("CWlanAgtHotSpotImpl::HotSpotStartAgain()" )))
    DEBUG( "CWlanAgtHotSpotImpl::HotSpotStartAgain()" );
    iHSServer.StartAgain( aIapId, aStatus );
    }
	

// -----------------------------------------------------------------------------
// CWlanAgtHotSpotImpl::HotSpotCloseConnection
// Sends CloseConnection request for an iAP to the HotSpot server
// -----------------------------------------------------------------------------
//
void CWlanAgtHotSpotImpl::HotSpotCloseConnection( const TUint aIapId, TRequestStatus& aStatus )
    {
    //LOGPRINT((_L("CWlanAgtHotSpotImpl::HotSpotCloseConnection()" )))
    DEBUG( "CWlanAgtHotSpotImpl::HotSpotCloseConnection()" );
    iHSServer.CloseConnection( aIapId, aStatus );
    }


// -----------------------------------------------------------------------------
// CWlanAgtHotSpotImpl::HotSpotCancel
// Sends Cancel request for an iAP to the HotSpot server
// -----------------------------------------------------------------------------
//
void CWlanAgtHotSpotImpl::HotSpotCancel( const TUint aIapId )
    {
    //LOGPRINT((_L("CWlanAgtHotSpotImpl::HotSpotCancel()" )))
    DEBUG( "CWlanAgtHotSpotImpl::HotSpotCancel()" );
    iHSServer.Cancel( aIapId );
    }

	
// ========================== OTHER EXPORTED FUNCTIONS =========================

// ---------------------------------------------------------
// Returns an instance of the proxy table.
// Returns: KErrNone
// ---------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }
