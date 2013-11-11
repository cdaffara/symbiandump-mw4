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
* Description:  USB UI notes notifier class.
 *
*/


// INCLUDE FILES
#include <hb/hbwidgets/hbdevicemessageboxsymbian.h>  
#include <hb/hbcore/hbtextresolversymbian.h>
#include "usbuinotifotgwarning.h"        // Own class definition
#include "usbuinotifdebug.h"                // Debugging macros


// CONSTANTS
/** granularity for allocating warning strings */
const TInt KUsbOtgWarningGranularity = 1; 

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CUsbUiNotifOtgWarning::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CUsbUiNotifOtgWarning* CUsbUiNotifOtgWarning::NewL()
    {
    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifOtgWarning::NewL()"));
    CUsbUiNotifOtgWarning* self = new (ELeave) CUsbUiNotifOtgWarning();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifOtgWarning::NewL completed()"));
    return self;
    }

// ----------------------------------------------------------------------------
// CUsbUiNotifOtgWarning::CUsbUiNotifOtgWarning
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CUsbUiNotifOtgWarning::CUsbUiNotifOtgWarning() :
    iStringIds(NULL), iNote( NULL)
    {
    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifOtgWarning::constructor()"));
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CUsbUiNotifOtgWarning::~CUsbUiNotifOtgWarning()
    {
        FLOG(_L("[USBUINOTIF]\t CUsbUiNotifOtgWarning::destructor()"));
    //Make sure that the request is completed. Note that inside the destructor,
    //this virtual function call is to local CUsbUiNotifOtgWarning::Cancel, 
    //not to any possibly derived class implementation. 
    delete iStringIds;
    Cancel();
    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifOtgWarning::destructor completed()"));
    }

void CUsbUiNotifOtgWarning::ConstructL()
    {
    CUSBUINotifierBase::ConstructL();
    iStringIds = new (ELeave) CDesCArrayFlat(KUsbOtgWarningGranularity);
    _LIT(KPartiallySupported, "txt_usb_info_partially_supported_usb_device_connec");
    iStringIds->AppendL( KPartiallySupported);
    }

// ----------------------------------------------------------------------------
// CUsbUiNotifOtgWarning::RegisterL
// Register notifier.
// ----------------------------------------------------------------------------
//
CUsbUiNotifOtgWarning::TNotifierInfo CUsbUiNotifOtgWarning::RegisterL()
    {
    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifOtgWarning::RegisterL"));
    iInfo.iUid = KUsbUiNotifOtgWarning;
    iInfo.iChannel = KUsbUiNotifOtgWarning;
    iInfo.iPriority = ENotifierPriorityVHigh;
        FLOG(_L("[USBUINOTIF]\t CUsbUiNotifOtgWarning::RegisterL completed()"));
    return iInfo;
    }

// ----------------------------------------------------------------------------
// CUsbUiNotifOtgWarning::Cancel
// Release all own resources (member variables)
// ----------------------------------------------------------------------------
//
void CUsbUiNotifOtgWarning::Cancel()
    {
    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifOtgWarning::Cancel"));
    if (iNote)
        {
        FLOG(_L("[USBUINOTIF]\t CUsbUiNotifOtgWarning::Cancel - delete iNote"));
        iNote->Close();
        delete iNote;
        iNote = NULL;
        }
    CUSBUINotifierBase::Cancel();

    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifOtgWarning::Cancel() completed"));
    }

// ----------------------------------------------------------------------------
// CUsbUiNotifOtgWarning::GetParamsL
// Mandatory for USB UI Notifiers when using asynchronous launch. 
// This notifier is synchronous so this function is not used.
// ----------------------------------------------------------------------------
//
void CUsbUiNotifOtgWarning::StartDialogL(const TDesC8& aBuffer,
        TInt aReplySlot, const RMessagePtr2& aMessage)
    {
    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifOtgWarning::StartDialogL"));
    if (iReplySlot != 0 || iNeedToCompleteMessage)
        {
        User::Leave( KErrInUse );
        }
        
    InitializeTextResolver();

    iMessage = aMessage;
    iNeedToCompleteMessage = ETrue;
    iReplySlot = aReplySlot;

    // Get parameters 
    //    
    TPckgC<TInt> pckg( iNoteId );
    pckg.Set( aBuffer );
    iNoteId = pckg();
	FTRACE(FPrint(_L("[USBUINOTIF]\t CUsbUiNotifOtgWarning::GetParamsL iNoteId: %d"), iNoteId ));  
    if ( iNoteId < 0 || iNoteId >= iStringIds->MdcaCount())
        {        
        User::Leave( KErrArgument);        
        }   
    if (iNote)
            {
            delete iNote;
            iNote = NULL;
            }
        
    iNote = CHbDeviceMessageBoxSymbian::NewL(
                CHbDeviceMessageBoxSymbian::EWarning, this);
    HBufC* stringHolder = HbTextResolverSymbian::LoadLC(iStringIds->MdcaPoint(iNoteId) );
    iNote->SetTextL(*stringHolder);
    iNote->ShowL();
    CleanupStack::PopAndDestroy( stringHolder );
   
    FLOG(_L("[USBUINOTIF]\t CUsbUiNotifOtgWarning::StartDialogL completed"));
    }

// ----------------------------------------------------------------------------
// Call back function to observe device message box closing.
// ----------------------------------------------------------------------------
//
void CUsbUiNotifOtgWarning::MessageBoxClosed(
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
