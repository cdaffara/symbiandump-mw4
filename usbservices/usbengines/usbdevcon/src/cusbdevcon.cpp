/*
* Copyright (c) 2007 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Takes control of device&vendor-specific control messages over EP0
*
*/


#include <usbman.h>

#include "cusbdevcon.h"
#include "cusbstatewatcher.h"
#include "crequestshandler.h"
#include "cstatemachine.h"
#include "debug.h"

// LITERALS
_LIT( KUsbDevConName, "UsbDevCon" );

// ---------------------------------------------------------------------------
// Two-phase construction
// ---------------------------------------------------------------------------
//
CUsbDevCon* CUsbDevCon::NewLC()
    {
    FLOG( _L( "[USBDEVCON]\tCUsbDevCon::NewLC" ) );
    
    CUsbDevCon* self = new (ELeave) CUsbDevCon();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phase construction
// ---------------------------------------------------------------------------
//  
CUsbDevCon* CUsbDevCon::NewL()
    {
    FLOG( _L( "[USBDEVCON]\tCUsbDevCon::NewL" ) );
    
    CUsbDevCon* self = CUsbDevCon::NewLC();
    CleanupStack::Pop(self);
    return self;    
    }

// ---------------------------------------------------------------------------
// Two-phase construction
// ---------------------------------------------------------------------------
//  
void CUsbDevCon::ConstructL()
    {
    
    FLOG( _L( "[USBDEVCON]\tCUsbDevCon::ConstructL" ) );
    
    // usbc
    User::LeaveIfError(iLdd.Open(0));
    FLOG( _L( "[USBDEVCON]\tCUsbDevCon::ConstructL RDevUsbcClient opened OK" ) );
    
    // usb manager
    User::LeaveIfError(iUsbManager.Connect());
    FLOG( _L( "[USBDEVCON]\tCUsbDevCon::ConstructL RUsb connected OK" ) );
    
    // usb watcher
    User::LeaveIfError(iUsbWatcher.Connect());
    FLOG( _L( "[USBDEVCON]\tCUsbDevCon::ConstructL RUsbWatcher connected OK" ) );
    
    // device state watcher
    iUsbStateWatcher = CUsbStateWatcher::NewL(*this, iLdd);
        
    // Requests handler
    iRequestsHandler = CRequestsHandler::NewL(iLdd, iUsbWatcher, iUsbManager);
    
    // state machine
    iStateMachine = CStateMachine::NewL(*iRequestsHandler, iLdd);
    
     User::LeaveIfError(iShutdownTimer.CreateLocal());
    
    // get usb state, and act accordingly to it
    TUsbcDeviceState usbcstate(EUsbcDeviceStateUndefined);
    iLdd.DeviceStatus(usbcstate);
    
    FTRACE(FPrint(
            _L("[USBDEVCON]\tCUsbDevCon::ConstructL: Usbc state = %d" ),usbcstate));
    
    ActAccordinglyToUsbStateL(usbcstate);
     
    }

// ---------------------------------------------------------------------------
// Default construction
// ---------------------------------------------------------------------------
//  
CUsbDevCon::CUsbDevCon() : CActive(EPriorityStandard),
                             iUsbStateWatcher(0),
                             iStateMachine (0),
                             iRequestsHandler(0),
                             iPrevUsbState(EUsbcDeviceStateUndefined)
    {
    CActiveScheduler::Add(this);
     }

// ---------------------------------------------------------------------------
// Destruction
// ---------------------------------------------------------------------------
//
CUsbDevCon::~CUsbDevCon()
    {
    FLOG( _L( "[USBDEVCON]\tCUsbDevCon::~CUsbDevCon" ) );
    
    Cancel();
    
    FLOG( _L( "[USBDEVCON]\tCUsbDevCon::~CUsbDevCon Cancel" ) );
    
    delete iStateMachine;
    
    FLOG( _L( "[USBDEVCON]\tCUsbDevCon::~CUsbDevCon StateMachine" ) );
    
    delete iRequestsHandler;
    
    FLOG( _L( "[USBDEVCON]\tCUsbDevCon::~CUsbDevCon RequestsHandler" ) );
    
    delete iUsbStateWatcher;
    
    FLOG( _L( "[USBDEVCON]\tCUsbDevCon::~CUsbDevCon UsbStateWatcher" ) );
    
    iUsbWatcher.Close();
    
    FLOG( _L( "[USBDEVCON]\tCUsbDevCon::~CUsbDevCon UsbWatcher" ) );
    
    iUsbManager.Close();
    
    FLOG( _L( "[USBDEVCON]\tCUsbDevCon::~CUsbDevCon UsbManager" ) );
    
    iLdd.Close();
    
    FLOG( _L( "[USBDEVCON]\tCUsbDevCon::~CUsbDevCon LDD" ) );
    
    iShutdownTimer.Close();
    
    FLOG( _L( "[USBDEVCON]\tCUsbDevCon::~CUsbDevCon Timer" ) );
    
    }   

// ---------------------------------------------------------------------------
// Acts accordingly to USB state
// ---------------------------------------------------------------------------
//
void CUsbDevCon::ActAccordinglyToUsbStateL(TUsbcDeviceState aUsbcState)
    {
    
    switch (aUsbcState)
        {
        case EUsbcDeviceStateUndefined:
            {
            
            FLOG( _L( "[USBDEVCON]\tCUsbDevCon::ActAccordinglyToUsbStateL State: Undefined" ) );

            StopL();
            break;
            }
                
        case EUsbcDeviceStateAttached:
            {
            FLOG( _L( "[USBDEVCON]\tCUsbDevCon::ActAccordinglyToUsbStateL State: Attached, ignored" ) );

            break;
            }
                
        case EUsbcDeviceStateSuspended:
            {
            FLOG( _L( "[USBDEVCON]\tCUsbDevCon::ActAccordinglyToUsbStateL State: Suspended" ) );
            // NO break here
            }
        case EUsbcDeviceStatePowered:
            {
            FLOG( _L( "[USBDEVCON]\tCUsbDevCon::ActAccordinglyToUsbStateL State: Powered" ) );

            // In powered or suspended state, we are not allowed to do any data 
            // communication. Hence if there are pending read/write requests,
            // we need cancel them. 
            // Not call StopL() here because we do not want to shut down this
            // process so earlier but in Undefined state.
            if ( iStateMachine->IsStarted() )
                {
                iStateMachine->Stop();
                // release device control
                User::LeaveIfError(iLdd.ReleaseDeviceControl());
                }
            break;
            }
                
        case EUsbcDeviceStateDefault:
            {
            // The request will only be started from default state.
            // If it has been started already, nothing will be done.
            FLOG( _L( "[USBDEVCON]\tCUsbDevCon::ActAccordinglyToUsbStateL State: Default" ) );

            StartL();
            break;
            }
                
        case EUsbcDeviceStateAddress:
            {
            FLOG( _L( "[USBDEVCON]\tCUsbDevCon::ActAccordinglyToUsbStateL State: Addressed" ) );

            StartL();
            break;
            }
                
        case EUsbcDeviceStateConfigured:
            {
            FLOG( _L( "[USBDEVCON]\tCUsbDevCon::ActAccordinglyToUsbStateL State: Configured" ) );

            StartL();
                
            break;
            }
        default:
            {
                
            FLOG( _L( "[USBDEVCON]\tCUsbDevCon::ActAccordinglyToUsbStateL State: ***Unknown***" ) );

            StopL();
            break;
            }
        }
            
    iPrevUsbState = aUsbcState;
            
    // listen to USB states change
    iUsbStateWatcher->Activate();
    
    }
    
 // ---------------------------------------------------------------------------
// Timer is completed
// ---------------------------------------------------------------------------
//   
void CUsbDevCon::RunL()
    {
    FTRACE(FPrint(
            _L("[USBDEVCON]\tCUsbDevCon::RunL: iStatus = %d" ),iStatus.Int()));
   
    if(KErrNone == iStatus.Int())
        {
        FLOG( _L( "[USBDEVCON]\tCUsbDevCon::RunL Exiting usbdevcon" ) );      
        
        // Shutdown timer is finished, exit program
        CActiveScheduler::Stop(); // destruct resources
        }
    }

// ---------------------------------------------------------------------------
// Cancellation
// ---------------------------------------------------------------------------
//   
void CUsbDevCon::DoCancel()
    {
    FLOG( _L( "[USBDEVCON]\tCUsbDevCon::DoCancel" ) )
    iShutdownTimer.Cancel();      
    }
    
// ----------------------------------------------------------------------------
// Standard active object error function.
// ----------------------------------------------------------------------------
//
TInt CUsbDevCon::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// Starts UsbDevCon services
// ---------------------------------------------------------------------------
//
void CUsbDevCon::StartL()
    {
    
    FLOG( _L( "[USBDEVCON]\tCUsbDevCon::StartL" ) );        

    if(!iStateMachine->IsStarted())
        {
        // set device control
        User::LeaveIfError(iLdd.SetDeviceControl());
        
        // start state machine
        iStateMachine->StartL();
        
        }
        
    // Cancel shutdown timer, if it is started
    iShutdownTimer.Cancel();
    }
    
// ---------------------------------------------------------------------------
// Stops UsbDevCon services
// ---------------------------------------------------------------------------
//
void CUsbDevCon::StopL()
    {
    
    FLOG( _L( "[USBDEVCON]\tCUsbDevCon::StopL" ) );
    
    if(iStateMachine->IsStarted())
        {
        
        // stop state machine
        iStateMachine->Stop();
        
        // release device control
        User::LeaveIfError(iLdd.ReleaseDeviceControl());
        
        }
         
    if(!IsActive()) // not waiting for timer
        {
        FLOG( _L( "[USBDEVCON]\tCUsbDevCon::StopL Starting timer" ) );
        // run timer
        iShutdownTimer.Cancel();

        // RunL will be called after KInactiveTimeForShutDown milliseconds
        iShutdownTimer.After(iStatus, TTimeIntervalMicroSeconds32(KInactiveTimeForShutDown)); 
        SetActive();
        FLOG( _L( "[USBDEVCON]\tCUsbDevCon::StopL Timer is started" ) );
        }
    }
    

// ----------------------------------------------------------------------------
// Constructs and installs the active scheduler, constructs UsbDevCon object.
// ----------------------------------------------------------------------------
//
static void StartUsbDevConL()
    {
    
    FLOG( _L( "[USBDEVCON]\tStartUsbDevConL" ) );
        
    // Construct and install the active scheduler
    CActiveScheduler *myScheduler = new ( ELeave ) CActiveScheduler();

    // Push onto the cleanup stack
    CleanupStack::PushL( myScheduler );

    // Install as the active scheduler
    CActiveScheduler::Install( myScheduler );
    
    CUsbDevCon* instance =  CUsbDevCon::NewLC();
    
    RProcess::Rendezvous(KErrNone); // signal to starter process, that usbdevcon started OK or failed to start
           
    FLOG( _L( "[USBDEVCON]\tStartUsbDevConL Usbdevcon is started successfully" ) );
    
    // returns only when UsbDevCon closing
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy( instance );
    CleanupStack::PopAndDestroy( myScheduler );
    }

// ---------------------------------------------------------------------------
// Main function of the application executable.
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    TInt err;
    
    // rename the thread so it is easy to find the panic application
    err = User::RenameThread(KUsbDevConName);
    
    if(KErrNone != err)
        {
        return err;
        }
    
    __UHEAP_MARK;
    
    // create clean-up stack
    CTrapCleanup* cleanup = CTrapCleanup::New();
    
    TRAP( err, StartUsbDevConL() );
    
    delete cleanup; // destroy clean-up stack
    __UHEAP_MARKEND;

    return err;
    }
