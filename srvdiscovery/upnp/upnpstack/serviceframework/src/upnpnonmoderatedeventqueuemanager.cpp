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
* Description:  Implements algoritms for nonmodereted events
*
*/


#include "upnpnonmoderatedeventqueuemanager.h"
#include "upnpgenamessagefactory.h"
#include "upnphttpnonmoderatedeventtransaction.h"
#include "upnpcommonupnplits.h"
#include "upnpcommonstructs.h"


#define KLogFile _L("UPnPStack.txt")
#include "upnpcustomlog.h"


// -----------------------------------------------------------------------------
// CUpnpNonModeratedEventQueueManager::CUpnpNonModeratedEventQueueManager
// -----------------------------------------------------------------------------
//
CUpnpNonModeratedEventQueueManager::CUpnpNonModeratedEventQueueManager( 
                                                     CUpnpServiceImplementation& aServiceImpl,
                                                     CUpnpEventController& aEventController,
                                                     CUpnpSubscriberLibrary* aSubscriberLibrary
                                                                       )
    :CUpnpEventQueueManagerBase( aServiceImpl, aEventController , aSubscriberLibrary )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CUpnpNonModeratedEventQueueManager::~CUpnpNonModeratedEventQueueManager
// -----------------------------------------------------------------------------
//
CUpnpNonModeratedEventQueueManager::~CUpnpNonModeratedEventQueueManager()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpNonModeratedEventQueueManager::NewLC
// -----------------------------------------------------------------------------
//
CUpnpNonModeratedEventQueueManager* CUpnpNonModeratedEventQueueManager::NewLC( 
                                           CUpnpServiceImplementation& aServiceImpl,
                                           CUpnpEventController& aEventController,
                                           CUpnpSubscriberLibrary* aSubscriberLibrary 
                                                                              )
    {
    CUpnpNonModeratedEventQueueManager* self = 
                 new (ELeave) CUpnpNonModeratedEventQueueManager( aServiceImpl, 
                                                                  aEventController, 
                                                                  aSubscriberLibrary );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// -----------------------------------------------------------------------------
// CUpnpNonModeratedEventQueueManager::NewL
// -----------------------------------------------------------------------------
//    
CUpnpNonModeratedEventQueueManager* CUpnpNonModeratedEventQueueManager::NewL( 
                                            CUpnpServiceImplementation& aServiceImpl,
                                            CUpnpEventController& aEventController,
                                            CUpnpSubscriberLibrary* aSubscriberLibrary 
                                                                            )
    {
    CUpnpNonModeratedEventQueueManager* self= CUpnpNonModeratedEventQueueManager::NewLC( 
                                                            aServiceImpl, 
                                                            aEventController, 
                                                            aSubscriberLibrary );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpNonModeratedEventQueueManager::ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpNonModeratedEventQueueManager::ConstructL()
    {
    BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CUpnpNonModeratedEventQueueManager::SendEvent
// -----------------------------------------------------------------------------
//
void CUpnpNonModeratedEventQueueManager::SendEvent()
    {
    TRAPD( err, SendEventL() );
    if( err )
        {
        SendingCompleted();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpNonModeratedEventQueueManager::SendEventL
// -----------------------------------------------------------------------------
//   
void CUpnpNonModeratedEventQueueManager::SendEventL()
    {
    if ( ! iSubscriberLibrary->SubscriberLibrary().Count() )
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
    iBody =NULL;
         
    iBody = CreateEventL( iServiceImpl.StateVariableList() );
        
    PrepareTransactionAndStartSendingL( KFirstSubscriberInQueue );
    }

   
// -----------------------------------------------------------------------------
// CUpnpNonModeratedEventQueueManager::CreateTranscationL
// -----------------------------------------------------------------------------
//
CUpnpHttpTransaction* CUpnpNonModeratedEventQueueManager::CreateTransactionL( CUpnpHttpMessage* aMessage )
    {
    return CUpnpHttpNonModeratedEventTransaction::NewL( aMessage, *this );
    }

// -----------------------------------------------------------------------------
// CUpnpNonModeratedEventQueueManager::FillEventBodyL
// -----------------------------------------------------------------------------
//
TInt CUpnpNonModeratedEventQueueManager::FillEventBodyL( TDes8& aEvent, 
                                                   const RPointerArray<CUpnpStateVariable>& aVariableList 
                                                       )
    {
    TInt eventableVariableCount(0);        
    for ( TInt i = 0; i < aVariableList.Count(); i++ )
        {
        if (   aVariableList[i]->Eventable().CompareF( KYes() ) == 0 
            && aVariableList[i]->ReadyForEventing() 
            &&!aVariableList[i]->IsModerated() )
            {                            
            AddVariableToEventBody( aEvent , aVariableList[i] );
                                              
            iServiceImpl.StateVariableEvented( aVariableList[i]->Name() );        
            eventableVariableCount++;
            }  
        }
    return eventableVariableCount;
    }
//end of file
