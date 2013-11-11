/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Sender for UDP data
*
*/


#include "sender.h"
#include "ikesocketdefs.h"
#include "ikedebug.h"

using namespace IkeSocket;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CSender* CSender::NewL( RSocket& aSocket,
                        RSocket& aSocketNAT,
                        RSocket& aSocketNokiaNAT,
                        MSenderCallback& aCallback,                                     
                        MIkeDebug& aDebug  )
    {
    CSender* self = new (ELeave) CSender( aSocket,
                                          aSocketNAT,
                                          aSocketNokiaNAT,
                                          aCallback,
                                          aDebug );  
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);    
    return self;            
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CSender::~CSender()
    {
    DEBUG_LOG( _L("CSender::~CSender") );
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CSender::CSender( RSocket& aSocket,
                  RSocket& aSocketNAT,
                  RSocket& aSocketNokiaNAT,
                  MSenderCallback& aCallback,
                  MIkeDebug& aDebug )
 : CActive( EPriorityStandard ),
   iSocket( aSocket ),
   iSocketNAT( aSocketNAT ),
   iSocketNokiaNAT( aSocketNokiaNAT ),
   iCallback( aCallback ),
   iDebug( aDebug )
    {
    CActiveScheduler::Add( this ); // Added to the Active Scheduler
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CSender::ConstructL()
    {
    DEBUG_LOG( _L("CSender::ConstructL") );
    }

// ---------------------------------------------------------------------------
// Sends UDP data.
// ---------------------------------------------------------------------------
//
TInt CSender::SendUdpData( const TInt aLocalPort,
                           const TInetAddr& aDestAddr,
                           const TDesC8& aUdpData,
                           const TUint aDscp )
    {
    RSocket* socket = NULL;
    iLocalPort = aLocalPort;
    iDestAddr = aDestAddr;
    
    if ( iLocalPort == 500 )
        {
        socket = &iSocket;
        }
    else if ( iLocalPort == 4500 )
        {
        socket = &iSocketNAT;
        }
    else 
        {        
        socket = &iSocketNokiaNAT;
        }
    
    TInt err = socket->SetOpt( KSoIpTOS, KSolInetIp, aDscp );
    
    if ( err == KErrNone )
        {
        err = socket->SetOpt( KSoUdpSynchronousSend, KSolInetUdp, 1 );
        }
    
    if ( err == KErrNone )
        {
        socket->SendTo( aUdpData, iDestAddr, 0, iStatus );    
        SetActive();
        }

#ifdef _DEBUG    
    TBuf<100> txt_addr;
    iDestAddr.Output( txt_addr );
    TUint32 port = iDestAddr.Port();
    DEBUG_LOG3( _L("Sending UDP data, local port=%d, dest address:port=%S:%d"),
            iLocalPort, &txt_addr, port );
    DEBUG_LOG2( _L(" DSCP=%d, err=%d"), aDscp, err );
#endif
    
    return err;
    }

// ---------------------------------------------------------------------------
// From CActive
// Handles request completion event about sending.
// ---------------------------------------------------------------------------
//
void CSender::RunL()
    {
    DEBUG_LOG1( _L("CSender::RunL, iStatus=%d"),
            iStatus.Int() );
    
    iCallback.SendCompleted( iStatus.Int() );
    }

// ---------------------------------------------------------------------------
// From CActive
// Implements cancellation of sending.
// ---------------------------------------------------------------------------
//
void CSender::DoCancel()
    {
    DEBUG_LOG1( _L("CSender::DoCancel, iLocalPort=%d"),
            iLocalPort );
    
    if ( iLocalPort == KIkePort500 )
        {
        iSocket.CancelSend();
        }
    else if ( iLocalPort == KIkePort4500 )
        {
        iSocketNAT.CancelSend();
        }
    else
        {
        iSocketNokiaNAT.CancelSend();
        }
    }
