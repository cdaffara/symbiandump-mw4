/*
* Copyright (c) 2006 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active object class used to manage asynchronous request.
*
*/


#include <e32property.h>
#include "usblcdactive.h"
#include <UsbWatcherInternalPSKeys.h>
#include <usbpersonalityids.h>
#include "debug.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructs a CUsbLcdActive object.
// ---------------------------------------------------------------------------
//
CUsbLcdActive* CUsbLcdActive::NewL(MLocodBearerPluginObserver& aObserver)
    {
    LOG_FUNC
    CUsbLcdActive* self = new (ELeave) CUsbLcdActive(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CUsbLcdActive::~CUsbLcdActive()
    {
    LOG_FUNC
    Cancel();
    iProperty.Close();
    }


// ---------------------------------------------------------------------------
// From class CActive
// ---------------------------------------------------------------------------
//
void CUsbLcdActive::DoCancel()
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// RunL() From class CActive
// ---------------------------------------------------------------------------
//
   
void CUsbLcdActive::RunL()
    {
    LOG_FUNC
    TInt value;
    
    if( iStatus != KErrNone )
       {
       LOG1("CUsbLcdActive::RunL() iStatus = %d",iStatus.Int());
       }
    else
       {

        TInt ret = iProperty.Get( value );
        LOG2("Personality: %d, ret: %d", value, ret);  
        iProperty.Subscribe(iStatus);
        SetActive();
        if (ret == KErrNone)
            { 
            HandleUsbPersonalityChange( value );
            }    
       }
    }

// ---------------------------------------------------------------------------
// From class CActive
// ---------------------------------------------------------------------------
//
    
TInt CUsbLcdActive::RunError( TInt /*aError*/ )
    {
    LOG_FUNC
    return KErrNone; 
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CUsbLcdActive::CUsbLcdActive(MLocodBearerPluginObserver& aObserver)
    : CActive(EPriorityStandard), iObserver(aObserver)
    {
    LOG_FUNC
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// Method to perform second phase construction.
// ---------------------------------------------------------------------------
//
void CUsbLcdActive::ConstructL()
    {  
    LOG_FUNC
    User::LeaveIfError(iProperty.Attach(KPSUidUsbWatcher, KUsbWatcherSelectedPersonality));
    Start();
    }

// ---------------------------------------------------------------------------
// Start() function is called to initiate RProperty subscription
// ---------------------------------------------------------------------------
//
void CUsbLcdActive::Start()
    {     
    LOG_FUNC
    iProperty.Subscribe(iStatus);
    SetActive();
    // Check the starting state
    TInt usbPersonalityId( KUsbWatcherSelectedPersonalityNone );
    TInt ret = iProperty.Get( usbPersonalityId );
    if ( ( ret == KErrNone ) && 
        ( usbPersonalityId != KUsbWatcherSelectedPersonalityNone ) )
        {
        HandleUsbPersonalityChange( usbPersonalityId ); 
        }
    }
     

// ---------------------------------------------------------------------------
// This function handles the USB active personality change, and notify LoCoD
// framework whether PC Suite is active 
// ---------------------------------------------------------------------------
//
void CUsbLcdActive::HandleUsbPersonalityChange( TInt aNewPersonalityId )
    {
    LOG_FUNC
    LOG1( "aNewPersonalityId: %d", aNewPersonalityId );
    TBool isPcSuiteActive( EFalse );
    if ( ( aNewPersonalityId == KUsbPersonalityIdPCSuite ) ||
         ( aNewPersonalityId == KUsbPersonalityIdPCSuiteMTP ) )
        {
        isPcSuiteActive = ETrue;
        }
    iObserver.NotifyBearerStatus( ELocodBearerUSB, isPcSuiteActive );
    }
    
//End of file
