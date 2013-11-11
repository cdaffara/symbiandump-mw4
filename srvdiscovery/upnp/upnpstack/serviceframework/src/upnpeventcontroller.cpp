/** @file
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Encapsulates http client for eventing
*
*/



#include <e32base.h>

#include "upnpsettings.h"
#include "upnpeventcontroller.h"
#include "upnpgenamessagefactory.h"
#include "upnphttpmessagefactory.h"
#include "upnphttpinitialeventtransaction.h"
#include "upnpnonmoderatedeventqueuemanager.h"
#include "upnpmoderatedeventqueuemanager.h"
#include "upnpinitialeventqueuemanager.h"
#include "upnpdeviceimplementationbase.h"
#include "upnpcommonupnplits.h"

#define KLogFile _L("UPnPStack.txt")
#include "upnpcustomlog.h"
  
  
// -----------------------------------------------------------------------------
// CUpnpEventController::CUpnpEventController
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CUpnpEventController::CUpnpEventController( CUpnpServiceImplementation& aServiceImpl )
    :iServiceImpl( aServiceImpl )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpEventController::CUpnpEventController
// Destructor
// -----------------------------------------------------------------------------
//  
CUpnpEventController::~CUpnpEventController()
    {
    iPendingTransactions.ResetAndDestroy();
    delete iModQueue;
    delete iNonModQueue;
    delete iInitialEventQueue;
    delete iSubscriberLibrary;
    delete iHttpEngine;
    }

