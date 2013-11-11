/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Actual implementation of DLL. 
*
*/


#include <f32file.h> 

#include "ictsengine.h"
#include "internetconnectivitycrkeys.h"
#include "ictsasyncwait.h"
#include "am_debug.h"
#include "hotspotclientserver.h"


using namespace CMManager;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CIctsEngine::CIctsEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CIctsEngine::CIctsEngine( TUint32 aIapId, 
                          TUint32 aNetworkId,
                          MIctsObserver& aClient ) :
iIapId( aIapId ), iNetworkId( aNetworkId ), iPollingStatus( EFalse ), 
iRepository( NULL ), iClient( aClient )
    {
    iPollingIntervalTimer =NULL;
    iPollingTimeTimer = NULL;  
    iHttpHandler =NULL;
    iPollingInterval=0;
    }

// ---------------------------------------------------------------------------
// CIctsEngine::ConstructL
// ---------------------------------------------------------------------------
//
void CIctsEngine::ConstructL()
    {
    DEBUG("CIctsEngine::ConstructL");
    _LIT8(KIp, "http://connectivity-test.ext.nokia.com/");
    
    // CenRep uses UTF 16
    HBufC16* uriBuffer = HBufC16::NewLC( KMaxIpLength );
    TPtr16 uriPtr = uriBuffer->Des();
    // For HTTP FW change it to 8bit
    HBufC8* uriBuffer8 = HBufC8::NewLC( KMaxIpLength );
    TPtr8 uriPtr8 = uriBuffer8->Des();
    TRAPD(err, iRepository = CRepository::NewL( 
                                    KCRUidInternetConnectivitySettings ) );
    if ( KErrNone == err )
        {
        TInt err = iRepository->Get( KIctsIpDestination, uriPtr );
        uriPtr8.Copy( uriPtr );
        iIPAddress = uriPtr8;
        if ( KErrNone != err )
            {
            iIPAddress = KIp();
            }
        }
    else
        {   
        iIPAddress = KIp();
        }
    CleanupStack::PopAndDestroy(uriBuffer8);
    CleanupStack::PopAndDestroy(uriBuffer);
    iPollingIntervalTimer = CIctsPollingIntervalTimer::NewL( *this );
    
    iPollingTimeTimer = CIctsPollingTimeTimer::NewL( *this );
    
    iHttpHandler = CIctsHttpHandler::NewL( *this, KHttpResponseTime );
    }

