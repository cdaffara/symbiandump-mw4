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
* Description:  Sender of UDP datagrams
*
*/


#include <in_sock.h>

#include "ikedatainterface.h"

// CLASS HEADER
#include "ikev2sender.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIkev2Sender* CIkev2Sender::NewL( MIkeDataInterface& aDataInterface,
                                  MIkev2SenderCallback& aCallback )
    {
    CIkev2Sender* self = new (ELeave) CIkev2Sender( aDataInterface,
                                                    aCallback );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CIkev2Sender::~CIkev2Sender()
    {    
    Cancel();
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CIkev2Sender::CIkev2Sender( MIkeDataInterface& aDataInterface,
                            MIkev2SenderCallback& aCallback )
 : CActive( EPriorityStandard ),
   iDataInterface( aDataInterface ),
   iCallback( aCallback )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Sends IKE message.
// ---------------------------------------------------------------------------
//
void CIkev2Sender::SendIkeMsg( TInt aLocalPort,
                               TInetAddr& aDestAddr,
                               TUint8 aDscp,
                               const TDesC8& aIkeMsg )
    {
    Cancel();    
    iDataInterface.SendUdpData( aLocalPort,
                                aDestAddr,
                                aIkeMsg,
                                aDscp,
                                iStatus );        
    SetActive();
    }


// ---------------------------------------------------------------------------
// From class CActive
// Handles completion of sending. 
// ---------------------------------------------------------------------------
//
void CIkev2Sender::RunL()
    {
    iCallback.SendIkeMsgCompleted( iStatus.Int() );
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles cancellation of sending. 
// ---------------------------------------------------------------------------
//
void CIkev2Sender::DoCancel()
    {
    iDataInterface.CancelSend();
    }
    
