/*
* Copyright (c) 2007, 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  USB UI queries notifier class.
 *
*/


// INCLUDE FILES
#include <hb/hbwidgets/hbdevicemessageboxsymbian.h>      // dialog
#include <hb/hbcore/hbtextresolversymbian.h>
#include <usbuinotif.h>                     // pck
#include "usbuinotifotgerror.h"             // Own class definition
#include "usbuinotifdebug.h"                // Debugging macros

// CONSTANTS
/** granularity for allocating error strings */
const TInt KUsbOtgErrorGranularity = 7; 


// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CUsbUiNotifOtgError::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CUsbUiNotifOtgError* CUsbUiNotifOtgError::NewL()
    {
    CUsbUiNotifOtgError* self = new (ELeave) CUsbUiNotifOtgError();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CUsbUiNotifOtgError::CUsbUiNotifOtgError
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CUsbUiNotifOtgError::CUsbUiNotifOtgError() :
    iStringIds(NULL)
    {
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CUsbUiNotifOtgError::~CUsbUiNotifOtgError()
    {
    //Make sure that the request is completed. Note that inside the destructor,
    //this virtual function call is to local CUsbUiNotifOtgError::Cancel, 
    //not to any possibly derived class implementation. 
    delete iStringIds;
    Cancel();
    }

void CUsbUiNotifOtgError::ConstructL()
    {
    CUSBUINotifierBase::ConstructL();
    iStringIds = new (ELeave) CDesCArrayFlat(KUsbOtgErrorGranularity);
    _LIT(KCurrentLimit, "txt_usb_info_error_in_usb_connection_disconnect_d");
    _LIT(KTooMuchCurrent, "txt_usb_info_unsupported_usb_device_disconnect_de");
    _LIT(KUnsupported, "txt_usb_info_unsupported_usb_device_disconnect_de");
    _LIT(KHubUnsupported, "txt_usb_info_hubs_are_not_supported_disconnect_us");
    _LIT(KUnRecoverable, "txt_usb_info_error_in_usb_connection_disconnect_d");
    _LIT(KAttachTimeOut, "txt_usb_info_remove_usb_cable_or_connect_a_device");
    _LIT(KNotEnoughMemory, "txt_usb_info_memory_full_close_some_applications");
    iStringIds->AppendL( KCurrentLimit);
    iStringIds->AppendL( KTooMuchCurrent);
    iStringIds->AppendL( KUnsupported);
    iStringIds->AppendL( KHubUnsupported);
    iStringIds->AppendL( KUnRecoverable);
    iStringIds->AppendL( KAttachTimeOut);
    iStringIds->AppendL( KNotEnoughMemory);
   
    }

// ----------------------------------------------------------------------------
// CUsbUiNotifOtgError::RegisterL
// Register notifier.
// ----------------------------------------------------------------------------
//
CUsbUiNotifOtgError::TNotifierInfo CUsbUiNotifOtgError::RegisterL()
    {
    iInfo.iUid = KUsbUiNotifOtgError;
    iInfo.iChannel = KUsbUiNotifOtgError;
    iInfo.iPriority = ENotifierPriorityVHigh;
    return iInfo;
    }

// ----------------------------------------------------------------------------
// CUsbUiNotifOtgError::GetParamsL
//  Jump to RunL as soon as possible.
// ----------------------------------------------------------------------------
//
void CUsbUiNotifOtgError::StartDialogL(const TDesC8& aBuffer, TInt aReplySlot,
        const RMessagePtr2& aMessage)
    {
    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifOtgError::StartDialogL"));
    if (iReplySlot != 0 || iNeedToCompleteMessage)
        {
        User::Leave( KErrInUse );
        }
  
    InitializeTextResolver();
    
    TInt errorId = 0;
    TPckgC<TInt> pckg( errorId );
    pckg.Set( aBuffer );
    errorId = pckg();
    
    FTRACE(FPrint(_L("[USBUINOTIF]\t CUsbUiNotifOtgError::GetParamsL iErrorId: %d"), errorId ));  
    if ( errorId < 0 || errorId >= iStringIds->MdcaCount())
        {        
        User::Leave( KErrArgument);        
        }    
    iMessage = aMessage;
    iNeedToCompleteMessage = ETrue;
    iReplySlot = aReplySlot;
   
    if (iQuery)
            {
            delete iQuery;
            iQuery = NULL;
            }
        
        iQuery = CHbDeviceMessageBoxSymbian::NewL(
                CHbDeviceMessageBoxSymbian::EWarning, this);
        iQuery->SetTimeout(0);
        HBufC* stringHolder = HbTextResolverSymbian::LoadLC(iStringIds->MdcaPoint(errorId) );
        iQuery->SetTextL(*stringHolder);
        iQuery->ShowL();
        CleanupStack::PopAndDestroy( stringHolder );
            
    
    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifOtgError::StartDialogL completed"));
    }

// ----------------------------------------------------------------------------
// CUsbUiNotifOtgError::Cancel
// Release all own resources (member variables)
// ----------------------------------------------------------------------------
//
void CUsbUiNotifOtgError::Cancel()
    {
    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifOtgError::Cancel"));
    if (iQuery)
        {
        iQuery->Close();
        delete iQuery;
        iQuery = NULL;
        }
    CUSBUINotifierBase::Cancel();

    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifOtgError::Cancel() completed"));
    }
// ----------------------------------------------------------------------------
// Call back function to observe device message box closing.
// ----------------------------------------------------------------------------
//
void CUsbUiNotifOtgError::MessageBoxClosed(
        const CHbDeviceMessageBoxSymbian* /*aMessageBox*/,
        CHbDeviceMessageBoxSymbian::TButtonId aButton)
    {
    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifMSMMError::MessageBoxClosed"));
    int returnValue = KErrNone;
    
    //iQuery will be deleted in Cancel. If Cancel is not called, it will be
    //deleted next time the query is shown. 

    if (aButton == CHbDeviceMessageBoxSymbian::EAcceptButton) 
        {
        returnValue = KErrNone;
        } 
    else 
        {
        returnValue = KErrCancel;
        }

    CompleteMessage( returnValue );
    
    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifMSMMError::MessageBoxClosed completed"));    
    }


// End of File