// ---------------------------------------------------------------------------
// CIctsEngine::NewL
// ---------------------------------------------------------------------------
//
CIctsEngine* CIctsEngine::NewL( TUint32 aIapId, 
                                                TUint32 aNetworkId,
                                                MIctsObserver& aClient
                                                 )
    {
    CIctsEngine* self =  new (ELeave) CIctsEngine( aIapId, 
                                                                   aNetworkId,
                                                                   aClient );
    CleanupStack::PushL( self );
    self->ConstructL();                                                            
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CIctsEngine::~CIctsEngine
// ---------------------------------------------------------------------------
//
CIctsEngine::~CIctsEngine()
    {
    DEBUG("CIctsEngine::~CIctsEngine()");
    if ( iRepository )
        {
        delete iRepository;
        }
    
    if ( iPollingIntervalTimer )
        {
        delete iPollingIntervalTimer;
        }
    
    if ( iPollingTimeTimer )
        {
        delete iPollingTimeTimer;
        }
    
    if ( iHttpHandler )
        {
        delete iHttpHandler;
        }
        
    iHsServer.Close();
    DEBUG("CIctsEngine::~CIctsEngine() DONE");
    }

// ---------------------------------------------------------------------------
// CIctsEngine::Start
// ---------------------------------------------------------------------------
//
void CIctsEngine::StartL()
    { 
    DEBUG("CIctsEngine::StartL()");
    if ( CheckSettingL() == EIctsRunAutomatically )
        {
        TInt result = iHttpHandler->SendHttpRequestL( iIPAddress , iIapId, iNetworkId );
            
        // Sending HTTP request did not succeed. RConnection failed.
        if ( KErrNone != result )
            {
            iClient.ConnectivityObserver( EConnectionNotOk, iString );
            }
        }
    else 
        {
        iClient.ConnectivityObserver( EConnectionNotOk, iString );
        }
    }

// ---------------------------------------------------------------------------
// CIctsEngine::CancelStart
// ---------------------------------------------------------------------------
//
void CIctsEngine::CancelStartL()
    { 
    DEBUG("CIctsEngine::CancelStartL()");
    iHttpHandler->CancelHttpRequestL(); 
    }

// ---------------------------------------------------------------------------
// CIctsEngine::StartPolling
// ---------------------------------------------------------------------------
//
void CIctsEngine::StartPolling( TInt aTime, 
                                        TInt aInterval )
    {
    DEBUG("CIctsEngine::StartPolling()");
    iPollingStatus = ETrue;
    iPollingInterval = aInterval;
    // Do not send right away HTTP request since most likely it will fail. -> hotspot browser case.
    iPollingIntervalTimer->After( KStartPollingWait );
    iPollingTimeTimer->After( aTime + KStartPollingWait );
    }

// ---------------------------------------------------------------------------
// CIctsEngine::StopPolling
// ---------------------------------------------------------------------------
//
void CIctsEngine::StopPolling()
    {
    DEBUG("CIctsEngine::StopPolling()");
    iPollingIntervalTimer->Cancel();
    iPollingTimeTimer->Cancel();
    iPollingStatus = EFalse;
    }

// ---------------------------------------------------------------------------
// CIctsEngine::CheckSettingL
// ---------------------------------------------------------------------------
//
TInt CIctsEngine::CheckSettingL()
    {
    DEBUG("CIctsEngine::CheckSettingL()");
    // Check connectivity test setting from CenRep
    TInt ret( EIctsRunAutomatically );
    iRepository->Get( KIctsTestPermission, ret );
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CIctsEngine::PollingIntervalTimeout
// ---------------------------------------------------------------------------
// 
void CIctsEngine::PollingIntervalTimeout()
    {
    DEBUG("CIctsEngine::PollingIntervalTimeout()");
    TRAPD( err, iHttpHandler->SendHttpRequestL( iIPAddress , 
                                            iIapId, 
                                            iNetworkId ));
        
    if ( KErrNone != err )
        {
        DEBUG1("CIctsEngine::PollingIntervalTimeout() err: %d", err);
        StopPolling();
        iClient.ConnectivityObserver( EConnectionNotOk, iString );    
        }
    iPollingIntervalTimer->After( iPollingInterval );
    }
    
// ---------------------------------------------------------------------------
// CIctsEngine::PollingTimeTimeout
// ---------------------------------------------------------------------------
// 
void CIctsEngine::PollingTimeTimeout()
    {
    DEBUG("CIctsEngine::PollingTimeTimeout()");
    StopPolling();
    iClient.ConnectivityObserver( ETimeout, iString );
    }

// ---------------------------------------------------------------------------
// CIctsEngine::HttpEventL
// ---------------------------------------------------------------------------
//  
void CIctsEngine::HttpEventL( TIctsTestResult aResult, TDesC& aString )
    {
    DEBUG("CIctsEngine::HttpEventL()");
   
    if( iPollingStatus )
        {
        DEBUG("CIctsEngine::HttpEventL() polling");
        // We are polling. Send only EConnectionOk result to client.
        // Otherwise continue polling.
        if ( aResult == EConnectionOk)   
            {
            DEBUG("CIctsEngine::HttpEventL() EConnectionOk");
            StopPolling();
            iClient.ConnectivityObserver( aResult, aString );
            }
    
        }
    else
        {
        DEBUG("CIctsEngine::HttpEventL() not polling");
        iClient.ConnectivityObserver( aResult, aString );
        DEBUG("CIctsEngine::HttpEventL() result sent");
        }
    }

// End of File
