/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Sender of UDP datagrams
*
*/


#include <in_sock.h>

#include "ikedatainterface.h"
#include "ikedebug.h"
#include "ikesocketdefs.h"
#include "ikemsgheader.h"

// CLASS HEADER
#include "ikev1sender.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIkev1Sender* CIkev1Sender::NewL( MIkeDataInterface& aDataInterface,
                                  MIkev1SenderCallback& aCallback,
                                  MIkeDebug& aDebug )
    {
    CIkev1Sender* self = new (ELeave) CIkev1Sender( aDataInterface,
                                                    aCallback,
                                                    aDebug );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CIkev1Sender::~CIkev1Sender()
    {
    HBufC8* udpData = iUdpData;
    iUdpData = NULL;
    
    // Sending is not completed via callback interface.
    Cancel();
    
    delete udpData;
    udpData = NULL;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CIkev1Sender::CIkev1Sender( MIkeDataInterface& aDataInterface,
                            MIkev1SenderCallback& aCallback,
                            MIkeDebug& aDebug )
 : CActive( EPriorityStandard ),
   iUdpData( NULL ),
   iDataInterface( aDataInterface ),
   iCallback( aCallback ),
   iDebug( aDebug )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Sends IKE message.
// ---------------------------------------------------------------------------
//
void CIkev1Sender::SendUdpData( HBufC8* aUdpData,                               
                                const TInetAddr& aDestAddr,
                                TInt aLocalPort,
                                TUint8 aDscp )
    {
    __ASSERT_DEBUG( iUdpData == NULL,
                    User::Invariant() );
    
    Cancel();
    
    iUdpData = aUdpData;
    
    // Send IKE message.
    DEBUG_LOG( _L("CIkev1Sender::SendUdpData, sending..."));
    iDataInterface.SendUdpData( aLocalPort,
                                aDestAddr,
                                *aUdpData,
                                aDscp,
                                iStatus );        
    SetActive();                
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles completion of sending. 
// ---------------------------------------------------------------------------
//
void CIkev1Sender::RunL()
    {
    DEBUG_LOG1( _L("CIkev1Sender::RunL, status=%d"),
            iStatus.Int() );

    if ( iUdpData != NULL )
        {
        delete iUdpData;
        iUdpData = NULL;
        
        iCallback.SendUdpDataCompleted( iStatus.Int() );
        }
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles cancellation of sending. 
// ---------------------------------------------------------------------------
//
void CIkev1Sender::DoCancel()
    {
    DEBUG_LOG( _L("CIkev1Sender::DoCancel"));

    iDataInterface.CancelSend();
    if ( iUdpData != NULL )
        {
        delete iUdpData;
        iUdpData = NULL;
        }
    }
    
