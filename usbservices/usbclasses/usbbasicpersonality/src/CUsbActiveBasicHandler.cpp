/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Basic personality implementation
*
*/


#include <usbuinotif.h>
#include "CUsbActiveBasicHandler.h"
#include "cusbpersonalitynotifier.h"
#include "debug.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CUsbActiveBasicHandler::CUsbActiveBasicHandler(TUsbPersonalityParams& aPersonalityParams)
    : CUsbPersonalityPlugin(aPersonalityParams)
    {
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CUsbActiveBasicHandler::ConstructL()
    {
    FLOG( _L( "[USBWATCHER]\tCUsbActiveBasicHandler: ConstructL" ) );
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CUsbActiveBasicHandler* CUsbActiveBasicHandler::NewL(TUsbPersonalityParams& aPersonalityParams)
    {
    CUsbActiveBasicHandler* self 
        = new ( ELeave ) CUsbActiveBasicHandler(aPersonalityParams);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // pop self
    return self;
    }
    
// Destructor
CUsbActiveBasicHandler::~CUsbActiveBasicHandler()
    {
    FLOG( _L( "[USBWATCHER]\tCUsbActiveBasicHandler::~CUsbActiveBasicHandler" ) );
    }

// ----------------------------------------------------------------------------
// This function will not be called.
// ----------------------------------------------------------------------------
//
void CUsbActiveBasicHandler::RunL()
    {
    }

// ----------------------------------------------------------------------------
// Standard active object error function.
// ----------------------------------------------------------------------------
//
TInt CUsbActiveBasicHandler::RunError( TInt /*aError*/ )
    {
    // Currently no leaving functions called in RunL, thus nothing should cause
    // this to be called -> return.
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// This is called before personality change is done.
// ----------------------------------------------------------------------------
//
void CUsbActiveBasicHandler::ConfirmPersonalityUnload(TRequestStatus& aStatus)
    {
    FLOG( _L( "[USBWATCHER]\tCUsbActiveBasicHandler::ConfirmPersonalityUnload" ) );
    iRequestStatus = &aStatus;
    aStatus = KRequestPending;
    User::RequestComplete(iRequestStatus, KErrNone);        
    }

// ----------------------------------------------------------------------------
// Called by personality handler when personality start needs to be prepared.
// ----------------------------------------------------------------------------
//
void CUsbActiveBasicHandler::PreparePersonalityStart(TRequestStatus& aStatus)
    {
    FLOG( _L( "[USBWATCHER]\tCUsbActiveBasicHandler: PreparePersonalityStart" ) );
    iRequestStatus = &aStatus;
    aStatus = KRequestPending;
    User::RequestComplete(iRequestStatus, KErrNone);    
    }

// ----------------------------------------------------------------------------
// Called by personality handler when personality start needs to be finished. 
// ----------------------------------------------------------------------------
//
void CUsbActiveBasicHandler::FinishPersonalityStart(TRequestStatus& aStatus)
    {
    FLOG( _L( "[USBWATCHER]\tCUsbActiveBasicHandler: FinishPersonalityStart" ) );
    iRequestStatus = &aStatus;
    aStatus = KRequestPending;    
    User::RequestComplete(iRequestStatus, KErrNone);
    }

// ----------------------------------------------------------------------------
// Called by personality handler when personality stop needs to be prepared. 
// ----------------------------------------------------------------------------
//
void CUsbActiveBasicHandler::PreparePersonalityStop(TRequestStatus& aStatus)
    {
    FLOG( _L( "[USBWATCHER]\tCUsbActiveBasicHandler: PreparePersonalityStop" ) );
    iRequestStatus = &aStatus;
    aStatus = KRequestPending;
    User::RequestComplete(iRequestStatus, KErrNone);    
    }

// ----------------------------------------------------------------------------
// Called by personality handler when personality stop needs to be finished. 
// ----------------------------------------------------------------------------
//
void CUsbActiveBasicHandler::FinishPersonalityStop(TRequestStatus& aStatus)
    {
    FLOG( _L( "[USBWATCHER]\tCUsbActiveBasicHandler: FinishPersonalityStop" ) );
    iRequestStatus = &aStatus;
    aStatus = KRequestPending;
    User::RequestComplete(iRequestStatus, KErrNone);
    }

// ----------------------------------------------------------------------------
// Indicates USB device state change to personality when USB is started.
// ----------------------------------------------------------------------------
// 
void CUsbActiveBasicHandler::StateChangeNotify( TUsbDeviceState /*aState*/ )
    {
    FLOG( _L( "[USBWATCHER]\tCUsbActiveBasicHandler::StateChangeNotify" ) );    
    }

// ----------------------------------------------------------------------------
// Standard active object cancellation function.
// ----------------------------------------------------------------------------
//
void CUsbActiveBasicHandler::DoCancel()
    {
    FLOG( _L( "[USBWATCHER]\tCUsbActiveBasicHandler::DoCancel" ) );
    }

// End of file
