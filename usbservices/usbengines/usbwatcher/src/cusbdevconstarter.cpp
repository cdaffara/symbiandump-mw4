/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implement class CUsbDevConStarter
*
*/


#include "cusbdevconstarter.h"
#include "debug.h"

_LIT16( KUsbDevConPath, "z:\\sys\\bin\\usbdevcon.exe" );
_LIT16( KUsbDevConParams, "" );


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//
CUsbDevConStarter* CUsbDevConStarter::NewL()
    {
    LOG_FUNC

    CUsbDevConStarter* self = new ( ELeave ) CUsbDevConStarter();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CUsbDevConStarter::~CUsbDevConStarter()
    {
    LOG_FUNC

    Cancel();
    }

// ----------------------------------------------------------------------------
// Start usbdevcon process unless it is already running.
// ----------------------------------------------------------------------------
//
void CUsbDevConStarter::Start()
    {
    LOG_FUNC

    iStarted = ETrue;
    if ( EIdle == iState )
        {
        TInt err = iProcess.Create( KUsbDevConPath, KUsbDevConParams );
        if ( KErrNone == err )
            {
            //start process and wait until it is constructed
            iProcess.Resume();
            iState = EWaitingRendezvous;
            iProcess.Rendezvous( iStatus );
            SetActive();
            }
        else
            {
            //error in starting, no restarting needed
            iStarted = EFalse;
            LOG1( "ERROR: RProcess::Create = %d" ,  err );
            }
        }

    }


// ----------------------------------------------------------------------------
// Logon to usbdevcon process for termination
// ----------------------------------------------------------------------------
//
void CUsbDevConStarter::Logon()
    {
    LOG_FUNC

    iState = ERunning;

    //request termination notify
    iProcess.Logon( iStatus );
    SetActive();
    }


// ----------------------------------------------------------------------------
// StopRestarting prevents usbdevcon process restarting.
// ----------------------------------------------------------------------------
//
void CUsbDevConStarter::StopRestarting()
    {
    LOG_FUNC

    iStarted = EFalse;
    }

// ----------------------------------------------------------------------------
// RunL handles the process termination.
// The process is restarted, if Stop has not been requested and it terminated
// without error.
// ----------------------------------------------------------------------------
//
void CUsbDevConStarter::RunL()
    {
    LOG_FUNC

    LOG2( "iStatus = %d, iState = %d", iStatus.Int(), iState );
    switch( iState )
        {
        case EIdle:
            {
            break;
            }
        case EWaitingRendezvous:
            {
            Logon();
            break;
            }
        case ERunning:
            {
            iState = EIdle;
            iProcess.Close();

            //Do not restart, if terminated with error.
            if( iStarted && ( iStatus == KErrNone ) )
                {
                Start();
                }
            break;
            }
        default:
            LOG( "Unhandled state" );
			break;
        }

    }

// ----------------------------------------------------------------------------
// Log error, if RunL leaves. RunL should never leave.
// ----------------------------------------------------------------------------
//
TInt CUsbDevConStarter::RunError( TInt aError )
    {
    LOG_FUNC

    LOG1( "aError = %d", aError );
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// DoCancel
// ----------------------------------------------------------------------------
//
void CUsbDevConStarter::DoCancel()
    {
    LOG_FUNC

    iProcess.LogonCancel( iStatus );
    iProcess.Close();
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CUsbDevConStarter::CUsbDevConStarter()
    : CActive( EPriorityStandard )
    , iStarted( EFalse )
    , iState( EIdle )
    {
    LOG_FUNC

    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CUsbDevConStarter::ConstructL()
    {
    LOG_FUNC
    //empty
    }

// End of file
