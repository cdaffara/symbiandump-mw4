/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Notification class
*
*/


#include "e32std.h"
#include "hssinterface.h"
#include "hsssrvnotifications.h"
#include "am_debug.h"
#include "hotspotclientserver.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CHssSrvNotifications::ConstructL
// ---------------------------------------------------------
//
void CHssSrvNotifications::ConstructL()
    {
    DEBUG( "CHssSrvNotifications::ConstructL()" );
    CActiveScheduler::Add( this );
    WaitForNotifications();   
    }

// ---------------------------------------------------------
// CHssSrvNotifications::CHssSrvNotifications
// ---------------------------------------------------------
// 
CHssSrvNotifications::CHssSrvNotifications(
    MHssSrvNotifications& aCallback,
    RHssInterface& aServer):
    CActive( CActive::EPriorityStandard ), 
    iCallback( &aCallback ),
    iServer( aServer ),
    iReturnData(),
    iDataPckg( iReturnData ),
    iCancelRequested( EFalse ),
    iIapId( 0 )
    {
    DEBUG( "CHssSrvNotifications::CHssSrvNotifications()" );
    }

// ---------------------------------------------------------
// CHssSrvNotifications::~CHssSrvNotifications
// ---------------------------------------------------------
//
CHssSrvNotifications::~CHssSrvNotifications()
    {
    DEBUG( "CHssSrvNotifications::~CHssSrvNotifications()" );
    Cancel();
    }
        
// ---------------------------------------------------------
// CHssSrvNotifications::NewL
// ---------------------------------------------------------
//
CHssSrvNotifications* CHssSrvNotifications::NewL(
    MHssSrvNotifications& aCallback,
    RHssInterface& aServer ) 
    {
    DEBUG( "CHssSrvNotifications::NewL()" );
    CHssSrvNotifications* self = new (ELeave) CHssSrvNotifications( aCallback, aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CHssSrvNotifications::RunL
// ---------------------------------------------------------
//
void CHssSrvNotifications::RunL()
    {
    DEBUG1( "CHssSrvNotifications::RunL(), status == %d", iStatus.Int() );

    if( iStatus == KRequestPending )
        {
        DEBUG( "ERROR in CHssSrvNotifications::RunL(): request still pending!" );
        return;
        }

    if( iStatus == KErrServerTerminated )
        {
        DEBUG( "ERROR in CHssSrvNotifications::RunL(): server terminated" );
        return;
        }

    switch( iStatus.Int() )
        {
        case EHssNewNetworksDetected:
        	{
        	iCallback->NewNetworksDetected( iIapId );
        	break;
        	}
        case EHssOldNetworksLost:
        	{
        	iCallback->OldNetworksLost( iIapId );
        	break;
        	}
        default:
            DEBUG1( "ERROR in CHssSrvNotifications::RunL: unknown notification: %d", iStatus.Int() );
        }
        
    // Make a new notification request if allowed
    if( !iCancelRequested )
        {
        WaitForNotifications();
        }
      }
        
// ---------------------------------------------------------
// CHssSrvNotifications::DoCancel
// ---------------------------------------------------------
//  
void CHssSrvNotifications::DoCancel()
    {
    DEBUG( "CHssSrvNotifications::DoCancel()" );
    iServer.CancelWaitForNotification();
    }
       
// ---------------------------------------------------------
// CHssSrvNotifications::WaitForNotifications
// ---------------------------------------------------------
//  
void CHssSrvNotifications::WaitForNotifications()
    {
    DEBUG( "CHssSrvNotifications::WaitForNotifications()" );
    iServer.WaitForNotification( iStatus, iDataPckg );    
    SetActive();
    }

// ---------------------------------------------------------
// CHssSrvNotifications::Activate
// ---------------------------------------------------------
//  
void CHssSrvNotifications::Activate( MHssSrvNotifications& aCallback )
    {
    DEBUG( "CHssSrvNotifications::Activate()" );
    iCallback = &aCallback;
    iCancelRequested = EFalse;
    WaitForNotifications();
    }
        
// ---------------------------------------------------------
// CHssSrvNotifications::SetCancelled
// ---------------------------------------------------------
//  
void CHssSrvNotifications::SetCancelled()
     {
     iCancelRequested = ETrue;
     }
