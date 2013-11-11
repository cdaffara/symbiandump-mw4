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
* Description:  Base class for event queue manager
*
*/


#include "upnpeventqueuemanagerbase.h"
#include "upnpgenamessagefactory.h"
#include "upnpsubscriberlibraryelement.h"
#include "upnpcommonupnplits.h"
#include "upnpcommonstructs.h"


#define KLogFile _L("UPnPStack.txt")
#include "upnpcustomlog.h"


// -----------------------------------------------------------------------------
// CUpnpEventQueueManagerBase::CUpnpEventQueueManagerBase
// -----------------------------------------------------------------------------
//
CUpnpEventQueueManagerBase::CUpnpEventQueueManagerBase( 
                                CUpnpServiceImplementation& aServiceImpl,
                                CUpnpEventController& aEventController,
                                CUpnpSubscriberLibrary* aSubscriberLibrary )
    :iServiceImpl( aServiceImpl ),
     iEventController( aEventController ),
     iSubscriberLibrary( aSubscriberLibrary )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CUpnpEventQueueManagerBase::~CUpnpEventQueueManagerBase
// -----------------------------------------------------------------------------
//
CUpnpEventQueueManagerBase::~CUpnpEventQueueManagerBase ()
    {
    delete iBody;
    }

// -----------------------------------------------------------------------------
// CUpnpEventQueueManagerBase::BaseConstructL
// -----------------------------------------------------------------------------
//
void CUpnpEventQueueManagerBase::BaseConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpEventQueueManagerBase::SendingCompleted
// -----------------------------------------------------------------------------
//    
void CUpnpEventQueueManagerBase::SendingCompleted()
    {
    iCurrentSubscriberIndex = 0;
    delete iBody;
    iBody = NULL;
    iIsSending = EFalse;
    if ( iNewValuesWaitingToBeSend )
        {
        iNewValuesWaitingToBeSend = EFalse;
        SendEvent();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpEventQueueManagerBase::SendToNextL
// -----------------------------------------------------------------------------
//    
void CUpnpEventQueueManagerBase::SendToNextL()
    {    
    PrepareTransactionAndStartSendingL( iCurrentSubscriberIndex );
    }
 
// -----------------------------------------------------------------------------
// CUpnpEventQueueManagerBase::PrepareTransactionAndStartSendingL
// -----------------------------------------------------------------------------
// 
void CUpnpEventQueueManagerBase::PrepareTransactionAndStartSendingL( TInt aSubscriberNo )
    {
    if (  aSubscriberNo >= iSubscriberLibrary->SubscriberLibrary().Count()
       || aSubscriberNo < 0 )
        {
        User::Leave( KErrNotFound );
        }
        
    CUpnpSubscriberLibraryElement* subscriber( NULL );
    subscriber = iSubscriberLibrary->SubscriberLibrary()[aSubscriberNo];
    
    CUpnpHttpMessage* message = reinterpret_cast<CUpnpHttpMessage*>(
                        RUpnpGenaMessageFactory::EventingLC(
                          subscriber->CallbackPath(), 
                          subscriber->CallbackAddress(), 
                          subscriber->Sid(), 
                          subscriber->Seq() ) );

    message->SetTcpTimeout( KMessageTimeout );
    message->SetBodyL( *iBody );        

    CleanupStack::Pop( message ); //ownership passed to "transcation"
    CUpnpHttpTransaction* transaction = CreateTransactionL( message );
    
    iEventController.SendTransactionL( transaction );
       
    subscriber->IncreaseSeq();  
    }

// -----------------------------------------------------------------------------
// CUpnpEventQueueManagerBase::TransactionCompletedL
// -----------------------------------------------------------------------------
//
void CUpnpEventQueueManagerBase::TransactionCompletedL( 
                                  CUpnpHttpTransaction* aCompletedTransaction  
                                                      )
    {
    ASSERT( iIsSending );
    if ( aCompletedTransaction->Response() && 
        !aCompletedTransaction->Response()->Is2xx() )
        {
        if ( aCompletedTransaction->Response()->Is3xx() )
            {
            //300 Responses are redirected by Symbian HTTP Client
            //This can be changed by removing HTTPFilter from HTTPTransaction
            }
        else
            {
            RemoveInvalidSubscriber( aCompletedTransaction );
            return;
            }
        }

    iCurrentSubscriberIndex++;
    if ( iCurrentSubscriberIndex >= iSubscriberLibrary->SubscriberLibrary().Count() )
        {        
        SendingCompleted();
        }
    else 
        {           
        SendToNextL();   
        }
    } 
        
// -----------------------------------------------------------------------------
// CUpnpEventQueueManagerBase::RemoveInvalidSubscriber
// -----------------------------------------------------------------------------
//
void CUpnpEventQueueManagerBase::RemoveInvalidSubscriber( 
                                    CUpnpHttpTransaction* aCompletedTransaction )
    {
    iSubscriberLibrary->Remove( static_cast<CUpnpGenaMessage*>( 
                                     aCompletedTransaction->Request() )->Sid() );
    }

// -----------------------------------------------------------------------------
// CUpnpEventQueueManagerBase::UpdateQueue
// -----------------------------------------------------------------------------
//
void CUpnpEventQueueManagerBase::UpdateQueue( TInt aPos )
    {
    if( iIsSending )
        {
        if ( aPos == iCurrentSubscriberIndex  )
            {
            TRAPD( err, SendToNextL() );
            if ( err )
                {
                SendingCompleted();
                }
            return;
            }   
        
        if ( aPos < iCurrentSubscriberIndex )
            {
            iCurrentSubscriberIndex--;
            }           
        }
    }
    
// -----------------------------------------------------------------------------
// CUpnpEventQueueManagerBase::CreateEventL
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpEventQueueManagerBase::CreateEventL
                        ( const RPointerArray<CUpnpStateVariable>& aVariableList )
    {      
    HBufC8* body = HBufC8::NewLC( KMaxGenaMessageLength );        
    TPtr8 bodyPtr = body->Des(); 
    bodyPtr.Zero();
    bodyPtr.Append( KGenaXml );
    bodyPtr.Append( KGenaPropertysetStart );
    
    TInt eventableVariableCount = FillEventBodyL( bodyPtr, aVariableList ); 
    if ( eventableVariableCount > 0 )
        {        
        bodyPtr.Append( KGenaPropertysetEnd );
        CleanupStack::Pop( body );
        return body;
        }
    else
        {
        CleanupStack::PopAndDestroy( body );
        return KNullDesC8().AllocL();             
        }
    }
   
// -----------------------------------------------------------------------------
// CUpnpEventQueueManagerBase::ClearVariables
// -----------------------------------------------------------------------------
//
void CUpnpEventQueueManagerBase::ClearVariables()
    {   
    for ( TInt i = 0; i < iServiceImpl.EventedStateVariables().Count(); i++ )
        {
        if ( iServiceImpl.EventedStateVariables()[i]->MaxEventRate() > 0 && 
            !iServiceImpl.EventedStateVariables()[i]->ReadyForEventing() )
            {
            }
        else
            {
            iServiceImpl.EventedStateVariables().Remove( i );
            i--;
            }
        }
    iServiceImpl.EventedStateVariables().Compress();   
    }

// -----------------------------------------------------------------------------
// CUpnpEventQueueManagerBase::ClearVariables
// -----------------------------------------------------------------------------
//
void CUpnpEventQueueManagerBase::AddVariableToEventBody( 
                                            TDes8& aBuffer , 
                                            CUpnpStateVariable * aVariable )
    {
    aBuffer.Append( KGenaPropertyStart );
    aBuffer.Append( aVariable->Name() );
    aBuffer.Append( KCloseBracket );
    aBuffer.Append( aVariable->Value() );
    aBuffer.Append( KOpenBracket );
    aBuffer.Append( UpnpString::KSlash() );
    aBuffer.Append( aVariable->Name() );
    aBuffer.Append( KGenaPropertyEnd );
    }
//end of file
