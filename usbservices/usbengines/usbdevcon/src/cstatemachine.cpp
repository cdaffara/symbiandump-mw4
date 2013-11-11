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
* Description:  usbdevcon state machine
*
*/


#include "cusbdevcon.h"
#include "cstatemachine.h"
#include "cep0reader.h"
#include "cep0writer.h"
#include "crequestshandler.h"
#include "debug.h"

const TUint KOneByte = 8; // for shifting data to one byte

// binary constant for checking if bit 7 is set
const TUint KUsbDevConBit7 = 0x80;

const TUint KLengthLoByte = 6;
const TUint KLengthHiByte = 7;
        
// ---------------------------------------------------------------------------
// Two-phase construction
// ---------------------------------------------------------------------------
//
CStateMachine* CStateMachine::NewL(CRequestsHandler& aRequestsHandler, RDevUsbcClient& iLdd)
    {
    FLOG( _L( "[USBDEVCON]\tCStateMachine::NewL" ) );
    
    CStateMachine* self = new (ELeave) CStateMachine(aRequestsHandler, iLdd);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }
    
// ---------------------------------------------------------------------------
// Two-phase constructon
// ---------------------------------------------------------------------------
//
void CStateMachine::ConstructL()
    {
    
    FLOG( _L( "[USBDEVCON]\tCStateMachine::ConstructL" ) );
    
    iBuffer.CreateL(0); // later will be reallocated with required size
    
    // reader
    iEP0Reader = CEP0Reader::NewL(*this,iLdd);
    
    // writer
    iEP0Writer = CEP0Writer::NewL(*this, iLdd);
    
    }
    
// ---------------------------------------------------------------------------
// Default construction
// ---------------------------------------------------------------------------
//
CStateMachine::CStateMachine(CRequestsHandler& aRequestsHandler, RDevUsbcClient& aLdd) : 
                            iRequestsHandler(aRequestsHandler),
                            iLdd (aLdd),
                            iState(ENoneState)
    {
    }

// ---------------------------------------------------------------------------
// Destruction
// ---------------------------------------------------------------------------
//
CStateMachine::~CStateMachine()
    {
    delete iEP0Writer;
    delete iEP0Reader;
    iBuffer.Close();    
    }   

// ---------------------------------------------------------------------------
// Starts state machine
// ---------------------------------------------------------------------------
//
void CStateMachine::StartL()
    {
    
    FLOG( _L( "[USBDEVCON]\tCStateMachine::Start" ) );
    
    // resetting, if it's been already started
    if(IsStarted())
        {
        Stop();
        }
    
    iState = ESetupStage;   
    iEP0Reader->ReadSetupPacketL();
                
    }
    
// ---------------------------------------------------------------------------
// Stops machine
// ---------------------------------------------------------------------------
//
void CStateMachine::Stop()
    {
    
    FLOG( _L( "[USBDEVCON]\tCStateMachine::Stop" ) );
        
    iEP0Reader->Cancel();
    iEP0Writer->Cancel();
    
    iState = ENoneState;
    
    }
    
// ---------------------------------------------------------------------------
// ETrue if state machine is started
// ---------------------------------------------------------------------------
//
TInt CStateMachine::IsStarted() const
    {
    
    if(ENoneState == iState)
        {
        FLOG( _L( "[USBDEVCON]\tCStateMachine::IsStarted == EFalse" ) );
        return EFalse;
        }
        else
        {
        FLOG( _L( "[USBDEVCON]\tCStateMachine::IsStarted == ETrue" ) );
        }
    return ETrue;
    }
    
// ---------------------------------------------------------------------------
// Something has been read from EP0 
// ---------------------------------------------------------------------------
//
void CStateMachine::ReadEP0L(RBuf8& aBuffer, const TRequestStatus& aStatus)
    {
    
    FTRACE(FPrint(
            _L("[USBDEVCON]\tCStateMachine::ReadEP0: BufferLength = %d, aStatus = %d" ),aBuffer.Length(), aStatus.Int()));
    
    // all errors while reading data lead to idle state (ESetupStage in our case)
    if(KErrNone != aStatus.Int())
        {
        // restart
        StartL();
        return;
        }
    
    TInt err(KErrNone);
    
    switch(iState)
        {
        case ESetupStage: // setup transaction received
            {
            
            FLOG( _L( "[USBDEVCON]\tCStateMachine::ReadEP0 processing ESetupStage" ) );
            
            if(aBuffer.Length() != KSetupPacketLength) // SetupPacket is always 8 bytes
                {
                StartL();
                return;
                }
            
            ProcessSetupPacketL(aBuffer);
                                
            break;
            }
        case EDataStage:  // some data received from host. This data is required for the request, received on setup stage
            {
            
            FLOG( _L( "[USBDEVCON]\tCStateMachine::ReadEP0 processing EDataStage" ) );

            // handle request, providing data together with request descriptor
            // iBuffer == request, aBuffer == data from host
            err = iRequestsHandler.Handle(iBuffer, aBuffer);
            
            if(KErrNone != err) // some error happened while handling request
                {
                iLdd.EndpointZeroRequestError(); // stall EP0
                }
                else
                {
                // send Status Packet, indicating that we received request, data, and handled request. OK
                iLdd.SendEp0StatusPacket();
                }
            
            // all done, go to idle state
            iState = ESetupStage;
            iEP0Reader->ReadSetupPacketL();
            
            break;
            
            }
        default:
            {
            FLOG( _L( "[USBDEVCON]\tCStateMachine::ReadEP0 processing ***NOT_DEFINED state***" ) );
            }
            
        }
    
    }

