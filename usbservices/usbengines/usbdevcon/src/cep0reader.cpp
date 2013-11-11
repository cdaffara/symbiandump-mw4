/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  EP0 Reader 
*
*/


#include "cep0reader.h"
#include "cstatemachine.h"
#include "cusbdevcon.h"
#include "debug.h"

// ---------------------------------------------------------------------------
// Two-phase construction
// ---------------------------------------------------------------------------
//
CEP0Reader* CEP0Reader::NewL(CStateMachine& aObserver, RDevUsbcClient& aLdd)
    {
    
    FLOG( _L( "[USBDEVCON]\tCEP0Reader::NewL" ) );
    
    CEP0Reader* self = new (ELeave) CEP0Reader(aObserver, aLdd);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// ---------------------------------------------------------------------------
// Default construction
// ---------------------------------------------------------------------------
//
CEP0Reader::CEP0Reader(CStateMachine& aObserver, RDevUsbcClient& aLdd) :
                                                            CActive(EPriorityMore),
                                                            iLdd(aLdd),
                                                            iObserver(aObserver)
                                                            
    {
    CActiveScheduler::Add(this);
    }
        
// ---------------------------------------------------------------------------
// Two-phase construction
// ---------------------------------------------------------------------------
//
void CEP0Reader::ConstructL()
    {
    
    FLOG( _L( "[USBDEVCON]\tCEP0Reader::ConstructL" ) );
    
    iBuffer.CreateL(0); // later will be reallocated with required size
    }
    
// ---------------------------------------------------------------------------
// Destruction
// ---------------------------------------------------------------------------
//
CEP0Reader::~CEP0Reader()
    {
    Cancel();
           
    iBuffer.Close();
    }   
    
// ---------------------------------------------------------------------------
// Cancellation of outstanding request
// ---------------------------------------------------------------------------
//
void CEP0Reader::DoCancel()
    {
    iLdd.ReadCancel(EEndpoint0);
    }

// ----------------------------------------------------------------------------
// Standard active object error function.
// ----------------------------------------------------------------------------
//
TInt CEP0Reader::RunError( TInt aError )
    {
    FTRACE(FPrint(
           _L("[USBDEVCON]\tCEP0Reader::RunError, aError = %d"), aError));
    return KErrNone;
    }
        
// ---------------------------------------------------------------------------
// Data has been read from EP0
// ---------------------------------------------------------------------------
//
void CEP0Reader::RunL()
    {

    FLOG( _L( "[USBDEVCON]\tCEP0Reader::RunL Data received:" ) );
    
    iObserver.ReadEP0L(iBuffer, iStatus);
    }

// ---------------------------------------------------------------------------
// Issue request to read setup packet
// ---------------------------------------------------------------------------
//
void CEP0Reader::ReadSetupPacketL()
    {
    
    FLOG( _L( "[USBDEVCON]\tCEP0Reader::ReadSetupPacket" ) );

    if(IsActive())
        {
        return;
        }
    
    iBuffer.Close();
    iBuffer.CreateL(KSetupPacketLength);
    
    iLdd.ReadPacket(iStatus, EEndpoint0, iBuffer, KSetupPacketLength);
    SetActive();
    
    }

// ---------------------------------------------------------------------------
// Issue request to read data
// ---------------------------------------------------------------------------
//  
void CEP0Reader::ReadL(TUint aDataLength)
    {
    
    FLOG( _L( "[USBDEVCON]\tCEP0Reader::Read" ) );

    if(IsActive())
        {
        return;
        }
    
    iBuffer.Close();
    iBuffer.CreateL(aDataLength);    
        
    iLdd.Read(iStatus, EEndpoint0, iBuffer, aDataLength);
    SetActive();
    
    }

