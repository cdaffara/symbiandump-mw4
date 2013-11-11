/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*
*/



#include "upnptcpsession.h"
#include "upnptcpsessionreader.h"
#include "upnptcpsessionwriter.h"
#include "upnphttpfiletransferwriter.h"
#include "upnphttpfiletransferreader.h"
#include "upnpsocketshutdown.h"
#include "upnpnotifytimer.h"



// -----------------------------------------------------------------------------
// CUpnpSocketShutdown::NewL
//
// -----------------------------------------------------------------------------
//
CUpnpSocketShutdown* CUpnpSocketShutdown::NewL( CUpnpTcpSession& aSession, RSocket& aSocket, TThreadPriority aPriority )
    {
    CUpnpSocketShutdown* self = new ( ELeave ) CUpnpSocketShutdown( aSession, aSocket, aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpSocketShutdown::CUpnpSocketShutdown
//
// -----------------------------------------------------------------------------
//
CUpnpSocketShutdown::CUpnpSocketShutdown( CUpnpTcpSession& aSession, RSocket& aSocket, TThreadPriority aPriority ):CActive( aPriority ),iSession( aSession ),iSocket( aSocket ),iInternalState(EUnknown), iSessionTimeouted( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpSocketShutdown::ConstructL
//
// -----------------------------------------------------------------------------
//
void CUpnpSocketShutdown::ConstructL()
    {
    CActiveScheduler::Add( this );
    iInputTimeoutTimer = CUpnpNotifyTimer::NewL( this );
    }

// -----------------------------------------------------------------------------
// CUpnpSocketShutdown::~CUpnpSocketShutdown
//
// -----------------------------------------------------------------------------
//
CUpnpSocketShutdown::~CUpnpSocketShutdown()
    {
    Cancel();
    delete iInputTimeoutTimer;
    }

// -----------------------------------------------------------------------------
// CUpnpSocketShutdown::ShutdownImmediate
// Disconnect connection
// -----------------------------------------------------------------------------
//
void CUpnpSocketShutdown::ShutdownImmediate()
    {
    iSession.iTcpReader->Cancel();       
    iSession.iTcpWriter->Cancel();      
    if( iSession.iFTReader )
        {
        iSession.iFTReader->Cancel();
        }
    if( iSession.iFTWriter )
        {
        iSession.iFTWriter->Cancel();
        }
    iSocket.Shutdown( iSocket.EImmediate, iStatus );
    iInternalState = ENotConnected;                
    SetActive();    
    }

// -----------------------------------------------------------------------------
// CUpnpSocketShutdown::ShutdownStop
// Disconnect connection
// -----------------------------------------------------------------------------
//
void CUpnpSocketShutdown::ShutdownStop()
    {
    if ( iInternalState == EUnknown )
        {
        iSession.iTcpReader->CancelTimers();
        if( iSession.iFTReader )
            {
            iSession.iFTReader->CancelTimers();
            }
        
        if( iSession.iTcpReader->IsActive() 
            || ( iSession.iFTReader && iSession.iFTReader->IsActive() ) 
          )
            {
            iSocket.Shutdown( iSocket.EStopOutput, iStatus );            
            iInternalState = EShuttingDownStopOutput ;               
            }
        else
            {
            iSocket.Shutdown( iSocket.ENormal, iStatus );
            iInternalState = EShuttingDownNormal;
            }
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSocketShutdown::MarksTimeout
// Marks if timeout occured on session
// -----------------------------------------------------------------------------
//
void CUpnpSocketShutdown::MarksTimeout()
    {
    iSessionTimeouted = ETrue;
    }

// -----------------------------------------------------------------------------
// CUpnpSocketShutdown::HandleError
// Disconnect connection
// -----------------------------------------------------------------------------
//
void CUpnpSocketShutdown::HandleError( TInt /*aError*/ )
    {
    switch ( iInternalState )
        {
        case EUnknown: // error happened during reading/writing - start shutdown procedure
            ShutdownStop();
            break;
        case EShuttingDownStopOutput: // input error appeares while shutdown stop output is pending
                                      // when it returns shutdown immediate is invoked in RunL  
            iInternalState = EInputErrorReceived;
            break;
        case EWaitingForInputError: // error response from reader after shutdown stop output                  
            ShutdownImmediate();
            break;
        case EShuttingDownNormal: // do nothimg session is being closed
        case ENotConnected: // do nothing session is closed
        default:
            break;        
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSocketShutdown::HandleError
// Disconnect connection
// -----------------------------------------------------------------------------
//
void CUpnpSocketShutdown::RunL()
    {
    switch( iInternalState )
        {
        case EShuttingDownNormal: // shutdown normal response came
            ShutdownImmediate();
            break;
        case EShuttingDownStopOutput: // shutdown stop output response came
                                // waiting for KErrEof from HandleError()                                
            if( !iSessionTimeouted )
                {
                iInternalState = EWaitingForInputError;                                
                iInputTimeoutTimer->Cancel();
                iInputTimeoutTimer->After(KSessionTimeout, EFalse);     
                break;                
                }
        case EInputErrorReceived: // input error came before stop output finished
            ShutdownImmediate();
            break;
        case ENotConnected:  // shutdown immediate response came          
            iSession.CloseSocketL();           
            break;
        default:
            break;    
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSocketShutdown::TimerEventL
// Disconnect connection
// -----------------------------------------------------------------------------
//        
void CUpnpSocketShutdown::TimerEventL( CUpnpNotifyTimer* /*aTimer*/ )
    {
     switch( iInternalState )
        {
        case EWaitingForInputError: // waiting for input error timeout 
            ShutdownImmediate();
            break;            
        default: 
            break;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSocketShutdown::DoCancel
// Disconnect connection
// -----------------------------------------------------------------------------
//
void CUpnpSocketShutdown::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpSocketShutdown::HandleError
// Disconnect connection
// -----------------------------------------------------------------------------
//
TInt CUpnpSocketShutdown::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }
  
    