// ---------------------------------------------------------------------------
// Processing setup packet 
// ---------------------------------------------------------------------------
//
void CStateMachine::ProcessSetupPacketL(RBuf8& aSetupPacket)
    {
    
    FLOG( _L( "[USBDEVCON]\tCStateMachine::ProcessSetupPacket" ) );
    
    TUint datalength(0); // data length, to be received from host
            
    if(IsDataFromHostRequired(aSetupPacket, datalength)) // then goes to data stage
        {
        
        FTRACE(FPrint(
            _L("[USBDEVCON]\tCStateMachine::ProcessSetupPacket. Data from host is required: %d bytes" ),datalength));

        // save request, until receiving following data
        iBuffer.Close();
        iBuffer.CreateL(aSetupPacket);
                
        // switch to Data state
        iState = EDataStage;
        iEP0Reader->ReadL(datalength);
    
        return;
        
        }
            
    TInt err(KErrNone); 
                        
    // Handle request. It does not require data from host   
    // aSetupPacket == request, iBuffer = result of the request
    err = iRequestsHandler.Handle(aSetupPacket, iBuffer);
        
    if(KErrNone != err) // some error happened while handling request
        {
        
        FTRACE(FPrint(
            _L("[USBDEVCON]\tCStateMachine::ProcessSetupPacket. Error while handling request, errcode: %d" ), err));

        iLdd.EndpointZeroRequestError(); // stall EP0
            
        // listen to EP0
        iState = ESetupStage;
        iEP0Reader->ReadSetupPacketL();
        
        return;
        }
            
    // send response, size of datalength
    if(IsDataFromDeviceRequired(aSetupPacket, datalength))
        {
        
        FTRACE(FPrint(
          _L("[USBDEVCON]\tCStateMachine::ProcessSetupPacket. Data from device is required: %d bytes" ),datalength));

        iState = EStatusStage;
        iEP0Writer->WriteL(iBuffer, datalength);
        
        return;
                
        }
    
    // status stage 
    iLdd.SendEp0StatusPacket();
                
    // all is done, listen to EP0, in setup stage
    iState = ESetupStage;
    iEP0Reader->ReadSetupPacketL();
    
    }
    

// ---------------------------------------------------------------------------
// Something has been written to EP0 
// ---------------------------------------------------------------------------
//
void CStateMachine::WroteEP0L(const TRequestStatus& aStatus)
    {
    
    FTRACE(FPrint(
            _L("[USBDEVCON]\tCStateMachine::WroteEP0: iStatus = %d" ), aStatus.Int()));

    // all errors while writing data lead to idle state (ESetupStage in our case)
    if(KErrNone != aStatus.Int())
        {
        // restart
        StartL();
        }
        
    switch(iState)
        {
        case EStatusStage:
            {
            
            FLOG( _L( "[USBDEVCON]\tCStateMachine::WroteEP0 EStatusStage -> ESetupStage" ) );
            
            // successfully wrote data to EP0
            // go to idle
            iState = ESetupStage;
            iEP0Reader->ReadSetupPacketL();
            
            break;
            }
        default:
            {
            FLOG( _L( "[USBDEVCON]\tCStateMachine::WroteEP0 ***ENoneState***" ) );
            }
        }
    }

// ---------------------------------------------------------------------------
// ETrue if data required to be send from host 
// ---------------------------------------------------------------------------
//
TBool CStateMachine::IsDataFromHostRequired(const RBuf8& aSetupPacket, TUint& aDataLength) const
    {
        // bits 6 and 7 of SetupPacket contain wLength - length of data in Data stage  
        aDataLength = static_cast<TUint16>(aSetupPacket[KLengthLoByte] |
                                            (aSetupPacket[KLengthHiByte] << KOneByte));
        if(0 == aDataLength)
            {
            // no data required in any direction
            return EFalse;
            }
            
        // direction of data
        // bit 7 of byte 0 of aSetupPacket (means bmRequestType one) contains:
        // 0, if no data or data goes from host to device
        // 1 means data goes from device to host
        if(KUsbDevConBit7 & aSetupPacket[0])
            { // bit 7 is set => data flow is from device to host
            return EFalse;
            }
            
        return ETrue;
        
    }

// ---------------------------------------------------------------------------
// ETrue if data required to be send to host
// ---------------------------------------------------------------------------
//
TBool CStateMachine::IsDataFromDeviceRequired(const RBuf8& aSetupPacket, TUint& aDataLength) const
    {
        // bytes 6 and 7 of SetupPacket contain wLength - length of data in Data stage
        aDataLength = static_cast<TUint16>(aSetupPacket[KLengthLoByte] |
                                            (aSetupPacket[KLengthHiByte] << KOneByte));
        if(0 == aDataLength)
            {
            // no data required in any direction
            return EFalse;
            }
            
        // direction of data
        // bit 7 of byte 0 of aSetupPacket (means bmRequestType one) contains:
        // 0, if no data or data goes from host to device
        // 1 means data goes from device to host
        if(KUsbDevConBit7 & aSetupPacket[0])
            { // bit 7 is set => data flow is from device to host
            return ETrue;
            }
            
        return EFalse;
    }