// -----------------------------------------------------------------------------
// CUpnpEventController::NewLC
// Two phased constructor
// -----------------------------------------------------------------------------
//  
CUpnpEventController* CUpnpEventController::NewLC( CUpnpServiceImplementation& aServiceImpl )
    {
    CUpnpEventController* self = new (ELeave) CUpnpEventController( aServiceImpl );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpEventController::NewL
// Two phased constructor
// -----------------------------------------------------------------------------
//  
CUpnpEventController* CUpnpEventController::NewL( CUpnpServiceImplementation& aServiceImpl )
    {
    CUpnpEventController* self=CUpnpEventController::NewLC( aServiceImpl );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpEventController::ConstructL
// Two phased constructor
// -----------------------------------------------------------------------------
//  
void CUpnpEventController::ConstructL()
    {
    iSubscriberLibrary = CUpnpSubscriberLibrary::NewL( *this );
    iHttpEngine = CUpnpHttpClientEngine::NewL( *this, CUpnpSettings::GetIapL() );
    iNonModQueue = CUpnpNonModeratedEventQueueManager::NewL( iServiceImpl , *this, iSubscriberLibrary );
    iModQueue =  CUpnpModeratedEventQueueManager::NewL(  iServiceImpl , *this, iSubscriberLibrary );
    iInitialEventQueue = CUpnpInitialEventQueueManager::NewL(  iServiceImpl , *this, iSubscriberLibrary );
    }

// -----------------------------------------------------------------------------
// CUpnpEventController::SendInitialNotification
// Sends initial event
// -----------------------------------------------------------------------------
//  
void CUpnpEventController::SendInitialNotification( )
    {
    iInitialEventQueue->SendEvent();
    }

// -----------------------------------------------------------------------------
// CUpnpEventController::SendNonModeratedNotification
// Sends not-moderated events
// -----------------------------------------------------------------------------
//  
void CUpnpEventController::SendNonModeratedNotification( )
    {
    iNonModQueue->SendEvent();
    }

// -----------------------------------------------------------------------------
// CUpnpEventController::SendModeratedNotification
// Sends moderated event
// -----------------------------------------------------------------------------
//  
void CUpnpEventController::SendModeratedNotification(  )
    {
    iModQueue->SendEvent();
    }
    
// -----------------------------------------------------------------------------
// CUpnpEventController::SendTransactionL
// Sends event to subscriber
// -----------------------------------------------------------------------------
//  
void  CUpnpEventController::SendTransactionL( CUpnpHttpTransaction* aTransaction )
    {
    CleanupStack::PushL( aTransaction );
    iPendingTransactions.AppendL( aTransaction );
    CleanupStack::Pop( aTransaction );
    
    iHttpEngine->SendL( *aTransaction );
    }
 
// -----------------------------------------------------------------------------
// CUpnpEventController::SubscribeL
// Proccess subscribe message
// -----------------------------------------------------------------------------
//  
void CUpnpEventController::SubscribeL( CUpnpGenaMessage* aMessage )
    {
    TInt subscribers = iSubscriberLibrary->SubscriberLibrary().Count();
        
    CUpnpSubscriberLibraryElement* subscriber( NULL );
    TUpnpErrorCode subscriptionState 
        = iSubscriberLibrary->AddInfoL( aMessage, &subscriber );

    // Responding:
    if ( subscriber && subscriptionState == EHttpOk )
        {
        // for a new subscriber: SID generation
        if ( subscribers < iSubscriberLibrary->SubscriberLibrary().Count() )
            {
            GenerateSidL( subscriber );
            }
            
        HBufC8* timeout( TimerHeaderLC( subscriber->Timeout() ) );
        
        CUpnpHttpMessage* messageOut = reinterpret_cast<CUpnpHttpMessage*>(
                                    RUpnpGenaMessageFactory::SubscribeResponseL(
                                                    aMessage->SessionId(), 
                                                    aMessage->Sender(), 
                                                    subscriber->Sid(), 
                                                    timeout->Des() )
                                                                            );
        CleanupStack::PopAndDestroy( timeout );                    
        timeout = NULL;
        CleanupStack::PushL( messageOut );

        iServiceImpl.SendL( messageOut );

        CleanupStack::PopAndDestroy( messageOut );
        messageOut = NULL;
        
            
        // for a new subscriber: initial event message
        if ( subscribers < iSubscriberLibrary->SubscriberLibrary().Count() )
            {
            SendInitialNotification( );
            }
        }
    else
        {       
        CUpnpHttpMessage* messageOut = RUpnpHttpMessageFactory::UpnpResponseErrorL( 
                                                                aMessage, 
                                                                subscriptionState 
                                                                                  );
        CleanupStack::PushL( messageOut );
        iServiceImpl.SendL( messageOut );
        CleanupStack::PopAndDestroy( messageOut );
        }       
    }

// -----------------------------------------------------------------------------
// CUpnpEventController::UnSubscribeL
// Proccess unsubscribe message
// -----------------------------------------------------------------------------
//  
void CUpnpEventController::UnSubscribeL( CUpnpGenaMessage* aMessage )
    {
    TUpnpErrorCode subscriptionState( CheckHeaders( aMessage ) );

    if ( subscriptionState == EUndefined )
        {
        subscriptionState = iSubscriberLibrary->Remove( aMessage->Sid() );
        } 
    
    if ( subscriptionState == EHttpOk )
        {
        CUpnpHttpMessage* messageOut = reinterpret_cast<CUpnpHttpMessage*>(
                RUpnpGenaMessageFactory::UnsubscribeResponseL( aMessage->SessionId(), 
                                                               aMessage->Sender() ) 
                                                                           );
        CleanupStack::PushL( messageOut );        
        iServiceImpl.SendL( messageOut );
        CleanupStack::PopAndDestroy( messageOut );   
        }
    else
        {
        CUpnpHttpMessage* messageOut = RUpnpHttpMessageFactory::UpnpResponseErrorL( 
                                                                    aMessage,
                                                                    subscriptionState
                                                                                   );
        CleanupStack::PushL( messageOut );
        iServiceImpl.SendL( messageOut );
        CleanupStack::PopAndDestroy( messageOut );
        }
    } 

// -----------------------------------------------------------------------------
// CUpnpEventController::CancelInvalidSessions
// Cancels not completed transaction 
// -----------------------------------------------------------------------------
// 
void CUpnpEventController::CancelInvalidSessions( const TDesC8& aSid )
    {
    for ( TInt i(0) ; i < iPendingTransactions.Count() ; i++ )
        {
        if ( static_cast<CUpnpGenaMessage*>( iPendingTransactions[i]->Request() )->Sid().Compare( aSid ) == 0 )
            {         
            iHttpEngine->Cancel( *iPendingTransactions[i] );
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpEventController::SubscribersAmount
// Returns number of subscribers 
// -----------------------------------------------------------------------------
// 
TInt CUpnpEventController::SubscribersAmount()
    {
    return iSubscriberLibrary->SubscriberLibrary().Count();
    }

// -----------------------------------------------------------------------------
// CUpnpEventController::CheckHeader
// Check if unsubscribe header are comlete 
// -----------------------------------------------------------------------------
// 
TUpnpErrorCode CUpnpEventController::CheckHeaders( CUpnpGenaMessage* aMessage )
    {
    TUpnpErrorCode subscriptionState( EUndefined );

    // Errors: Incompatible headers
    if ( aMessage->Sid().Compare( KNoHeader) != 0 )
        {
        if ( aMessage->Nt().Compare( KNoHeader) != 0 )
            {
            subscriptionState = EBadRequest;
            }
        else if (aMessage->Callback().Compare( KNoHeader ) != 0 )
            {
            subscriptionState = EBadRequest;
            }
        else
            {
            }
        }
    return subscriptionState;
    }

// -----------------------------------------------------------------------------
// CUpnpEventController::GenerateSidL
// Genereated uuid for a subscriber 
// -----------------------------------------------------------------------------
// 
void CUpnpEventController::GenerateSidL( CUpnpSubscriberLibraryElement* aSubscriber )
    {
    HBufC8* uuid = iServiceImpl.DeviceImpl().GenerateUuidL();
    CleanupStack::PushL( uuid );
    aSubscriber->SetSidL( *uuid );
    CleanupStack::PopAndDestroy( uuid );
    LOGS("  new subscriber:");
    }
// -----------------------------------------------------------------------------
// CUpnpEventController::SubscribersAmount
// Returns evented state variables
// -----------------------------------------------------------------------------
//    
RPointerArray<CUpnpStateVariable>& CUpnpEventController::EventedStateVariables()
    {
    return iServiceImpl.EventedStateVariables();
    }
   
// -----------------------------------------------------------------------------
// CUpnpEventController::SubscriberRemoved
// Resumes processing when subscriber  has been removed
// -----------------------------------------------------------------------------
//    
void CUpnpEventController::SubscriberRemoved( CUpnpSubscriberLibraryElement* aSubscriber , TInt aPos )
    {
    CancelInvalidSessions( aSubscriber->Sid() );
    
    iNonModQueue->UpdateQueue( aPos );
    iModQueue->UpdateQueue( aPos );
    }

// -----------------------------------------------------------------------------
// CUpnpEventController::TimerHeaderLC
// Creates timeout header
// -----------------------------------------------------------------------------
//    
HBufC8* CUpnpEventController::TimerHeaderLC( const TDesC8& aTimeout )
    {
    HBufC8* timeout= HBufC8::NewLC( aTimeout.Length() + KTimeoutPrefix().Length() );
    timeout->Des().Zero();
    timeout->Des().Append( KTimeoutPrefix );
    timeout->Des().Append( aTimeout );
    return timeout;
    }
// -----------------------------------------------------------------------------
// CUpnpEventController::ClientResponseRecivedLD
// Handles event response
// -----------------------------------------------------------------------------
//          
void CUpnpEventController::ClientResponseRecivedLD( CUpnpHttpTransaction& aEndedTransaction )
{
    TInt idx( iPendingTransactions.Find( &aEndedTransaction ) );
    ASSERT( idx >= 0 ); //it should never happen that we received transaction that wasn't sent by us
    iPendingTransactions.Remove( idx );
    aEndedTransaction.ProcessResponseL();
    delete ( &aEndedTransaction );
}
//end of file
