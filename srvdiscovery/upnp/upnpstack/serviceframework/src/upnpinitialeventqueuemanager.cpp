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
* Description:  Implements algoritms for initial events
*
*/


#include "upnpinitialeventqueuemanager.h"
#include "upnpgenamessagefactory.h"
#include "upnphttpinitialeventtransaction.h"
#include "upnpcommonupnplits.h"
#include "upnpcommonstructs.h"


#define KLogFile _L("UPnPStack.txt")
#include "upnpcustomlog.h"


// -----------------------------------------------------------------------------
// CUpnpInitialEventQueueManager::CUpnpInitialEventQueueManager
// -----------------------------------------------------------------------------
//
CUpnpInitialEventQueueManager::CUpnpInitialEventQueueManager( 
                                            CUpnpServiceImplementation& aServiceImpl,
                                            CUpnpEventController& aEventController,
                                            CUpnpSubscriberLibrary* aSubscriberLibrary
                                                             )
    :CUpnpEventQueueManagerBase( aServiceImpl, aEventController, aSubscriberLibrary )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CUpnpInitialEventQueueManager::~CUpnpInitialEventQueueManager
// -----------------------------------------------------------------------------
//
CUpnpInitialEventQueueManager::~CUpnpInitialEventQueueManager()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpInitialEventQueueManager::NewLC
// -----------------------------------------------------------------------------
//
CUpnpInitialEventQueueManager* CUpnpInitialEventQueueManager::NewLC( 
                                           CUpnpServiceImplementation& aServiceImpl,
                                           CUpnpEventController& aEventController,
                                           CUpnpSubscriberLibrary* aSubscriberLibrary 
                                                                    )
    {
    CUpnpInitialEventQueueManager* self = 
        new (ELeave) CUpnpInitialEventQueueManager( aServiceImpl, 
                                                    aEventController, 
                                                    aSubscriberLibrary );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// -----------------------------------------------------------------------------
// CUpnpInitialEventQueueManager::NewL
// -----------------------------------------------------------------------------
//    
CUpnpInitialEventQueueManager* CUpnpInitialEventQueueManager::NewL( 
                                          CUpnpServiceImplementation& aServiceImpl,
                                          CUpnpEventController& aEventController,
                                          CUpnpSubscriberLibrary* aSubscriberLibrary 
                                                                   )
    {
    CUpnpInitialEventQueueManager* self= CUpnpInitialEventQueueManager::NewLC( 
                                                            aServiceImpl, 
                                                            aEventController, 
                                                            aSubscriberLibrary 
                                                                              );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpInitialEventQueueManager::ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpInitialEventQueueManager::ConstructL()
    {
    BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CUpnpInitialEventQueueManager::SendEvent
// -----------------------------------------------------------------------------
//
void CUpnpInitialEventQueueManager::SendEvent()
    {
    TRAP_IGNORE( SendEventL() );
    }

// -----------------------------------------------------------------------------
// CUpnpInitialEventQueueManager::SendEventL
// -----------------------------------------------------------------------------
//   
void CUpnpInitialEventQueueManager::SendEventL()
    {
    // Initial event is sent to subscriber which is last on the list
    // it has been just added into it
    //
    TInt lastSubscriberIndex( iSubscriberLibrary->SubscriberLibrary().Count() - 1 );
    
    if ( iBody )
        {
        delete iBody;
        iBody =NULL;
        }
         
    iBody = CreateEventL( iServiceImpl.StateVariableList() );
    
    PrepareTransactionAndStartSendingL( lastSubscriberIndex );   
    }

// -----------------------------------------------------------------------------
// CUpnpInitialEventQueueManager::CreateTransactionL
// -----------------------------------------------------------------------------
//
CUpnpHttpTransaction* CUpnpInitialEventQueueManager::CreateTransactionL( CUpnpHttpMessage* aMessage )
    {
    return CUpnpHttpInitialEventTransaction::NewL( aMessage );
    }

// -----------------------------------------------------------------------------
// CUpnpInitialEventQueueManager::FillEventBody
// -----------------------------------------------------------------------------
//
TInt CUpnpInitialEventQueueManager::FillEventBodyL( TDes8& aEvent, 
                                              const RPointerArray<CUpnpStateVariable>& aVariableList 
                                                   )
    {
    TInt eventableVariableCount(0);        
    for ( TInt i = 0; i < aVariableList.Count(); i++ )
        {
        if (   aVariableList[i]->Eventable().CompareF( KYes() ) == 0 
            && aVariableList[i]->ReadyForEventing() )
            {                            
            AddVariableToEventBody( aEvent , aVariableList[i] );
        
            eventableVariableCount++;
            }  
        }
    return eventableVariableCount;
    }
//end if file
