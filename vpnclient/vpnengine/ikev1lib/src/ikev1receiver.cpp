/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "ikev1receiver.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIkev1Receiver* CIkev1Receiver::NewL( MIkeDataInterface& aDataInterface,
                                      MIkev1ReceiverCallback& aCallback )
    {
    CIkev1Receiver* self = new (ELeave) CIkev1Receiver( aDataInterface,
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
CIkev1Receiver::~CIkev1Receiver()
    {
    Cancel();
    
    delete iUdpData;    
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CIkev1Receiver::CIkev1Receiver( MIkeDataInterface& aDataInterface,
                                MIkev1ReceiverCallback& aCallback )
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
void CIkev1Receiver::ConstructL()
    {
    StartReceive();
    }

// ---------------------------------------------------------------------------
// Starts receive.
// ---------------------------------------------------------------------------
//
void CIkev1Receiver::StartReceive()
    {
    iReceiving = ETrue;
    DoReceive();
    }

// ---------------------------------------------------------------------------
// Cancels receive.
// ---------------------------------------------------------------------------
//
void CIkev1Receiver::CancelReceive()
    {
    iReceiving = EFalse;
    Cancel();
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles completion of receive. 
// ---------------------------------------------------------------------------
//
void CIkev1Receiver::RunL()
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
        
        iCallback.IkeMsgReceivedL( *ikeHdr, iSrcAddr, iLocalPort );                
        }
    else
        {
        iReceiving = EFalse;
        iCallback.ReceiveError( iStatus.Int() );
        }
    
    delete iUdpData;
    iUdpData = NULL;
    
    if ( iReceiving &&
         iStatus.Int() == KErrNone )
        {
        // Continue receiving.
        DoReceive();
        }
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles cancellation of receive. 
// ---------------------------------------------------------------------------
//
void CIkev1Receiver::DoCancel()
    {
    iDataInterface.CancelReceive();
    
    delete iUdpData;
    iUdpData = NULL;
    }

// ---------------------------------------------------------------------------
// Handles a leave occurring in RunL().
// Handles cancellation of receive. 
// ---------------------------------------------------------------------------
//
TInt CIkev1Receiver::RunError( TInt aError )
    {
    iReceiving = EFalse;
    
    delete iUdpData;
    iUdpData = NULL;
    
    iCallback.ReceiveError( aError );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Receives UDP data. 
// ---------------------------------------------------------------------------
//
void CIkev1Receiver::DoReceive()
    {
    iDataInterface.ReceiveUdpData( iUdpData, iSrcAddr, iLocalPort, iStatus );
    SetActive();
    }


