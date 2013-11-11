/*
 * Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <hb/hbwidgets/hbdevicemessageboxsymbian.h>
#include <hb/hbcore/hbtextresolversymbian.h>
#include <usbuinotif.h>                     // pck
#include "usbuinqueriesnotifiermdrv.h"      // Own class definition
#include "usbuinotifdebug.h"                // Debugging macros

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CUSBUIQueriesNotifier::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CUSBUIQueriesNotifier* CUSBUIQueriesNotifier::NewL()
    {
        FLOG(_L("[USBUINOTIF]\t CUSBUIQueriesNotifier::NewL"));
    CUSBUIQueriesNotifier* self = new (ELeave) CUSBUIQueriesNotifier();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CUSBUIQueriesNotifier::CUSBUIQueriesNotifier
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CUSBUIQueriesNotifier::CUSBUIQueriesNotifier()
    {
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CUSBUIQueriesNotifier::~CUSBUIQueriesNotifier()
    {
    //Make sure that the request is completed. Note that inside the destructor,
    //this virtual function call is to local CUSBUIQueriesNotifier::Cancel, 
    //not to any possibly derived class implementation. 
    Cancel();
    }

// ----------------------------------------------------------------------------
// CUSBUIQueriesNotifier::RegisterL
// Register notifier.
// ----------------------------------------------------------------------------
//
CUSBUIQueriesNotifier::TNotifierInfo CUSBUIQueriesNotifier::RegisterL()
    {
    FLOG(_L("[USBUINOTIF]\t CUSBUIQueriesNotifier::RegisterL"));
    iInfo.iUid = KQueriesNotifier;
    iInfo.iChannel = KQueriesNotifier;
    iInfo.iPriority = ENotifierPriorityVHigh; // must be higher than the one 
        // in cableconnected notifier
    FLOG(_L("[USBUINOTIF]\t CUSBUIQueriesNotifier::RegisterL completed"));
    return iInfo;
    }


// ----------------------------------------------------------------------------
// CUSBUIQueriesNotifier::GetParamsL
// ----------------------------------------------------------------------------
//
void CUSBUIQueriesNotifier::StartDialogL(const TDesC8& aBuffer,
        TInt aReplySlot, const RMessagePtr2& aMessage)
    {
    FLOG(_L("[USBUINOTIF]\t  CUSBUIQueriesNotifier::StartDialogL"));
    if ( iReplySlot != 0 || iNeedToCompleteMessage)
        {
        User::Leave( KErrInUse );
        }
        
    InitializeTextResolver();

    iMessage = aMessage;
    iNeedToCompleteMessage = ETrue;
    iReplySlot = aReplySlot;

    // Get parameters 
    //
    TUSBQueriesNotiferParams params; //stores parameters from aBuffer
    TPckgC<TUSBQueriesNotiferParams> pckg( params );
    pckg.Set( aBuffer );
    // Save the type of the query for later use (dialog selection)
    //
    iQueryType = pckg().iQuery;
         
    if (iUSBQueryDlg)
        {
        delete iUSBQueryDlg;
        iUSBQueryDlg = NULL;
        }
    iUSBQueryDlg = CHbDeviceMessageBoxSymbian::NewL(
                       CHbDeviceMessageBoxSymbian::EWarning, this);
    iUSBQueryDlg->SetTimeout(0);
    HBufC* stringHolder = NULL;
    switch (iQueryType)
            {
            case EUSBStorageMediaFailure:
                {
                _LIT(KMassStorageFail, "txt_usb_info_unable_to_show_a_memory_to_other_devi");
                stringHolder = HbTextResolverSymbian::LoadLC( KMassStorageFail );
                 break;
                }
            case EUSBDiskFull:
                {
                _LIT(KDiskFull, "txt_usb_info_disk_full_remove_some_files_and_try");
                stringHolder = HbTextResolverSymbian::LoadLC( KDiskFull );
		         break;
                }
            case EUSBNotEnoughRam:
                {
                _LIT(KNotEnoughMemory, "txt_usb_info_memory_full_close_some_applications");
                 stringHolder = HbTextResolverSymbian::LoadLC( KNotEnoughMemory );
                 break;
                }
            default:
                {
                FTRACE( FPrint( _L( "[USBUINOTIF]\t CUSBUIQueriesNotifier::ERROR! Unknown query type: %d" ),iQueryType ) );
                }
            }
   
    if (stringHolder)
        {
        iUSBQueryDlg->SetTextL(*stringHolder);
        }
        
    iUSBQueryDlg->ShowL();
    FLOG(_L("[USBUINOTIF]\t CUSBUIQueriesNotifier::StartDialogL() ShowL returned"));     
    
    CleanupStack::PopAndDestroy( stringHolder );
    
    
    FLOG(_L("[USBUINOTIF]\t CUSBUIQueriesNotifier::StartDialogL completed"));
    }

// ----------------------------------------------------------------------------
// CUSBUIQueriesNotifier::Cancel
// Release all own resources (member variables)
// ----------------------------------------------------------------------------
//
void CUSBUIQueriesNotifier::Cancel()
    {
    FLOG(_L("[USBUINOTIF]\t CUSBUIQueriesNotifier::Cancel"));
    if (iUSBQueryDlg)
        {
        iUSBQueryDlg->Close();
        delete iUSBQueryDlg;
        iUSBQueryDlg = NULL;
        }
 
    CUSBUINotifierBase::Cancel();
    FLOG(_L("[USBUINOTIF]\t CUSBUIQueriesNotifier::Cancel() completed"));
    }


void CUSBUIQueriesNotifier::MessageBoxClosed(
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
