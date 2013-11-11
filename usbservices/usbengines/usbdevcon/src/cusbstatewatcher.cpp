/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Watches USB states
*
*/


#include "cusbstatewatcher.h"
#include "debug.h"

// ---------------------------------------------------------------------------
// Two-phase construction 
// ---------------------------------------------------------------------------
//
CUsbStateWatcher* CUsbStateWatcher::NewL(CUsbDevCon& aObserver, RDevUsbcClient& aLdd)
    {
    
    FLOG( _L( "[USBDEVCON]\tCUsbStateWatcher::NewL" ) );

    CUsbStateWatcher* self = new (ELeave) CUsbStateWatcher(aObserver, aLdd);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// ---------------------------------------------------------------------------
// Default construction 
// ---------------------------------------------------------------------------
//
CUsbStateWatcher::CUsbStateWatcher(CUsbDevCon& aObserver, RDevUsbcClient& aLdd) :
                                                            CActive(EPriorityMore),
                                                            iState(EUsbcNoState),
                                                            iObserver(aObserver),
                                                            iLdd(aLdd)
                                                        
    {
    CActiveScheduler::Add(this);
    }
        
// ---------------------------------------------------------------------------
// Two-phase construction 
// ---------------------------------------------------------------------------
//
void CUsbStateWatcher::ConstructL()
    {
    }
    
// ---------------------------------------------------------------------------
// Destruction 
// ---------------------------------------------------------------------------
//
CUsbStateWatcher::~CUsbStateWatcher()
    {
    Cancel();
    }   
    
// ---------------------------------------------------------------------------
// Cancellation 
// ---------------------------------------------------------------------------
//
void CUsbStateWatcher::DoCancel()
    {
    iLdd.AlternateDeviceStatusNotifyCancel();
    }
    
// ----------------------------------------------------------------------------
// Standard active object error function.
// ----------------------------------------------------------------------------
//
TInt CUsbStateWatcher::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// USB state has changed 
// ---------------------------------------------------------------------------
//
void CUsbStateWatcher::RunL()
    {
    if (!(iState & KUsbAlternateSetting))
        {
        iObserver.ActAccordinglyToUsbStateL(TUsbcDeviceState(iState));
        }
        else 
        {
        // Alternate interface setting changed to iDeviceState & ~KUsbAlternateSetting
        }
    }

// ---------------------------------------------------------------------------
// Start listening to USB state changes 
// ---------------------------------------------------------------------------
//
void CUsbStateWatcher::Activate()
    {
    
    FLOG( _L( "[USBDEVCON]\tCUsbStateWatcher::Activate" ) );
    
    if(IsActive())
        {
        return;
        }
        
    iLdd.AlternateDeviceStatusNotify(iStatus, iState);
    SetActive();
    FLOG( _L( "[USBDEVCON]\tCUsbStateWatcher::Activate end" ) );
     
    }


