/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This implements TUsbIndicatorHandler class.
*
*/

#include <e32std.h>
#include <hbindicatorsymbian.h>
#include <hbsymbianvariant.h>
#include <usbman.h>
#include "debug.h"
#include "cusbindicatorhandler.h"


// ---------------------------------------------------------------------------
// C++ Constructor 
// ---------------------------------------------------------------------------
//

CUsbIndicatorHandler::CUsbIndicatorHandler()
    {
    // No implementation required
    }

CUsbIndicatorHandler::~CUsbIndicatorHandler()
    {
    delete iSymbianIndicator;  

    }

CUsbIndicatorHandler* CUsbIndicatorHandler::NewLC()
    {
    CUsbIndicatorHandler* self = new (ELeave) CUsbIndicatorHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CUsbIndicatorHandler* CUsbIndicatorHandler::NewL()
    {
    CUsbIndicatorHandler* self = CUsbIndicatorHandler::NewLC();
    CleanupStack::Pop(); // self;
    return self;
    }

void CUsbIndicatorHandler::ConstructL()
    {
     LOG_FUNC    
        
    iDevStateB4Suspend = EUsbDeviceStateUndefined;
    iSymbianIndicator = CHbIndicatorSymbian::NewL();
    }

// ---------------------------------------------------------------------------
// Change USB Indicator 
// ---------------------------------------------------------------------------
//
void CUsbIndicatorHandler::HandleDeviceStateChangeL( TUsbDeviceState aStateOld, 
            TUsbDeviceState aStateNew, TInt aPersonality )
    {
    LOG_FUNC

    LOG2( "USB device state changed: %d ==> %d", aStateOld, aStateNew );
    
    if ( EUsbDeviceStateAddress == aStateNew ) // Entering addressed state
        {
       // going down configured -> addressed     
       if( ( EUsbDeviceStateConfigured == aStateOld ) ||
            ( ( EUsbDeviceStateSuspended == aStateOld ) &&
              ( EUsbDeviceStateConfigured == iDevStateB4Suspend ) ) ) 
            {
            ShowUsbConnectedIndicatorL( EFalse, aPersonality );
            }
        
        // connecting indicator will be shown when addressed state is entered going up or down
        // note that when disconnecting the cable (no need to show connecting indicator)
        // the state change is CONFIGURED => SUSPENDED => POWERED => UNDEFINED    
        // filter out case of addressed -> suspended -> addressed
        if ( ( EUsbDeviceStateSuspended != aStateOld ) || 
             ( EUsbDeviceStateAddress != iDevStateB4Suspend) )
            {
            ShowUsbAddressedIndicator( ETrue );
            }
        }
    
    else if ( EUsbDeviceStateConfigured == aStateNew ) // Entering configured state 
        {
        // filter out case of configed -> suspended -> configed
        if ( ( EUsbDeviceStateSuspended != aStateOld ) || 
             ( EUsbDeviceStateConfigured != iDevStateB4Suspend) )
            {
            ShowUsbAddressedIndicator( EFalse );    
            ShowUsbConnectedIndicatorL( ETrue, aPersonality );
            }
        }
     
    else if ( EUsbDeviceStateUndefined == aStateNew ) // Entering undefined
        {
        ShowUsbAddressedIndicator( EFalse );
        ShowUsbConnectedIndicatorL( EFalse, aPersonality );
        }   
        
        
    else if ( EUsbDeviceStateSuspended == aStateNew )
    // If current state is suspend, we do not change the indicator, but save
    // the previous state
        {
        iDevStateB4Suspend = aStateOld;
        }
    else // New device state is not configured, nor suspended, nor addressed
        {
        ShowUsbConnectedIndicatorL( EFalse, aPersonality );       
        } 
  
    }
    
// ---------------------------------------------------------------------------
// Show USB Indicator
// ---------------------------------------------------------------------------
//
void CUsbIndicatorHandler::ShowUsbConnectedIndicatorL( TBool aActivate, TInt aPersonality )
    {
     LOG_FUNC    
     TBool success = EFalse;
     _LIT(KUsbIndicator, "com.nokia.hb.indicator.usb.device.connected/1.0");  
     
     if (aActivate)
        {
        RUsb usbman;
        User::LeaveIfError(usbman.Connect());
        CleanupClosePushL(usbman);
        HBufC* description = NULL;
        User::LeaveIfError(usbman.GetDescription(aPersonality,
             description));
        CleanupStack::PopAndDestroy(&usbman);
        CleanupStack::PushL(description);
        CHbSymbianVariant* parameters = CHbSymbianVariant::NewL(description, CHbSymbianVariant::EDes);
        CleanupStack::PopAndDestroy(description);       
        success = iSymbianIndicator->Activate(KUsbIndicator, parameters);   
        LOG1( "calling CHbIndicatorSymbian::Activate(), returned %d", success);
        if (!success)
            {
            TInt indicatorerror = iSymbianIndicator->Error();
            LOG1( "activating the indicator failed with %d", indicatorerror);
            }
        else
            {
            iConnectedIndicatorActive = ETrue;    
            }    
        delete parameters;  
        }    
     else if (iConnectedIndicatorActive)
        {
        success = iSymbianIndicator->Deactivate(KUsbIndicator); 
        LOG1( "calling CHbIndicatorSymbian::Deactivate(), returned %d", success);
        if (success)
            {
            iConnectedIndicatorActive = EFalse;    
            }        
        LOG( "calling CHbIndicatorSymbian::Deactivate()");
        }
   
   }

void CUsbIndicatorHandler::ShowUsbAddressedIndicator( TBool aActivate  )
    {
     LOG_FUNC    
     TBool success = EFalse;
     _LIT(KUsbIndicator, "com.nokia.hb.indicator.usb.device.inprogress/1.0");  
     if (aActivate)
        {
        success = iSymbianIndicator->Activate(KUsbIndicator);   
        LOG1( "calling CHbIndicatorSymbian::Activate(), returned %d", success);
        if (!success)
            {
            TInt indicatorerror = iSymbianIndicator->Error();
            LOG1( "activating the indicator failed with %d", indicatorerror);
            }
        else
            {
            iAddressedIndicatorActive = ETrue;    
            }
        }    
     else if (iAddressedIndicatorActive)
        {
        success = iSymbianIndicator->Deactivate(KUsbIndicator); 
        LOG1( "calling CHbIndicatorSymbian::Deactivate(), returned %d", success);
        if (success)
            {
            iAddressedIndicatorActive = EFalse;    
            }
        LOG( "calling CHbIndicatorSymbian::Deactivate()");
        }
   }
   


// End of file
