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
* Description:  EP0 Writer 
*
*/


#include "cep0writer.h"
#include "cstatemachine.h"
#include "cusbdevcon.h"
#include "debug.h"

// ---------------------------------------------------------------------------
// Two-phase construction
// ---------------------------------------------------------------------------
//
CEP0Writer* CEP0Writer::NewL(CStateMachine& aObserver, RDevUsbcClient& aLdd)
    {
    
    FLOG( _L( "[USBDEVCON]\tCEP0Writer::NewL" ) );
    
    CEP0Writer* self = new (ELeave) CEP0Writer(aObserver, aLdd);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// ---------------------------------------------------------------------------
// Default construction
// ---------------------------------------------------------------------------
//
CEP0Writer::CEP0Writer(CStateMachine& aObserver, RDevUsbcClient& aLdd) :
                                                            CActive(EPriorityMore),
                                                            iObserver(aObserver),
                                                            iLdd(aLdd)
    {
    CActiveScheduler::Add(this);
    }
        
// ---------------------------------------------------------------------------
// Two-phase construction
// ---------------------------------------------------------------------------
//
void CEP0Writer::ConstructL()
    {
    
    FLOG( _L( "[USBDEVCON]\tCEP0Writer::ConstructL" ) );
    
    iBuffer.CreateL(0); // later will be set with needed size
    }
    
// ---------------------------------------------------------------------------
// Destruction
// ---------------------------------------------------------------------------
//
CEP0Writer::~CEP0Writer()
    {
    Cancel(); 
           
    iBuffer.Close();
        
    }   
    
// ---------------------------------------------------------------------------
// Cancellation
// ---------------------------------------------------------------------------
//
void CEP0Writer::DoCancel()
    {
    iLdd.WriteCancel(EEndpoint0);
    }

// ---------------------------------------------------------------------------
// Data has been wrote to EP0 
// ---------------------------------------------------------------------------
//  
void CEP0Writer::RunL()
    {

    FLOG( _L( "[USBDEVCON]\tCEP0Writer::RunL" ) );

    iObserver.WroteEP0L(iStatus);
    }
    
// ----------------------------------------------------------------------------
// Standard active object error function.
// ----------------------------------------------------------------------------
//
TInt CEP0Writer::RunError( TInt aError )
    {
    FTRACE(FPrint(
           _L("[USBDEVCON]\tCEP0Reader::RunError, aError = %d"), aError));
    return KErrNone;                    
    }

// ---------------------------------------------------------------------------
// Issue request to write data
// ---------------------------------------------------------------------------
//
void CEP0Writer::WriteL(const RBuf8& aBuffer, TUint aDataLength)
    {

    FLOG( _L( "[USBDEVCON]\tCEP0Writer::Write" ) );

    if(IsActive())
        {
        return;
        }
    
    FTRACE(FPrint(
            _L("[USBDEVCON]\tCEP0Writer::Write. aBuffer Length = %d aDataLength = %d" ),aBuffer.Length(), aDataLength));

    iBuffer.Close();
    iBuffer.CreateL(aBuffer, aDataLength);   
    
    iLdd.Write(iStatus, EEndpoint0, iBuffer, aDataLength, ETrue);
    SetActive();
    
    }


