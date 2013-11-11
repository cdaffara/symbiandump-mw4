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
* Description:  Receiver of UDP datagrams
*
*/


#include <in_sock.h>

#include "ikedatainterface.h"
#include "ikemsgheader.h"

// CLASS HEADER
#include "ikev2receiver.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIkev2Receiver* CIkev2Receiver::NewL( MIkeDataInterface& aDataInterface,
                                      MIkev2ReceiverCallback& aCallback )
    {
    CIkev2Receiver* self = new (ELeave) CIkev2Receiver( aDataInterface,
                                                        aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CIkev2Receiver::~CIkev2Receiver()
    {
    StopReceive();
    
    delete iUdpData;    
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CIkev2Receiver::CIkev2Receiver( MIkeDataInterface& aDataInterface,
                                MIkev2ReceiverCallback& aCallback )
 : CActive( EPriorityStandard ),
   iUdpData( NULL ),
   iDataInterface( aDataInterface ),
   iCallback( aCallback )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CIkev2Receiver::ConstructL()
    {
    StartReceive();
    }

// ---------------------------------------------------------------------------
// Starts receive.
// ---------------------------------------------------------------------------
//
void CIkev2Receiver::StartReceive()
    {
    iReceivingData = ETrue;
    DoReceive();
    }

// ---------------------------------------------------------------------------
// Stops receive.
// ---------------------------------------------------------------------------
//
void CIkev2Receiver::StopReceive()
    {
    iReceivingData = EFalse;
    Cancel();
    iDataInterface.StopReceive();
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles completion of receive. 
// ---------------------------------------------------------------------------
//
void CIkev2Receiver::RunL()
    {
    if ( iStatus.Int() == KErrNone )
        {
        __ASSERT_DEBUG( iUdpData != NULL,
                        User::Invariant() );
        
        const ThdrISAKMP* ikeHdr = ThdrISAKMP::Ptr( iUdpData->Des() );
        TInt msgLth = iUdpData->Length();
        
        // Ignore possible <non-ESP marker> in the beginning of IKE message.
        TUint32 ikeMsgHdrOctets = GET32( ikeHdr );
        if ( ikeMsgHdrOctets == NON_ESP_MARKER )
            {
            ikeHdr  = ikeHdr->GotoOffset( NON_ESP_MARKER_SIZE );
            msgLth -= NON_ESP_MARKER_SIZE;
            }
        
        iCallback.IkeMsgReceived( *ikeHdr, iSrcAddr, iLocalPort );                
        }
    
    delete iUdpData;
    iUdpData = NULL;
    
    if ( iStatus.Int() == KErrNone )
        {
        if ( iReceivingData )
            {
            // Continue receiving.
            DoReceive();
            }
        }
    else
        {
        iCallback.ReceiveError( iStatus.Int() );
        }
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles cancellation of receive. 
// ---------------------------------------------------------------------------
//
void CIkev2Receiver::DoCancel()
    {
    iDataInterface.CancelReceive();
    
    delete iUdpData;
    iUdpData = NULL;
    }
    
// ---------------------------------------------------------------------------
// Receives UDP data. 
// ---------------------------------------------------------------------------
//
void CIkev2Receiver::DoReceive()
    {
    iDataInterface.ReceiveUdpData( iUdpData, iSrcAddr, iLocalPort, iStatus );
    SetActive();
    }

