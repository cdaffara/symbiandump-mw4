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
* Description:  Implements algoritms for modereted events
*
*/


#include "upnpmoderatedeventqueuemanager.h"
#include "upnpgenamessagefactory.h"
#include "upnpsubscriberlibraryelement.h"
#include "upnphttpmoderatedeventtransaction.h"
#include "upnpnotifytimer.h"

// -----------------------------------------------------------------------------
// CUpnpModeratedEventQueueManager::CUpnpModeratedEventQueueManager
// -----------------------------------------------------------------------------
//
CUpnpModeratedEventQueueManager::CUpnpModeratedEventQueueManager( 
                                        CUpnpServiceImplementation& aServiceImpl,
                                        CUpnpEventController& aEventController,
                                        CUpnpSubscriberLibrary* aSubscriberLibrary
                                                                )
    :CUpnpEventQueueManagerBase( aServiceImpl, aEventController , aSubscriberLibrary )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CUpnpModeratedEventQueueManager::~CUpnpModeratedEventQueueManager
// -----------------------------------------------------------------------------
//
CUpnpModeratedEventQueueManager::~CUpnpModeratedEventQueueManager()
    {
    delete iModerationTimer;
    }

// -----------------------------------------------------------------------------
// CUpnpModeratedEventQueueManager::CUpnpModeratedEventQueueManager
// -----------------------------------------------------------------------------
//
CUpnpModeratedEventQueueManager* CUpnpModeratedEventQueueManager::NewLC( 
                                        CUpnpServiceImplementation& aServiceImpl,
                                        CUpnpEventController& aEventController,
                                        CUpnpSubscriberLibrary* aSubscriberLibrary 
                                                                        )
    {
    CUpnpModeratedEventQueueManager* self = 
        new (ELeave) CUpnpModeratedEventQueueManager( aServiceImpl, 
                                                      aEventController, 
                                                      aSubscriberLibrary 
                                                     );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// -----------------------------------------------------------------------------
// CUpnpModeratedEventQueueManager::NewL
// -----------------------------------------------------------------------------
//
CUpnpModeratedEventQueueManager* CUpnpModeratedEventQueueManager::NewL( 
                                       CUpnpServiceImplementation& aServiceImpl,
                                       CUpnpEventController& aEventController,
                                       CUpnpSubscriberLibrary* aSubscriberLibrary 
                                                                       )
    {
    CUpnpModeratedEventQueueManager* self=
            CUpnpModeratedEventQueueManager::NewLC( aServiceImpl, 
                                                    aEventController, 
                                                    aSubscriberLibrary );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpModeratedEventQueueManager::ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpModeratedEventQueueManager::ConstructL()
    {
    BaseConstructL();
    iModerationTimer = CUpnpNotifyTimer::NewL( this );
    }

// -----------------------------------------------------------------------------
// CUpnpModeratedEventQueueManager::StartSendingEventL
// -----------------------------------------------------------------------------
//    
void CUpnpModeratedEventQueueManager::StartSendingEventL()
    {
    if ( !iSubscriberLibrary->SubscriberLibrary().Count() )
        {
        return;
        }
    
    if ( iIsSending ) 
        {
        iNewValuesWaitingToBeSend = ETrue;
        return;
        }
    
    iIsSending = ETrue;
     
    iCurrentSubscriberIndex = 0;
    for ( TInt i = 0; i < iServiceImpl.EventedStateVariables().Count(); i++ )
        {
        if ( iServiceImpl.EventedStateVariables()[i]->MaxEventRate() > 0 )
            {
            iServiceImpl.EventedStateVariables()[i]->EventTick();
            }
        }    

    delete iBody;
    iBody = NULL;
      
    iBody = CreateEventL( iServiceImpl.StateVariableList() );
    ClearVariables();
        
    PrepareTransactionAndStartSendingL( KFirstSubscriberInQueue );
    }

// -----------------------------------------------------------------------------
// CUpnpModeratedEventQueueManager::SendEvent
// -----------------------------------------------------------------------------
//
void CUpnpModeratedEventQueueManager::SendEvent()
    {
    if ( ! iSubscriberLibrary->SubscriberLibrary().Count() )
        {
        return;
        }
    
    iModerationTimer->Cancel();
    iModerationTimer->After( KMaxEventRate ); 
    }

// -----------------------------------------------------------------------------
// CUpnpModeratedEventQueueManager::CreateTransactionL
// -----------------------------------------------------------------------------
//
CUpnpHttpTransaction* CUpnpModeratedEventQueueManager::CreateTransactionL( CUpnpHttpMessage* aMessage )
    {
    return CUpnpHttpModeratedEventTransaction::NewL( aMessage, *this );
    }
 
// -----------------------------------------------------------------------------
// CUpnpModeratedEventQueueManager::WaitBeforeSending
// -----------------------------------------------------------------------------
//
void CUpnpModeratedEventQueueManager::WaitBeforeSending()
    {
    iModerationTimer->Cancel();
    iModerationTimer->After( KMaxEventRate );   
    }

// -----------------------------------------------------------------------------
// CUpnpModeratedEventQueueManager::FillEventBodyL
// -----------------------------------------------------------------------------
//
TInt CUpnpModeratedEventQueueManager::FillEventBodyL( TDes8& aEvent, 
                                                 const RPointerArray<CUpnpStateVariable>& aVariableList 
                                                     )
    {
    TInt eventableVariableCount(0);        
    for ( TInt i = 0; i < aVariableList.Count(); i++ )
        {
        if (   aVariableList[i]->Eventable().CompareF( KYes() ) == 0 
            && aVariableList[i]->ReadyForEventing() 
            && aVariableList[i]->IsModerated()  )
            {                            
            AddVariableToEventBody( aEvent , aVariableList[i] );
                                              
            iServiceImpl.StateVariableEvented( aVariableList[i]->Name() );        
            eventableVariableCount++;
            }  
        }
    return eventableVariableCount;
    }

// -----------------------------------------------------------------------------
// CUpnpModeratedEventQueueManager::TimerEventL
// -----------------------------------------------------------------------------
//
void CUpnpModeratedEventQueueManager::TimerEventL( CUpnpNotifyTimer* /*aTimer*/ )
    {
    if ( iModerationTimer->iStatus.Int() == KErrNone )
        {
        if ( !iIsSending )  
            {
            TRAPD( err ,StartSendingEventL() );
            if( err )
                {
                SendingCompleted();
                }
            }
        else
            {
            WaitBeforeSending();
            }
        }
    }
//end of file
