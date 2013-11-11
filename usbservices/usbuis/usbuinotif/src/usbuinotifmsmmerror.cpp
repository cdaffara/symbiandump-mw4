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
* Description:  USB UI MSMM Error notifier class.
 *
*/


// INCLUDE FILES
#include <hb/hbwidgets/hbdevicemessageboxsymbian.h>      // dialog
#include <hb/hbwidgets/hbdevicenotificationdialogsymbian.h> //discreet
#include <hb/hbcore/hbtextresolversymbian.h>
#include <usb/hostms/srverr.h>
#include <usbuinotif.h>                     // pck
#include "usbuinotifmsmmerror.h"            // Own class definition
#include "usbuinotifdebug.h"                // Debugging macros


// CONSTANTS
/** granularity for allocating error strings */
const TInt KUsbMsmmErrorGranularity = 5; 
_LIT(KUSBUIconFileName, "qtg_large_usb");
// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CUsbUiNotifOtgError::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CUsbUiNotifMSMMError* CUsbUiNotifMSMMError::NewL()
    {
    CUsbUiNotifMSMMError* self = new (ELeave) CUsbUiNotifMSMMError();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CUsbUiNotifMSMMError::CUsbUiNotifMSMMError
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CUsbUiNotifMSMMError::CUsbUiNotifMSMMError() :
    iStringIds(NULL)
    {
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CUsbUiNotifMSMMError::~CUsbUiNotifMSMMError()
    {
    //Make sure that the request is completed. Note that inside the destructor,
    //this virtual function call is to local CUsbUiNotifMSMMError::Cancel, 
    //not to any possibly derived class implementation. 
    delete iStringIds;
    Cancel();
    }

void CUsbUiNotifMSMMError::ConstructL()
    {
    CUSBUINotifierBase::ConstructL();
    iStringIds = new (ELeave) CDesCArrayFlat(KUsbMsmmErrorGranularity);
    _LIT(KGeneralError,"txt_usb_info_error_in_usb_connection_disconnect_d");
    _LIT(KUnknownFileSys, "txt_usb_info_unknown_file_system_disconnect_devic");
    _LIT(KOutOfMemory, "txt_usb_info_disk_full_remove_some_files_and_try");
    _LIT(KSafeToRemove, "txt_usb_dpinfo_safe_to_remove");
    _LIT(KUnableToEject,"txt_usb_info_unable_to_eject_the_usb_device_some" );
    
    iStringIds->AppendL(KGeneralError);
    iStringIds->AppendL(KUnknownFileSys);
    iStringIds->AppendL(KOutOfMemory);
    iStringIds->AppendL(KSafeToRemove);
    iStringIds->AppendL(KUnableToEject);
    }

// ----------------------------------------------------------------------------
// CUsbUiNotifMSMMError::RegisterL
// Register notifier.
// ----------------------------------------------------------------------------
//
CUsbUiNotifMSMMError::TNotifierInfo CUsbUiNotifMSMMError::RegisterL()
    {
    iInfo.iUid = KUsbUiNotifMsmmError;
    iInfo.iChannel = KUsbUiNotifMsmmError;
    iInfo.iPriority = ENotifierPriorityVHigh;
    return iInfo;
    }

// ----------------------------------------------------------------------------
// CUsbUiNotifMSMMError::GetParamsL
//  Jump to RunL as soon as possible.
// ----------------------------------------------------------------------------
//
void CUsbUiNotifMSMMError::StartDialogL(const TDesC8& aBuffer, TInt aReplySlot,
        const RMessagePtr2& aMessage)
    {
    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifMSMMError::StartDialogL"));
    if (iReplySlot != 0 || iNeedToCompleteMessage)
        {
        User::Leave( KErrInUse );
        }

    InitializeTextResolver();

    iMessage = aMessage;
    iNeedToCompleteMessage = ETrue;
    iReplySlot = aReplySlot;
    _LIT(KUsbDisconnected, "txt_usb_dpophead_usb_disconnected");
    // Get parameters 
    //    
    THostMsErrData error;
    TPckgC<THostMsErrData> pckg( error );
    pckg.Set( aBuffer );
    int errorId = EUsbMSMMGeneralError;
    switch (pckg().iError)
        {
        case  EHostMsErrUnknownFileSystem:
            errorId = EUsbMSMMUnknownFileSystem;
            break;        
        case  EHostMsErrOutOfMemory:
            errorId = EUsbMSMMOutOfMemory;
            break;       
        case EHostMsErrNone:
            errorId = EUsbMSMMSafeToRemove;
            break;
        case EHostMsErrInUse:
            errorId = EUsbMSMMUnableToEject;
            break;
        default:
            errorId = EUsbMSMMGeneralError;
        }
    HBufC* stringHolder = HbTextResolverSymbian::LoadLC(iStringIds->MdcaPoint(errorId)); 
   
   
    if ( errorId == EUsbMSMMSafeToRemove)
        {
        // "safe to remove" discreet popup
        HBufC* header = HbTextResolverSymbian::LoadLC(KUsbDisconnected);
        if (iDiscreet)
            {
             FLOG(_L("[USBUINOTIF]\t CUsbUiNotifMSMMError::StartDialogL() deleting previous dialog"));
             delete iDiscreet;
             iDiscreet = NULL;
            }
        iDiscreet = CHbDeviceNotificationDialogSymbian::NewL(this);
        iDiscreet->SetTitleL(*header);
        iDiscreet->SetTextL(*stringHolder);
        iDiscreet->SetIconNameL(KUSBUIconFileName );
        iDiscreet->ShowL();
        CleanupStack::PopAndDestroy( header );   
        }
    else
        {
         //Delete the query in case the client didn't cancel the notifier 
   			 //or close the session after the previous query.
   
        if (iQuery)
            {
            FLOG(_L("[USBUINOTIF]\t CUsbUiNotifMSMMError::StartDialogL() deleting previous dialog"));
            delete iQuery;
            iQuery = NULL;
            }
        
        iQuery = CHbDeviceMessageBoxSymbian::NewL(
                CHbDeviceMessageBoxSymbian::EWarning, this);
        iQuery->SetTimeout(0);
    
  
        iQuery->SetTextL(*stringHolder);
        iQuery->ShowL();
        }
   
    CleanupStack::PopAndDestroy( stringHolder );
   
        
    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifMSMMError::StartDialogL() completed"));
    }

// ----------------------------------------------------------------------------
// CUsbUiNotifMSMMError::Cancel
// Release all own resources (member variables)
// ----------------------------------------------------------------------------
//
void CUsbUiNotifMSMMError::Cancel()
    {
    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifMSMMError::Cancel"));
    if (iQuery)
        {
        FLOG(_L("[USBUINOTIF]\t CUsbUiNotifMSMMError::Cancel closing iQuery"));
        iQuery->Close();
        FLOG(_L("[USBUINOTIF]\t CUsbUiNotifMSMMError::Cancel iQuery closed"));
        delete iQuery;
        FLOG(_L("[USBUINOTIF]\t CUsbUiNotifMSMMError::Cancel iQuery deleted"));
        iQuery = NULL;
        }
    if (iDiscreet)
        {
        iDiscreet->Close();
        delete iDiscreet;
        iDiscreet = NULL;
        }
    CUSBUINotifierBase::Cancel();
    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifMSMMError::Cancel() completed"));
    }

// ----------------------------------------------------------------------------
// Call back function to observe device message box closing.
// ----------------------------------------------------------------------------
//
void CUsbUiNotifMSMMError::MessageBoxClosed(
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

void CUsbUiNotifMSMMError::NotificationDialogActivated(
     const CHbDeviceNotificationDialogSymbian* /*aDialog*/)
	{
       
	}
   
void CUsbUiNotifMSMMError::NotificationDialogClosed(
        const CHbDeviceNotificationDialogSymbian* /*aDialog*/, TInt /*aCompletionCode*/)
	{
    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifMSMMError::NotificationDialogClosed()"));
    CompleteMessage( KErrCancel ); 
    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifMSMMError::NotificationDialogClosed() complete"));
    }
// End of File
