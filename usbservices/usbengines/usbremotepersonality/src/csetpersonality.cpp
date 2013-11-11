/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles Set personality request
*
*/


#include <usbwatcher.h>

#include "csetpersonality.h"
#include "cremotepersonalityhandler.h"
#include "debug.h"

// ---------------------------------------------------------------------------
// Two-phase construction
// ---------------------------------------------------------------------------
//  
CSetPersonality* CSetPersonality::NewL(CRemotePersonalityHandler& aCallBack)
    {
    
    FLOG( _L( "[USBREMOTEPERSONALITY]\tCSetPersonality::NewL" ) );
    
    CSetPersonality* self = new (ELeave) CSetPersonality(aCallBack);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// ---------------------------------------------------------------------------
// Default construction
// ---------------------------------------------------------------------------
//
CSetPersonality::CSetPersonality(CRemotePersonalityHandler& aCallBack) : 
                                        CActive(EPriorityStandard),
                                        iCallBack(aCallBack)
    {
    CActiveScheduler::Add(this);
     }
        
// ---------------------------------------------------------------------------
// Two-phase construction
// ---------------------------------------------------------------------------
//
void CSetPersonality::ConstructL()
    {
    
    FLOG( _L( "[USBREMOTEPERSONALITY]\tCSetPersonality::ConstructL" ) );
    
    }
    
// ---------------------------------------------------------------------------
// Destruction
// ---------------------------------------------------------------------------
//
CSetPersonality::~CSetPersonality()
    {
    
    FLOG( _L( "[USBREMOTEPERSONALITY]\tCSetPersonality::~CSetPersonality" ) );
    Cancel();
      
    }   
  
// ---------------------------------------------------------------------------
// Sets Usb Watcher
// ---------------------------------------------------------------------------
//
void CSetPersonality::SetUsbWatcher(RUsbWatcher* aUsbWatcher)
    {
    iUsbWatcher = aUsbWatcher;
    }     

// ---------------------------------------------------------------------------
// Cancellation
// ---------------------------------------------------------------------------
//
void CSetPersonality::DoCancel()
    {
    iUsbWatcher->CancelSetPersonality();
    }
    
// ----------------------------------------------------------------------------
// Standard active object error function.
// ----------------------------------------------------------------------------
//
TInt CSetPersonality::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// Asynchronous request has been completed
// ---------------------------------------------------------------------------
//
void CSetPersonality::RunL()
    {
        
        FTRACE(FPrint(
          _L("[USBREMOTEPERSONALITY]\tCSetPersonality::RunL iStatus.Int() = %d" ), iStatus.Int()));

        CRemotePersonalityHandler::TLastResult result(CRemotePersonalityHandler::EUndefinedError); 
        
         // set result code according to the problem happened
        switch(iStatus.Int())
            {
            case KErrNone:
                {
                result =CRemotePersonalityHandler::ESuccess;
                break;
                }
            case KErrNotFound:
                {
                result = CRemotePersonalityHandler::ENonExistingPersonality;
                break;
                }
            case KErrNotSupported:
                {
                result = CRemotePersonalityHandler::EFeatureIsNotSupported;
                break;
                }
            case KErrCancel: 
            default:
                {
                result = CRemotePersonalityHandler::EUndefinedError;
                }
            }
       iCallBack.SetPersonalityCallBack(result);
    }

// ---------------------------------------------------------------------------
// Process SetPersonality request
// ---------------------------------------------------------------------------
//  
void CSetPersonality::SetPersonality(TUint aPersonalityId)
    {
    
    FTRACE(FPrint(
          _L("[USBREMOTEPERSONALITY]\tCSetPersonality::SetPersonality Personality Id = %d" ), aPersonalityId));
    
    if(IsActive())
        {
        Cancel();
        }
        
    // No Ask-on-connection query, no confirmation for unload personality   
    iUsbWatcher->SetPersonality(iStatus, aPersonalityId, EFalse, ETrue);
    SetActive();
    
    }


