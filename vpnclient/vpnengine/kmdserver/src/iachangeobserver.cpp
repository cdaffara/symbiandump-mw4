/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  active object, that monitors internal address change
*
*/


#include "ikedebug.h"
#include "ikepluginsessionif.h"
#include "internaladdress.h"
#include "eventmediatorapi.h"

// CLASS HEADER
#include "iachangeobserver.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIaChangeObserver* CIaChangeObserver::NewL( TUint32 aVpnIapId,
                                            const TInetAddr& aDnsServerAddr,
                                            MIkePluginSessionIf& aIkePluginSession,
                                            MIkeDebug& aDebug )
    {
    CIaChangeObserver* self = new (ELeave) CIaChangeObserver( aVpnIapId,
                                                              aDnsServerAddr,
                                                              aIkePluginSession,
                                                              aDebug );
    CleanupStack::PushL( self );
    self->ConstructL();    
    CleanupStack::Pop( self );
    return self;
    
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CIaChangeObserver::~CIaChangeObserver()
    {
    Cancel();            
    iEventMediator.Close();
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CIaChangeObserver::CIaChangeObserver( TUint32 aVpnIapId,
                                      const TInetAddr& aDnsServerAddr,
                                      MIkePluginSessionIf& aIkePluginSession,
                                      MIkeDebug& aDebug )
 : CActive( EPriorityStandard ),
   iVpnIapId( aVpnIapId ),
   iDnsServerAddr( aDnsServerAddr ),
   iIkePluginSession( aIkePluginSession ),
   iDebug( aDebug )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
void CIaChangeObserver::ConstructL()
    {
    User::LeaveIfError( iEventMediator.Connect() );    
    }

// ---------------------------------------------------------------------------
// Requests asynchronous internal address change notification.
// ---------------------------------------------------------------------------
//
void CIaChangeObserver::StartObserving()
    {
    iIkePluginSession.NotifyInternalAddressChanged( iInternalAddress,
                                                    iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles completion of asynchronous notification request. 
// ---------------------------------------------------------------------------
//
void CIaChangeObserver::RunL()
    {
    __ASSERT_DEBUG( iStatus.Int() == KErrNone ||
                    iStatus.Int() == KErrCancel,
                    User::Invariant() );

    if ( iStatus.Int() == KErrNone )
        {
        // VPN NET id is not needed in reporting internal address change.
        TConnectionInfo connectionInfo( iVpnIapId, 0 );
        TConnectionInfoBuf pckgConnectionInfo( connectionInfo );
        
        TVPNAddressPckg pckgVpnAddress( iInternalAddress );        
        TInt err = iEventMediator.ReportEvent( EKmdAddressChangeEvent,
                                               pckgConnectionInfo,
                                               pckgVpnAddress );
        err = err;
    
#ifdef _DEBUG                            
        TBuf<80> txt_addr;        
        iInternalAddress.iVPNIfAddr.OutputWithScope(txt_addr);
        DEBUG_LOG3(_L("Internal address changed to %S, VPN IAP id=%d, err=%d"),
                &txt_addr, iVpnIapId, err );
#endif            
        
        StartObserving();    
        }
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles cancellation of asynchronous notification request. 
// ---------------------------------------------------------------------------
//
void CIaChangeObserver::DoCancel()
    {
    iIkePluginSession.CancelNotifyInternalAddressChanged();
    }

